#pragma once

#include <vector>
#include <regex>

enum class GameResult {
    BlackWin = -1,
    Draw,
    WhiteWin,
    InProgress,
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

struct PGNInfo
{
    PGNHeader header;
    std::vector<int> moves;

    PGNInfo(const std::string& headerStr, const std::string& movesStr);
};

enum class MoveType {
    Quiet,
    Capture,
    Promotion,
    Castling,
    CapturePromotion,
};

enum class MoveAnnotation {
    None,
    Check,
    Checkmate
};

struct MoveInfo
{
    Piece piece, promoted = Piece::E;
    char disambiguate;
    MoveType type;
    Sq target;
    MoveAnnotation annotation = MoveAnnotation::None;

    MoveInfo(const std::smatch& match, bool isWhiteToMove);
    void printInfo();
  private:
    void parseMoves(const std::smatch& match, bool isWhiteToMove);
};
