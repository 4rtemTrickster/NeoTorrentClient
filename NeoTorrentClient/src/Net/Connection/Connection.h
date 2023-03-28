#pragma once
#include "Containers/ThreadSafeQueue/ThreadSafeQueue.h"
#include "Net/Message/Message.h"
#include "Net/IPeer/IComp.h"

namespace NTC
{
    template <typename T>
    class Connection : public std::enable_shared_from_this<Connection<T>>
    {
    public:
        enum class Direction
        {
            D_In,
            D_Out
        };

        Connection(std::string&& handshakeMsg, Direction direction, boost::asio::io_context& asioContext,
                   boost::asio::ip::tcp::socket socket,
                   containers::ThreadSafeQueue<OwnedMessage<T>>& qIn);

        virtual ~Connection() = default;

        void Send(const Message<T>& msg);


        [[nodiscard]]
        inline uint32_t GetID() const { return id_; }

        [[nodiscard]]
        inline auto GetEndpoint() const { return socket_.remote_endpoint(); }

        void AcceptConnection(IComp<T>* peer, uint32_t id);
        void Connect(const boost::asio::ip::tcp::resolver::results_type& endpoint);
        void Disconnect();

        [[nodiscard]]
        bool IsConnected() const { return socket_.is_open(); }

    protected:
        void WriteValidation();
        void ReadValidation(IComp<T>* peer = nullptr);

        void ReadHeader();
        void ReadBody();

        void WriteHeader();
        void WriteBody();

        void AddToIncomingMessageQueue();

        bool HandshakeComp();

        boost::asio::ip::tcp::socket socket_;
        boost::asio::io_context& asioContext_;

        containers::ThreadSafeQueue<Message<T>> qMessagesOut_;
        containers::ThreadSafeQueue<OwnedMessage<T>>& qMessagesIn;
        Message<T> msgTemporaryIn_;

        Direction direction_;

        std::string HandshakeOut{};
        std::string HandshakeIn{};

        uint32_t id_ = 0;
    };

    template <typename T>
    Connection<T>::Connection(std::string&& handshakeMsg,
                              Direction direction, boost::asio::io_context& asioContext,
                              boost::asio::ip::tcp::socket socket,
                              containers::ThreadSafeQueue<OwnedMessage<T>>& qIn)
        : socket_(std::move(socket)), asioContext_(asioContext), qMessagesIn(qIn), HandshakeOut(std::move(handshakeMsg))
    {
        direction_ = direction;
        HandshakeIn.resize(68);
    }

    template <typename T>
    void Connection<T>::Send(const Message<T>& msg)
    {
        boost::asio::post(
            asioContext_,
            [this, msg]()
            {
                bool bIsWritingMessage = !qMessagesOut_.is_empty();
                qMessagesOut_.push(msg);

                if (bIsWritingMessage)
                    WriteHeader();
            });
    }

    template <typename T>
    void Connection<T>::AcceptConnection(IComp<T>* peer, uint32_t id)
    {
        if (direction_ == Direction::D_In && socket_.is_open())
        {
            id_ = id;
            ReadValidation(peer);
            WriteValidation();
        }
    }

    template <typename T>
    void Connection<T>::Connect(const boost::asio::ip::tcp::resolver::results_type& endpoint)
    {
        if (direction_ == Direction::D_Out)
        {
            boost::asio::async_connect(
                socket_,
                endpoint,
                [this](std::error_code ec, boost::asio::ip::tcp::endpoint endpoint)
                {
                    if (!ec)
                    {
                        WriteValidation();
                        ReadValidation();
                    }
                });
        }
    }

    template <typename T>
    void Connection<T>::Disconnect()
    {
        if (IsConnected())
            boost::asio::post(asioContext_, [this]() { socket_.close(); });
    }

    template <typename T>
    void Connection<T>::WriteValidation()
    {
        NTC_TRACE("WriteValidation to {0}", socket_.remote_endpoint());
        boost::asio::async_write(
            socket_,
            boost::asio::buffer(HandshakeOut.data(), 69 * sizeof(char)),
            [this](std::error_code ec, std::size_t length)
            {
                if (!ec)
                {
                    if (direction_ == Direction::D_In)
                        ReadHeader();
                }
                else
                {
                    NTC_ERROR("[{0}] WriteValidation fail. Error code: {1}, Message: {2}", socket_.remote_endpoint(),
                              std::to_string(ec.value()), ec.message());
                    socket_.close();
                }
            });
    }

    template <typename T>
    void Connection<T>::ReadValidation(IComp<T>* peer)
    {
        NTC_TRACE("ReadValidation from {0}", socket_.remote_endpoint());
        boost::asio::async_read(
            socket_,
            boost::asio::buffer(HandshakeIn.data(), 69 * sizeof(char)),
            [this, peer](std::error_code ec, std::size_t length)
            {
                if (!ec)
                {
                    if (direction_ == Direction::D_Out)
                    {
                        if (HandshakeComp())
                        {
                            NTC_TRACE("[{0}] Handshake validated!", socket_.remote_endpoint());
                            if (peer)
                                peer->OnValidated(this->shared_from_this());

                            ReadHeader();
                        }
                        else
                        {
                            NTC_TRACE("[{0}] Handshake validation failed!", socket_.remote_endpoint());
                            socket_.close();
                        }
                    }
                    else
                    {
                        NTC_TRACE("Write validation to [{0}]", socket_.remote_endpoint());
                        WriteValidation();
                    }
                }
                else
                {
                    NTC_TRACE("Peer[{0}] disconnected (ReadValidation). Error code: {1}, Message: {2}",
                              socket_.remote_endpoint(), std::to_string(ec.value()), ec.message());
                    socket_.close();
                }
            });
    }

