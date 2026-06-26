#pragma once 

#include <cstdint>

struct MoveMetadata {
  uint16_t move;
  uint8_t castling_rights;
  uint8_t captured_piece;
  int enpassant_square;
};

struct alignas(64) MoveStack {
  MoveMetadata prev_moves[64];
};