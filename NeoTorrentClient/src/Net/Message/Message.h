#pragma once

namespace NTC
{
    /**
     * \brief Message Header is sent at start of all messages
     * \tparam T  The template allows us to use "enum class" to ensure that the messages are valid at compile time
     */
    template<typename T>
    struct MessageHeader
    {
        uint32_t LengthPrefix = 0;
        T Id{};
    };

    template<typename T>
    struct Message
    {
        MessageHeader<T> Header{};
        std::vector<uint8_t> Body;

        inline std::size_t size() const { return Body.size(); }

        friend std::ostream& operator<<(std::ostream& os, const Message<T>& msg)
        {
            os << "LengthPrefix: " << msg.Header.LengthPrefix << "ID: " << static_cast<int>(msg.Header.Id);
            return os;
        }

        // Convenience Operator overloads - These allow us to add and remove stuff from
        // the body vector as if it were a stack, so First in, Last Out. These are a 
        // template in itself, because we dont know what data type the user is pushing or 
        // popping, so lets allow them all. NOTE: It assumes the data type is fundamentally
        // Plain Old Data (POD). TLDR: Serialise & Deserialise into/from a vector

        /**
         * \brief Pushes any POD-like data into the message buffer
         */
        template<typename DataType>
        friend Message<T>& operator<<(Message<T>& msg, const DataType& data)
        {
            static_assert(std::is_standard_layout_v<DataType>, "Data is too complex to be pushed into vector");

            // Cache current size of vector, as this will be the point we insert the data
            std::size_t i = msg.Body.size();

            msg.Body.resize(msg.Body.size() + sizeof(DataType));

            std::memcpy(msg.Body.data() + i, &data, sizeof(DataType));

            msg.Header.LengthPrefix = msg.size();

            return msg;
        }

        /**
         * \brief Pulls any POD-like data form the message buffer
         */
        template<typename DataType>
        friend Message<T>& operator>> (Message<T>& msg, DataType& data)
        {
            static_assert(std::is_standard_layout_v<DataType>, "Data is too complex to be pulled from vector");

            std::size_t i = msg.Body.size() - sizeof(DataType);

            std::memcpy(&data, msg.Body.data() + i, sizeof(DataType));

            msg.Body.resize(i);

            msg.Header.LengthPrefix = msg.size();

            return msg;
        }
    };

    template<typename T>
    class Connection;

    template<typename T>
    struct OwnedMessage
    {
        Ref<Connection<T>> remote = nullptr;
        Message<T> msg;

        friend std::ostream& operator << (std::ostream& os, const OwnedMessage<T>& msg)
        {
            os << msg.msg;
            return os;
        }
    };
}