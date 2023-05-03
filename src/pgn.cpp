#include <fstream>
#include <iostream>
#include <regex>
#include <string>

#include "board.hpp"
#include "pgn.hpp"

// PGN File format specification
// Source: http://www.saremba.de/chessgml/standards/pgn/pgn-complete.htm

const std::string HEADER_PATTERN = "\\[(\\w+) \"([\\w., -\\/?:]+)\"\\]";
const std::string MOVE_PATTERN =
    "([NBRQK])?([a-h]|[1-8])?(x)?([a-h][1-8])(=)?([NBRQ])?([+#])?|(O-O|O-O-O)";

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
            parseResult(value);

        header = match.suffix();
    }
}

void PGNHeader::parseResult(const std::string& result_str) {
    if (result_str == "1-0")
        result = GameResult::WhiteWin;
    else if (result_str == "1/2-1/2")
        result = GameResult::Draw;
    else if (result_str == "0-1")
        result = GameResult::BlackWin;
    else if (result_str == "*")
        result = GameResult::InProgress;
}

PGNInfo::PGNInfo(const std::string& headerStr, const std::string& movesStr)
    : header(PGNHeader(headerStr)), moves({}) {
    std::regex rgx(MOVE_PATTERN);
    std::smatch match;
    std::string moves = movesStr;
    bool isWhiteToMove = true;
    int count = 0;
    while (std::regex_search(moves, match, rgx)) {
        count++;
        std::cout << count << ". " << match[0]
                  << "\n================================================\n";
        MoveInfo move(match, isWhiteToMove);
        move.printInfo();

        moves = match.suffix();
        isWhiteToMove = !isWhiteToMove;
    }
}

MoveInfo::MoveInfo(const std::smatch& match, bool isWhiteToMove) {
    parseMoves(match, isWhiteToMove);
}

void MoveInfo::parseMoves(const std::smatch& match, bool isWhiteToMove) {
    if (!match[8].str().empty()) {
        piece = Piece::K;
        promoted = Piece::E;
        disambiguate = 0;
        type = MoveType::Castling;
        target = Sq::g1;
        return;
    }

    PieceTypes pieceType = PieceTypes::PAWN;
    // Piece
    switch (match[1].str()[0]) {
    case 'N':
        pieceType = PieceTypes::KNIGHT;
        break;
    case 'B':
        pieceType = PieceTypes::BISHOP;
        break;
    case 'R':
        pieceType = PieceTypes::ROOK;
        break;
    case 'Q':
        pieceType = PieceTypes::QUEEN;
        break;
    case 'K':
        pieceType = PieceTypes::KING;
        break;
    }
    piece = (Piece)(isWhiteToMove ? (int)pieceType : (int)pieceType + 6);
    disambiguate = match[2].str()[0];

    for (int i = 0; i < 64; i++) {
        if (match[4] == strCoords[i])
            target = (Sq)i;
    }

    if (!match[3].str().empty() && !match[5].str().empty())
        type = MoveType::CapturePromotion;
    else if (!match[3].str().empty())
        type = MoveType::Capture;
    else if (!match[5].str().empty())
        type = MoveType::Promotion;
    else
        type = MoveType::Quiet;

    if (type == MoveType::Promotion || type == MoveType::CapturePromotion) {
        size_t ind = pieceStr.find(match[6]);
        if (ind != std::string::npos)
            promoted = (Piece)ind;
    }

    if (match[7] == "#")
        annotation = MoveAnnotation::Checkmate;
    else if (match[7] == "+")
        annotation = MoveAnnotation::Check;
}

void MoveInfo::printInfo() {
    std::cout << "        Piece: " << pieceStr[(int)piece] << "\n";
    std::cout << "     Promoted: " << pieceStr[(int)promoted] << "\n";
    std::cout << "  Annotations: " << (int)annotation << "\n";
    std::cout << "         Type: " << (int)type << "\n";
    std::cout << "       Target: " << strCoords[(int)target] << "\n";
    std::cout << " Disambiguate: " << disambiguate << "\n\n";
}
