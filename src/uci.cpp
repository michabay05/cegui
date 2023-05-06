#include <fstream>
#include <iostream>
#include <regex>

const std::string filename = "tests/test_uci.txt";
const std::string pattern = "(\\w+)";

#include "board.hpp"
void parsePositionCmd(const std::string& positionCmd) {
    std::string posCmd = positionCmd;
    size_t spaceInd = posCmd.find(' ');
    std::string posType = posCmd.substr(0, spaceInd);
}

void parseCommand(const std::string& cmdStr) {
    std::regex regex(pattern);
    std::smatch match;
    std::string cmd = cmdStr;

    while (std::regex_search(cmd, match, regex)) {
        if (match[1] == "position")
            parsePositionCmd(cmd.substr(cmd.find(match[1]) + match[1].str().size() + 1));
        /* else if (match[1] == "ucinewgame")
            parsePositionCmd("startpos"); */
        cmd = match.suffix();
    }
}

void uciTest() {
    std::ifstream file(filename);
    std::string buf;

    while (std::getline(file, buf)) {
        if (buf[0] == '#') {
            std::cout << "=================" << buf.substr(1) << " SECTION "
                      << "=================\n";
            continue;
        }
        parseCommand(buf);
    }
}
