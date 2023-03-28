#include "NTCpch.h"
#include "PeerRetriever.h"

#pragma warning(suppress : 4244)
#include <cpr/cpr.h>

#include "AppSettings/AppSettings.h"
#include "Torrent/Bencode/BencodeDecoder/BencodeDecoder.h"
#include "Torrent/Bencode/BencodeTypes/IBencodeElement.h"
#include "Torrent/Bencode/BencodeTypes/BencodeDictionary/BencodeDictionary.h"
#include "Torrent/Bencode/BencodeTypes/BencodeInt/BencodeInt.h"
#include "Torrent/Bencode/BencodeTypes/BencodeList/BencodeList.h"
#include "Torrent/Bencode/BencodeVisitor/BencodeVisitor.h"

#define HTTP_STATUS_OK 200

namespace NTC
{
    Ref<PeerVector> PeerRetriever::GetPeersList(Ref<ITorrentFile>& file,
                                                            int64_t bytesDownloaded)
    {
        cpr::Response response = cpr::Get(cpr::Url{file->GetAnnounce()},
                                          cpr::Parameters
                                          {
                                              {"info_hash", file->GetInfoHash()},
                                              {"peer_id", AppSettings::ClientId},
                                              {"uploaded", "0"},
                                              {"downloaded", std::to_string(bytesDownloaded)},
                                              {"left", std::to_string(file->GetLength())},
                                              {"port", std::to_string(AppSettings::Port)},
                                              {"compact", "0"}
                                          });

        if (response.status_code == HTTP_STATUS_OK)
        {
            NTC_INFO("Retrieve response from tracker: SUCCESS");
            NTC_TRACE("Tracker response: {0}", response.text);

            return ProcessResponse(response.text);
        }
        else
            NTC_ERROR("Retrieving response from tracker: FAILED\nError code: {0}\nError message: {1}",
                  static_cast<int>(response.error.code),
                  response.error.message);

        return {};
    }

    Ref<PeerVector> PeerRetriever::ProcessResponse(std::string& response)
    {
        BencodeVisitor visitor;
        Ref<IBencodeElement> DecodedResponse = BencodeDecoder::Decode(response);

        if (typeid(*DecodedResponse) != typeid(BencodeDictionary))
            throw std::runtime_error("Received malformed response [No global dictionary]");

        DecodedResponse->Accept(&visitor);
        Ref<BencodeDictionary> ResponseDictionary = visitor.GetDictionaryValue();

#pragma region ErrorAndWarningHandling
        if (ResponseDictionary->contains("failure reason"))
        {
            ResponseDictionary->at("failure reason")->Accept(&visitor);
            throw std::runtime_error(
                "The tracker responded with an error message:" + visitor.GetStringValue()->GetValue());
        }

        if (ResponseDictionary->contains("warning message"))
        {
            ResponseDictionary->at("warning message")->Accept(&visitor);
            NTC_WARN("The tracker responded with an warning message: {0}", visitor.GetStringValue()->GetValue());
        }

#pragma endregion

        //TODO: You need to handle interval and min interval parameters here!

        if (!ResponseDictionary->contains("peers"))
            throw std::runtime_error(
                "Response returned by the tracker is not in the correct format. ['peers' not found]");

        Ref<IBencodeElement> EncodedPeers = ResponseDictionary->at("peers");

        EncodedPeers->Accept(&visitor);

        Ref<PeerVector> ret = CreateRef<PeerVector>();

        if (typeid(*EncodedPeers) == typeid(BencodeString))
            HandlePeersAsABinaryBlob(visitor.GetStringValue(), ret);
        else if (typeid(*EncodedPeers) == typeid(BencodeList))
            HandlePeersAsAList(visitor.GetListValue(), ret);
        else
            throw std::runtime_error(
                "Response returned by the tracker is not in the correct format. ['peers' has the wrong type]");

        return ret;
    }

    void PeerRetriever::HandlePeersAsABinaryBlob(Ref<BencodeString> peersStr, Ref<PeerVector> toReturn)
    {
        // Essentially, every 6 bytes represent a single peer with the first 4 bytes being the IP
        // and the last 2 bytes being the port number.
        constexpr std::size_t peerInfoSize = 6;

        std::string& peersString = peersStr->GetValue();

        if (peersString.length() % peerInfoSize != 0)
            throw std::runtime_error(
                "Received malformed 'peers' from tracker. ['peers' length needs to be divisible by 6]");

        const std::size_t peerNum = peersString.length() / peerInfoSize;

        for (std::size_t i = 0; i < peerNum; ++i)
        {
            std::size_t offset = i * peerInfoSize;

            std::stringstream peerIp;
            peerIp << std::to_string(static_cast<uint8_t>(peersString[offset])) << ".";
            peerIp << std::to_string(static_cast<uint8_t>(peersString[offset + 1])) << ".";
            peerIp << std::to_string(static_cast<uint8_t>(peersString[offset + 2])) << ".";
            peerIp << std::to_string(static_cast<uint8_t>(peersString[offset + 3]));
            int peerPort = bytesToInt(peersString.substr(offset + 4, 2));

            toReturn->push_back(CreateRef<Peer>(peerIp.str(), peerPort));
        }
    }

    void PeerRetriever::HandlePeersAsAList(Ref<BencodeList> peersList, Ref<PeerVector> toReturn)
    {
        BencodeVisitor visitor;
        for (auto& elem : *peersList)
        {
            elem->Accept(&visitor);
            Ref<BencodeDictionary> peerDict = visitor.GetDictionaryValue();

            if (!peerDict->contains("ip"))
                throw std::runtime_error("Received malformed 'peers' from tracker. [Item does not contain key 'ip']");

            peerDict->at("ip")->Accept(&visitor);
            std::string peerIp(visitor.GetStringValue()->GetValue());

            if(!peerDict->contains("port"))
                throw std::runtime_error("Received malformed 'peers' from tracker. [Item does not contain key 'port']");

            peerDict->at("port")->Accept(&visitor);
            int64_t peerPort = visitor.GetIntValue()->GetValue();

            toReturn->push_back(CreateRef<Peer>(peerIp, peerPort));
        }
    }
}