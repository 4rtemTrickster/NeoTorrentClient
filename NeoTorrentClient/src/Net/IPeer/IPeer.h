#pragma once
#include "IComp.h"
#include "Net/Message/Message.h"
#include "Net/Connection/Connection.h"

namespace NTC
{
    template<typename T>
    class IPeer : public IComp<T>
    {
    public:
        IPeer(uint16_t port);
        virtual ~IPeer() override;

        bool Connect(const std::string& ip, const uint16_t port, std::string&& handshakeMsg);

        bool Start();
        void Stop();

        /**
         * \brief ASYNC - Instruct asio to wait for connection
         */
        void WaitForConnection();
        void SendMsg(Ref<Connection<T>> dest, const Message<T>& msg);
        void SendMessageToAll(const Message<T>& msg, Ref<Connection<T>> ignored = nullptr);

        void Update(std::size_t MaxMessages = -1);

        virtual void OnValidated(Ref<Connection<T>> con) override
        {
            con;
        }

    protected:
        virtual bool OnConnection(Ref<Connection<T>> con) { return false; }

        virtual void OnDisconnection(Ref<Connection<T>> con)
        {
        }

        virtual void OnMessage(Ref<Connection<T>> src, Message<T> msg)
        {
        }
        
        containers::ThreadSafeQueue<OwnedMessage<T>> qMessagesIn_;
        std::deque<Ref<Connection<T>>> deqConnections_;

        boost::asio::io_context asioContext_;
        std::future<void> ContextProm_;

        boost::asio::ip::tcp::acceptor asioAcceptor_;

        uint32_t idCounter = 1;
    };
    
    template <typename T>
    IPeer<T>::IPeer(uint16_t port)
        : asioAcceptor_(asioContext_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
    {
    }

    template <typename T>
    IPeer<T>::~IPeer()
    {
        if (!asioContext_.stopped())
            Stop();
    }

    template <typename T>
    bool IPeer<T>::Connect(const std::string& ip, const uint16_t port, std::string&& handshakeMsg)
    {
        using namespace boost::asio::ip;

        try
        {
            NTC_TRACE("Connecting to: Ip: {0} Port: {1}", ip, std::to_string(port));
            tcp::resolver resolver(asioContext_);
            tcp::resolver::results_type endpoint = resolver.resolve(ip, std::to_string(port));

            Ref<Connection<T>> connection = CreateRef<Connection<T>>(
                std::move(std::string(handshakeMsg)),
                Connection<T>::Direction::D_Out,
                asioContext_,
                tcp::socket(asioContext_),
                qMessagesIn_);

            connection->Connect(endpoint);
            NTC_TRACE("Connected to: Ip: {0} Port: {1}", ip, std::to_string(port));

            deqConnections_.push_back(connection);
        }
        catch (std::exception& e)
        {
            NTC_CRITICAL("Exeption during connection {0}", e.what());
            return false;
        }

        return true;
    }

    template <typename T>
    bool IPeer<T>::Start()
    {
        NTC_TRACE("Peer starting...");
        try
        {
            WaitForConnection();
            
            ContextProm_ = std::async(std::launch::async, [this]() { asioContext_.run(); });
        }
        catch (std::exception& e)
        {
            NTC_CRITICAL("IPeer<T>::Start() exeption {0}", e.what());
            return false;
        }

        NTC_TRACE("Peer started");
        return true;
    }

    template <typename T>
    void IPeer<T>::Stop()
    {
        asioContext_.stop();
        if (ContextProm_.valid()) ContextProm_.wait();
        NTC_TRACE("IPeer stopped");
    }

    template <typename T>
    void IPeer<T>::WaitForConnection()
    {
        NTC_TRACE("WaitForConnection");
        asioAcceptor_.async_accept(
            [this](std::error_code ec, boost::asio::ip::tcp::socket socket)
            {
                if (!ec)
                {
                    NTC_TRACE("New connection: {0}", socket.remote_endpoint());
                    Ref<Connection<T>> newConn = CreateRef<Connection<T>>(
                        //TODO: now we are responding with "" to close the connection
                        std::move(std::string()),
                        Connection<T>::Direction::D_In,
                        asioContext_,
                        std::move(socket),
                        qMessagesIn_);

                    if (OnConnection(newConn))
                    {
                        deqConnections_.push_back(std::move(newConn));
                        deqConnections_.back()->AcceptConnection(this, idCounter++);

                        NTC_TRACE("Connection approved");
                    }
                    else
                        NTC_INFO("Connection denied");
                }
                else
                    NTC_ERROR("New connection error: {0}", ec.message());

                WaitForConnection();
            });
    }

    template <typename T>
    void IPeer<T>::SendMsg(Ref<Connection<T>> dest, const Message<T>& msg)
    {
        if (dest && dest->IsConnected())
            dest->Send(msg);
        else
        {
            OnDisconnection(dest);
            dest.reset();

            deqConnections_.erase(
                std::remove(deqConnections_.begin(), deqConnections_.end(), dest),
                deqConnections_);
        }
    }

    template <typename T>
    void IPeer<T>::SendMessageToAll(const Message<T>& msg, Ref<Connection<T>> ignored)
    {
        bool bInvalidClientExists = false;

        for (auto& dest : deqConnections_)
        {
            if (dest && dest->IsConnected())
            {
                if (dest != ignored)
                    dest->Send(msg);
            }
            else
            {
                OnDisconnection(dest);
                dest.reset();
                bInvalidClientExists = true;
            }
        }

        if (bInvalidClientExists)
            deqConnections_.erase(
                std::remove(deqConnections_.begin(), deqConnections_.end(), nullptr),
                deqConnections_);
    }

    template <typename T>
    void IPeer<T>::Update(std::size_t MaxMessages)
    {
        std::size_t MessageCount = 0;

        while (MessageCount < MaxMessages && !qMessagesIn_.is_empty())
        {
            OwnedMessage<T> msg;
            qMessagesIn_.wait_and_pop(msg);

            OnMessage(msg.remote, msg.msg);
        }
    }
}