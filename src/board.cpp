#include "board.hpp"
#include "fen.hpp"

#include <iostream>

#include "attack.hpp"
#include "bitboard.hpp"
#include "magics.hpp"

const std::string pieceStr = "PNBRQKpnbrqk ";

const std::array<std::string, 65> strCoords = {
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8", "a7", "b7", "c7", "d7", "e7",
    "f7", "g7", "h7", "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6", "a5", "b5",
    "c5", "d5", "e5", "f5", "g5", "h5", "a4", "b4", "c4", "d4", "e4", "f4", "g4",
    "h4", "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3", "a2", "b2", "c2", "d2",
    "e2", "f2", "g2", "h2", "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1", " "};

const std::array<int, 64> castlingRights = {
    7,  15, 15, 15, 3,  15, 15, 11, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 13, 15, 15, 15, 12, 15, 15, 14};

const std::array<std::string, 8> Board::position = {
    "8/8/8/8/8/8/8/8 w - - 0 1",
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
    "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
    "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
    "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
    "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",
    "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10",
    "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1",
};

Position::Position() {
    pieces.fill(0);
    units.fill(0);
}

void Position::updateUnits() {
    units.fill(0);
    for (int piece = (int)PieceTypes::PAWN; piece <= (int)PieceTypes::KING; piece++) {
        units[(int)PieceColor::LIGHT] |= pieces[piece];
        units[(int)PieceColor::DARK] |= pieces[piece + 6];
    }
    units[(int)PieceColor::BOTH] = units[(int)PieceColor::LIGHT] | units[(int)PieceColor::DARK];
}

Board::Board() {
    FENInfo f(position[1]);
    parseFen(f);
}

Board::Board(const std::string &fen) {
    FENInfo f(fen);
    parseFen(f);
}

void Board::display() const {
    std::cout << "\n    +---+---+---+---+---+---+---+---+\n";
    for (int r = 0; r < 8; r++) {
        std::cout << "  " << 8 - r << " |";
        for (int f = 0; f < 8; f++)
            std::cout << " " << pieceStr[pos.getPieceOnSquare(SQ(r, f))] << " |";
        std::cout << "\n    +---+---+---+---+---+---+---+---+\n";
    }
    std::cout << "      a   b   c   d   e   f   g   h\n\n";
    std::cout << "      Side to move: " << (state.side == PieceColor::LIGHT ? "white" : "black") << "\n";
    printCastling();
    std::cout << "         Enpassant: "
              << (state.enpassant != Sq::noSq ? strCoords[(int)state.enpassant] : "noSq") << "\n";
    std::cout << "        Full moves: " << state.fullMoves << "\n";
}

void Board::printCastling() const {
    using enum CastlingRights;

    std::string castlingLtrs = "----";
    if (state.castling & (1 << (int)wk))
        castlingLtrs[0] = 'K';
    if (state.castling & (1 << (int)wq))
        castlingLtrs[1] = 'Q';
    if (state.castling & (1 << (int)bk))
        castlingLtrs[2] = 'k';
    if (state.castling & (1 << (int)bq))
        castlingLtrs[3] = 'q';
    std::cout << "        Full moves: " << castlingLtrs << "\n";
}

int Position::getPieceOnSquare(const int sq) const {
    using enum Piece;
    for (int i = (int)P; i <= (int)k; i++) {
        if (getBit(pieces[i], sq))
            return i;
    }
    return (int)E;
}

bool Board::isSquareAttacked(const PieceColor side, const int sq, const Board& b) {
    // Attacked by white pawns
    if ((side == PieceColor::LIGHT) &&
        (Attack::pawnAttacks[(int)PieceColor::DARK][sq] & b.pos.pieces[(int)Piece::P]))
        return true;
    // Attacked by black pawns
    if ((side == PieceColor::DARK) &&
        (Attack::pawnAttacks[(int)PieceColor::LIGHT][sq] & b.pos.pieces[(int)Piece::p]))
        return true;
    // Attacked by knights
    if (Attack::knightAttacks[sq] &
        b.pos.pieces[side == PieceColor::LIGHT ? (int)Piece::N : (int)Piece::n])
        return true;
    // Attacked by bishops
    if (Magics::getBishopAttack(sq, b.pos.units[(int)PieceColor::BOTH]) &
        b.pos.pieces[side == PieceColor::LIGHT ? (int)Piece::B : (int)Piece::b])
        return true;
    // Attacked by rooks
    if (Magics::getRookAttack(sq, b.pos.units[(int)PieceColor::BOTH]) &
        b.pos.pieces[side == PieceColor::LIGHT ? (int)Piece::R : (int)Piece::r])
        return true;
    // Attacked by queens
    if (Magics::getQueenAttack(sq, b.pos.units[(int)PieceColor::BOTH]) &
        b.pos.pieces[side == PieceColor::LIGHT ? (int)Piece::Q : (int)Piece::q])
        return true;
    // Attacked by kings
    if (Attack::kingAttacks[sq] &
        b.pos.pieces[side == PieceColor::LIGHT ? (int)Piece::K : (int)Piece::k])
        return true;

    // If all of the above cases fail, return false
    return false;
}

bool Board::isInCheck() const {
    uint8_t piece = state.side == PieceColor::LIGHT ? (int)Piece::k : (int)Piece::K;
    return isSquareAttacked(state.side, Bitboard::lsbIndex(pos.pieces[piece]), *this);
}

void Board::parseFen(FENInfo fen) {
    for (int i = 0; i < 64; i++) {
        if (fen.board[i] == Piece::E) continue;
        setBit(pos.pieces[(int)fen.board[i]], i);
    }
    pos.updateUnits();
    state.side = fen.side;
    state.xside = (PieceColor)(1 - (int)fen.side);
    state.enpassant = fen.enpassant;
    state.castling = fen.castling;
    state.halfMoves = fen.halfMoves;
    state.fullMoves = fen.fullMoves;
}
