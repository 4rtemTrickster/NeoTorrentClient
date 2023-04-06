#include "NTCpch.h"
#include "TorrentPeer.h"

#include "AppSettings/AppSettings.h"

namespace NTC
{
    TorrentPeer TorrentPeer::instance_;

    TorrentPeer::TorrentPeer()
        : IPeer(AppSettings::Port)
    {
    }

    bool TorrentPeer::OnConnection(Ref<Connection<MessageID>> con)
    {
        NTC_TRACE("OnConnection");
        return true;
    }

    void TorrentPeer::OnDisconnection(Ref<Connection<MessageID>> con)
    {
        NTC_TRACE("OnDisconnection");
    }

    void TorrentPeer::OnMessage(Ref<Connection<MessageID>> src, Message<MessageID> msg)
    {
        switch (msg.Header.Id)
        {
        case MessageID::MID_KeepAlive:
            NTC_TRACE("[{0}] Get KeepAlive", src->GetEndpoint());
            break;
        case MessageID::MID_Choke:
            NTC_TRACE("[{0}] Get Choke", src->GetEndpoint());
            break;
        case MessageID::MID_Unchoke:
            NTC_TRACE("[{0}] Get Unchoke", src->GetEndpoint());
            break;
        case MessageID::MID_Interested:
            NTC_TRACE("[{0}] Get Interested", src->GetEndpoint());
            break;
        case MessageID::MID_NotInterested:
            NTC_TRACE("[{0}] Get NotInterested", src->GetEndpoint());
            break;
        case MessageID::MID_Have:
            NTC_TRACE("[{0}] Get Have", src->GetEndpoint());
            break;
        case MessageID::MID_Bitfield:
            NTC_TRACE("[{0}] Get Bitfield", src->GetEndpoint());
            break;
        case MessageID::MID_Request:
            NTC_TRACE("[{0}] Get Request", src->GetEndpoint());
            break;
        case MessageID::MID_Piece:
            NTC_TRACE("[{0}] Get Piece", src->GetEndpoint());
            break;
        case MessageID::MID_Cancel:
            NTC_TRACE("[{0}] Get Cancel", src->GetEndpoint());
            break;
        case MessageID::MID_Port:
            NTC_TRACE("[{0}] Get Port", src->GetEndpoint());
            break;

        default:
            NTC_ERROR("[{0}] Incorrect message id: {1}", src->GetEndpoint(), std::to_string(static_cast<int>(msg.Header.Id)));
        }
    }

    void TorrentPeer::OnValidated(Ref<Connection<MessageID>> con)
    {
        NTC_TRACE("OnValidated");
    }
}
