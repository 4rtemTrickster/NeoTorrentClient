#pragma once
#include <boost/functional/hash.hpp>

#include "Torrent/Bencode/BencodeTypes/IBencodeElement.h"

namespace NTC
{
    class BencodeString : public IBencodeElement
    {
    public:
        using value_type = std::string;
        BencodeString() : value_(std::string()) {}
        BencodeString(const std::string& value) : value_(value) {}
        BencodeString(std::string&& value) : value_(std::move(value)) {}
        BencodeString(const char* value) : value_(value) {}

        virtual ~BencodeString() override = default;

        [[nodiscard]]
        inline const std::string& GetValue() const { return value_; }
        inline std::string& GetValue() { return value_; }
        
        virtual std::string Encode() const override;

        virtual void Accept(IBencodeVisitor* visitor) override;

        static Ref<BencodeString> Read(const std::string& encoded, std::string::size_type& index);

        friend bool operator==(const BencodeString& lhs, const BencodeString& rhs);
        friend bool operator!=(const BencodeString& lhs, const BencodeString& rhs);
        friend bool operator<(const BencodeString& lhs, const BencodeString& rhs);
        friend bool operator<=(const BencodeString& lhs, const BencodeString& rhs);
        friend bool operator>(const BencodeString& lhs, const BencodeString& rhs);
        friend bool operator>=(const BencodeString& lhs, const BencodeString& rhs);
        
        friend bool operator==(const BencodeString& lhs, const std::string& rhs);
        friend bool operator!=(const BencodeString& lhs, const std::string& rhs);
        friend bool operator<(const BencodeString& lhs, const std::string& rhs);
        friend bool operator<=(const BencodeString& lhs, const std::string& rhs);
        friend bool operator>(const BencodeString& lhs, const std::string& rhs);
        friend bool operator>=(const BencodeString& lhs, const std::string& rhs);

        bool operator()(const BencodeString& lhs, const BencodeString& rhs) const { return lhs.value_ < rhs.value_; }

    protected:
        std::string value_;
    };
}