#include "NTCpch.h"
#include "BencodeVisitor.h"

#include "Torrent/Bencode/BencodeTypes/BencodeDictionary/BencodeDictionary.h"
#include "Torrent/Bencode/BencodeTypes/BencodeInt/BencodeInt.h"
#include "Torrent/Bencode/BencodeTypes/BencodeList/BencodeList.h"
#include "Torrent/Bencode/BencodeTypes/BencodeString/BencodeString.h"

namespace NTC
{
    void BencodeVisitor::VisitString(Ref<BencodeString> bstr)
    {
        intValue = nullptr;
        listValue = nullptr;
        dictionaryValue = nullptr;
        stringValue = bstr;
    }

    void BencodeVisitor::VisitInt(Ref<BencodeInt> bint)
    {
        listValue = nullptr;
        dictionaryValue = nullptr;
        stringValue = nullptr;
        intValue = bint;
    }

    void BencodeVisitor::VistitList(Ref<BencodeList> blist)
    {
        intValue = nullptr;
        dictionaryValue = nullptr;
        stringValue = nullptr;
        listValue = blist;
    }

    void BencodeVisitor::VistDictionary(Ref<BencodeDictionary> bdic)
    {
        intValue = nullptr;
        stringValue = nullptr;
        listValue = nullptr;
        dictionaryValue = bdic;
    }
}