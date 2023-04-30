#include "attack.hpp"
#include "bitboard.hpp"
#include "board.hpp"
#include "magics.hpp"
#include "move.hpp"
#include "perft.hpp"

void test() {
    const std::string fen = Board::position[7];
    Board b(fen);
    b.display();
    std::cout << "FEN: " << fen << "\n";
    for (int i = 1; i <= 4; i++)
        Perft::test(b, i);
}

enum class Mode { GUI, Terminal, Debug };

Mode parseCmdArgs(int argc, char **argv) {
    Mode mode = Mode::Debug;
    if (argc != 2) {
        return mode;
    }
    std::string mode_str = argv[1];
    if (mode_str == "gui")
        mode = Mode::GUI;
    else if (mode_str == "term")
        mode = Mode::Terminal;
    else if (mode_str == "debug")
        mode = Mode::Debug;
    return mode;
}

void init() {
    Attack::init();
}

int main(int argc, char **argv) {
    init();
    switch (parseCmdArgs(argc, argv)) {
        case Mode::GUI:
            std::cout << "*************** TODO: GUI mode hasn't yet been implemented\n";
            return 1;
        break;
        case Mode::Terminal:
            std::cout << "*************** TODO: Terminal mode hasn't yet been implemented\n";
            return 1;
        break;
        case Mode::Debug:
            test();
        break;
    }
}
