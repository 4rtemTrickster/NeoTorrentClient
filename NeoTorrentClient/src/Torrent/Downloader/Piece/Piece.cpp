#include "NTCpch.h"
#include "Piece.h"

namespace NTC
{
    Piece::Piece(std::size_t index, std::vector<Ref<Block>> blocks, const std::string& hashValue)
        : index_(index), blocks_(blocks), hashValue_(hashValue) {}
    
}