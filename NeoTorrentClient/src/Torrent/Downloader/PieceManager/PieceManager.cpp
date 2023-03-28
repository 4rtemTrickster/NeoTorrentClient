#include "NTCpch.h"
#include "PieceManager.h"

namespace NTC
{
    PieceManager::PieceManager(Ref<ITorrentFile> file)
        : file_(file)
    {
        InitiatePieces();
        
        //TODO: Multiple file torrents
        //TODO: Move download path to application settings
        downloadedFile_.open("D:/Downloads/Torrents/" + file_->GetName(), std::ios::binary | std::ios::out);
        downloadedFile_.seekp(file->GetLength() - 1);
        downloadedFile_.write("", 1);
    }

    PieceManager::~PieceManager()
    {
        downloadedFile_.close();
    }

    void PieceManager::InitiatePieces()
    {
        totalPiecesCount = file_->GetPieceHashes()->size();
        missingPieces_.reserve(totalPiecesCount);
        
        const int64_t totalLength = file_->GetLength();

        // number of blocks in a normal piece (i.e. pieces that are not the last one)
        int64_t blockCount = static_cast<int>(ceil(file_->GetPieceLength() / BlockSize));
        int64_t remLength = file_->GetPieceLength();

        for (std::size_t i = 0; i < totalPiecesCount; ++i)
        {
            // The final piece is likely to have a smaller size.
            if (i == totalPiecesCount - 1)
            {
                remLength = totalLength % file_->GetPieceLength();
                
                blockCount = std::max(static_cast<int>(ceil(remLength / BlockSize)), 1);
            }

            std::vector<Ref<Block>> blocks;
            blocks.reserve(blockCount);

            for (int64_t offset = 0; offset < blockCount; ++offset)
            {
                Ref<Block> block = CreateRef<Block>();
                block->piece = i;
                block->status = BlockStatus::BS_Missing;
                block->offset = offset * BlockSize;

                int64_t blockSize = BlockSize;
                if (i == totalPiecesCount - 1 && offset == blockCount - 1)
                    blockSize = remLength % BlockSize;

                block->length = blockSize;
                blocks.emplace_back(block);
            }
            
            Ref<Piece> piece = CreateRef<Piece>(i, blocks, file_->GetPieceHashes()->at(i));
            missingPieces_.emplace_back(piece);
        }
    }
}
