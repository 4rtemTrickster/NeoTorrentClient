#pragma once
#include "Torrent/TorrentFile/ITorrentFile.h"

namespace NTC
{
    class Downloader
    {
    public:
        static void DownloadFile(Ref<ITorrentFile> file);
    };
}