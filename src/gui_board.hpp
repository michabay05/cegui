#pragma once

#include "gui_defs.hpp"
#include "board.hpp"

struct GUIBoard {
    Board board;
    Rectangle rect;
    Sq selected;
    uint64_t preview;

    GUIBoard(Board& b, Rectangle r);
    void updateSelection();
};
