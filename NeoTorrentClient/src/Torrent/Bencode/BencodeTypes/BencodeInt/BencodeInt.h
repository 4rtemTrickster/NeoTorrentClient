#pragma once
#include "Torrent/Bencode/BencodeTypes/IBencodeElement.h"

namespace NTC
{
    class BencodeInt : public IBencodeElement
    {
    public:
        using value_type = int64_t;
        
        BencodeInt() : value_(0) {}
        BencodeInt(int64_t value) : value_(value) {}
        virtual ~BencodeInt() override = default;

        inline const int64_t& GetValue() const { return value_; }
        inline int64_t& GetValue() { return value_; }

        virtual std::string Encode() const override;

        virtual void Accept(IBencodeVisitor* visitor) override;

        static Ref<BencodeInt> Read(const std::string& encoded, std::string::size_type& index);

    protected:
        int64_t value_;
    };
}