#include "NTCpch.h"
#include "TorrentFileFactory.h"

#include "HexConverter/HexConverter.h"
#include "Torrent/TorrentFile/MultipleFileTorrent/MultipleFileTorrent.h"
#include "Torrent/TorrentFile/SingleFileTorrent/SingleFileTorrent.h"

#include "Torrent/Bencode/BencodeTypes/BencodeInt/BencodeInt.h"
#include "Torrent/Bencode/BencodeTypes/BencodeList/BencodeList.h"
#include "Torrent/Bencode/BencodeVisitor/BencodeVisitor.h"

namespace NTC
{
    std::future<void> TorrentFileFactory::stringSeparationResult;
    std::future<void> TorrentFileFactory::HasingResult;

    struct OptionalParameters
    {
        Ref<AnnounceList_t> announceList;
        Ref<int64_t> creationDate;
        Ref<std::string> comment;
        Ref<std::string> createdBy;
        Ref<std::string> encoding;
        Ref<int64_t> privateField;
    };

    Ref<ITorrentFile> TorrentFileFactory::CreateTorrentFile(Ref<BencodeDictionary>& dictionary)
    {
        NTC_PROFILE_FUNCTION();
        NTC_TRACE("Creating new torrent file");

        // Different visitors are created to asynchronously calculate the parameters of torrent files,
        // but hashing takes much longer than calculating all the parameters sequentially.
        BencodeVisitor RequiredVisitor;

        Ref<BencodeDictionary> InfoDic = TryGetDictionaryValue(dictionary, "info", RequiredVisitor);

        if (!InfoDic)
        {
            NTC_ERROR("Can not find info dictionary while parsing");
            return {};
        }
        
        std::string infoHash;

        HasingResult = std::async(std::launch::async, [&]()
        {
            NTC_PROFILE_SCOPE("Hash");

            //TODO: This fucking hashing takes 90% of the entire creation time
            boost::compute::detail::sha1 sha{InfoDic->Encode()};
            
            HexConverter::Decode(sha, infoHash);
        });

        //Required parameters
        Ref<std::string> announce(TryGetStringValue(dictionary, "announce", RequiredVisitor));
        Ref<int64_t> pieceLength(TryGetIntValue(InfoDic, "piece length", RequiredVisitor));
        Ref<std::string> pieces(TryGetStringValue(InfoDic, "pieces", RequiredVisitor));
        Ref<std::string> name(TryGetStringValue(InfoDic, "name", RequiredVisitor));
        Ref<std::vector<std::string>> piecesHashes;
        if (pieces != nullptr)
            stringSeparationResult = std::async(std::launch::async, [&]()
            {
                piecesHashes = SeparatePiecesStr(pieces);
            });
        
        Ref<ITorrentFile> NewTorrent;
        
        if (InfoDic->contains("files"))
            NewTorrent = CreateMultipleFileTorrent(InfoDic, announce, pieceLength, name, piecesHashes);
        else
            NewTorrent = CreateSingleFileTorrent(InfoDic, announce, pieceLength, name, piecesHashes);

        
        // Optional parameters
        if(NewTorrent != nullptr)
        {
            // Different visitors are created to asynchronously calculate the parameters of torrent files,
            // but hashing takes much longer than calculating all the parameters sequentially.
            // I decided to leave it that way for now until I can optimize the hashing process
            BencodeVisitor OptionalVisitor;
            
            Ref<BencodeList> announceListOfLists = TryGetListValue(dictionary, "announce-list", OptionalVisitor);

            const OptionalParameters parameters{
                TraverseAnnounceList(announceListOfLists),
                TryGetIntValue(dictionary, "creation date", OptionalVisitor),
                TryGetStringValue(dictionary, "comment", OptionalVisitor),
                TryGetStringValue(dictionary, "created by", OptionalVisitor),
                TryGetStringValue(dictionary, "encoding", OptionalVisitor),
                TryGetIntValue(InfoDic, "private", OptionalVisitor)
            };

            if(parameters.announceList != nullptr) NewTorrent->SetAnnounceList(std::move(*parameters.announceList));
            if(parameters.creationDate != nullptr) NewTorrent->SetCreationDate(*parameters.creationDate);
            if(parameters.comment != nullptr) NewTorrent->SetComment(std::move(*parameters.comment));
            if(parameters.createdBy != nullptr) NewTorrent->SetCreatedBy(std::move(*parameters.createdBy));
            if(parameters.encoding != nullptr) NewTorrent->SetEncoding(std::move(*parameters.encoding));
            if(parameters.privateField != nullptr) NewTorrent->SetPrivate(*parameters.privateField);
        }

        {
            NTC_PROFILE_SCOPE("Wating for hashing");
            HasingResult.wait();
        }

        if(!infoHash.empty()) NewTorrent->SetInfoHash(std::move(infoHash));
        else return {};

        return NewTorrent;
    }


