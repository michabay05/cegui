#include "fen.hpp"
#include "board.hpp"

FENInfo::FENInfo(const std::string& fen) {
    for (int r = 0; r < 8; r++) {
        for (int f = 0; f < 8; f++) {
            board[SQ(r, f)] = Piece::E;
        }
    }
    parse(fen);
}

void FENInfo::parse(const std::string& fen) {
    int currIndex = 0;

    // Parse the piece portion of the fen and place them on the board
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            if (fen[currIndex] == ' ')
                break;
            if (fen[currIndex] == '/')
                currIndex++;
            if (fen[currIndex] >= '0' && fen[currIndex] <= '8') {
                file += (fen[currIndex] - '0');
                currIndex++;
            }
            if ((fen[currIndex] >= 'A' && fen[currIndex] <= 'Z') ||
                (fen[currIndex] >= 'a' && fen[currIndex] <= 'z')) {
                size_t piece;
                if ((piece = pieceStr.find(fen[currIndex])) != (int)Piece::E) {
                    board[SQ(rank, file)] = (Piece)piece;
                    // setBit(board.pos.pieces[piece], SQ(rank, file));
                }
                currIndex++;
            }
        }
    }
    currIndex++;
    // Parse side to move
    if (fen[currIndex] == 'w')
        side = PieceColor::LIGHT;
    else if (fen[currIndex] == 'b')
        side = PieceColor::DARK;
    currIndex += 2;

    // Check if king is in check in the initial position
    // If the king is in check, the king has to have a chance to escape it.

    /* if (board.isInCheck()) {
        std::cout << "Failed to initialized board!\n";
        std::cout << "     [Problem] - Initial position can't have a king being in "
                     "check and it's "
                     "not the checked side to move.\n";
        std::cout << "  [Solution 1] - Modify the position to remove the checked "
                     "king from check.\n";
        std::cout << "  [Solution 2] - Change side to move to the side of the "
                     "checked king.\n\n";
        std::exit(1);
    } */

    // Parse castling rights
    while (fen[currIndex] != ' ') {
        switch (fen[currIndex]) {
        case 'K':
            setBit(castling, (int)CastlingRights::wk);
            break;
        case 'Q':
            setBit(castling, (int)CastlingRights::wq);
            break;
        case 'k':
            setBit(castling, (int)CastlingRights::bk);
            break;
        case 'q':
            setBit(castling, (int)CastlingRights::bq);
            break;
        }
        currIndex++;
    }
    currIndex++;

    // Parse enpassant square
    if (fen[currIndex] != '-') {
        int f = fen[currIndex] - 'a';
        currIndex++;
        int r = 8 - (fen[currIndex] - '0');
        enpassant = (Sq)SQ(r, f);
        currIndex++;
    } else {
        enpassant = Sq::noSq;
        currIndex++;
    }
    currIndex++;

    size_t count;
    // Parse the number of half moves
    std::string spaceInd = fen.substr(currIndex);
    count = spaceInd.find(" ");
    halfMoves = atoi(fen.substr(currIndex, count - 1).c_str());
    currIndex += (int)count;

    // Parse the number of full moves
    spaceInd = fen.substr(currIndex);
    count = spaceInd.find(" ");
    fullMoves = atoi(fen.substr(currIndex, count - 1).c_str());
}
