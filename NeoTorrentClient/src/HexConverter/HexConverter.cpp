#include "NTCpch.h"
#include "HexConverter.h"

namespace NTC
{
    void HexConverter::Decode(const std::string& src, std::string& dest)
    {
        NTC_PROFILE_FUNCTION();
        const std::string::size_type hashLength = src.length();
        dest.clear();
        dest.resize(hashLength / 2);

        static char sbstr[2] = {"\0"};

        for (std::size_t i = 0, j = 0; i < hashLength; i += 2, ++j)
        {
            sbstr[0] = src.at(i);
            sbstr[1] = src.at(i+1);
            char c = (char)(int)strtol(sbstr, nullptr, 16);
            dest.at(j) = c;
        }
    }

    void HexConverter::Encode(const std::string& value, std::string& dest)
    {
        static constexpr char hexDigits[] = "0123456789ABCDEF";
        
        dest.clear();
        dest.reserve(value.length() * 2);

        for (unsigned char c : value)
        {
            dest.push_back('\\');
            dest.push_back('x');
            dest.push_back(hexDigits[c >> 4]);
            dest.push_back(hexDigits[c & 15]);
        }
    }
}