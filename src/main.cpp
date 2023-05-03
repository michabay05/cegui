#include "attack.hpp"
#include "bitboard.hpp"
#include "board.hpp"
#include "magics.hpp"
#include "move.hpp"
#include "perft.hpp"
#include "pgn.hpp"

#include <fstream>
#include <iostream>
#include <string>

const std::string filename = "tests/test_random_pgn.txt";

void test() {
    std::ifstream file(filename);
    std::string buf;
    std::stringstream headerSS, movesSS;
    bool isMoveSection = false;

    while (std::getline(file, buf)) {
        if (buf.empty()) {
            isMoveSection = true;
            continue;
        }
        if (!isMoveSection)
            headerSS << buf << "\n";
        else
            movesSS << buf << "\n";
    }

    std::string header = headerSS.str();
    std::string moves = movesSS.str();

    PGNInfo pgn = PGNInfo(header, moves);
    file.close();
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
