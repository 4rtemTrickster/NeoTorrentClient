#pragma once

namespace NTC
{
    class IBencodeElement;
    class BencodeDictionary;
    class BencodeList;
    class BencodeInt;
    class BencodeString;

    class IBencodeVisitor
    {
    public:
        virtual ~IBencodeVisitor() = default;

        virtual void VisitString(Ref<BencodeString> bstr) = 0;
        virtual void VisitInt(Ref<BencodeInt> bint) = 0;
        virtual void VistitList(Ref<BencodeList> blist) = 0; 
        virtual void VistDictionary(Ref<BencodeDictionary> bdic) = 0;

        virtual Ref<BencodeInt> GetIntValue() = 0;
        virtual Ref<BencodeString> GetStringValue() = 0;
        virtual Ref<BencodeList> GetListValue() = 0;
        virtual Ref<BencodeDictionary> GetDictionaryValue() = 0;
    };
}