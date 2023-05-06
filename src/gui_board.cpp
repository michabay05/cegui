#include "gui_board.hpp"
#include "defs.hpp"
#include "move.hpp"

GUIBoard::GUIBoard(Board& b, Rectangle r) {
    board = b;
    rect = r;
    selected = Sq::noSq;
    target = Sq::noSq;
    preview = 0ULL;
    isInCheck = false;
}

static bool isInBound(Rectangle bounds, Vector2 pos) {
    return (pos.x >= bounds.x && pos.x <= bounds.x + bounds.width) &&
           (pos.y >= bounds.y && pos.y <= bounds.y + bounds.height);
}

void GUIBoard::setSelection() {
    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        return;
    Vector2 mousePos = GetMousePosition();
    if (!isInBound(rect, mousePos)) {
        selected = Sq::noSq;
        return;
    }
    int col = (mousePos.x - PADDING[0]) / SQ_SIZE;
    int row = (mousePos.y - PADDING[1]) / SQ_SIZE;
    Sq tempSelected = (Sq)SQ(row, col);
    Piece p = (Piece)board.pos.getPieceOnSquare((int)tempSelected);
    PieceColor pc = (int)p < 6 ? PieceColor::LIGHT : PieceColor::DARK;
    if (p == Piece::E || pc != board.state.side) {
        return;
    }
    if (tempSelected == selected) {
        selected = Sq::noSq;
        return;
    }
    selected = tempSelected;
}

void GUIBoard::setTarget() {
    if (selected == Sq::noSq) {
        target = Sq::noSq;
        return;
    }
    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        return;
    Vector2 mousePos = GetMousePosition();
    if (!isInBound(rect, mousePos)) {
        target = Sq::noSq;
        return;
    }
    int col = (mousePos.x - PADDING[0]) / SQ_SIZE;
    int row = (mousePos.y - PADDING[1]) / SQ_SIZE;
    Sq tempTarget = (Sq)SQ(row, col);
    if (getBit(preview, (int)tempTarget))
        target = tempTarget;
}

void GUIBoard::genMoves() {
    generatedMoves = Move::MoveList();
    Piece p = (Piece)board.pos.getPieceOnSquare((int)selected);
    if (board.isInCheck()) {
        Move::generate(generatedMoves, board);
        return;
    }
    switch ((PieceTypes)COLORLESS(p)) {
    case PieceTypes::PAWN:
        Move::generatePawns(generatedMoves, board);
        break;
    case PieceTypes::KNIGHT:
        Move::generateKnights(generatedMoves, board);
        break;
    case PieceTypes::BISHOP:
        Move::generateBishops(generatedMoves, board);
        break;
    case PieceTypes::ROOK:
        Move::generateRooks(generatedMoves, board);
        break;
    case PieceTypes::QUEEN:
        Move::generateQueens(generatedMoves, board);
        break;
    case PieceTypes::KING:
        Move::generateKings(generatedMoves, board);
        break;
    }
}

void GUIBoard::setMovePreviews() {
    genMoves();

    // Reset the preview bitboard
    preview = 0ULL;
    Board clone = board;
    for (int i = 0; i < generatedMoves.count; i++) {
        if (!Move::make(&board, generatedMoves.list[i], Move::MoveType::allMoves)) continue;
        if (Move::getSource(generatedMoves.list[i]) == (int)selected)
            setBit(preview, Move::getTarget(generatedMoves.list[i]));
        board = clone;
    }
}

bool GUIBoard::makeMove() {
    if (target == Sq::noSq)
        return false;
    int move = generatedMoves.search((int)selected, (int)target);
    selected = Sq::noSq;
    target = Sq::noSq;
    return Move::make(&board, move, Move::MoveType::allMoves);
}
