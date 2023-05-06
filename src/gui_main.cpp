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
    0.15f * (SCREEN_WIDTH - (8 * SQ_SIZE)),
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
    Vector2 center = {0.9 * SCREEN_WIDTH, 70};
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

void update(GUIBoard& gb) {
    gb.setSelection();
    gb.setMovePreviews();
    gb.setTarget();
    gb.makeMove();
}

void drawPromotedChoices(const GUIBoard& gb, const Texture& tex) {
    const Color PROMOTION_BKGD = {24, 24, 24, 170};
    DrawRectangleRounded(gb.promotedRect, 0.5, 2, PROMOTION_BKGD);
    float dimension = PROMOTED_RECT_WIDTH / 4.0f;
    for (int i = (int)PieceTypes::KNIGHT; i <= (int)PieceTypes::QUEEN; i++) {
        Rectangle rect = {
            gb.promotedRect.x + ((i - 1) * dimension) + X_OFFSET,
            gb.promotedRect.y + (PROMOTED_RECT_HEIGHT / 2.0f) - (dimension / 2.0f), dimension,
            dimension,
            // PROMOTED_RECT_HEIGHT,
        };
        drawPiece(tex, rect, (Piece)(gb.board.state.side == PieceColor::LIGHT ? i : i + 6));
    }
}

const int EVAL_FONT_SIZE = 15;

void drawEvalBar(const GUIBoard& gb, const Font& font) {
    Vector2 evalBarDim = {40.0f, gb.boardRect.height};
    Rectangle r = {
        (gb.boardRect.x / 2.0f) - (evalBarDim.x / 2.0f),
        (SCREEN_HEIGHT - evalBarDim.y) / 2.0f,
        evalBarDim.x,
        evalBarDim.y,
    };
    DrawRectangleRounded(r, 0.15, 5, DARKGREEN);
    std::string str = "20.7";
    Vector2 t = MeasureTextEx(font, str.c_str(), EVAL_FONT_SIZE, 0);
    DrawTextEx(font, str.c_str(), {r.x + (r.width / 2.0f) - (t.x / 2), 550}, EVAL_FONT_SIZE, 1, WHITE);
}

void render(const GUIBoard& gb, const Texture& tex, const Font& font) {
    drawBoard(gb);
    for (int r = 0; r < 8; r++) {
        for (int f = 0; f < 8; f++) {
            Piece p = (Piece)gb.board.pos.getPieceOnSquare(SQ(r, f));
            if (p == Piece::E)
                continue;
            drawPieceAt(tex, r, f, p);
        }
    }
    drawSideToMove(gb);
    if ((ROW(gb.target) == 0 || ROW(gb.target) == 7) &&
        gb.board.pos.getPieceOnSquare((int)gb.target))
        drawPromotedChoices(gb, tex);

    drawEvalBar(gb, font);
}


int gui_main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "CEGUI");

    Board b("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
    Rectangle r = {
        PADDING[0],
        PADDING[1],
        8 * SQ_SIZE,
        8 * SQ_SIZE,
    };
    GUIBoard gb(b, r);

    Texture tex = LoadTexture("assets/pieceSpriteSheet.png");
    SetTextureFilter(tex, TEXTURE_FILTER_BILINEAR);

    Font font = LoadFontEx("assets/fonts/Inter-Medium.ttf", EVAL_FONT_SIZE, 0, 0);
    SetTextureFilter(font.texture, TEXTURE_FILTER_POINT);

    while (!WindowShouldClose()) {
        update(gb);
        BeginDrawing();
        ClearBackground({24, 24, 24, 255});

        render(gb, tex, font);

        EndDrawing();
    }

    UnloadFont(font);
    UnloadTexture(tex);
    CloseWindow();
    return 0;
}