    Ref<MultipleFileTorrent> TorrentFileFactory::CreateMultipleFileTorrent(Ref<BencodeDictionary>& InfoDic,
                                                                             Ref<std::string>& announce,
                                                                             Ref<int64_t>& pieceLength,
                                                                             Ref<std::string>& name,
                                                                             Ref<std::vector<std::string>>& pieceHashes)
    {
        NTC_PROFILE_FUNCTION();
        BencodeVisitor MultipleVisitor;
        Ref<BencodeList> FilesList = TryGetListValue(InfoDic, "files", MultipleVisitor);

        std::list<MultipleFileTorrent::file> files;
        for (auto& element : *FilesList)
        {
            NTC_PROFILE_SCOPE("FilesList");
            Ref<BencodeDictionary> dic = TryGetDictionaryValue(element, MultipleVisitor);
            Ref<int64_t> length = TryGetIntValue(dic, "length", MultipleVisitor);

            Ref<std::string> md5sum = TryGetStringValue(InfoDic, "md5sum", MultipleVisitor);

            Ref<BencodeList> pathList = TryGetListValue(dic, "path", MultipleVisitor);
            std::list<std::string> path;

            for (auto& pathPart : *pathList)
            {
                NTC_PROFILE_SCOPE("paths");
                Ref<std::string> pp = TryGetStringValue(pathPart, MultipleVisitor);

                if (pp != nullptr)
                    path.push_back(*pp);
                else
                    NTC_ERROR("Path part value error");
            }

            if (length != nullptr)
                files.emplace_back(*length, std::move(path));
            else
            {
                NTC_ERROR(
                    "Cannot create a torrent file representation 'cause one of the required fields is not filled in");
                return nullptr;
            }

            if(md5sum != nullptr)
                files.back().SetMd5Sum(std::move(*md5sum));
        }
        
        stringSeparationResult.wait();
        
        if (announce != nullptr &&
            pieceLength != nullptr &&
            name != nullptr &&
            pieceHashes != nullptr &&
            !files.empty()
        )
            return CreateRef<MultipleFileTorrent>(
                std::move(*announce),
                *pieceLength,
                std::move(pieceHashes),
                std::move(*name),
                std::move(files)
            );
        

        NTC_ERROR("Cannot create a torrent file representation 'cause one of the required fields is not filled in");
        
        return {};
    }

    Ref<SingleFileTorrent> TorrentFileFactory::CreateSingleFileTorrent(Ref<BencodeDictionary>& InfoDic,
                                                                         Ref<std::string>& announce,
                                                                         Ref<int64_t>& pieceLength,
                                                                         Ref<std::string>& name,
                                                                         Ref<std::vector<std::string>>& pieceHashes)
    {
        NTC_PROFILE_FUNCTION();
        BencodeVisitor SingleVisitor;
        
        Ref<int64_t> length = TryGetIntValue(InfoDic, "length", SingleVisitor);
        Ref<std::string> md5sum = TryGetStringValue(InfoDic, "md5sum", SingleVisitor);
        
        stringSeparationResult.wait();

        Ref<SingleFileTorrent> NewTorrent;

        if (announce != nullptr &&
            pieceLength != nullptr &&
            name != nullptr &&
            pieceHashes != nullptr &&
            length != nullptr)
            NewTorrent = CreateRef<SingleFileTorrent>(
                std::move(*announce),
                *pieceLength,
                std::move(pieceHashes),
                std::move(*name),
                *length);

        if(md5sum != nullptr) NewTorrent->SetMd5Sum(std::move(*md5sum));
        

        if(NewTorrent != nullptr)
            return NewTorrent;

        
        NTC_ERROR("Cannot create a torrent file representation 'cause one of the required fields is not filled in");

        return {};
    }

