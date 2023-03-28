#pragma once
#include "Torrent/Bencode/BencodeTypes/IBencodeElement.h"

namespace NTC
{
    class BencodeDecoder
    {
    public:
        BencodeDecoder() = delete;
        static Ref<IBencodeElement> Decode(const std::string& encoded);
        static Ref<IBencodeElement> Decode(const std::string& encoded, std::string::size_type& index);
    };
}