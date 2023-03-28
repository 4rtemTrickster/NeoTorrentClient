#pragma once

namespace NTC
{
    enum class BlockStatus : uint8_t
    {
        BS_Missing = 0,
        BS_Pending = 1,
        BS_Retrieved = 2
    };
    
    /**
    * \brief A part of a piece that is requested and transferred between peers.
    * A Block, by convention, usually has the size of 2 ^ 14 bytes,
    * except for the last Block in a piece.
    */
    struct Block
    {
        int64_t piece;
        int64_t offset;
        int64_t length;
        BlockStatus status;
        //TODO: string sucks
        std::string data;
    };
}