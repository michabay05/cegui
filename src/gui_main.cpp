#include "board.hpp"
#include "defs.hpp"
#include "gui_board.hpp"
#include "gui_defs.hpp"
#include "raylib.h"
#include <algorithm>
#include <cmath>
#include <cstdio>

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 600;

const Color BACKGROUND = {24, 24, 24, 255};
const Color LIGHT_COLOR = {118, 150, 86, 255};
const Color DARK_COLOR = {238, 238, 210, 255};

const Color SELECTED_COLOR = {187, 203, 43, 255};
const Color PREVIEW_COLOR = {54, 129, 227, 200};
const Color IN_CHECK_COLOR = {179, 32, 42, 210};

const float PADDING[2] = {
    0.15f * (SCREEN_WIDTH - (8 * SQ_SIZE)),
    0.5f * (SCREEN_HEIGHT - (8 * SQ_SIZE)),
};

const int SQ_SIZE = 70;

void drawBoard(const GUIBoard& gb) {
    Sq kingSq = Sq::noSq;
    if (gb.board.isOppInCheck()) {
        bool isWhiteToMove = gb.board.state.side == PieceColor::LIGHT;
        for (int i = 0; i < 64; i++) {
            if (getBit(gb.board.pos.pieces[isWhiteToMove ? 5 : 11], i))
                kingSq = (Sq)i;
        }
    }
    for (int r = 0; r < 8; r++) {
        for (int f = 0; f < 8; f++) {
            Color sqColor = !SQCLR(r, f) ? LIGHT_COLOR : DARK_COLOR;
            if (SQ(r, f) == (int)gb.selected)
                sqColor = ColorAlphaBlend(sqColor, SELECTED_COLOR, WHITE);
            else if (getBit(gb.preview, SQ(r, f)))
                sqColor = ColorAlphaBlend(sqColor, PREVIEW_COLOR, WHITE);
            else if (kingSq == (Sq)SQ(r, f))
                sqColor = ColorAlphaBlend(sqColor, IN_CHECK_COLOR, WHITE);
            DrawRectangleV({f * SQ_SIZE + PADDING[0], r * SQ_SIZE + PADDING[1]}, {SQ_SIZE, SQ_SIZE},
                           sqColor);
        }
    }
}

void drawPiece(const Texture& tex, Rectangle rect, Piece piece) {
    float frameWidth = tex.width / 6.0f;
    float frameHeight = tex.height / 2.0f;
    int frameRow = (int)piece >= 6;
    int frameCol = (int)piece % 6;

    DrawTexturePro(tex, {frameCol * frameWidth, frameRow * frameHeight, frameWidth, frameHeight},
                   rect, {0.0f, 0.0f}, 0.0f, WHITE);
}

void drawSideToMove(const GUIBoard& gb) {
    Color color = WHITE;
    if (gb.board.state.side == PieceColor::DARK)
        color = {69, 66, 66, 255};
    Vector2 center = {0.7 * SCREEN_WIDTH, 70};
    DrawCircleV(center, 25.0f, LIGHT_COLOR);
    DrawCircleV(center, 20.0f, color);
}

void drawPieceAt(const Texture& tex, int row, int col, Piece piece) {
    float scaledDimension = 1.00 * SQ_SIZE;
    Rectangle pieceTexRect = {
        ((float)col * SQ_SIZE + PADDING[0]) + ((SQ_SIZE - scaledDimension) / 2.0f),
        ((float)row * SQ_SIZE + PADDING[1]) + ((SQ_SIZE - scaledDimension) / 2.0f), scaledDimension,
        scaledDimension};
    drawPiece(tex, pieceTexRect, piece);
}

