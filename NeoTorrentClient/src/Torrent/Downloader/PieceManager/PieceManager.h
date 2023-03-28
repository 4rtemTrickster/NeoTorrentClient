#pragma once
#include "Torrent/Downloader/Piece/Piece.h"
#include "Torrent/TorrentFile/ITorrentFile.h"

namespace NTC
{
    class PieceManager
    {
    public:
        PieceManager(Ref<ITorrentFile> file);
        virtual ~PieceManager();
    protected:
        static constexpr int64_t BlockSize = 16384;
        
        void InitiatePieces();
        
        Ref<ITorrentFile> file_;
        
        std::vector<Ref<Piece>> missingPieces_;

        std::ofstream downloadedFile_;

        std::size_t totalPiecesCount;
    };
}