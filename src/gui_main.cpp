#include "board.hpp"
#include "gui_board.hpp"
#include "gui_defs.hpp"
#include <raylib.h>

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 600;

const Color BACKGROUND = {24, 24, 24, 255};
const Color LIGHT_COLOR = {118, 150, 86, 255};
const Color DARK_COLOR = {238, 238, 210, 255};

const Color SELECTED_COLOR = {187, 203, 43, 255};
const Color PREVIEW_COLOR = {54, 129, 227, 200};

const float PADDING[2] = {
    0.5f * (SCREEN_WIDTH - (8 * SQ_SIZE)),
    0.5f * (SCREEN_HEIGHT - (8 * SQ_SIZE)),
};

const int SQ_SIZE = 70;

void drawBoard(const GUIBoard& gb) {
    for (int r = 0; r < 8; r++) {
        for (int f = 0; f < 8; f++) {
            Color sqColor = !SQCLR(r, f) ? LIGHT_COLOR : DARK_COLOR;
            if (SQ(r, f) == (int)gb.selected)
                sqColor = ColorAlphaBlend(sqColor, SELECTED_COLOR, WHITE);
            else if (getBit(gb.preview, SQ(r, f)))
                sqColor = ColorAlphaBlend(sqColor, PREVIEW_COLOR, WHITE);
            DrawRectangleV({f * SQ_SIZE + PADDING[0], r * SQ_SIZE + PADDING[1]}, {SQ_SIZE, SQ_SIZE},
                           sqColor);
        }
    }
}

void drawPiece(const Texture& tex, int row, int col, Piece piece) {
    float frameWidth = tex.width / 6.0f;
    float frameHeight = tex.height / 2.0f;
    int frameRow = (int)piece >= 6;
    int frameCol = (int)piece % 6;

    const float SCALE_FACTOR = 0.97;
    float scaledDimension = SCALE_FACTOR * SQ_SIZE;
    Rectangle pieceTexRect = {
        ((float)col * SQ_SIZE + PADDING[0]) + ((SQ_SIZE - scaledDimension) / 2.0f),
        ((float)row * SQ_SIZE + PADDING[1]) + ((SQ_SIZE - scaledDimension) / 2.0f), scaledDimension,
        scaledDimension};
    DrawTexturePro(tex, {frameCol * frameWidth, frameRow * frameHeight, frameWidth, frameHeight},
                   pieceTexRect, {0.0f, 0.0f}, 0.0f, WHITE);
}

#include "iostream"
void update(GUIBoard& gb) {
    gb.setSelection();
    gb.setMovePreviews();
    gb.setTarget();
    gb.makeMove();
}

void render(const GUIBoard& gb, const Texture& tex) {
    drawBoard(gb);
    for (int r = 0; r < 8; r++) {
        for (int f = 0; f < 8; f++) {
            Piece p = (Piece)gb.board.pos.getPieceOnSquare(SQ(r, f));
            if (p == Piece::E)
                continue;
            drawPiece(tex, r, f, p);
        }
    }
}

int gui_main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "CEGUI");

    Board b("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");
    Rectangle r = {
        PADDING[0],
        PADDING[1],
        8 * SQ_SIZE,
        8 * SQ_SIZE,
    };
    GUIBoard gb(b, r);

    Texture tex = LoadTexture("assets/neo-pieces-spritesheet.png");
    SetTextureFilter(tex, TEXTURE_FILTER_BILINEAR);

    while (!WindowShouldClose()) {
        update(gb);
        BeginDrawing();
        ClearBackground({24, 24, 24, 255});

        render(gb, tex);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
