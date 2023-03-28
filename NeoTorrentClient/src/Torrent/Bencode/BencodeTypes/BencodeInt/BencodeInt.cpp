#include "NTCpch.h"
#include "BencodeInt.h"

namespace NTC
{
    std::string BencodeInt::Encode() const
    {
        return "i" + std::to_string(value_) + "e";
    }

    void BencodeInt::Accept(IBencodeVisitor* visitor)
    {
        visitor->VisitInt(CreateRef<BencodeInt>(*this));
    }

    Ref<BencodeInt> BencodeInt::Read(const std::string& encoded, std::string::size_type& index)
    {
        if(encoded.at(index) == 'i') ++index;

        std::string::size_type end = encoded.find_first_of('e', index);

        int64_t val = std::stoll(std::string_view(encoded.c_str() + index, end).data());
        
        index = end + 1;

        return CreateRef<BencodeInt>(val);
    }
}