#pragma once
#include "Torrent/Bencode/BencodeVisitor/IBencodeVisitor.h"

namespace NTC
{
    using BList = std::list<Ref<IBencodeElement>>;
    using BMap = std::map<BencodeString, Ref<IBencodeElement>, BencodeString>;
    
    class IBencodeElement
    {
    public:
        using value_type = IBencodeElement;
        virtual ~IBencodeElement() = default;
        virtual std::string Encode() const = 0;

        virtual void Accept(IBencodeVisitor* visitor) = 0;
    };
}