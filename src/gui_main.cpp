#include "gui_defs.hpp"
#include "board.hpp"
#include "gui_board.hpp"
#include <raylib.h>

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 600;

const Color BACKGROUND = {24, 24, 24, 255};
const Color LIGHT_COLOR = {118, 150, 86, 255};
const Color DARK_COLOR = {238, 238, 210, 255};

const Color SELECTED_COLOR = {187, 203, 43, 255};

const float padding[2] = {
    0.5f * (SCREEN_WIDTH - (8 * SQ_SIZE)),
    0.5f * (SCREEN_HEIGHT - (8 * SQ_SIZE)),
};

void drawBoard(GUIBoard& gb) {
    for (int r = 0; r < 8; r++) {
        for (int f = 0; f < 8; f++) {
            Color sqColor = !SQCLR(r, f) ? LIGHT_COLOR : DARK_COLOR;
            if (SQ(r, f) == (int)gb.selected)
                sqColor = ColorAlphaBlend(sqColor, SELECTED_COLOR, WHITE);
            DrawRectangleV({f * SQ_SIZE + padding[0], r * SQ_SIZE + padding[1]}, {SQ_SIZE, SQ_SIZE},
                           sqColor);
        }
    }
}

void drawPiece(const Texture& tex, int row, int col, Piece piece) {
    float frameWidth = tex.width / 6.0f;
    float frameHeight = tex.height / 2.0f;
    int frameRow = (int)piece >= 6;
    int frameCol = (int)piece % 6;

    const float SCALE_FACTOR = 0.94;
    float scaledDimension = SCALE_FACTOR * SQ_SIZE;
    Rectangle pieceTexRect = {
        ((float)row * SQ_SIZE + padding[0]) + ((SQ_SIZE - scaledDimension) / 2.0f),
        ((float)col * SQ_SIZE + padding[1]) + ((SQ_SIZE - scaledDimension) / 2.0f), scaledDimension,
        scaledDimension};
    DrawTexturePro(tex, {frameCol * frameWidth, frameRow * frameHeight, frameWidth, frameHeight},
                   pieceTexRect, {0.0f, 0.0f}, 0.0f, WHITE);
}

void update(GUIBoard& gb) {
    gb.updateSelection();
}

int gui_main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "CEGUI");

    Board b("rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2");
    Rectangle r = {
        padding[0],
        padding[1],
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

        drawBoard(gb);
        for (int i = 0; i <= 11; i++)
            drawPiece(tex, i / 8, i % 8, (Piece)i);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
