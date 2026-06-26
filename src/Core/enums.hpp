#pragma once

#include <cstdint>

enum Color : uint8_t {
    WHITE = 0,
    BLACK = 1
};

enum Pieces : int8_t {
  NO_PIECE = -1,
  KING = 0,
  QUEEN = 1, 
  BISHOP = 2, 
  KNIGHT = 3,
  ROOK = 4, 
  PAWN = 5
};