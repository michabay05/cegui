#pragma once

#include "defs.hpp"
#include "fen.hpp"
#include <array>
#include <string>

extern const std::string pieceStr;
extern const std::array<std::string, 65> strCoords;
extern const std::array<int, 64> castlingRights;

struct Position
{
    std::array<uint64_t, 12> pieces{};
    std::array<uint64_t, 3> units{};

    Position();
    int getPieceOnSquare(const int sq) const;
    void updateUnits();
};

struct State
{
    PieceColor side = PieceColor::LIGHT;
    PieceColor xside = PieceColor::DARK;
    Sq enpassant = Sq::noSq;
    int castling = 0;
    int fullMoves = 0;
    int halfMoves = 0;

    State() = default;
    inline void changeSide() {
        side = xside;
        xside = (PieceColor)((int)side ^ 1);
    }
};

struct Board
{
    static const std::array<std::string, 8> position;
    Position pos;
    State state;

    Board();
    Board(const std::string& fen);
    void display() const;
    void printCastling() const;
    static bool isSquareAttacked(const PieceColor clr, const int sq, const Board& b);
    bool isInCheck() const;
    bool isOppInCheck() const;

  private:
    void parseFen(FENInfo fenInfo);
};

enum class CastlingRights : uint8_t { wk, wq, bk, bq };
