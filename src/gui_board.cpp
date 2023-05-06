#include "gui_board.hpp"
#include "defs.hpp"
#include "move.hpp"
#include <raylib.h>

const int PROMOTED_RECT_WIDTH = 300;
const int PROMOTED_RECT_HEIGHT = 100;
const int X_OFFSET = 5;

GUIBoard::GUIBoard(Board& b, Rectangle r) {
    board = b;
    boardRect = r;
    promotedRect = {
        boardRect.x + (boardRect.width / 2) - ((float)PROMOTED_RECT_WIDTH / 2),
        boardRect.y + (boardRect.height / 2) - ((float)PROMOTED_RECT_HEIGHT / 2),
        PROMOTED_RECT_WIDTH + 3 * X_OFFSET,
        PROMOTED_RECT_HEIGHT,
    };
    selected = Sq::noSq;
    target = Sq::noSq;
    promoted = Piece::E;
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
    if (!isInBound(boardRect, mousePos)) {
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
    if (!isInBound(boardRect, mousePos)) {
        target = Sq::noSq;
        return;
    }
    int col = (mousePos.x - PADDING[0]) / SQ_SIZE;
    int row = (mousePos.y - PADDING[1]) / SQ_SIZE;
    Sq tempTarget = (Sq)SQ(row, col);
    if (getBit(preview, (int)tempTarget))
        target = tempTarget;
    if (ROW(target) == 0 || ROW(target) == 7)
        setPromotedPiece();
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
        if (!Move::make(&board, generatedMoves.list[i], Move::MoveType::allMoves))
            continue;
        if (Move::getSource(generatedMoves.list[i]) == (int)selected)
            setBit(preview, Move::getTarget(generatedMoves.list[i]));
        board = clone;
    }
}

bool GUIBoard::makeMove() {
    if (target == Sq::noSq)
        return false;
    int move = generatedMoves.search((int)selected, (int)target, (int)promoted);
    if (move == 0)
        return false;
    selected = Sq::noSq;
    target = Sq::noSq;
    promoted = Piece::E;
    return Move::make(&board, move, Move::MoveType::allMoves);
}

void GUIBoard::setPromotedPiece() {
    if (!(board.state.side == PieceColor::LIGHT && ROW((int)target) == 0) &&
        !(board.state.side == PieceColor::DARK && ROW((int)target) == 7))
        return;

    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        return;
    Vector2 mousePos = GetMousePosition();
    if (!isInBound(promotedRect, mousePos))
        return;

    promoted = (Piece)(((mousePos.x - PADDING[0]) / (PROMOTED_RECT_WIDTH / 4.0f)) - 1);
}
