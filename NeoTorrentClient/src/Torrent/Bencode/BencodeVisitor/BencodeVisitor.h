#pragma once
#include "IBencodeVisitor.h"

#include "Torrent/Bencode/BencodeTypes/IBencodeElement.h"


namespace NTC
{
    class BencodeVisitor final : public IBencodeVisitor
    {
    public:
        BencodeVisitor() = default;
        ~BencodeVisitor() override = default;
        
        void VisitString(Ref<BencodeString> bstr) override;
        void VisitInt(Ref<BencodeInt> bint) override;
        void VistitList(Ref<BencodeList> blist) override;
        void VistDictionary(Ref<BencodeDictionary> bdic) override;
        
        virtual Ref<BencodeInt> GetIntValue() override { return intValue; }
        virtual Ref<BencodeString> GetStringValue() override { return stringValue; }
        virtual Ref<BencodeList> GetListValue() override { return listValue; }
        virtual Ref<BencodeDictionary> GetDictionaryValue() override { return dictionaryValue; }

        //protected:
        Ref<BencodeInt> intValue;
        Ref<BencodeString> stringValue;
        Ref<BencodeList> listValue;
        Ref<BencodeDictionary> dictionaryValue;
    };
}