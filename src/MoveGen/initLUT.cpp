#include <cmath>
#include <immintrin.h>
#include "../Board/masks.hpp"
#include "LUT.hpp"

size_t rookRelevantOccupancyMask(int square){

  uint64_t relevant_occupancy_mask = 0;

  int row = square / 8;
  int column = square % 8;

  // up
  for(int i = row + 1; i < 7; i++){
    relevant_occupancy_mask |= 1ULL << (i * 8 + column);
  }

  // down
  for(int i = 1; i < row; i++){
    relevant_occupancy_mask |= 1ULL << (i * 8 + column);
  }

  // left
  for(int i = 1; i < column; i++){
    relevant_occupancy_mask |= 1ULL << (row * 8 + i);
  }
  
  // right
  for(int i = column + 1; i < 7; i++){
    relevant_occupancy_mask |= 1ULL << (row * 8 + i);
  }

  // fill rook_relevant_occupancy_mask entry at square = square
  rook_relevant_occupancy_mask[square] = relevant_occupancy_mask;

  int bits = _mm_popcnt_u64(relevant_occupancy_mask);
  size_t relevant_occupancy_combinations_n = 1 << bits;

  return relevant_occupancy_combinations_n;
};

size_t bishopRelevantOccupancyMask(int square){
    
  uint64_t relevant_occupancy_mask = 0;

  int row = square / 8;
  int column = square % 8;

  // up right diagonal
  for(int i = 1; i < std::min((7 - row), (7 - column)); i++){
    relevant_occupancy_mask |= 1ULL << ((row + i) * 8 + (column + i));
  }

  // down right diagonal
  for(int i = 1; i < std::min((row), (7 - column)); i++){
    relevant_occupancy_mask |= 1ULL << ((row - i) * 8 + (column + i));
  }

  // down left diagonal
  for(int i = 1; i < std::min((row), (column)); i++){
    relevant_occupancy_mask |= 1ULL << ((row - i) * 8 + (column - i));
  }

  // up left diagonal
  for(int i = 1; i < std::min((7 - row), (column)); i++){
    relevant_occupancy_mask |= 1ULL << ((row + i) * 8 + (column - i));
  }

    // fill bishop_relevant_occupancy_mask entry at square = square
  bishop_relevant_occupancy_mask[square] = relevant_occupancy_mask;

  int bits = _mm_popcnt_u64(relevant_occupancy_mask);
  size_t relevant_occupancy_combinations_n = 1 << bits;

  return relevant_occupancy_combinations_n;
};

uint64_t rookPseudolegalMoves(int square, uint64_t occupancy){

  uint64_t movement_mask = 0;

  int row = square / 8;
  int column = square % 8;

  int current_row = row;
  int current_column = column;

  // up
  while(current_row < 7){
    movement_mask |= 1ULL << (++current_row * 8 + current_column);
    if((1ULL << ((current_row) * 8 + current_column)) & occupancy){
      break;
    }
  }

  current_row = row;
  current_column = column;

  // right
  while(current_column < 7){
    movement_mask |= 1ULL << (current_row * 8 + ++current_column);
    if((1ULL << ((current_row) * 8 + current_column)) & occupancy){
      break;
    }
  }

  current_row = row;
  current_column = column;

  // down
  while(current_row > 0){
    movement_mask |= 1ULL << (--current_row * 8 + current_column);
    if((1ULL << ((current_row) * 8 + current_column)) & occupancy){
      break;
    }
  }

  current_row = row;
  current_column = column;

  // left
  while(current_column > 0){
    movement_mask |= 1ULL << (current_row * 8 + --current_column);
    if((1ULL << ((current_row) * 8 + current_column)) & occupancy){
      break;
    }
  }

  return movement_mask;
}

uint64_t bishopPseudolegalMoves(int square, uint64_t occupancy){

  uint64_t movement_mask = 0;

  int row = square / 8;
  int column = square % 8;

  int current_row = row;
  int current_column = column;

  // up right diagonal
  while(current_row < 7 && current_column < 7){
    movement_mask |= 1ULL << (++current_row * 8 + ++current_column);
    if((1ULL << ((current_row) * 8 + current_column)) & occupancy){
      break;
    }
  }

  current_row = row;
  current_column = column;

  // down right diagonal
  while(current_row > 0 && current_column < 7){
    movement_mask |= 1ULL << (--current_row * 8 + ++current_column);
    if((1ULL << ((current_row) * 8 + current_column)) & occupancy){
      break;
    }
  }

  current_row = row;
  current_column = column;

  // down left diagonal
  while(current_row > 0 && current_column > 0){
    movement_mask |= 1ULL << (--current_row * 8 + --current_column);
    if((1ULL << ((current_row) * 8 + current_column)) & occupancy){
      break;
    }
  }

  current_row = row;
  current_column = column;

  // up left diagonal
  while(current_row < 7 && current_column > 0){
    movement_mask |= 1ULL << (++current_row * 8 + --current_column);
    if((1ULL << ((current_row) * 8 + current_column)) & occupancy){
      break;
    }
  }

  return movement_mask;
}

