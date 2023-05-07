#pragma once

#include "defs.hpp"

struct FENInfo
{
    Piece board[64];
    PieceColor side;
    int castling;
    Sq enpassant;
    int halfMoves;
    int fullMoves;

    FENInfo(const std::string& fen);
private:
    void parse(const std::string& fen);
};
