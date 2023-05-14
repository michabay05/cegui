#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "attack.hpp"
#include "gui_defs.hpp"


#include "uci.hpp"
#include "fen.hpp"
#include "board.hpp"
void test() {
    uciTest();
}

enum class Mode { GUI, Terminal, Debug };

Mode parseCmdArgs(int argc, char** argv) {
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

void init() { Attack::init(); }

int main(int argc, char** argv) {
    init();
    switch (parseCmdArgs(argc, argv)) {
    case Mode::GUI:
        return gui_main();
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