uint64_t knightPseudolegalMoves(int square){

  uint64_t movement_mask = 0;

  uint64_t knight_square = 1ULL << square;
    
  // up right move
  movement_mask |= ((knight_square & ~(Masks::FILE_H | Masks::RANK_SEVENTH | Masks::RANK_EIGHTH))) << 17;

  // right up move
  movement_mask |= ((knight_square & ~(Masks::FILE_G | Masks::FILE_H | Masks::RANK_EIGHTH))) << 10;

  // right bottom move
  movement_mask |= ((knight_square & ~(Masks::FILE_G | Masks::FILE_H | Masks::RANK_FIRST))) >> 6;

  // down right move
  movement_mask |= ((knight_square & ~(Masks::FILE_H | Masks::RANK_FIRST | Masks::RANK_SECOND))) >> 15;

  // down left move
  movement_mask |= ((knight_square & ~(Masks::FILE_A | Masks::RANK_FIRST | Masks::RANK_SECOND))) >> 17;

  // left down move
  movement_mask |= ((knight_square & ~(Masks::FILE_A | Masks::FILE_B | Masks::RANK_FIRST))) >> 10;

  // left up move
  movement_mask |= ((knight_square & ~(Masks::FILE_A | Masks::FILE_B | Masks::RANK_EIGHTH))) << 6;

  // up left move
  movement_mask |= ((knight_square & ~(Masks::FILE_A | Masks::RANK_SEVENTH | Masks::RANK_EIGHTH))) << 15;

  return movement_mask;
}

uint64_t kingPseudolegalMoves(int square){

  uint64_t movement_mask = 0;

  uint64_t king_square = 1ULL << square;
    
  // up move
  movement_mask |= ((king_square & ~(Masks::RANK_EIGHTH))) << 8;

  // up right move
  movement_mask |= ((king_square & ~(Masks::FILE_H | Masks::RANK_EIGHTH))) << 9;

  // right move
  movement_mask |= ((king_square & ~(Masks::FILE_H))) << 1;

  // dow right move
  movement_mask |= ((king_square & ~(Masks::RANK_FIRST | Masks::FILE_H))) >> 7;

  // down
  movement_mask |= ((king_square & ~(Masks::RANK_FIRST))) >> 8;

  // down left move
  movement_mask |= ((king_square & ~(Masks::RANK_FIRST | Masks::FILE_A))) >> 9;

  // left move
  movement_mask |= ((king_square & ~(Masks::FILE_A))) >> 1;

  // up left move
  movement_mask |= ((king_square & ~(Masks::RANK_EIGHTH | Masks::FILE_A))) << 7;

  return movement_mask;
}

void fillPseudomovesLookupTable(){

  int current_offset = 0;

  // rook LUT
  for(int i = 0; i < 64; i++){

    rook_pseudomoves_lookup_table_offsets[i] = current_offset;

    size_t relevant_occupancy_combinations_n = rookRelevantOccupancyMask(i);
    uint64_t mask = rook_relevant_occupancy_mask[i];

    for(uint64_t combinations_n = 0; combinations_n < relevant_occupancy_combinations_n; combinations_n++){
      uint64_t generated_occupancy = _pdep_u64(combinations_n, mask);

      // calculate movement bitboard
      uint64_t movement_mask = rookPseudolegalMoves(i, generated_occupancy);

      // add movement to LUT
      pseudomoves_lookup_table[current_offset + combinations_n] = movement_mask;
    }

    current_offset += relevant_occupancy_combinations_n;

  }

  // bishop LUT
  for(int i = 0; i < 64; i++){

    bishop_pseudomoves_lookup_table_offsets[i] = current_offset;

    size_t relevant_occupancy_combinations_n = bishopRelevantOccupancyMask(i);
    uint64_t mask = bishop_relevant_occupancy_mask[i];

    for(uint64_t combinations_n = 0; combinations_n < relevant_occupancy_combinations_n; combinations_n++){
      uint64_t generated_occupancy = _pdep_u64(combinations_n, mask);

      // calculate movement bitboard
      uint64_t movement_mask = bishopPseudolegalMoves(i, generated_occupancy);

      // add movement to LUT
      pseudomoves_lookup_table[current_offset + combinations_n] = movement_mask;
    }

    current_offset += relevant_occupancy_combinations_n;

  }

  knight_pseudomoves_lookup_table_offsets = current_offset;
  // knight LUT
  for(int i = 0; i < 64; i++){

    uint64_t movement_mask = knightPseudolegalMoves(i);

    pseudomoves_lookup_table[current_offset] = movement_mask;

    current_offset++;
  }

  king_pseudomoves_lookup_table_offsets = current_offset;
  // king LUT
  for(int i = 0; i < 64; i++){

    uint64_t movement_mask = kingPseudolegalMoves(i);

    pseudomoves_lookup_table[current_offset] = movement_mask;

    current_offset++;
  }
};