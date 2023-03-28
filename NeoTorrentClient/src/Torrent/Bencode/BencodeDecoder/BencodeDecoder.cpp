#include "NTCpch.h"
#include "BencodeDecoder.h"

#include "Torrent/Bencode/BencodeTypes/BencodeDictionary/BencodeDictionary.h"
#include "Torrent/Bencode/BencodeTypes/BencodeInt/BencodeInt.h"
#include "Torrent/Bencode/BencodeTypes/BencodeList/BencodeList.h"
#include "Torrent/Bencode/BencodeTypes/BencodeString/BencodeString.h"

namespace NTC
{
    Ref<IBencodeElement> BencodeDecoder::Decode(const std::string& encoded)
    {
        NTC_PROFILE_FUNCTION();
        std::string::size_type index = 0;
        return Decode(encoded, index);
    }

    Ref<IBencodeElement> BencodeDecoder::Decode(const std::string& encoded, std::string::size_type& index)
    {
        switch (encoded.at(index))
        {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9': return BencodeString::Read(encoded, index);
        case 'i': return BencodeInt::Read(encoded, index);
        case 'l': return BencodeList::Read(encoded, index);
        case 'd': return BencodeDictionary::Read(encoded, index);
        }

        NTC_WARN("Failed to identify type: " + std::to_string(encoded.at(index)));
        return nullptr;
    }
}