    template <typename T>
    void Connection<T>::ReadHeader()
    {
        std::string lengthPrefix;
        lengthPrefix.resize(4);
        
        boost::asio::async_read(
            socket_,
            boost::asio::buffer(lengthPrefix.data(), 4 * sizeof(char)),
            [this, lengthPrefix](std::error_code ec, std::size_t length)
            {
                if (!ec)
                {
                    NTC_TRACE("[{0}] Get bytestring: {1}", socket_.remote_endpoint(), lengthPrefix);
                    msgTemporaryIn_.Header.LengthPrefix = bytesToInt(lengthPrefix);
                    if (msgTemporaryIn_.Header.LengthPrefix > 0)
                    {
                        std::string id;
                        id.resize(1);
                        
                        boost::asio::async_read(
                            socket_,
                            boost::asio::buffer(id.data(), sizeof(char)),
                            [this, id](std::error_code ec, std::size_t length)
                            {

                                if(!ec)
                                {
                                    msgTemporaryIn_.Header.Id = static_cast<T>(bytesToInt(id));
                                }
                                else
                                {
                                    NTC_ERROR("[{0}] read header fail(id). Error code: {1}, Message: {2}",
                                              socket_.remote_endpoint(),
                                              std::to_string(ec.value()), ec.message());
                                    
                                    socket_.close();
                                }
                            });
                    }
                    else
                        msgTemporaryIn_.Header.Id = static_cast<T>(UCHAR_MAX - 1);
                        
                    
                    
                    NTC_INFO("[{0}] Readed header: Length: {1} ID: {2}", socket_.remote_endpoint(),
                             std::to_string(msgTemporaryIn_.Header.LengthPrefix), std::to_string(static_cast<uint32_t>(msgTemporaryIn_.Header.Id)));
                    
                    if (msgTemporaryIn_.Header.LengthPrefix > 0)
                    {
                        msgTemporaryIn_.Body.resize(msgTemporaryIn_.Header.LengthPrefix);
                        ReadBody();
                    }
                    else
                        AddToIncomingMessageQueue();
                }
                else
                {
                    NTC_ERROR("[{0}] read header fail. Error code: {1}, Message: {2}", socket_.remote_endpoint(),
                              std::to_string(ec.value()), ec.message());
                    socket_.close();
                }
            });
    }

    template <typename T>
    void Connection<T>::ReadBody()
    {
        boost::asio::async_read(
            socket_,
            boost::asio::buffer(msgTemporaryIn_.Body.data(), msgTemporaryIn_.Body.size()),
            [this](std::error_code ec, std::size_t length)
            {
                if (!ec)
                    AddToIncomingMessageQueue();
                else
                {
                    NTC_ERROR("[{0}] read body fail. Error code: {1}, Message: {2}", socket_.remote_endpoint(),
                              std::to_string(ec.value()), ec.message());
                    socket_.close();
                }
            });
    }

    template <typename T>
    void Connection<T>::WriteHeader()
    {
        boost::asio::async_write(
            socket_,
            boost::asio::buffer(&qMessagesOut_.front().Header, sizeof(MessageHeader<T>)),
            [&](std::error_code ec, std::size_t length)
            {
                if (!ec)
                {
                    if (qMessagesOut_.front().Body.size() > 0)
                        WriteBody();
                    else
                    {
                        Message<T> tmp;
                        qMessagesOut_.wait_and_pop(tmp);

                        if (!qMessagesOut_.is_empty())
                            WriteHeader();
                    }
                }
                else
                {
                    NTC_ERROR("[{0}] write header fail. Error code: {1}, Message: {2}", socket_.remote_endpoint(),
                              std::to_string(ec.value()), ec.message());
                    socket_.close();
                }
            });
    }

    template <typename T>
    void Connection<T>::WriteBody()
    {
        boost::asio::async_write(
            socket_,
            boost::asio::buffer(qMessagesOut_.front().Body.data(), qMessagesOut_.front().Body.size()),
            [this](std::error_code ec, std::size_t length)
            {
                if (!ec)
                {
                    Message<T> tmp;
                    qMessagesOut_.wait_and_pop(tmp);

                    if (qMessagesOut_.is_empty())
                        WriteHeader();
                }
                else
                {
                    NTC_ERROR("[{0}] write body fail. Error code: {1}, Message: {2}", socket_.remote_endpoint(),
                              std::to_string(ec.value()), ec.message());
                    socket_.close();
                }
            });
    }

    template <typename T>
    void Connection<T>::AddToIncomingMessageQueue()
    {
        NTC_TRACE("[{0}] AddToIncomingMessageQueue", socket_.remote_endpoint());
        if (direction_ == Direction::D_Out)
            qMessagesIn.push({this->shared_from_this(), msgTemporaryIn_});
        else
            qMessagesIn.push({nullptr, msgTemporaryIn_});

        ReadHeader();
    }

    template <typename T>
    bool Connection<T>::HandshakeComp()
    {
        //NTC_INFO("comp handshakes:\n{0}\n{1}", HandshakeIn, HandshakeOut);
        if (HandshakeIn.substr(28, 20) == HandshakeOut.substr(28, 20))
            return true;

        return false;
    }
}