#include "NTCpch.h"
#include "utility.h"

namespace NTC
{
    int bytesToInt(const std::string& bytes)
    {
        //TODO: Use bitwise operation to convert
        std::string binStr;
        const std::string::size_type byteCount = bytes.size();
        for (std::size_t i = 0; i < byteCount; i++)
            binStr += std::bitset<8>(bytes[i]).to_string();
        return stoi(binStr, 0, 2);
    }
}