void drawCapturedPieces(const GUIBoard& gb, const Texture& tex) {
    float xOffset = 10;
    Vector2 topLeft = {
        gb.boardRect.x + gb.boardRect.width + xOffset,
        gb.boardRect.y + 40,
    };
    float lastwpos = topLeft.x;
    float lastbpos = topLeft.x;
    for (size_t i = 0; i < gb.capturedPieces.size(); i++) {
        int color = (int)gb.capturedPieces[i] < 6 ? 0 : 1;
        float scaledSize = 0.75 * SQ_SIZE;
        float xpos;
        if (color == 0) {
            lastwpos += scaledSize;
            xpos = lastwpos;
        } else {
            lastbpos += scaledSize;
            xpos = lastbpos;
        }
        Rectangle rect = {xpos, topLeft.y + (color * scaledSize), scaledSize, scaledSize};
        drawPiece(tex, rect, gb.capturedPieces[i]);
    }
}

void drawPromotedChoices(const GUIBoard& gb, const Texture& tex) {
    const Color PROMOTION_BKGD = {24, 24, 24, 170};
    DrawRectangleRounded(gb.promotedRect, 0.5, 2, PROMOTION_BKGD);
    float dimension = PROMOTED_RECT_WIDTH / 4.0f;
    for (int i = (int)PieceTypes::KNIGHT; i <= (int)PieceTypes::QUEEN; i++) {
        Rectangle rect = {
            gb.promotedRect.x + ((i - 1) * dimension) + X_OFFSET,
            gb.promotedRect.y + (PROMOTED_RECT_HEIGHT / 2.0f) - (dimension / 2.0f),
            dimension,
            dimension,
        };
        drawPiece(tex, rect, (Piece)(gb.board.state.side == PieceColor::LIGHT ? i : i + 6));
    }
}

const int EVAL_FONT_SIZE = 15;

float findFactor(const float currEval) { return std::pow(currEval, 1.0f / 3.0f); }

void drawEvalBar(const GUIBoard& gb, const Font& font) {
    float currEval = 0.0f;
    Vector2 evalBarDim = {35.0f, gb.boardRect.height};
    float whiteHeight = evalBarDim.y / 2.0f;
    float blackHeight = evalBarDim.y / 2.0f;
    Rectangle wr = {(gb.boardRect.x / 2.0f) - (evalBarDim.x / 2.0f),
                    ((SCREEN_HEIGHT - evalBarDim.y) / 2.0f) + blackHeight, evalBarDim.x,
                    whiteHeight};
    Rectangle br = {(gb.boardRect.x / 2.0f) - (evalBarDim.x / 2.0f),
                    (SCREEN_HEIGHT - evalBarDim.y) / 2.0f, evalBarDim.x, blackHeight};
    const Color WHITE_EVAL_COLOR = WHITE;
    const Color BLACK_EVAL_COLOR = {40, 40, 40, 255};
    DrawRectangleRec(wr, WHITE_EVAL_COLOR);
    DrawRectangleRec(br, BLACK_EVAL_COLOR);

    std::string str;
    char temp[10] = {0};
    switch (gb.gameState) {
    case GameState::Normal:
        sprintf(temp, "%.1f", currEval);
        str = temp;
        break;
    case GameState::Checkmate:
        if (gb.board.state.xside == PieceColor::LIGHT)
            str = "1-0";
        else
            str = "0-1";
        break;
    case GameState::Stalemate:
        str = "1/2";
        break;
    }
    Vector2 t = MeasureTextEx(font, str.c_str(), EVAL_FONT_SIZE, 0);
    Vector2 textPos = {wr.x + (wr.width / 2.0f) - (t.x / 2), 550};
    DrawTextEx(font, str.c_str(), textPos, EVAL_FONT_SIZE, 0, BLACK_EVAL_COLOR);
}

const float MOVE_TEXT_FONT_SIZE = 25;
bool isSliderHeld = false;

void isSlidingBarHeld(Rectangle sliderRect) {
    if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        isSliderHeld = false;
        return;
    }
    if (isInBound(sliderRect, GetMousePosition()))
        isSliderHeld = true;
}

Rectangle moveListRect = {0.65f * SCREEN_WIDTH, 0.05f * SCREEN_HEIGHT, 320, 300};
float scrollBarWidth = 15;
const Rectangle scrollBarRect = {(moveListRect.x + moveListRect.width) - scrollBarWidth,
                                 moveListRect.y, scrollBarWidth, moveListRect.height};
