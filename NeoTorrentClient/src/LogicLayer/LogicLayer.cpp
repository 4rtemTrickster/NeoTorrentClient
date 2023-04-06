#include "NTCpch.h"
#include "LogicLayer.h"

#include "AppSettings/AppSettings.h"
#include "Torrent/TorrentPeer/TorrentPeer.h"

namespace NTC
{
    void LogicLayer::OnAttach()
    {
        Logger::Init();
        TorrentPeer::GetInstance().Start();

        //TODO: Change to settings file
        AppSettings::ClientId = "-NT0001-";

        std::random_device rd; // Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
        std::uniform_int_distribution distrib(1, 9);
        for (int i = 0; i < 12; i++)
            AppSettings::ClientId += std::to_string(distrib(gen));

        AppSettings::Port = 8080;
    }

    void LogicLayer::OnDetach()
    {
        TorrentPeer::GetInstance().Stop();
    }

    void LogicLayer::OnUpdate(float ts)
    {
        TorrentPeer::GetInstance().Update();
    }
}
