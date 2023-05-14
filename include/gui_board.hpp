#pragma once

#include "board.hpp"
#include "gui_defs.hpp"
#include "move.hpp"
#include <vector>

extern const int PROMOTED_RECT_WIDTH;
extern const int PROMOTED_RECT_HEIGHT;
extern const int X_OFFSET;

bool isInBound(Rectangle bounds, Vector2 pos);

enum class GameState {
    Normal,
    Checkmate,
    Stalemate,
};

struct GUIBoard
{
    Board board;
    Rectangle boardRect, promotedRect;
    Sq selected, target;
    Piece promoted;
    uint64_t preview;
    Move::MoveList generatedMoves;
    GameState gameState;
    std::vector<Piece> capturedPieces;
    std::vector<int> moveList;

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
};