Rectangle scrollSliderRect = {scrollBarRect.x, scrollBarRect.y, scrollBarRect.width,
                              0.15f * scrollBarRect.height};

void drawMoveList(const GUIBoard& gb, const Font& font) {
    (void)gb;
    DrawRectangleRec(moveListRect, DARKGRAY);
    size_t i = 0;
    const Vector2 offset = {35, 25};
    Vector2 pos = {0, moveListRect.y + offset.y};
    Vector2 textDim{};
    while (i < gb.moveList.size() && pos.y < moveListRect.y + moveListRect.height) {
        std::string moveText = Move::toString(gb.moveList[i]);
        if (i % 2 == 0) {
            pos.x = moveListRect.x + 35;
            char temp[10] = {0};
            sprintf(temp, "%ld", (i / 2) + 1);
            DrawTextEx(font, temp, {moveListRect.x + 10, pos.y}, MOVE_TEXT_FONT_SIZE, 0,
                       {255, 255, 255, 190});
        } else {
            pos.x = moveListRect.x + (moveListRect.width / 2) + 15;
        }

        textDim = MeasureTextEx(font, moveText.c_str(), MOVE_TEXT_FONT_SIZE, 0);
        DrawTextEx(font, moveText.c_str(), pos, MOVE_TEXT_FONT_SIZE, 0, RAYWHITE);
        i++;
        if (i % 2 == 0)
            pos.y += textDim.y + offset.y;
    }
    DrawRectangleRec(scrollBarRect, DARK_COLOR);
    DrawRectangleRounded(scrollSliderRect, 0.8, 5, BEIGE);
}

void update(GUIBoard& gb) {
    gb.setSelection();
    gb.setMovePreviews();
    gb.setTarget();
    gb.makeMove();
    gb.updateGameState();
    isSlidingBarHeld(scrollSliderRect);
    if (isSliderHeld) {
        scrollSliderRect.y =
            std::clamp(GetMouseY(), (int)scrollBarRect.y,
                       (int)(scrollBarRect.y + scrollBarRect.height - scrollSliderRect.height));
    }
}

void render(const GUIBoard& gb, const Texture& tex, const Font& evalFont, const Font& moveFont) {
    drawBoard(gb);
    for (int r = 0; r < 8; r++) {
        for (int f = 0; f < 8; f++) {
            Piece p = (Piece)gb.board.pos.getPieceOnSquare(SQ(r, f));
            if (p == Piece::E)
                continue;
            drawPieceAt(tex, r, f, p);
        }
    }
    if ((ROW(gb.target) == 0 || ROW(gb.target) == 7) &&
        COLORLESS(gb.board.pos.getPieceOnSquare((int)gb.target)) == (int)PieceTypes::PAWN)
        drawPromotedChoices(gb, tex);

    drawEvalBar(gb, evalFont);
    // drawCapturedPieces(gb, tex);
    drawMoveList(gb, moveFont);
}

int gui_main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "CEGUI");

    Board b(Board::position[1]);
    Rectangle r = {
        PADDING[0],
        PADDING[1],
        8 * SQ_SIZE,
        8 * SQ_SIZE,
    };
    GUIBoard gb(b, r);

    Texture tex = LoadTexture("assets/pieceSpriteSheet.png");
    SetTextureFilter(tex, TEXTURE_FILTER_BILINEAR);

    Font evalFont = LoadFontEx("assets/fonts/Inter-Bold.ttf", EVAL_FONT_SIZE, 0, 0);
    SetTextureFilter(evalFont.texture, TEXTURE_FILTER_POINT);

    Font moveTextFont = LoadFontEx("assets/fonts/Inter-Medium.ttf", MOVE_TEXT_FONT_SIZE, 0, 0);
    SetTextureFilter(moveTextFont.texture, TEXTURE_FILTER_POINT);

    while (!WindowShouldClose()) {
        update(gb);
        BeginDrawing();
        ClearBackground({24, 24, 24, 255});

        render(gb, tex, evalFont, moveTextFont);

        EndDrawing();
    }

    UnloadFont(evalFont);
    UnloadFont(moveTextFont);
    UnloadTexture(tex);

    CloseWindow();
    return 0;
}
