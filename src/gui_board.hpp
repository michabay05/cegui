#pragma once

#include "gui_defs.hpp"
#include "board.hpp"
#include "move.hpp"

struct GUIBoard {
    Board board;
    Rectangle rect;
    Sq selected, target;
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
};
