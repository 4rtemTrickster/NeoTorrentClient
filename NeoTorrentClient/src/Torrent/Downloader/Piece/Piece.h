#pragma once
#include "Torrent/Downloader/Block/Block.h"

namespace NTC
{
    /**
    * \brief A class representation of a piece of the Torrent content.
    * Each piece except the final one has a length equal to the
    * value specified by the 'piece length' attribute in the
    * Torrent file.
    * A piece is what is defined in the torrent meta-data. However,
    * when sharing data between peers a smaller unit is used - this
    * smaller piece is refereed to as `Block` by the unofficial
    * specification.
    */
    class Piece
    {
    public:
        explicit Piece(std::size_t index, std::vector<Ref<Block>> blocks, const std::string& hashValue);
        virtual ~Piece() = default;

    protected:
        const std::size_t index_;
        std::vector<Ref<Block>> blocks_;
        const std::string hashValue_;
    };
}