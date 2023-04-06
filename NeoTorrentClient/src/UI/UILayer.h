#pragma once
#include "Walnut/Layer.h"

#include "ImGuiFileDialog.h"

#include "AppSettings/AppSettings.h"
#include "FileReader/FileReader.h"
#include "Torrent/Bencode/BencodeTypes/BencodeDictionary/BencodeDictionary.h"
#include "Torrent/Downloader/Downloader.h"
#include "Torrent/TorrentFileFactory/TorrentFileFactory.h"
#include "Torrent/Bencode/BencodeDecoder/BencodeDecoder.h"
#include "Torrent/TorrentPeer/TorrentPeer.h"

namespace NTC
{
    class UILayer : public Walnut::Layer
    {
    public:
        static void MenuBarCallback();
        
        virtual void OnAttach() override;

        virtual void OnDetach() override;

        virtual void OnUIRender() override;

    protected:
        std::future<void> netF;
    };
}
