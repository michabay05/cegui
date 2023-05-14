#include "gui_board.hpp"
#include "defs.hpp"
#include "move.hpp"

const int PROMOTED_RECT_WIDTH = 300;
const int PROMOTED_RECT_HEIGHT = 100;
const int X_OFFSET = 5;

GUIBoard::GUIBoard(Board& b, Rectangle r)
    : selected(Sq::noSq), target(Sq::noSq), promoted(Piece::E), preview(0ULL),
      gameState(GameState::Normal), capturedPieces{} {
    board = b;
    boardRect = r;
    promotedRect = {
        boardRect.x + (boardRect.width / 2) - ((float)PROMOTED_RECT_WIDTH / 2),
        boardRect.y + (boardRect.height / 2) - ((float)PROMOTED_RECT_HEIGHT / 2),
        PROMOTED_RECT_WIDTH + 3 * X_OFFSET,
        PROMOTED_RECT_HEIGHT,
    };
}

bool isInBound(Rectangle bounds, Vector2 pos) {
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
    if (COLORLESS(board.pos.getPieceOnSquare((int)selected)) == (int)PieceTypes::PAWN &&
        (ROW(target) == 0 || ROW(target) == 7))
        setPromotedPiece();
}

void GUIBoard::setPromotedPiece() {
    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        return;
    Vector2 mousePos = GetMousePosition();
    if (!isInBound(promotedRect, mousePos))
        return;

    promoted = (Piece)(((mousePos.x - PADDING[0]) / (PROMOTED_RECT_WIDTH / 4.0f)) - 1);
    if (board.state.side == PieceColor::DARK)
        promoted = (Piece)((int)promoted + 6);
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

bool GUIBoard::isMoveLegal(int move) {
    Board clone = board;
    return Move::make(&clone, move, Move::MoveType::allMoves);
}

void GUIBoard::setMovePreviews() {
    genMoves();

    // Reset the preview bitboard
    preview = 0ULL;
    for (int i = 0; i < generatedMoves.count; i++) {
        if (!isMoveLegal(generatedMoves.list[i]))
            continue;
        if (Move::getSource(generatedMoves.list[i]) == (int)selected)
            setBit(preview, Move::getTarget(generatedMoves.list[i]));
    }
}

#include <iostream>
bool GUIBoard::makeMove() {
    if (target == Sq::noSq)
        return false;
    int move = generatedMoves.search((int)selected, (int)target, (int)promoted);
    if (move == 0)
        return false;
    if (Move::isCapture(move))
        capturedPieces.push_back((Piece)board.pos.getPieceOnSquare((int)target));
    moveList.push_back(move);
    selected = Sq::noSq;
    target = Sq::noSq;
    promoted = Piece::E;
    return Move::make(&board, move, Move::MoveType::allMoves);
}

bool GUIBoard::areNoLegalMoves() {
    Move::MoveList ml;
    Move::generate(ml, board);
    for (int i = 0; i < ml.count; i++) {
        if (isMoveLegal(ml.list[i]))
            return false;
    }
    return true;
}

void GUIBoard::updateGameState() {
    if (areNoLegalMoves()) {
        if (board.isOppInCheck())
            gameState = GameState::Checkmate;
        else
            gameState = GameState::Stalemate;
        return;
    }
    gameState = GameState::Normal;
}
