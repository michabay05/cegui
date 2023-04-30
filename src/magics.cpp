#include "magics.hpp"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "attack.hpp"
#include "bitboard.hpp"
#include "board.hpp"


namespace Magics {

#include "magic_constants.hpp"

uint32_t randomState = 1804289383;

uint32_t random32() {
    uint32_t number = randomState;

    // XOR shift algorithm
    number ^= number << 13;
    number ^= number >> 17;
    number ^= number << 5;

    // Update random number state
    randomState = number;

    // Return random number
    return number;
}

uint64_t random64() {
    uint64_t rand1, rand2, rand3, rand4;
    rand1 = (uint64_t)(random32() & 0xFFFF);
    rand2 = (uint64_t)(random32() & 0xFFFF);
    rand3 = (uint64_t)(random32() & 0xFFFF);
    rand4 = (uint64_t)(random32() & 0xFFFF);
    return rand1 | (rand2 << 16) | (rand3 << 32) | (rand4 << 48);
}

uint64_t genRandomMagic() { return random64() & random64() & random64(); }

uint64_t findMagicNumber(const int sq, const int relevantBits, const PieceTypes piece) {
    // 4096(1 << 12) - because it's maximum possible occupancy variations
    uint64_t usedAttacks[4096], occupancies[4096], attacks[4096], magicNumber;
    uint64_t possibleOccupancy = (piece == PieceTypes::BISHOP) ? Attack::genBishopOccupancy(sq)
                                                               : Attack::genRookOccupancy(sq);
    int occupancyIndices = 1 << relevantBits;
    for (int count = 0; count < occupancyIndices; count++) {
        occupancies[count] = Attack::setOccupancy(count, relevantBits, possibleOccupancy);
        attacks[count] = (piece == PieceTypes::BISHOP)
                             ? Attack::genBishopAttack(sq, occupancies[count])
                             : Attack::genRookAttack(sq, occupancies[count]);
    }

    for (int randCount = 0; randCount < 100000000; randCount++) {
        magicNumber = genRandomMagic();
        if (Bitboard::countBits((possibleOccupancy * magicNumber) & 0xFF00000000000000) < 6)
            continue;
        memset(usedAttacks, 0, sizeof(usedAttacks));
        int count, failFlag;
        for (count = 0, failFlag = 0; !failFlag && count < occupancyIndices; count++) {
            int magicInd = (int)((occupancies[count] * magicNumber) >> (64 - relevantBits));
            if (usedAttacks[magicInd] == 0)
                usedAttacks[magicInd] = attacks[count];
            else if (usedAttacks[magicInd] != attacks[count])
                failFlag = 1;
        }
        if (!failFlag)
            return magicNumber;
    }
    std::cout << "Failed to find magic number for "
              << ((piece == PieceTypes::BISHOP) ? "bishop" : "rook") << " on " << strCoords[sq]
              << "\n";
    return 0;
}

void initMagics() {
    int sq;
    std::cout << "ROOK: {\n";
    for (sq = 0; sq < 64; sq++)
        std::cout << "0x" << std::hex
                  << findMagicNumber(sq, Attack::rookRelevantBits[sq], PieceTypes::ROOK) << std::dec
                  << "ULL,\n";
    std::cout << "\n}\n\n";
    std::cout << "BISHOP: {\n";
    for (sq = 0; sq < 64; sq++)
        std::cout << "0x" << std::hex
                  << findMagicNumber(sq, Attack::bishopRelevantBits[sq], PieceTypes::BISHOP)
                  << std::dec << "ULL,\n";
    std::cout << "};";
}

uint64_t getBishopAttack(const int sq, uint64_t blockerBoard) {
    blockerBoard &= Attack::bishopOccMasks[sq];
    blockerBoard *= bishopMagics[sq];
    blockerBoard >>= (64 - Attack::bishopRelevantBits[sq]);
    return Attack::bishopAttacks[sq][blockerBoard];
}

uint64_t getRookAttack(const int sq, uint64_t blockerBoard) {
    blockerBoard &= Attack::rookOccMasks[sq];
    blockerBoard *= rookMagics[sq];
    blockerBoard >>= (64 - Attack::rookRelevantBits[sq]);
    return Attack::rookAttacks[sq][blockerBoard];
}

uint64_t getQueenAttack(const int sq, uint64_t blockerBoard) {
    return getBishopAttack(sq, blockerBoard) | getRookAttack(sq, blockerBoard);
}
} // namespace Magics
