# ROADMAP

## GUI MODE

- [ ] Board
  - [ ] Draw grid
  - [ ] Color each grid by exchanging the light and dark colors
- [ ] Piece
  - [ ] Find pieces online
  - [ ] Draw them centered in each square
- [ ] Controls
  - [ ] Hightlight selected piece
  - [ ] Hightlight preview possible moves
  - [ ] Move piece
    - [ ] Have a functioning method to make moves
    - [ ] Highlight _legal_ target square for the piece
    - [ ] When target square is clicked, move piece there
- [ ] Evaluation bar from some engine
- [ ] Show list of moves played
- [ ] Export as
  - [ ] FEN
  - [ ] PGN

## TERMINAL MODE

- Coming in the future . . .

## Rules

- [ ] Board
  - [ ] Piece bitboards
  - [ ] Units bitboards
  - [ ] Handle states
  - [ ] Set commands from FEN strings
- [ ] Precalculate attacks
  - [ ] Leaper pieces
    - [ ] Pawn
    - [ ] Knight
    - [ ] King
  - [ ] Sliding pieces
    - [ ] Magic bitboards
    - [ ] Bishop
    - [ ] Rook
    - [ ] Queen(combination of rook and bishop)
- [ ] Moves
  - [ ] Encoding moves as a 32-bit integer
  - [ ] Decoding move information
  - [ ] Converting moves to strings
  - [ ] Parsing moves from string
  - [ ] Make moves
    - [ ] Move piece from source to target square
    - [ ] Deal with captures, promotions, enpassant moves and castling
    - [ ] Check if the currently made move results in a check - (If yes, unmake move)

## Parsing

- [x] FEN
  - [x] Parse piece placement
  - [x] Parse side to move
  - [x] Parse castling rights
  - [x] Parse enpassant square
  - [x] Parse half moves
  - [x] Parse full moves
- [ ] Move notations
  - [ ] SAN - Short Algebraic notation
  - [ ] Coordinate notation
- [ ] PGN
  - [ ] Parse header section
  - [ ] Parse move section
    - [ ] Parse SAN moves
    - [ ] Parse SAN moves notation
    - [ ] Parse result
- [ ] UCI
  - [UCI commands notes](https://gist.github.com/aliostad/f4470274f39d29b788c1b09519e67372)
