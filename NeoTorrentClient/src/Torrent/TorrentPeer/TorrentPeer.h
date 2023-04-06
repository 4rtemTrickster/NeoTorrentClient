#pragma once
#include "NTCpch.h"

#include "AppSettings/AppSettings.h"
#include "Net/IPeer/IPeer.h"

namespace NTC
{
    enum class MessageID : uint8_t
    {
        MID_KeepAlive = UCHAR_MAX - 1,
        MID_Choke = 0,
        MID_Unchoke = 1,
        MID_Interested = 2,
        MID_NotInterested = 3,
        MID_Have = 4,
        MID_Bitfield = 5,
        MID_Request = 6,
        MID_Piece = 7,
        MID_Cancel = 8,
        MID_Port = 9
    };

    class TorrentPeer : public IPeer<MessageID>
    {
    public:
        static bool ConnectToPeer(const std::string& ip, const uint16_t port, std::string&& handshakeMsg)
        {
            return instance_.Connect(ip, port, std::move(handshakeMsg));
        }

        static bool OnStart() { return instance_.Start(); }
        static void OnStop() { instance_.Stop(); }
        static void OnUpdate() { instance_.Update(); }
        
    protected:
        static TorrentPeer instance_;
        
        TorrentPeer();

        virtual bool OnConnection(Ref<Connection<MessageID>> con) override;

        virtual void OnDisconnection(Ref<Connection<MessageID>> con) override;

        virtual void OnMessage(Ref<Connection<MessageID>> src, Message<MessageID> msg) override;

        virtual void OnValidated(Ref<Connection<MessageID>> con) override;
    };
}
