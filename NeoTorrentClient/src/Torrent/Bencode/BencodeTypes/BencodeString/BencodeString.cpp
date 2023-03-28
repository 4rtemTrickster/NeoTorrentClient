#include "NTCpch.h"
#include "BencodeString.h"

namespace NTC
{
    std::string BencodeString::Encode() const
    {
        return std::to_string(value_.length()) + ":" + value_;
    }

    void BencodeString::Accept(IBencodeVisitor* visitor)
    {
        visitor->VisitString(CreateRef<BencodeString>(*this));
    }

    Ref<BencodeString> BencodeString::Read(const std::string& encoded, std::string::size_type& index)
    {
        std::string::size_type colonIndex = encoded.find(':', index);
        std::string size_str(std::string_view(encoded.c_str() + index, encoded.c_str() + colonIndex));
        std::string::size_type length = std::stoi(size_str);

        index = colonIndex + 1;

        std::string res(std::string_view(encoded.c_str() + index, length));

        index = index + length;

        return CreateRef<BencodeString>(std::move(res));
    }

    bool operator<(const BencodeString& lhs, const BencodeString& rhs)
    {
        return lhs.value_ < rhs.value_;
    }

    bool operator<=(const BencodeString& lhs, const BencodeString& rhs)
    {
        return !(rhs < lhs);
    }

    bool operator>(const BencodeString& lhs, const BencodeString& rhs)
    {
        return rhs < lhs;
    }

    bool operator>=(const BencodeString& lhs, const BencodeString& rhs)
    {
        return !(lhs < rhs);
    }

    bool operator<(const BencodeString& lhs, const std::string& rhs)
    {
        return lhs.value_ < rhs;
    }

    bool operator<=(const BencodeString& lhs, const std::string& rhs)
    {
        return !(rhs < lhs);
    }

    bool operator>(const BencodeString& lhs, const std::string& rhs)
    {
        return rhs < lhs;
    }

    bool operator>=(const BencodeString& lhs, const std::string& rhs)
    {
        return !(lhs < rhs);
    }

    bool operator==(const BencodeString& lhs, const BencodeString& rhs)
    {
        return lhs.value_ == rhs.value_;
    }

    bool operator==(const BencodeString& lhs, const std::string& rhs)
    {
        return lhs.value_ == rhs;
    }

    bool operator!=(const BencodeString& lhs, const BencodeString& rhs)
    {
        return !(lhs == rhs);
    }

    bool operator!=(const BencodeString& lhs, const std::string& rhs)
    {
        return !(lhs == rhs);
    }
}