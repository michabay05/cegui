#include "include/vendor/raygui.h"
#include "include/vendor/raylib.h"

#define FONT_SIZE 20

void test(const Font& f) {
    Rectangle r = {150, 150, 150, 150};
    Rectangle content = { 100, 100, 25, 25 };
    Vector2 scroll{};
    GuiSetFont(f);
}

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(1000, 600, "Raygui test");

    Font f = LoadFontEx("assets/fonts/Inter-Regular.ttf", FONT_SIZE, 0, 0);
    SetTextureFilter(f.texture, TEXTURE_FILTER_POINT);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground({30, 30, 30, 255});
        test(f);
        EndDrawing();
    }
    return 0;
}
