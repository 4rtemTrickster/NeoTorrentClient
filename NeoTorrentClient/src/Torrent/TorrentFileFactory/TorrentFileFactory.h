#pragma once
#include "Torrent/Bencode/BencodeTypes/BencodeDictionary/BencodeDictionary.h"
#include "Torrent/Bencode/BencodeTypes/BencodeList/BencodeList.h"
#include "Torrent/TorrentFile/ITorrentFile.h"
#include "Torrent/TorrentFile/MultipleFileTorrent/MultipleFileTorrent.h"
#include "Torrent/TorrentFile/SingleFileTorrent/SingleFileTorrent.h"

namespace NTC
{
    class TorrentFileFactory
    {
    public:
        TorrentFileFactory() = delete;
        TorrentFileFactory(const TorrentFileFactory&) = delete;
        TorrentFileFactory(TorrentFileFactory&&) = delete;

        [[nodiscard]]
        static Ref<ITorrentFile> CreateTorrentFile(Ref<BencodeDictionary>& dictionary);

    private:
        [[nodiscard]]
        static Ref<MultipleFileTorrent> CreateMultipleFileTorrent(Ref<BencodeDictionary>& InfoDic,
                                                                  Ref<std::string>& announce,
                                                                  Ref<int64_t>& pieceLength,
                                                                  Ref<std::string>& name,
                                                                  Ref<std::vector<std::string>>& pieceHashes);

        [[nodiscard]]
        static Ref<SingleFileTorrent> CreateSingleFileTorrent(Ref<BencodeDictionary>& InfoDic,
                                                              Ref<std::string>& announce,
                                                              Ref<int64_t>& pieceLength,
                                                              Ref<std::string>& name,
                                                              Ref<std::vector<std::string>>& pieceHashes);
        

        static Ref<std::vector<std::string>> SeparatePiecesStr(Ref<std::string>& pieces);
        static Ref<AnnounceList_t> TraverseAnnounceList(Ref<BencodeList>& announceListOfLists);

        static inline Ref<std::string> TryGetStringValue(Ref<BencodeDictionary>& dic, const std::string& key, IBencodeVisitor& visitor);
        static inline Ref<std::string> TryGetStringValue(Ref<IBencodeElement>& element, IBencodeVisitor& visitor);
        static inline Ref<int64_t> TryGetIntValue(Ref<BencodeDictionary>& dic, const std::string& key, IBencodeVisitor& visitor);
        static inline Ref<int64_t> TryGetIntValue(Ref<IBencodeElement>& element, IBencodeVisitor& visitor);
        static inline Ref<BencodeList> TryGetListValue(Ref<BencodeDictionary>& dic, const std::string& key, IBencodeVisitor& visitor);
        static inline Ref<BencodeDictionary> TryGetDictionaryValue(Ref<BencodeDictionary>& dic, const std::string& key, IBencodeVisitor& visitor);
        static inline Ref<BencodeDictionary> TryGetDictionaryValue(Ref<IBencodeElement>& element, IBencodeVisitor& visitor);

        static std::future<void> stringSeparationResult;
        static std::future<void> HasingResult;
    };
}