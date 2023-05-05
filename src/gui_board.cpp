#include "gui_board.hpp"

GUIBoard::GUIBoard(Board& b, Rectangle r) {
    board = b;
    rect = r;
    selected = Sq::noSq;
    preview = 0ULL;
}

static bool isInBound(Rectangle bounds, Vector2 pos) {
    return (pos.x >= bounds.x && pos.x <= bounds.x + bounds.width) &&
           (pos.y >= bounds.y && pos.y <= bounds.y + bounds.height);
}

void GUIBoard::updateSelection() {
    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        return;
    Vector2 mousePos = GetMousePosition();
    if (!isInBound(rect, mousePos)) {
        selected = Sq::noSq;
        return;
    }
    int col = (mousePos.x - padding[0]) / SQ_SIZE;
    int row = (mousePos.y - padding[1]) / SQ_SIZE;
    Sq tempSelected = (Sq)SQ(row, col);
    if (tempSelected == selected) {
        selected = Sq::noSq;
        return;
    }
    selected = tempSelected;
}
