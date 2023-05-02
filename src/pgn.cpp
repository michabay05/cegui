#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <string_view>

// PGN File format specification
// Source: http://www.saremba.de/chessgml/standards/pgn/pgn-complete.htm#c8.1.1

const std::string filename = "tests/test_pgn_with_comments.txt";
const std::string HEADER_PATTERN = "\\[(\\w+) \"([\\w., -\\/?:]+)\"\\]";
const std::string move_pattern =
    "([NBRQK])?([a-h]|[1-8])?(x)?([a-h][1-8])(=)?([NBRQK])?([+#])?|(O-O|O-O-O)";

enum class GameResult {
    BlackWin = -1,
    Draw,
    WhiteWin,
};

struct PGNHeader
{
    // Required tags
    std::string event;
    std::string site;
    std::string date;
    int round = -1;
    std::string players[2];
    GameResult result;

    PGNHeader(const std::string& header);
    void parseResult(const std::string& result_str);
};

PGNHeader::PGNHeader(const std::string& headerStr) {
    std::regex rgx(HEADER_PATTERN);
    std::smatch match;
    std::string header = headerStr;
    while (std::regex_search(header, match, rgx)) {
        // [ <  KEY   > "<  VALUE >"]
        // [ <match[1]> "<match[2]>"]
        std::string key = match[1];
        std::string value = match[2];

        if (value == "?") {
            header = match.suffix();
            continue;
        }

        if (key == "Event")
            event = value;
        else if (key == "Site")
            site = value;
        else if (key == "Date")
            date = value;
        else if (key == "Round")
            round = std::stoi(value);
        else if (key == "White")
            players[0] = value;
        else if (key == "Black")
            players[1] = value;
        else if (key == "Result")
            parseResult(match[2]);

        header = match.suffix();
    }

    std::cout << " Event: " << event << "\n";
    std::cout << "  Site: " << site << "\n";
    std::cout << "  Date: " << date << "\n";
    std::cout << " Round: " << round << "\n";
    std::cout << " White: " << players[0] << "\n";
    std::cout << " Black: " << players[1] << "\n";
    std::cout << "Result: " << (int)result << "\n";
}

void PGNHeader::parseResult(const std::string& result_str) {
    if (result_str == "1-0")
        result = GameResult::WhiteWin;
    else if (result_str == "1/2-1/2")
        result = GameResult::Draw;
    else if (result_str == "0-1")
        result = GameResult::BlackWin;
}

struct PGNInfo
{
    PGNHeader header;
    std::vector<int> moves;

    PGNInfo(const std::string& headerStr, const std::string& movesStr);
};

PGNInfo::PGNInfo(const std::string& headerStr, const std::string& movesStr)
    : header(PGNHeader(headerStr)), moves({}) {}

int main() {
    std::ifstream file(filename);
    std::string buf;
    std::stringstream headerSS, movesSS;
    bool isMoveSection = false;
    std::regex rgx(move_pattern);
    std::smatch match;

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

void move_section(std::string moves, std::smatch match, std::regex rgx) {
    int count = 0;
    while (std::regex_search(moves, match, rgx)) {
        std::cout << match[0] << "\n";
        for (size_t i = 0; i < match.size(); i++) {
            std::cout << i << ": " << match[i].str() << "\n";
        }
        moves = match.suffix();
        count++;
        std::cin.get();
    }
    std::cout << "Match count: " << count << "\n";
}