    Ref<std::vector<std::string>> TorrentFileFactory::SeparatePiecesStr(Ref<std::string>& pieces)
    {
        NTC_PROFILE_FUNCTION();

        if(pieces->size() % HasSize != 0)
        {
            NTC_ERROR("Pieces size % HasSize != 0");
            return nullptr;
        }

        //TODO: YOU NEED to optimize this shit! we a copying almost full .torrent file
        Ref<std::vector<std::string>> pieceHashes = CreateRef<std::vector<std::string>>();

        const std::size_t piecesCount = pieces->size() / HasSize;
        pieceHashes->reserve(piecesCount);

        for (std::size_t i = 0; i < piecesCount; ++i)
            pieceHashes->emplace_back(std::string_view(pieces.get()->c_str() + i * HasSize, HasSize));

        return pieceHashes;
    }

    Ref<AnnounceList_t> TorrentFileFactory::TraverseAnnounceList(Ref<BencodeList>& announceListOfLists)
    {
        NTC_PROFILE_FUNCTION();

        if(announceListOfLists == nullptr) return {};
        
        Ref<AnnounceList_t> ret = CreateRef<AnnounceList_t>();
        for (auto& element : *announceListOfLists)
        {
            auto innerList = DynamicCast<BencodeList>(element);
            if(innerList != nullptr)
            {
                for (auto& innerElement : *innerList)
                {
                    auto castedElement = DynamicCast<BencodeString>(innerElement);
                    if(castedElement != nullptr) ret->push_back(castedElement->GetValue());
                }
            }
        }

        return ret;
    }

    Ref<std::string> TorrentFileFactory::TryGetStringValue(Ref<BencodeDictionary>& dic, const std::string& key, IBencodeVisitor& visitor)
    {
        if (dic->contains(key))
        {
            dic->at(key)->Accept(&visitor);
            return CreateRef<std::string>(visitor.GetStringValue()->GetValue());
        }
        else
            NTC_WARN("Element by key: " + key + " does not exist");

        return {};
    }

    Ref<std::string> TorrentFileFactory::TryGetStringValue(Ref<IBencodeElement>& element, IBencodeVisitor& visitor)
    {
        element->Accept(&visitor);
        return CreateRef<std::string>(visitor.GetStringValue()->GetValue());
    }

    Ref<int64_t> TorrentFileFactory::TryGetIntValue(Ref<BencodeDictionary>& dic, const std::string& key, IBencodeVisitor& visitor)
    {
        if (dic->contains(key))
        {
            dic->at(key)->Accept(&visitor);
            return CreateRef<int64_t>(visitor.GetIntValue()->GetValue());
        }
        else
            NTC_WARN("Element by key: " + key + " does not exist");

        return {};
    }

    Ref<int64_t> TorrentFileFactory::TryGetIntValue(Ref<IBencodeElement>& element, IBencodeVisitor& visitor)
    {
        element->Accept(&visitor);
        Ref<BencodeInt> bvalue(visitor.GetIntValue());
        if(bvalue != nullptr)
            return CreateRef<int64_t>(bvalue->GetValue());

        return {};
    }

    Ref<BencodeList> TorrentFileFactory::TryGetListValue(Ref<BencodeDictionary>& dic, const std::string& key, IBencodeVisitor& visitor)
    {
        if (dic->contains(key))
        {
            dic->at(key)->Accept(&visitor);
            return visitor.GetListValue();
        }
        else
            NTC_WARN("Element by key: " + key + " does not exist");

        return {};
    }

    Ref<BencodeDictionary> TorrentFileFactory::TryGetDictionaryValue(Ref<BencodeDictionary>& dic,
                                                                     const std::string& key, IBencodeVisitor& visitor)
    {
        if (dic->contains(key))
        {
            dic->at(key)->Accept(&visitor);
            return visitor.GetDictionaryValue();
        }
        else
            NTC_WARN("Element by key:" + key + " does not exist");

        return {};
    }

    Ref<BencodeDictionary> TorrentFileFactory::TryGetDictionaryValue(Ref<IBencodeElement>& element, IBencodeVisitor& visitor)
    {
        element->Accept(&visitor);
        return visitor.GetDictionaryValue();
    }
}