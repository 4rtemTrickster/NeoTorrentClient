#pragma once
#include "NTCpch.h"
#include "Torrent/Bencode/BencodeTypes/IBencodeElement.h"
#include "Torrent/Bencode/BencodeTypes/BencodeString/BencodeString.h"

namespace NTC
{
    class BencodeDictionary : public IBencodeElement
    {
    public:
        using value_type = IBencodeElement;
        
        BencodeDictionary() = default;
        BencodeDictionary(BMap&& map);

        virtual ~BencodeDictionary() override = default;

        friend std::ostream& operator<<(std::ostream& os, const BencodeDictionary& obj)
        {
            return os << obj.Encode();
        }

        inline const BMap& GetValue() const { return Map_; }
        inline BMap& GetValue() { return Map_; }

        Ref<IBencodeElement> operator[](const std::string& key);
        Ref<IBencodeElement> at(const std::string& key);

        inline bool contains(const BMap::key_type& key) const { return Map_.contains(key); }

        virtual std::string Encode() const override;

        virtual void Accept(IBencodeVisitor* visitor) override;

        static Ref<BencodeDictionary> Read(const std::string& encoded, std::string::size_type& index);

    protected:
        BMap Map_;
    };
}