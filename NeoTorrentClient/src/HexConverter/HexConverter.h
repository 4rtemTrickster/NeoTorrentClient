#pragma once

namespace NTC
{
    class HexConverter
    {
    public:
        HexConverter() = delete;
        
        /**
         * \brief Converts a string in hexadecimal form to a string in char form
         * \param src The string to be decoded
         * \param dest String to write 
         */
        static void Decode(const std::string& src, std::string& dest);

        /**
         * \brief Hex-encode a string
         * \param src The string to be encoded
         * \param dest String to write
         */
        static void Encode(const std::string& src, std::string& dest);
        
    };
}