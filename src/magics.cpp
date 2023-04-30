#include "magics.hpp"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "attack.hpp"
#include "bitboard.hpp"
#include "board.hpp"

namespace Magics {

// clang-format off
const std::array<uint64_t, 64> bishopMagics = {
	0x5048200440504100ULL, 0x102288004a8002ULL, 0x410c088082820aULL,
	0x20082080210040a0ULL, 0x1104000401400ULL, 0x44010420c4418002ULL,
	0x260420820092000ULL, 0x12420041084010ULL, 0x2000041010511110ULL,
	0xc4040c104a03ULL, 0x4006881084008110ULL, 0x82481000100ULL,
	0x8402840420058010ULL, 0x621212808c00000ULL, 0x20008241202000ULL,
	0x8008300423004c5ULL, 0x4928081020d90400ULL, 0x10c40c6004009a10ULL,
	0x1180202040100ULL, 0x2121005820460004ULL, 0x1004100202022400ULL,
	0x204100a02008402ULL, 0x800800108080240ULL, 0x1000024010468ULL,
	0x3090a009200100ULL, 0x41c0830039804ULL, 0x4224100041010020ULL,
	0x40808008020002ULL, 0x8001001045004000ULL, 0x110004154805000ULL,
	0x2008c20424050408ULL, 0x602210a010105ULL, 0x18020800916021ULL,
	0x2082222000100102ULL, 0x102108800300044ULL, 0x30902008000b0050ULL,
	0x804080200002008ULL, 0x880b02080010080ULL, 0x1002220040041400ULL,
	0x8012920c30980ULL, 0x40410188c0012021ULL, 0x4032838000222ULL,
	0x21000c004804a401ULL, 0xc10182018000100ULL, 0xe01210020a009020ULL,
	0x244108802000040ULL, 0x182020802310100ULL, 0x895040020008cULL,
	0x200118020a618001ULL, 0x116250150100800ULL, 0x1000044141c0000ULL,
	0xc000014022880e08ULL, 0x200000405040402ULL, 0x89880a2218420000ULL,
	0x40048400820120ULL, 0x84218409120010ULL, 0x402100480088880aULL,
	0x8030008201012010ULL, 0x600a82440c0400ULL, 0x1002081400208802ULL,
	0x1404804a0024c10ULL, 0x4010582004014202ULL, 0xa20204822180049ULL,
	0x2820404268200ULL,
};

const std::array<uint64_t, 64> rookMagics = {
	0x80102040008000ULL, 0x8400a20001001c0ULL, 0x100200010090042ULL,
	0x2080040800801000ULL, 0x200204850840200ULL, 0x200100104080200ULL,
	0x200020001408804ULL, 0x8200010080402a04ULL, 0x11c800040002081ULL,
	0x41804000806008ULL, 0x863001020010044ULL, 0x102000a20104201ULL,
	0x1001008010004ULL, 0x400800200040080ULL, 0xa00808002000100ULL,
	0x881000894422100ULL, 0x8288004400081ULL, 0x4848020004000ULL,
	0x4101090020004010ULL, 0x404220010400a00ULL, 0xa3010008000410ULL,
	0x180808004000200ULL, 0x4400098a1810ULL, 0x4200020000890844ULL,
	0x10a0208080004003ULL, 0x2880200040005000ULL, 0x8420002100410010ULL,
	0x2200080080100080ULL, 0x200040080800800ULL, 0x40080800200ULL,
	0x4004010080800200ULL, 0x2000004200008104ULL, 0x40004262800080ULL,
	0x30004002402001ULL, 0x800802000801000ULL, 0x20c1002009001004ULL,
	0x2040802402800800ULL, 0xa0004d2001008ULL, 0x2040488104001002ULL,
	0x3004082000104ULL, 0x802040008000ULL, 0x820100841254000ULL,
	0x3820041001868020ULL, 0x9001011004210008ULL, 0x20080004008080ULL,
	0x5100040002008080ULL, 0x2090508102040028ULL, 0x1400010040820004ULL,
	0x121800040122a80ULL, 0xc204009008300ULL, 0x401001444200100ULL,
	0x20815000080180ULL, 0x222000410082200ULL, 0x980040002008080ULL,
	0x4106220110486400ULL, 0x211000042008100ULL, 0x6000144081002202ULL,
	0x8040001b006381ULL, 0x88402000100901ULL, 0x200081000210055ULL,
	0x102002008100402ULL, 0x201a000408011082ULL, 0x1000589008010204ULL,
	0x80a518621004c02ULL,
};
// clang-format on

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
                  << "ULL\n";
    std::cout << "\n}\n\n";
    std::cout << "BISHOP: {\n";
    for (sq = 0; sq < 64; sq++)
        std::cout << "0x" << std::hex
                  << findMagicNumber(sq, Attack::bishopRelevantBits[sq], PieceTypes::BISHOP)
                  << std::dec << "ULL\n";
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
