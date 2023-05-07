#pragma once

#include "gui_defs.hpp"
#include "board.hpp"
#include "move.hpp"

extern const int PROMOTED_RECT_WIDTH;
extern const int PROMOTED_RECT_HEIGHT;
extern const int X_OFFSET;

enum class GameState {
    Normal,
    Checkmate,
    Stalemate,
};

struct GUIBoard {
    Board board;
    Rectangle boardRect, promotedRect;
    Sq selected, target;
    Piece promoted;
    uint64_t preview;
    Move::MoveList generatedMoves;
    GameState gameState;

    GUIBoard(Board& b, Rectangle r);
    void setSelection();
    void setTarget();
    void setMovePreviews();
    bool makeMove(); 
    void setPromotedPiece();
    void updateGameState();
private:
    void genMoves();
    bool areNoLegalMoves();
    bool isMoveLegal(int move);
    bool areAllMovesIllegal(Move::MoveList ml);
};
