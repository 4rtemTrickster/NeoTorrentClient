#pragma once
#include "Torrent/Bencode/BencodeTypes/BencodeString/BencodeString.h"
#include "Torrent/TorrentFile/ITorrentFile.h"

namespace NTC
{
    struct Peer
    {
        std::string IP;
        int64_t Port;
    };

    using PeerVector = std::vector<Ref<Peer>>;
    
    class PeerRetriever
    {
    public:
        static Ref<PeerVector> GetPeersList(Ref<ITorrentFile>& file,
                                                        int64_t bytesDownloaded = 0);

    protected:
        static Ref<PeerVector> ProcessResponse(std::string& response);

        /**
         * \brief Handles the first case where peer information is sent in a binary blob (compact)
         * \param peersStr Encoded peers string
         */
        static void HandlePeersAsABinaryBlob(Ref<BencodeString> peersStr,
                                             Ref<PeerVector> toReturn);

        /**
         * \brief Handles the second case where peer information is stored in a list
         * \param peersList Encoded peers string
         */
        static void HandlePeersAsAList(Ref<BencodeList> peersList,
                                       Ref<PeerVector> toReturn);
    };
}