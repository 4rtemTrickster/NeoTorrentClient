#include "NTCpch.h"
#include "Downloader.h"

#include "AppSettings/AppSettings.h"
#include "Net/IPeer/IPeer.h"
#include "PeerRetriever/PeerRetriever.h"
#include "Torrent/TorrentPeer/TorrentPeer.h"

namespace NTC
{
    void Downloader::DownloadFile(Ref<ITorrentFile> file)
    {
        Ref<std::vector<Ref<Peer>>> Peers = PeerRetriever::GetPeersList(file);

        if (!Peers) return;

        const std::string protocol = "BitTorrent protocol";
        std::stringstream buffer;
        buffer << (char)protocol.length();
        buffer << protocol;
        std::string reserved;
        for (int i = 0; i < 8; i++)
            reserved.push_back('\0');
        buffer << reserved;
        buffer << file->GetInfoHash();
        buffer << AppSettings::ClientId;
        assert(buffer.str().length() == protocol.length() + 49);

        for (auto& peer : *Peers)
        {
            TorrentPeer::ConnectToPeer(peer->IP, static_cast<uint16_t>(peer->Port), std::string(buffer.str()));
        }
    }
}