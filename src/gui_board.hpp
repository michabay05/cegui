#pragma once

#include "gui_defs.hpp"
#include "board.hpp"
#include "move.hpp"

extern const int PROMOTED_RECT_WIDTH;
extern const int PROMOTED_RECT_HEIGHT;
extern const int X_OFFSET;

struct GUIBoard {
    Board board;
    Rectangle boardRect, promotedRect;
    Sq selected, target;
    Piece promoted;
    uint64_t preview;
    Move::MoveList generatedMoves;
    bool isInCheck;

    GUIBoard(Board& b, Rectangle r);
    void setSelection();
    void setTarget();
    void genMoves();
    void setMovePreviews();
    void isCheck();
    bool makeMove(); 
    void setPromotedPiece();
};
