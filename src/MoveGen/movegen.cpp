#include <immintrin.h>
#include "movegen.hpp"
#include "../Core/pop_lsb.hpp"
#include "LUT.hpp"

// WHITE PAWN
void generateUnpinnedWhitePawnLegalMoves(Position& position, MoveList& move_list){

  uint64_t pawn_bitboard = position.piece_bitboard[5];
  uint64_t not_occupancy_bitmap = ~(position.occupied_squares_bitboard[0] | position.occupied_squares_bitboard[1]);
  uint64_t pawn_moves = 0;

  // one step forward
  uint64_t eight_rank_mask = 0xff00000000000000;

  pawn_moves = (pawn_bitboard << 8) & not_occupancy_bitmap;

  uint64_t promotion_pawn_moves = pawn_moves & eight_rank_mask;
  uint64_t non_promotion_pawn_moves = pawn_moves ^ promotion_pawn_moves;

  while(promotion_pawn_moves){
    int square_to = popLSB(promotion_pawn_moves); 
    int square_from = square_to - 8;

    uint16_t move = (square_from << 10) | (square_to << 4) | 0x8;
    move_list.moves[move_list.head++] = move;

    move = (square_from << 10) | (square_to << 4) | 0x9;
    move_list.moves[move_list.head++] = move;

    move = (square_from << 10) | (square_to << 4) | 0xa;
    move_list.moves[move_list.head++] = move;

    move = (square_from << 10) | (square_to << 4) | 0xb;
    move_list.moves[move_list.head++] = move;
  }

  while(non_promotion_pawn_moves){
    int square_to = popLSB(non_promotion_pawn_moves); 
    int square_from = square_to - 8;

    uint16_t move = (square_from << 10) | (square_to << 4);
    move_list.moves[move_list.head++] = move;
  }

  // two steps forward
  uint64_t second_rank_mask = 0x000000000000ff00;

  pawn_moves = ((pawn_bitboard & second_rank_mask) << 16) & (not_occupancy_bitmap) & (not_occupancy_bitmap << 8);

  while(pawn_moves){
    int square_to = popLSB(pawn_moves); 
    int square_from = square_to - 16;

    uint16_t move = (square_from << 10) | (square_to << 4) | 0x1;
    move_list.moves[move_list.head++] = move;
  }

  // right capture
  uint64_t h_file_mask = 0x8080808080808080;

  pawn_moves = ((pawn_bitboard & ~h_file_mask) << 9) & (position.occupied_squares_bitboard[1]);

  promotion_pawn_moves = pawn_moves & eight_rank_mask;
  non_promotion_pawn_moves = pawn_moves ^ promotion_pawn_moves;

  while(promotion_pawn_moves){
    int square_to = popLSB(promotion_pawn_moves); 
    int square_from = square_to - 9;

    uint16_t move = (square_from << 10) | (square_to << 4) | 0xc;
    move_list.moves[move_list.head++] = move;

    move = (square_from << 10) | (square_to << 4) | 0xd;
    move_list.moves[move_list.head++] = move;

    move = (square_from << 10) | (square_to << 4) | 0xe;
    move_list.moves[move_list.head++] = move;

    move = (square_from << 10) | (square_to << 4) | 0xf;
    move_list.moves[move_list.head++] = move;
  }

  while(non_promotion_pawn_moves){
    int square_to = popLSB(non_promotion_pawn_moves); 

    int square_from = square_to - 9;

    uint16_t move = (square_from << 10) | (square_to << 4) | 0x4;
    move_list.moves[move_list.head++] = move;
  }

  // left capture
  uint64_t a_file_mask = 0x0101010101010101; 

  pawn_moves = ((pawn_bitboard & ~a_file_mask) << 7) & (position.occupied_squares_bitboard[1]);

  promotion_pawn_moves = pawn_moves & eight_rank_mask;
  non_promotion_pawn_moves = pawn_moves ^ promotion_pawn_moves;

  while(promotion_pawn_moves){
    int square_to = popLSB(promotion_pawn_moves); 
    int square_from = square_to - 7;

    uint16_t move = (square_from << 10) | (square_to << 4) | 0xc;
    move_list.moves[move_list.head++] = move;

    move = (square_from << 10) | (square_to << 4) | 0xd;
    move_list.moves[move_list.head++] = move;

    move = (square_from << 10) | (square_to << 4) | 0xe;
    move_list.moves[move_list.head++] = move;

    move = (square_from << 10) | (square_to << 4) | 0xf;
    move_list.moves[move_list.head++] = move;
  }

  while(non_promotion_pawn_moves){
    int square_to = popLSB(non_promotion_pawn_moves); 

    int square_from = square_to - 7;

    uint16_t move = (square_from << 10) | (square_to << 4) | 0x4;
    move_list.moves[move_list.head++] = move;
  }

  // right en passant
  pawn_moves = ((pawn_bitboard & ~h_file_mask) << 1) & ((1ULL << (position.enpassant_square)));

  if(pawn_moves){
    uint64_t new_occupancy = not_occupancy_bitmap | pawn_moves | (pawn_moves >> 1);
    new_occupancy &= ~(pawn_moves << 8);
    int king_square = __builtin_ctzll(position.piece_bitboard[0]);
    uint64_t rook_ray_from_king = pseudomoves_lookup_table[rook_pseudomoves_lookup_table_offsets[king_square] + _pext_u64(~new_occupancy, rook_relevant_occupancy_mask[king_square])];
    uint64_t rook_checkers = rook_ray_from_king & (position.piece_bitboard[7] | position.piece_bitboard[10]);

    if(!rook_checkers){
      int square_to = popLSB(pawn_moves) + 8;
      int square_from = square_to - 9;

      uint16_t move = (square_from << 10) | (square_to << 4) | 0x5;
      move_list.moves[move_list.head++] = move;
    }
  }

  // left en passant
  pawn_moves = ((pawn_bitboard & ~a_file_mask) >> 1) & ((1ULL << (position.enpassant_square)));

  if(pawn_moves){

    uint64_t new_occupancy = not_occupancy_bitmap | pawn_moves | (pawn_moves << 1);
    new_occupancy &= ~(pawn_moves << 8);
    int king_square = __builtin_ctzll(position.piece_bitboard[0]);
    uint64_t rook_ray_from_king = pseudomoves_lookup_table[rook_pseudomoves_lookup_table_offsets[king_square] + _pext_u64(~new_occupancy, rook_relevant_occupancy_mask[king_square])];
    uint64_t rook_checkers = rook_ray_from_king & (position.piece_bitboard[7] | position.piece_bitboard[10]);
    
    if(!rook_checkers){

      int square_to = popLSB(pawn_moves) + 8; 
      int square_from = square_to - 7;
    
      uint16_t move = (square_from << 10) | (square_to << 4) | 0x5;
      move_list.moves[move_list.head++] = move;
    }
  }
}

void generatePinnedWhitePawnLegalMoves(Position& position, MoveList& move_list){

  uint64_t pawn_bitboard = position.piece_bitboard[5];
  uint64_t not_occupancy_bitmap = ~(position.occupied_squares_bitboard[0] | position.occupied_squares_bitboard[1]);
  uint64_t pawn_moves = 0;

  // one step forward
  while(pawn_bitboard){
    int square_from = popLSB(pawn_bitboard); 

    pawn_moves = (1ULL << (square_from + 8)) & not_occupancy_bitmap & position.pin_ray_bitboards[square_from];

    if(pawn_moves){
      int square_to = popLSB(pawn_moves);

      uint16_t move = (square_from << 10) | (square_to << 4);
      move_list.moves[move_list.head++] = move;
    }
  }

  pawn_bitboard = position.piece_bitboard[5];

  // two step forward
  uint64_t second_rank_mask = 0x000000000000ff00;
  
  while(pawn_bitboard){
    int square_from = popLSB(pawn_bitboard); 

    pawn_moves = (((1ULL << square_from) & second_rank_mask) << 16) & (not_occupancy_bitmap) & (not_occupancy_bitmap << 8) & position.pin_ray_bitboards[square_from];

    if(pawn_moves){
      int square_to = popLSB(pawn_moves);

      uint16_t move = (square_from << 10) | (square_to << 4) | 0x1;
      move_list.moves[move_list.head++] = move;
    }
  }

  pawn_bitboard = position.piece_bitboard[5];

  // right capture
  uint64_t h_file_mask = 0x8080808080808080;
  uint64_t eight_rank_mask = 0xff00000000000000;
  
  while(pawn_bitboard){
    int square_from = popLSB(pawn_bitboard); 

    pawn_moves = (((1ULL << square_from) & ~h_file_mask) << 9) & (position.occupied_squares_bitboard[1]) & position.pin_ray_bitboards[square_from];

    if(pawn_moves){
      if(pawn_moves & eight_rank_mask){
        int square_to = popLSB(pawn_moves);

        uint16_t move = (square_from << 10) | (square_to << 4) | 0xc;
        move_list.moves[move_list.head++] = move;

        move = (square_from << 10) | (square_to << 4) | 0xd;
        move_list.moves[move_list.head++] = move;

        move = (square_from << 10) | (square_to << 4) | 0xe;
        move_list.moves[move_list.head++] = move;

        move = (square_from << 10) | (square_to << 4) | 0xf;
        move_list.moves[move_list.head++] = move;
      }else{
        int square_to = popLSB(pawn_moves);

        uint16_t move = (square_from << 10) | (square_to << 4) | 0x4;
        move_list.moves[move_list.head++] = move;
      }
    }
  }

  pawn_bitboard = position.piece_bitboard[5];

  // left capture
  uint64_t a_file_mask = 0x0101010101010101; 

  while(pawn_bitboard){
    int square_from = popLSB(pawn_bitboard); 

    pawn_moves = (((1ULL << square_from) & ~a_file_mask) << 7) & (position.occupied_squares_bitboard[1]) & position.pin_ray_bitboards[square_from];

    if(pawn_moves){
      if(pawn_moves & eight_rank_mask){
        int square_to = popLSB(pawn_moves);

        uint16_t move = (square_from << 10) | (square_to << 4) | 0xc;
        move_list.moves[move_list.head++] = move;

        move = (square_from << 10) | (square_to << 4) | 0xd;
        move_list.moves[move_list.head++] = move;

        move = (square_from << 10) | (square_to << 4) | 0xe;
        move_list.moves[move_list.head++] = move;

        move = (square_from << 10) | (square_to << 4) | 0xf;
        move_list.moves[move_list.head++] = move;
      }else{
        int square_to = popLSB(pawn_moves);

        uint16_t move = (square_from << 10) | (square_to << 4) | 0x4;
        move_list.moves[move_list.head++] = move;
      }
    }
  } 

  pawn_bitboard = position.piece_bitboard[5];

  // right en passant
  pawn_moves = ((pawn_bitboard & ~h_file_mask) << 1) & (1ULL << (position.enpassant_square));
  pawn_moves = (pawn_moves << 8) & position.pin_ray_bitboards[__builtin_ctzll(pawn_moves) - 1];

  if(pawn_moves){

    uint64_t new_occupancy = not_occupancy_bitmap | (pawn_moves >> 8) | (pawn_moves >> 9);
    new_occupancy &= ~(pawn_moves);
    int king_square = __builtin_ctzll(position.piece_bitboard[0]);
    uint64_t rook_ray_from_king = pseudomoves_lookup_table[rook_pseudomoves_lookup_table_offsets[king_square] + _pext_u64(~new_occupancy, rook_relevant_occupancy_mask[king_square])];
    uint64_t rook_checkers = rook_ray_from_king & (position.piece_bitboard[7] | position.piece_bitboard[10]);

    if(!rook_checkers){
      int square_to = popLSB(pawn_moves);
      int square_from = square_to - 9;

      uint16_t move = (square_from << 10) | (square_to << 4) | 0x5;
      move_list.moves[move_list.head++] = move;
    }
  }

  // left en passant
  pawn_moves = ((pawn_bitboard & ~a_file_mask) >> 1) & (1ULL << (position.enpassant_square));
  pawn_moves = (pawn_moves << 8) & position.pin_ray_bitboards[__builtin_ctzll(pawn_moves) + 1];

  if(pawn_moves){
    uint64_t new_occupancy = not_occupancy_bitmap | (pawn_moves >> 7) | (pawn_moves >> 8);
    new_occupancy &= ~(pawn_moves);
    int king_square = __builtin_ctzll(position.piece_bitboard[0]);
    uint64_t rook_ray_from_king = pseudomoves_lookup_table[rook_pseudomoves_lookup_table_offsets[king_square] + _pext_u64(~new_occupancy, rook_relevant_occupancy_mask[king_square])];
    uint64_t rook_checkers = rook_ray_from_king & (position.piece_bitboard[7] | position.piece_bitboard[10]);

    if(!rook_checkers){
      int square_to = popLSB(pawn_moves);
      int square_from = square_to - 7;

      uint16_t move = (square_from << 10) | (square_to << 4) | 0x5;
      move_list.moves[move_list.head++] = move;
    }
  }
}

void generateUnpinnedCheckWhitePawnLegalMoves(Position& position, uint64_t check_ray, MoveList& move_list){

  uint64_t pawn_bitboard = position.piece_bitboard[5];
  uint64_t not_occupancy_bitmap = ~(position.occupied_squares_bitboard[0] | position.occupied_squares_bitboard[1]);
  uint64_t pawn_moves = 0;

  // one step forward
  uint64_t eight_rank_mask = 0xff00000000000000;

  pawn_moves = (pawn_bitboard << 8) & not_occupancy_bitmap & check_ray;

  uint64_t promotion_pawn_moves = pawn_moves & eight_rank_mask;
  uint64_t non_promotion_pawn_moves = pawn_moves ^ promotion_pawn_moves;

  while(promotion_pawn_moves){
    int square_to = popLSB(promotion_pawn_moves); 
    int square_from = square_to - 8;

    uint16_t move = (square_from << 10) | (square_to << 4) | 0x8;
    move_list.moves[move_list.head++] = move;

    move = (square_from << 10) | (square_to << 4) | 0x9;
    move_list.moves[move_list.head++] = move;

    move = (square_from << 10) | (square_to << 4) | 0xa;
    move_list.moves[move_list.head++] = move;

    move = (square_from << 10) | (square_to << 4) | 0xb;
    move_list.moves[move_list.head++] = move;
  }

  while(non_promotion_pawn_moves){
    int square_to = popLSB(non_promotion_pawn_moves); 
    int square_from = square_to - 8;

    uint16_t move = (square_from << 10) | (square_to << 4);
    move_list.moves[move_list.head++] = move;
  }

  // two steps forward
  uint64_t second_rank_mask = 0x000000000000ff00;

  pawn_moves = ((pawn_bitboard & second_rank_mask) << 16) & (not_occupancy_bitmap) & (not_occupancy_bitmap << 8) & check_ray;

  while(pawn_moves){
    int square_to = popLSB(pawn_moves); 
    int square_from = square_to - 16;

    uint16_t move = (square_from << 10) | (square_to << 4) | 0x1;
    move_list.moves[move_list.head++] = move;
  }

  // right capture
  uint64_t h_file_mask = 0x8080808080808080;

  pawn_moves = ((pawn_bitboard & ~h_file_mask) << 9) & (position.occupied_squares_bitboard[1]) & check_ray;

  promotion_pawn_moves = pawn_moves & eight_rank_mask;
  non_promotion_pawn_moves = pawn_moves ^ promotion_pawn_moves;

  while(promotion_pawn_moves){
    int square_to = popLSB(promotion_pawn_moves); 
    int square_from = square_to - 9;

    uint16_t move = (square_from << 10) | (square_to << 4) | 0xc;
    move_list.moves[move_list.head++] = move;

    move = (square_from << 10) | (square_to << 4) | 0xd;
    move_list.moves[move_list.head++] = move;

    move = (square_from << 10) | (square_to << 4) | 0xe;
    move_list.moves[move_list.head++] = move;

    move = (square_from << 10) | (square_to << 4) | 0xf;
    move_list.moves[move_list.head++] = move;
  }

  while(non_promotion_pawn_moves){
    int square_to = popLSB(non_promotion_pawn_moves); 

    int square_from = square_to - 9;

    uint16_t move = (square_from << 10) | (square_to << 4) | 0x4;
    move_list.moves[move_list.head++] = move;
  }

  // left capture
  uint64_t a_file_mask = 0x0101010101010101; 

  pawn_moves = ((pawn_bitboard & ~a_file_mask) << 7) & (position.occupied_squares_bitboard[1]) & check_ray;

  promotion_pawn_moves = pawn_moves & eight_rank_mask;
  non_promotion_pawn_moves = pawn_moves ^ promotion_pawn_moves;

  while(promotion_pawn_moves){
    int square_to = popLSB(promotion_pawn_moves); 
    int square_from = square_to - 7;

    uint16_t move = (square_from << 10) | (square_to << 4) | 0xc;
    move_list.moves[move_list.head++] = move;

    move = (square_from << 10) | (square_to << 4) | 0xd;
    move_list.moves[move_list.head++] = move;

    move = (square_from << 10) | (square_to << 4) | 0xe;
    move_list.moves[move_list.head++] = move;

    move = (square_from << 10) | (square_to << 4) | 0xf;
    move_list.moves[move_list.head++] = move;
  }

  while(non_promotion_pawn_moves){
    int square_to = popLSB(non_promotion_pawn_moves); 

    int square_from = square_to - 7;

    uint16_t move = (square_from << 10) | (square_to << 4) | 0x4;
    move_list.moves[move_list.head++] = move;
  }

  // right en passant
  pawn_moves = ((pawn_bitboard & ~h_file_mask) << 1) & ((1ULL << (position.enpassant_square))) & (check_ray);

  if(pawn_moves){
    uint64_t new_occupancy = not_occupancy_bitmap | pawn_moves | (pawn_moves >> 1);
    new_occupancy &= ~(pawn_moves << 8);
    int king_square = __builtin_ctzll(position.piece_bitboard[0]);
    uint64_t rook_ray_from_king = pseudomoves_lookup_table[rook_pseudomoves_lookup_table_offsets[king_square] + _pext_u64(~new_occupancy, rook_relevant_occupancy_mask[king_square])];
    uint64_t rook_checkers = rook_ray_from_king & (position.piece_bitboard[7] | position.piece_bitboard[10]);

    if(!rook_checkers){
      int square_to = popLSB(pawn_moves) + 8;
      int square_from = square_to - 9;

      uint16_t move = (square_from << 10) | (square_to << 4) | 0x5;
      move_list.moves[move_list.head++] = move;
    }
  }

  // left en passant
  pawn_moves = ((pawn_bitboard & ~a_file_mask) >> 1) & ((1ULL << (position.enpassant_square))) & (check_ray);

  if(pawn_moves){
    uint64_t new_occupancy = not_occupancy_bitmap | pawn_moves | (pawn_moves << 1);
    new_occupancy &= ~(pawn_moves << 8);
    int king_square = __builtin_ctzll(position.piece_bitboard[0]);
    uint64_t rook_ray_from_king = pseudomoves_lookup_table[rook_pseudomoves_lookup_table_offsets[king_square] + _pext_u64(~new_occupancy, rook_relevant_occupancy_mask[king_square])];
    uint64_t rook_checkers = rook_ray_from_king & (position.piece_bitboard[7] | position.piece_bitboard[10]);
    
    if(!rook_checkers){
      int square_to = popLSB(pawn_moves) + 8; 
      int square_from = square_to - 7;
    
      uint16_t move = (square_from << 10) | (square_to << 4) | 0x5;
      move_list.moves[move_list.head++] = move;
    }
  }
}

void generatePinnedCheckWhitePawnLegalMoves(Position& position, uint64_t check_ray, MoveList& move_list){

  uint64_t pawn_bitboard = position.piece_bitboard[5];
  uint64_t not_occupancy_bitmap = ~(position.occupied_squares_bitboard[0] | position.occupied_squares_bitboard[1]);
  uint64_t pawn_moves = 0;

  // one step forward
  while(pawn_bitboard){
    int square_from = popLSB(pawn_bitboard); 

    pawn_moves = (1ULL << (square_from + 8)) & not_occupancy_bitmap & position.pin_ray_bitboards[square_from] & check_ray;

    if(pawn_moves){
      int square_to = popLSB(pawn_moves);

      uint16_t move = (square_from << 10) | (square_to << 4);
      move_list.moves[move_list.head++] = move;
    }
  }

  pawn_bitboard = position.piece_bitboard[5];

  // two step forward
  uint64_t second_rank_mask = 0x000000000000ff00;
  
  while(pawn_bitboard){
    int square_from = popLSB(pawn_bitboard); 

    pawn_moves = (((1ULL << square_from) & second_rank_mask) << 16) & (not_occupancy_bitmap) & (not_occupancy_bitmap << 8) & position.pin_ray_bitboards[square_from] & check_ray;

    if(pawn_moves){
      int square_to = popLSB(pawn_moves);

      uint16_t move = (square_from << 10) | (square_to << 4) | 0x1;
      move_list.moves[move_list.head++] = move;
    }
  }

  pawn_bitboard = position.piece_bitboard[5];

  // right capture
  uint64_t h_file_mask = 0x8080808080808080;
  
  while(pawn_bitboard){
    int square_from = popLSB(pawn_bitboard); 

    pawn_moves = (((1ULL << square_from) & ~h_file_mask) << 9) & (position.occupied_squares_bitboard[1]) & position.pin_ray_bitboards[square_from] & check_ray;

    if(pawn_moves){
      int square_to = popLSB(pawn_moves);

      uint16_t move = (square_from << 10) | (square_to << 4) | 0x4;
      move_list.moves[move_list.head++] = move;
    }
  }

  pawn_bitboard = position.piece_bitboard[5];

  // left capture
  uint64_t a_file_mask = 0x0101010101010101; 

  while(pawn_bitboard){
    int square_from = popLSB(pawn_bitboard); 

    pawn_moves = (((1ULL << square_from) & ~a_file_mask) << 7) & (position.occupied_squares_bitboard[1]) & position.pin_ray_bitboards[square_from] & check_ray;

    if(pawn_moves){
      int square_to = popLSB(pawn_moves);

      uint16_t move = (square_from << 10) | (square_to << 4) | 0x4;
      move_list.moves[move_list.head++] = move;
    }
  } 

  pawn_bitboard = position.piece_bitboard[5];

  // right en passant
  pawn_moves = ((pawn_bitboard & ~h_file_mask) << 1) & (1ULL << (position.enpassant_square)) & check_ray;
  pawn_moves = (pawn_moves << 8) & position.pin_ray_bitboards[__builtin_ctzll(pawn_moves) - 1];

  if(pawn_moves){
    uint64_t new_occupancy = not_occupancy_bitmap | (pawn_moves >> 8) | (pawn_moves >> 9);
    new_occupancy &= ~(pawn_moves);
    int king_square = __builtin_ctzll(position.piece_bitboard[0]);
    uint64_t rook_ray_from_king = pseudomoves_lookup_table[rook_pseudomoves_lookup_table_offsets[king_square] + _pext_u64(~new_occupancy, rook_relevant_occupancy_mask[king_square])];
    uint64_t rook_checkers = rook_ray_from_king & (position.piece_bitboard[7] | position.piece_bitboard[10]);

    if(!rook_checkers){
      int square_to = popLSB(pawn_moves);
      int square_from = square_to - 9;

      uint16_t move = (square_from << 10) | (square_to << 4) | 0x5;
      move_list.moves[move_list.head++] = move;
    }
  }

  // left en passant
  pawn_moves = ((pawn_bitboard & ~a_file_mask) >> 1) & (1ULL << (position.enpassant_square)) & check_ray;
  pawn_moves = (pawn_moves << 8) & position.pin_ray_bitboards[__builtin_ctzll(pawn_moves) + 1];

  if(pawn_moves){
    uint64_t new_occupancy = not_occupancy_bitmap | (pawn_moves >> 7) | (pawn_moves >> 8);
    new_occupancy &= ~(pawn_moves);
    int king_square = __builtin_ctzll(position.piece_bitboard[0]);
    uint64_t rook_ray_from_king = pseudomoves_lookup_table[rook_pseudomoves_lookup_table_offsets[king_square] + _pext_u64(~new_occupancy, rook_relevant_occupancy_mask[king_square])];
    uint64_t rook_checkers = rook_ray_from_king & (position.piece_bitboard[7] | position.piece_bitboard[10]);

    if(!rook_checkers){
      int square_to = popLSB(pawn_moves);
      int square_from = square_to - 7;

      uint16_t move = (square_from << 10) | (square_to << 4) | 0x5;
      move_list.moves[move_list.head++] = move;
    }
  }
}

// BLACK PAWN
void generateUnpinnedBlackPawnLegalMoves(Position& position, MoveList& move_list){

  uint64_t pawn_bitboard = position.piece_bitboard[11];
  uint64_t not_occupancy_bitmap = ~(position.occupied_squares_bitboard[0] | position.occupied_squares_bitboard[1]);
  uint64_t pawn_moves = 0;

  // one step forward
  uint64_t first_rank_mask = 0x00000000000000ff;

  pawn_moves = (pawn_bitboard >> 8) & not_occupancy_bitmap;

  uint64_t promotion_pawn_moves = pawn_moves & first_rank_mask;
  uint64_t non_promotion_pawn_moves = pawn_moves ^ promotion_pawn_moves;

  while(promotion_pawn_moves){
    int square_to = popLSB(promotion_pawn_moves); 
    int square_from = square_to + 8;

    uint16_t move = (square_from << 10) | (square_to << 4) | 0x8;
    move_list.moves[move_list.head++] = move;

    move = (square_from << 10) | (square_to << 4) | 0x9;
    move_list.moves[move_list.head++] = move;

    move = (square_from << 10) | (square_to << 4) | 0xa;
    move_list.moves[move_list.head++] = move;

    move = (square_from << 10) | (square_to << 4) | 0xb;
    move_list.moves[move_list.head++] = move;
  }

  while(non_promotion_pawn_moves){
    int square_to = popLSB(non_promotion_pawn_moves); 
    int square_from = square_to + 8;

    uint16_t move = (square_from << 10) | (square_to << 4);
    move_list.moves[move_list.head++] = move;
  }

  // two steps forward
  uint64_t seventh_rank_mask = 0x00ff000000000000;

  pawn_moves = ((pawn_bitboard & seventh_rank_mask) >> 16) & (not_occupancy_bitmap) & (not_occupancy_bitmap >> 8);

  while(pawn_moves){
    int square_to = popLSB(pawn_moves); 
    int square_from = square_to + 16;

    uint16_t move = (square_from << 10) | (square_to << 4) | 0x1;
    move_list.moves[move_list.head++] = move;
  }

  // right capture
  uint64_t h_file_mask = 0x8080808080808080;

  pawn_moves = ((pawn_bitboard & ~h_file_mask) >> 7) & (position.occupied_squares_bitboard[0]);

  promotion_pawn_moves = pawn_moves & first_rank_mask;
  non_promotion_pawn_moves = pawn_moves ^ promotion_pawn_moves;

  while(promotion_pawn_moves){
    int square_to = popLSB(promotion_pawn_moves); 
    int square_from = square_to + 7;

    uint16_t move = (square_from << 10) | (square_to << 4) | 0xc;
    move_list.moves[move_list.head++] = move;

    move = (square_from << 10) | (square_to << 4) | 0xd;
    move_list.moves[move_list.head++] = move;

    move = (square_from << 10) | (square_to << 4) | 0xe;
    move_list.moves[move_list.head++] = move;

    move = (square_from << 10) | (square_to << 4) | 0xf;
    move_list.moves[move_list.head++] = move;
  }

  while(non_promotion_pawn_moves){
    int square_to = popLSB(non_promotion_pawn_moves); 

    int square_from = square_to + 7;

    uint16_t move = (square_from << 10) | (square_to << 4) | 0x4;
    move_list.moves[move_list.head++] = move;
  }

  // left capture
  uint64_t a_file_mask = 0x0101010101010101; 

  pawn_moves = ((pawn_bitboard & ~a_file_mask) >> 9) & (position.occupied_squares_bitboard[0]);

  promotion_pawn_moves = pawn_moves & first_rank_mask;
  non_promotion_pawn_moves = pawn_moves ^ promotion_pawn_moves;

  while(promotion_pawn_moves){
    int square_to = popLSB(promotion_pawn_moves); 
    int square_from = square_to + 9;

    uint16_t move = (square_from << 10) | (square_to << 4) | 0xc;
    move_list.moves[move_list.head++] = move;

    move = (square_from << 10) | (square_to << 4) | 0xd;
    move_list.moves[move_list.head++] = move;

    move = (square_from << 10) | (square_to << 4) | 0xe;
    move_list.moves[move_list.head++] = move;

    move = (square_from << 10) | (square_to << 4) | 0xf;
    move_list.moves[move_list.head++] = move;
  }

  while(non_promotion_pawn_moves){
    int square_to = popLSB(non_promotion_pawn_moves); 

    int square_from = square_to + 9;

    uint16_t move = (square_from << 10) | (square_to << 4) | 0x4;
    move_list.moves[move_list.head++] = move;
  }

  // right en passant
  pawn_moves = ((pawn_bitboard & ~h_file_mask) << 1) & ((1ULL << (position.enpassant_square)));

  if(pawn_moves){
    uint64_t new_occupancy = not_occupancy_bitmap | pawn_moves | (pawn_moves >> 1);
    new_occupancy &= ~(pawn_moves >> 8);
    int king_square = __builtin_ctzll(position.piece_bitboard[6]);

    uint64_t rook_ray_from_king = pseudomoves_lookup_table[rook_pseudomoves_lookup_table_offsets[king_square] + _pext_u64(~new_occupancy, rook_relevant_occupancy_mask[king_square])];
    uint64_t rook_checkers = rook_ray_from_king & (position.piece_bitboard[1] | position.piece_bitboard[4]);

    if(!rook_checkers){
      int square_to = popLSB(pawn_moves) - 8;
      int square_from = square_to + 7;

      uint16_t move = (square_from << 10) | (square_to << 4) | 0x5;
      move_list.moves[move_list.head++] = move;
    }
  }

  // left en passant
  pawn_moves = ((pawn_bitboard & ~a_file_mask) >> 1) & ((1ULL << (position.enpassant_square)));

  if(pawn_moves){
    uint64_t new_occupancy = not_occupancy_bitmap | pawn_moves | (pawn_moves << 1);
    new_occupancy &= ~(pawn_moves >> 8);
    int king_square = __builtin_ctzll(position.piece_bitboard[6]);
    uint64_t rook_ray_from_king = pseudomoves_lookup_table[rook_pseudomoves_lookup_table_offsets[king_square] + _pext_u64(~new_occupancy, rook_relevant_occupancy_mask[king_square])];
    uint64_t rook_checkers = rook_ray_from_king & (position.piece_bitboard[1] | position.piece_bitboard[4]);
    
    if(!rook_checkers){
      int square_to = popLSB(pawn_moves) - 8; 
      int square_from = square_to + 9;
    
      uint16_t move = (square_from << 10) | (square_to << 4) | 0x5;
      move_list.moves[move_list.head++] = move;
    }
  }
}

void generatePinnedBlackPawnLegalMoves(Position& position, MoveList& move_list){

  uint64_t pawn_bitboard = position.piece_bitboard[11];
  uint64_t not_occupancy_bitmap = ~(position.occupied_squares_bitboard[0] | position.occupied_squares_bitboard[1]);
  uint64_t pawn_moves = 0;

  // one step forward
  while(pawn_bitboard){
    int square_from = popLSB(pawn_bitboard); 

    pawn_moves = (1ULL << (square_from - 8)) & not_occupancy_bitmap & position.pin_ray_bitboards[square_from];

    if(pawn_moves){
      int square_to = popLSB(pawn_moves);

      uint16_t move = (square_from << 10) | (square_to << 4);
      move_list.moves[move_list.head++] = move;
    }
  }

  pawn_bitboard = position.piece_bitboard[11];

  // two step forward
  uint64_t seventh_rank_mask = 0x00ff000000000000;
  
  while(pawn_bitboard){
    int square_from = popLSB(pawn_bitboard); 

    pawn_moves = (((1ULL << square_from) & seventh_rank_mask) >> 16) & (not_occupancy_bitmap) & (not_occupancy_bitmap >> 8) & position.pin_ray_bitboards[square_from];

    if(pawn_moves){
      int square_to = popLSB(pawn_moves);

      uint16_t move = (square_from << 10) | (square_to << 4) | 0x1;
      move_list.moves[move_list.head++] = move;
    }
  }

  pawn_bitboard = position.piece_bitboard[11];

  // right capture
  uint64_t h_file_mask = 0x8080808080808080;
  uint64_t first_rank_mask = 0x00000000000000ff;

  while(pawn_bitboard){
    int square_from = popLSB(pawn_bitboard); 

    pawn_moves = (((1ULL << square_from) & ~h_file_mask) >> 7) & (position.occupied_squares_bitboard[0]) & position.pin_ray_bitboards[square_from];

    if(pawn_moves){

      if(pawn_moves & first_rank_mask){
        int square_to = popLSB(pawn_moves); 

        uint16_t move = (square_from << 10) | (square_to << 4) | 0xc;
        move_list.moves[move_list.head++] = move;

        move = (square_from << 10) | (square_to << 4) | 0xd;
        move_list.moves[move_list.head++] = move;

        move = (square_from << 10) | (square_to << 4) | 0xe;
        move_list.moves[move_list.head++] = move;

        move = (square_from << 10) | (square_to << 4) | 0xf;
        move_list.moves[move_list.head++] = move;

      }else{
        int square_to = popLSB(pawn_moves);

        uint16_t move = (square_from << 10) | (square_to << 4) | 0x4;
        move_list.moves[move_list.head++] = move;
      }
    }
  }

  pawn_bitboard = position.piece_bitboard[11];

  // left capture
  uint64_t a_file_mask = 0x0101010101010101; 

  while(pawn_bitboard){
    int square_from = popLSB(pawn_bitboard); 

    pawn_moves = (((1ULL << square_from) & ~a_file_mask) >> 9) & (position.occupied_squares_bitboard[0]) & position.pin_ray_bitboards[square_from];

    if(pawn_moves){

      if(pawn_moves & first_rank_mask){
        int square_to = popLSB(pawn_moves); 

        uint16_t move = (square_from << 10) | (square_to << 4) | 0xc;
        move_list.moves[move_list.head++] = move;

        move = (square_from << 10) | (square_to << 4) | 0xd;
        move_list.moves[move_list.head++] = move;

        move = (square_from << 10) | (square_to << 4) | 0xe;
        move_list.moves[move_list.head++] = move;

        move = (square_from << 10) | (square_to << 4) | 0xf;
        move_list.moves[move_list.head++] = move;
      }else{
        int square_to = popLSB(pawn_moves);

        uint16_t move = (square_from << 10) | (square_to << 4) | 0x4;
        move_list.moves[move_list.head++] = move;
      }
    }
  } 

  pawn_bitboard = position.piece_bitboard[11];

  // right en passant
  pawn_moves = ((pawn_bitboard & ~h_file_mask) << 1) & (1ULL << (position.enpassant_square));
  pawn_moves = (pawn_moves >> 8) & position.pin_ray_bitboards[__builtin_ctzll(pawn_moves) - 1];

  if(pawn_moves){
    uint64_t new_occupancy = not_occupancy_bitmap | (pawn_moves << 7) | (pawn_moves << 8);
    new_occupancy &= ~(pawn_moves);
    int king_square = __builtin_ctzll(position.piece_bitboard[6]);
    uint64_t rook_ray_from_king = pseudomoves_lookup_table[rook_pseudomoves_lookup_table_offsets[king_square] + _pext_u64(~new_occupancy, rook_relevant_occupancy_mask[king_square])];
    uint64_t rook_checkers = rook_ray_from_king & (position.piece_bitboard[1] | position.piece_bitboard[4]);

    if(!rook_checkers){
      int square_to = popLSB(pawn_moves);
      int square_from = square_to + 7;

      uint16_t move = (square_from << 10) | (square_to << 4) | 0x5;
      move_list.moves[move_list.head++] = move;
    }
  }

  // left en passant
  pawn_moves = ((pawn_bitboard & ~a_file_mask) >> 1) & (1ULL << (position.enpassant_square));
  pawn_moves = (pawn_moves >> 8) & position.pin_ray_bitboards[__builtin_ctzll(pawn_moves) + 1];

  if(pawn_moves){
    uint64_t new_occupancy = not_occupancy_bitmap | (pawn_moves << 8) | (pawn_moves << 9);
    new_occupancy &= ~(pawn_moves);
    int king_square = __builtin_ctzll(position.piece_bitboard[6]);
    uint64_t rook_ray_from_king = pseudomoves_lookup_table[rook_pseudomoves_lookup_table_offsets[king_square] + _pext_u64(~new_occupancy, rook_relevant_occupancy_mask[king_square])];
    uint64_t rook_checkers = rook_ray_from_king & (position.piece_bitboard[1] | position.piece_bitboard[4]);

    if(!rook_checkers){
      int square_to = popLSB(pawn_moves);
      int square_from = square_to + 9;

      uint16_t move = (square_from << 10) | (square_to << 4) | 0x5;
      move_list.moves[move_list.head++] = move;
    }
  }
}

void generateUnpinnedCheckBlackPawnLegalMoves(Position& position, uint64_t check_ray, MoveList& move_list){

  uint64_t pawn_bitboard = position.piece_bitboard[11];
  uint64_t not_occupancy_bitmap = ~(position.occupied_squares_bitboard[0] | position.occupied_squares_bitboard[1]);
  uint64_t pawn_moves = 0;

  // one step forward
  uint64_t first_rank_mask = 0x0000000000000000ff;

  pawn_moves = (pawn_bitboard >> 8) & not_occupancy_bitmap & check_ray;

  uint64_t promotion_pawn_moves = pawn_moves & first_rank_mask;
  uint64_t non_promotion_pawn_moves = pawn_moves ^ promotion_pawn_moves;

  while(promotion_pawn_moves){
    int square_to = popLSB(promotion_pawn_moves); 
    int square_from = square_to + 8;

    uint16_t move = (square_from << 10) | (square_to << 4) | 0x8;
    move_list.moves[move_list.head++] = move;

    move = (square_from << 10) | (square_to << 4) | 0x9;
    move_list.moves[move_list.head++] = move;

    move = (square_from << 10) | (square_to << 4) | 0xa;
    move_list.moves[move_list.head++] = move;

    move = (square_from << 10) | (square_to << 4) | 0xb;
    move_list.moves[move_list.head++] = move;
  }

  while(non_promotion_pawn_moves){
    int square_to = popLSB(non_promotion_pawn_moves); 
    int square_from = square_to + 8;

    uint16_t move = (square_from << 10) | (square_to << 4);
    move_list.moves[move_list.head++] = move;
  }

  // two steps forward
  uint64_t seventh_rank_mask = 0x00ff000000000000;

  pawn_moves = ((pawn_bitboard & seventh_rank_mask) >> 16) & (not_occupancy_bitmap) & (not_occupancy_bitmap >> 8) & check_ray;

  while(pawn_moves){
    int square_to = popLSB(pawn_moves); 
    int square_from = square_to + 16;

    uint16_t move = (square_from << 10) | (square_to << 4) | 0x1;
    move_list.moves[move_list.head++] = move;
  }

  // right capture
  uint64_t h_file_mask = 0x8080808080808080;

  pawn_moves = ((pawn_bitboard & ~h_file_mask) >> 7) & (position.occupied_squares_bitboard[0]) & check_ray;

  promotion_pawn_moves = pawn_moves & first_rank_mask;
  non_promotion_pawn_moves = pawn_moves ^ promotion_pawn_moves;

  while(promotion_pawn_moves){
    int square_to = popLSB(promotion_pawn_moves); 
    int square_from = square_to + 7;

    uint16_t move = (square_from << 10) | (square_to << 4) | 0xc;
    move_list.moves[move_list.head++] = move;

    move = (square_from << 10) | (square_to << 4) | 0xd;
    move_list.moves[move_list.head++] = move;

    move = (square_from << 10) | (square_to << 4) | 0xe;
    move_list.moves[move_list.head++] = move;

    move = (square_from << 10) | (square_to << 4) | 0xf;
    move_list.moves[move_list.head++] = move;
  }

  while(non_promotion_pawn_moves){
    int square_to = popLSB(non_promotion_pawn_moves); 

    int square_from = square_to + 7;

    uint16_t move = (square_from << 10) | (square_to << 4) | 0x4;
    move_list.moves[move_list.head++] = move;
  }

  // left capture
  uint64_t a_file_mask = 0x0101010101010101; 

  pawn_moves = ((pawn_bitboard & ~a_file_mask) >> 9) & (position.occupied_squares_bitboard[0]) & check_ray;

  promotion_pawn_moves = pawn_moves & first_rank_mask;
  non_promotion_pawn_moves = pawn_moves ^ promotion_pawn_moves;

  while(promotion_pawn_moves){
    int square_to = popLSB(promotion_pawn_moves); 
    int square_from = square_to + 9;

    uint16_t move = (square_from << 10) | (square_to << 4) | 0xc;
    move_list.moves[move_list.head++] = move;

    move = (square_from << 10) | (square_to << 4) | 0xd;
    move_list.moves[move_list.head++] = move;

    move = (square_from << 10) | (square_to << 4) | 0xe;
    move_list.moves[move_list.head++] = move;

    move = (square_from << 10) | (square_to << 4) | 0xf;
    move_list.moves[move_list.head++] = move;
  }

  while(non_promotion_pawn_moves){
    int square_to = popLSB(non_promotion_pawn_moves); 

    int square_from = square_to + 9;

    uint16_t move = (square_from << 10) | (square_to << 4) | 0x4;
    move_list.moves[move_list.head++] = move;
  }

  // right en passant
  pawn_moves = ((pawn_bitboard & ~h_file_mask) << 1) & ((1ULL << (position.enpassant_square))) & (check_ray);

  if(pawn_moves){

    uint64_t new_occupancy = not_occupancy_bitmap | pawn_moves | (pawn_moves >> 1);
    new_occupancy &= ~(pawn_moves >> 8);
    int king_square = __builtin_ctzll(position.piece_bitboard[6]);
    uint64_t rook_ray_from_king = pseudomoves_lookup_table[rook_pseudomoves_lookup_table_offsets[king_square] + _pext_u64(~new_occupancy, rook_relevant_occupancy_mask[king_square])];
    uint64_t rook_checkers = rook_ray_from_king & (position.piece_bitboard[1] | position.piece_bitboard[4]);

    if(!rook_checkers){
      int square_to = popLSB(pawn_moves) - 8;
      int square_from = square_to + 7;

      uint16_t move = (square_from << 10) | (square_to << 4) | 0x5;
      move_list.moves[move_list.head++] = move;
    }
  }

  // left en passant
  pawn_moves = ((pawn_bitboard & ~a_file_mask) >> 1) & ((1ULL << (position.enpassant_square))) & (check_ray);

  if(pawn_moves){
    uint64_t new_occupancy = not_occupancy_bitmap | pawn_moves | (pawn_moves << 1);
    new_occupancy &= ~(pawn_moves >> 8);
    int king_square = __builtin_ctzll(position.piece_bitboard[6]);
    uint64_t rook_ray_from_king = pseudomoves_lookup_table[rook_pseudomoves_lookup_table_offsets[king_square] + _pext_u64(~new_occupancy, rook_relevant_occupancy_mask[king_square])];
    uint64_t rook_checkers = rook_ray_from_king & (position.piece_bitboard[1] | position.piece_bitboard[4]);
    
    if(!rook_checkers){
      int square_to = popLSB(pawn_moves) - 8; 
      int square_from = square_to + 9;
    
      uint16_t move = (square_from << 10) | (square_to << 4) | 0x5;
      move_list.moves[move_list.head++] = move;
    }
  }
}

void generatePinnedCheckBlackPawnLegalMoves(Position& position, uint64_t check_ray, MoveList& move_list){

  uint64_t pawn_bitboard = position.piece_bitboard[11];
  uint64_t not_occupancy_bitmap = ~(position.occupied_squares_bitboard[0] | position.occupied_squares_bitboard[1]);
  uint64_t pawn_moves = 0;

  // one step forward
  while(pawn_bitboard){
    int square_from = popLSB(pawn_bitboard); 

    pawn_moves = (1ULL << (square_from - 8)) & not_occupancy_bitmap & position.pin_ray_bitboards[square_from] & check_ray;

    if(pawn_moves){
      int square_to = popLSB(pawn_moves);

      uint16_t move = (square_from << 10) | (square_to << 4);
      move_list.moves[move_list.head++] = move;
    }
  }

  pawn_bitboard = position.piece_bitboard[11];

  // two step forward
  uint64_t seventh_rank_mask = 0x00ff000000000000;
  
  while(pawn_bitboard){
    int square_from = popLSB(pawn_bitboard); 

    pawn_moves = (((1ULL << square_from) & seventh_rank_mask) >> 16) & (not_occupancy_bitmap) & (not_occupancy_bitmap >> 8) & position.pin_ray_bitboards[square_from] & check_ray;

    if(pawn_moves){
      int square_to = popLSB(pawn_moves);

      uint16_t move = (square_from << 10) | (square_to << 4) | 0x1;
      move_list.moves[move_list.head++] = move;
    }
  }

  pawn_bitboard = position.piece_bitboard[11];

  // right capture
  uint64_t h_file_mask = 0x8080808080808080;
  
  while(pawn_bitboard){
    int square_from = popLSB(pawn_bitboard); 

    pawn_moves = (((1ULL << square_from) & ~h_file_mask) >> 7) & (position.occupied_squares_bitboard[0]) & position.pin_ray_bitboards[square_from] & check_ray;

    if(pawn_moves){
      int square_to = popLSB(pawn_moves);

      uint16_t move = (square_from << 10) | (square_to << 4) | 0x4;
      move_list.moves[move_list.head++] = move;
    }
  }

  pawn_bitboard = position.piece_bitboard[11];

  // left capture
  uint64_t a_file_mask = 0x0101010101010101; 

  while(pawn_bitboard){
    int square_from = popLSB(pawn_bitboard); 

    pawn_moves = (((1ULL << square_from) & ~a_file_mask) >> 9) & (position.occupied_squares_bitboard[0]) & position.pin_ray_bitboards[square_from] & check_ray;

    if(pawn_moves){
      int square_to = popLSB(pawn_moves);

      uint16_t move = (square_from << 10) | (square_to << 4) | 0x4;
      move_list.moves[move_list.head++] = move;
    }
  } 

  pawn_bitboard = position.piece_bitboard[11];

  // right en passant
  pawn_moves = ((pawn_bitboard & ~h_file_mask) << 1) & (1ULL << (position.enpassant_square)) & check_ray;
  pawn_moves = (pawn_moves >> 8) & position.pin_ray_bitboards[__builtin_ctzll(pawn_moves) - 1];

  if(pawn_moves){
    uint64_t new_occupancy = not_occupancy_bitmap | (pawn_moves << 7) | (pawn_moves << 8);
    new_occupancy &= ~(pawn_moves);
    int king_square = __builtin_ctzll(position.piece_bitboard[6]);
    uint64_t rook_ray_from_king = pseudomoves_lookup_table[rook_pseudomoves_lookup_table_offsets[king_square] + _pext_u64(~new_occupancy, rook_relevant_occupancy_mask[king_square])];
    uint64_t rook_checkers = rook_ray_from_king & (position.piece_bitboard[1] | position.piece_bitboard[4]);

    if(!rook_checkers){
      int square_to = popLSB(pawn_moves);
      int square_from = square_to + 7;

      uint16_t move = (square_from << 10) | (square_to << 4) | 0x5;
      move_list.moves[move_list.head++] = move;
    }
  }

  // left en passant
  pawn_moves = ((pawn_bitboard & ~a_file_mask) >> 1) & (1ULL << (position.enpassant_square)) & check_ray;
  pawn_moves = (pawn_moves >> 8) & position.pin_ray_bitboards[__builtin_ctzll(pawn_moves) + 1];

  if(pawn_moves){
    uint64_t new_occupancy = not_occupancy_bitmap | (pawn_moves << 8) | (pawn_moves << 9);
    new_occupancy &= ~(pawn_moves);
    int king_square = __builtin_ctzll(position.piece_bitboard[6]);
    uint64_t rook_ray_from_king = pseudomoves_lookup_table[rook_pseudomoves_lookup_table_offsets[king_square] + _pext_u64(~new_occupancy, rook_relevant_occupancy_mask[king_square])];
    uint64_t rook_checkers = rook_ray_from_king & (position.piece_bitboard[1] | position.piece_bitboard[4]);

    if(!rook_checkers){
      int square_to = popLSB(pawn_moves);
      int square_from = square_to + 9;

      uint16_t move = (square_from << 10) | (square_to << 4) | 0x5;
      move_list.moves[move_list.head++] = move;
    }
  }
}
///////////////////////////////////////////////////////////

// starting from move_list.head, writes legal moves in move_list.moves
void generateLegalMoves(Position& position, MoveList& move_list){

  uint64_t occupancy = position.occupied_squares_bitboard[0] | position.occupied_squares_bitboard[1];
  int piece_offset = 3 + (position.turn ? 3 : -3);
  int king_attackers_n = 0;
  uint64_t pinned_pieces = 0;
  uint64_t check_ray = 0;

  int king_square = __builtin_ctzll(position.piece_bitboard[piece_offset]);

  // ROOKS AND QUEENS
  // checkers
  uint64_t ray_from_king = pseudomoves_lookup_table[rook_pseudomoves_lookup_table_offsets[king_square] + _pext_u64(occupancy, rook_relevant_occupancy_mask[king_square])];
  uint64_t checkers = ray_from_king & (position.piece_bitboard[7 - piece_offset] | position.piece_bitboard[10 - piece_offset]);
  king_attackers_n += __builtin_popcountll(checkers);

  // check ray
  if(checkers){
    int checker_square = __builtin_ctzll(checkers);
    uint64_t ray_from_checker = pseudomoves_lookup_table[rook_pseudomoves_lookup_table_offsets[checker_square] + _pext_u64(occupancy, rook_relevant_occupancy_mask[checker_square])];
    check_ray = ray_from_king & ray_from_checker;
    check_ray |= checkers;
  }

  // pins
  uint64_t potential_pinned = ray_from_king & position.occupied_squares_bitboard[position.turn];
  uint64_t occupancy_without_potential_pinned = occupancy & ~(potential_pinned);
  uint64_t ray_from_king_without_potential_pinned = pseudomoves_lookup_table[rook_pseudomoves_lookup_table_offsets[king_square] + _pext_u64(occupancy_without_potential_pinned, rook_relevant_occupancy_mask[king_square])];
  uint64_t pinners = ray_from_king_without_potential_pinned & (position.piece_bitboard[7 - piece_offset] | position.piece_bitboard[10 - piece_offset]) & ~(checkers); 

  while(pinners){
    int square_from = popLSB(pinners);
    uint64_t ray_from_pinner_without_potential_pins = pseudomoves_lookup_table[rook_pseudomoves_lookup_table_offsets[square_from] + _pext_u64(occupancy_without_potential_pinned, rook_relevant_occupancy_mask[square_from])];
    uint64_t pin_ray = ray_from_pinner_without_potential_pins & ray_from_king_without_potential_pinned;
    position.pin_ray_bitboards[__builtin_ctzll(pin_ray & potential_pinned)] = pin_ray | (1ULL << square_from);
    pinned_pieces |= pin_ray & potential_pinned;
  }

  // BISHOP AND QUEENS
  // checkers
  ray_from_king = pseudomoves_lookup_table[bishop_pseudomoves_lookup_table_offsets[king_square] + _pext_u64(occupancy, bishop_relevant_occupancy_mask[king_square])];
  checkers = ray_from_king & (position.piece_bitboard[7 - piece_offset] | position.piece_bitboard[8 - piece_offset]);
  king_attackers_n += __builtin_popcountll(checkers);

  // check ray
  if(checkers){
    int checker_square = __builtin_ctzll(checkers);
    uint64_t ray_from_checker = pseudomoves_lookup_table[bishop_pseudomoves_lookup_table_offsets[checker_square] + _pext_u64(occupancy, bishop_relevant_occupancy_mask[checker_square])];
    check_ray = ray_from_king & ray_from_checker;
    check_ray |= checkers;
  }

  // pins
  potential_pinned = ray_from_king & position.occupied_squares_bitboard[position.turn];
  occupancy_without_potential_pinned = occupancy & ~(potential_pinned);
  ray_from_king_without_potential_pinned = pseudomoves_lookup_table[bishop_pseudomoves_lookup_table_offsets[king_square] + _pext_u64(occupancy_without_potential_pinned, bishop_relevant_occupancy_mask[king_square])];
  pinners = ray_from_king_without_potential_pinned & (position.piece_bitboard[7 - piece_offset] | position.piece_bitboard[8 - piece_offset]) & ~(checkers); 

  while(pinners){
    int square_from = popLSB(pinners);
    uint64_t ray_from_pinner_without_potential_pins = pseudomoves_lookup_table[bishop_pseudomoves_lookup_table_offsets[square_from] + _pext_u64(occupancy_without_potential_pinned, bishop_relevant_occupancy_mask[square_from])];
    uint64_t pin_ray = ray_from_pinner_without_potential_pins & ray_from_king_without_potential_pinned;
    position.pin_ray_bitboards[__builtin_ctzll(pin_ray & potential_pinned)] = pin_ray | (1ULL << square_from);
    pinned_pieces |= pin_ray & potential_pinned;
  }

  // KNIGHTS
  // checkers
  checkers = pseudomoves_lookup_table[knight_pseudomoves_lookup_table_offsets + king_square] & (position.piece_bitboard[9 - piece_offset]);
  king_attackers_n += __builtin_popcountll(checkers);

  // check ray
  check_ray |= checkers;
  
  // PAWNS
  // checkers
  uint64_t a_file_mask = 0x0101010101010101; 
  uint64_t h_file_mask = 0x8080808080808080;
  uint64_t pawn_turn_mask = -position.turn;
  uint64_t pawn_ray_from_king = ((position.piece_bitboard[piece_offset] & ~pawn_turn_mask & ~(a_file_mask)) << 7) | ((position.piece_bitboard[piece_offset] & ~pawn_turn_mask & ~(h_file_mask)) << 9) | 
                                ((position.piece_bitboard[piece_offset] & pawn_turn_mask & ~(a_file_mask)) >> 9) | ((position.piece_bitboard[piece_offset] & pawn_turn_mask & ~(h_file_mask)) >> 7);
  checkers = pawn_ray_from_king & (position.piece_bitboard[11 - piece_offset]);
  king_attackers_n += __builtin_popcountll(checkers);

  check_ray |= checkers;

  // get squares attacked by enemy
  uint64_t attacked_squares = getAttackedSquares(position);

  if(king_attackers_n == 0){ // NOT CHECK

    // generate pinned rook and queen moves
    uint64_t pinned_rook_and_queen = (position.piece_bitboard[piece_offset + 1] | position.piece_bitboard[piece_offset + 4]) & pinned_pieces;
    while(pinned_rook_and_queen){

      int square_from = popLSB(pinned_rook_and_queen);

      uint64_t movement_mask = pseudomoves_lookup_table[rook_pseudomoves_lookup_table_offsets[square_from] + _pext_u64(occupancy, rook_relevant_occupancy_mask[square_from])];
      movement_mask &= ~(position.occupied_squares_bitboard[position.turn]);
      movement_mask &= position.pin_ray_bitboards[square_from];

      uint64_t captures = movement_mask & position.occupied_squares_bitboard[!position.turn];
      uint64_t not_captures = movement_mask ^ captures;

      uint16_t move = square_from << 10;
        
      while(captures){
        
        int square_to = popLSB(captures);
        move |= (square_to << 4) | 0x4;

        move_list.moves[move_list.head++] = move;

        move &= 0xfc00;
      }

      while(not_captures){

        int square_to = popLSB(not_captures);
        move |= square_to << 4;

        move_list.moves[move_list.head++] = move;

        move &= 0xfc00;
      }
    }

    // generate unpinned rook and queen moves
    uint64_t unpinned_rook_and_queen = (position.piece_bitboard[piece_offset + 1] | position.piece_bitboard[piece_offset + 4]) & ~(pinned_pieces);
    while(unpinned_rook_and_queen){

      int square_from = popLSB(unpinned_rook_and_queen);

      uint64_t movement_mask = pseudomoves_lookup_table[rook_pseudomoves_lookup_table_offsets[square_from] + _pext_u64(occupancy, rook_relevant_occupancy_mask[square_from])];
      movement_mask &= ~(position.occupied_squares_bitboard[position.turn]);

      uint64_t captures = movement_mask & position.occupied_squares_bitboard[!position.turn];
      uint64_t not_captures = movement_mask ^ captures;

      uint16_t move = square_from << 10;
        
      while(captures){
        
        int square_to = popLSB(captures);
        move |= (square_to << 4) | 0x4;

        move_list.moves[move_list.head++] = move;

        move &= 0xfc00;
      }

      while(not_captures){

        int square_to = popLSB(not_captures);
        move |= square_to << 4;

        move_list.moves[move_list.head++] = move;

        move &= 0xfc00;
      }
    }

    // generate pinned bishop and queen moves
    uint64_t pinned_bishop_and_queen = (position.piece_bitboard[piece_offset + 1] | position.piece_bitboard[piece_offset + 2]) & pinned_pieces;
    while(pinned_bishop_and_queen){

      int square_from = popLSB(pinned_bishop_and_queen);

      uint64_t movement_mask = pseudomoves_lookup_table[bishop_pseudomoves_lookup_table_offsets[square_from] + _pext_u64(occupancy, bishop_relevant_occupancy_mask[square_from])];
      movement_mask &= ~(position.occupied_squares_bitboard[position.turn]);
      movement_mask &= position.pin_ray_bitboards[square_from];

      uint64_t captures = movement_mask & position.occupied_squares_bitboard[!position.turn];
      uint64_t not_captures = movement_mask ^ captures;

      uint16_t move = square_from << 10;
        
      while(captures){
        
        int square_to = popLSB(captures);
        move |= (square_to << 4) | 0x4;

        move_list.moves[move_list.head++] = move;

        move &= 0xfc00;
      }

      while(not_captures){

        int square_to = popLSB(not_captures);
        move |= square_to << 4;

        move_list.moves[move_list.head++] = move;

        move &= 0xfc00;
      }
    }

    // generate unpinned bishop and queen moves
    uint64_t unpinned_bishop_and_queen = (position.piece_bitboard[piece_offset + 1] | position.piece_bitboard[piece_offset + 2]) & ~(pinned_pieces);
    while(unpinned_bishop_and_queen){

      int square_from = popLSB(unpinned_bishop_and_queen);

      uint64_t movement_mask = pseudomoves_lookup_table[bishop_pseudomoves_lookup_table_offsets[square_from] + _pext_u64(occupancy, bishop_relevant_occupancy_mask[square_from])];
      movement_mask &= ~(position.occupied_squares_bitboard[position.turn]);

      uint64_t captures = movement_mask & position.occupied_squares_bitboard[!position.turn];
      uint64_t not_captures = movement_mask ^ captures;

      uint16_t move = square_from << 10;
        
      while(captures){
        
        int square_to = popLSB(captures);
        move |= (square_to << 4) | 0x4;

        move_list.moves[move_list.head++] = move;

        move &= 0xfc00;
      }

      while(not_captures){

        int square_to = popLSB(not_captures);
        move |= square_to << 4;

        move_list.moves[move_list.head++] = move;

        move &= 0xfc00;
      }
    }

    // generate unpinned knight moves
    uint64_t unpinned_knight = position.piece_bitboard[piece_offset + 3] & ~(pinned_pieces);

    while(unpinned_knight){

      int square_from = popLSB(unpinned_knight);

      uint64_t movement_mask = pseudomoves_lookup_table[knight_pseudomoves_lookup_table_offsets + square_from];
      movement_mask &= ~(position.occupied_squares_bitboard[position.turn]);

      uint64_t captures = movement_mask & position.occupied_squares_bitboard[!position.turn];
      uint64_t not_captures = movement_mask ^ captures;

      uint16_t move = square_from << 10;
        
      while(captures){
        
        int square_to = popLSB(captures);
        move |= (square_to << 4) | 0x4;

        move_list.moves[move_list.head++] = move;

        move &= 0xfc00;
      }

      while(not_captures){

        int square_to = popLSB(not_captures);
        move |= square_to << 4;

        move_list.moves[move_list.head++] = move;

        move &= 0xfc00;
      }
    }

    uint64_t pawn_bitboard = position.piece_bitboard[piece_offset + 5];
    uint64_t pinned_pawn = pawn_bitboard & pinned_pieces;
    uint64_t unpinned_pawn = pawn_bitboard ^ pinned_pawn;

    // generate pawn moves
    if(position.turn){
      position.piece_bitboard[11] = unpinned_pawn;
      generateUnpinnedBlackPawnLegalMoves(position, move_list);

      position.piece_bitboard[11] = pinned_pawn;
      generatePinnedBlackPawnLegalMoves(position, move_list);

      position.piece_bitboard[11] = pawn_bitboard;
    }else{
      position.piece_bitboard[5] = unpinned_pawn;
      generateUnpinnedWhitePawnLegalMoves(position, move_list);

      position.piece_bitboard[5] = pinned_pawn;
      generatePinnedWhitePawnLegalMoves(position, move_list);

      position.piece_bitboard[5] = pawn_bitboard;
    }

    // generate king moves
    uint64_t king_movement_mask = pseudomoves_lookup_table[king_pseudomoves_lookup_table_offsets + king_square];
    king_movement_mask &= ~(position.occupied_squares_bitboard[position.turn]);
    king_movement_mask &= ~(attacked_squares);

    uint64_t captures = king_movement_mask & position.occupied_squares_bitboard[!position.turn];
    uint64_t not_captures = king_movement_mask ^ captures;

        
    while(captures){
        
      int square_to = popLSB(captures);

      uint16_t move = (king_square << 10) | (square_to << 4) | 0x4;

      move_list.moves[move_list.head++] = move;
    }

    while(not_captures){

      int square_to = popLSB(not_captures);
        
      uint16_t move = (king_square << 10) | (square_to << 4);

      move_list.moves[move_list.head++] = move;
    }

    // generate special moves (castle, promotion)
    // king side castling
    if(((position.castling_rights >> (position.turn << 1)) & 0x1) && (king_square == 4 || king_square == 60) && !(occupancy & (3ULL << (king_square + 1))) && !(attacked_squares & (7ULL << (king_square)))){
     
      uint16_t move = (king_square << 10) | ((king_square + 2) << 4) | 0x2;
      move_list.moves[move_list.head++] = move;
    }

    // queen side castling
    if(((position.castling_rights >> (position.turn << 1)) & 0x2) && (king_square == 4 || king_square == 60) && !(occupancy & (7ULL << (king_square - 3))) && !(attacked_squares & (7ULL << (king_square - 2)))){
     
      uint16_t move = (king_square << 10) | ((king_square - 2) << 4) | 0x3;
      move_list.moves[move_list.head++] = move;
    }

  }else if(king_attackers_n == 1){ // CHECK

    // generate pinned rook and queen moves
    uint64_t pinned_rook_and_queen = (position.piece_bitboard[piece_offset + 1] | position.piece_bitboard[piece_offset + 4]) & pinned_pieces;
    while(pinned_rook_and_queen){

      int square_from = popLSB(pinned_rook_and_queen);

      uint64_t movement_mask = pseudomoves_lookup_table[rook_pseudomoves_lookup_table_offsets[square_from] + _pext_u64(occupancy, rook_relevant_occupancy_mask[square_from])];
      movement_mask &= ~(position.occupied_squares_bitboard[position.turn]);
      movement_mask &= position.pin_ray_bitboards[square_from] & check_ray;

      uint64_t captures = movement_mask & position.occupied_squares_bitboard[!position.turn];
      uint64_t not_captures = movement_mask ^ captures;

      uint16_t move = square_from << 10;
        
      while(captures){
        
        int square_to = popLSB(captures);
        move |= (square_to << 4) | 0x4;

        move_list.moves[move_list.head++] = move;

        move &= 0xfc00;
      }

      while(not_captures){

        int square_to = popLSB(not_captures);
        move |= square_to << 4;

        move_list.moves[move_list.head++] = move;

        move &= 0xfc00;
      }
    }

    // generate unpinned rook and queen moves
    uint64_t unpinned_rook_and_queen = (position.piece_bitboard[piece_offset + 1] | position.piece_bitboard[piece_offset + 4]) & ~(pinned_pieces);
    while(unpinned_rook_and_queen){

      int square_from = popLSB(unpinned_rook_and_queen);

      uint64_t movement_mask = pseudomoves_lookup_table[rook_pseudomoves_lookup_table_offsets[square_from] + _pext_u64(occupancy, rook_relevant_occupancy_mask[square_from])];
      movement_mask &= ~(position.occupied_squares_bitboard[position.turn]) & check_ray;

      uint64_t captures = movement_mask & position.occupied_squares_bitboard[!position.turn];
      uint64_t not_captures = movement_mask ^ captures;

      uint16_t move = square_from << 10;
        
      while(captures){
        
        int square_to = popLSB(captures);
        move |= (square_to << 4) | 0x4;

        move_list.moves[move_list.head++] = move;

        move &= 0xfc00;
      }

      while(not_captures){

        int square_to = popLSB(not_captures);
        move |= square_to << 4;

        move_list.moves[move_list.head++] = move;

        move &= 0xfc00;
      }
    }

    // generate pinned bishop and queen moves
    uint64_t pinned_bishop_and_queen = (position.piece_bitboard[piece_offset + 1] | position.piece_bitboard[piece_offset + 2]) & pinned_pieces;
    while(pinned_bishop_and_queen){

      int square_from = popLSB(pinned_bishop_and_queen);

      uint64_t movement_mask = pseudomoves_lookup_table[bishop_pseudomoves_lookup_table_offsets[square_from] + _pext_u64(occupancy, bishop_relevant_occupancy_mask[square_from])];
      movement_mask &= ~(position.occupied_squares_bitboard[position.turn]);
      movement_mask &= position.pin_ray_bitboards[square_from] & check_ray;

      uint64_t captures = movement_mask & position.occupied_squares_bitboard[!position.turn];
      uint64_t not_captures = movement_mask ^ captures;

      uint16_t move = square_from << 10;
        
      while(captures){
        
        int square_to = popLSB(captures);
        move |= (square_to << 4) | 0x4;

        move_list.moves[move_list.head++] = move;

        move &= 0xfc00;
      }

      while(not_captures){

        int square_to = popLSB(not_captures);
        move |= square_to << 4;

        move_list.moves[move_list.head++] = move;

        move &= 0xfc00;
      }
    }

    // generate unpinned bishop and queen moves
    uint64_t unpinned_bishop_and_queen = (position.piece_bitboard[piece_offset + 1] | position.piece_bitboard[piece_offset + 2]) & ~(pinned_pieces);
    while(unpinned_bishop_and_queen){

      int square_from = popLSB(unpinned_bishop_and_queen);

      uint64_t movement_mask = pseudomoves_lookup_table[bishop_pseudomoves_lookup_table_offsets[square_from] + _pext_u64(occupancy, bishop_relevant_occupancy_mask[square_from])];
      movement_mask &= ~(position.occupied_squares_bitboard[position.turn]) & check_ray;

      uint64_t captures = movement_mask & position.occupied_squares_bitboard[!position.turn];
      uint64_t not_captures = movement_mask ^ captures;

      uint16_t move = square_from << 10;
        
      while(captures){
        
        int square_to = popLSB(captures);
        move |= (square_to << 4) | 0x4;

        move_list.moves[move_list.head++] = move;

        move &= 0xfc00;
      }

      while(not_captures){

        int square_to = popLSB(not_captures);
        move |= square_to << 4;

        move_list.moves[move_list.head++] = move;

        move &= 0xfc00;
      }
    }

    // generate unpinned knight moves
    uint64_t unpinned_knight = position.piece_bitboard[piece_offset + 3] & ~(pinned_pieces);
    while(unpinned_knight){

      int square_from = popLSB(unpinned_knight);

      uint64_t movement_mask = pseudomoves_lookup_table[knight_pseudomoves_lookup_table_offsets + square_from];
      movement_mask &= ~(position.occupied_squares_bitboard[position.turn]) & check_ray;

      uint64_t captures = movement_mask & position.occupied_squares_bitboard[!position.turn];
      uint64_t not_captures = movement_mask ^ captures;

      uint16_t move = square_from << 10;
        
      while(captures){
        
        int square_to = popLSB(captures);
        move |= (square_to << 4) | 0x4;

        move_list.moves[move_list.head++] = move;

        move &= 0xfc00;
      }

      while(not_captures){

        int square_to = popLSB(not_captures);
        move |= square_to << 4;

        move_list.moves[move_list.head++] = move;

        move &= 0xfc00;
      }
    }

    uint64_t pawn_bitboard = position.piece_bitboard[piece_offset + 5];
    uint64_t pinned_pawn = pawn_bitboard & pinned_pieces;
    uint64_t unpinned_pawn = pawn_bitboard ^ pinned_pawn;

    // generate unpinned pawn moves
    if(position.turn){

      position.piece_bitboard[11] = unpinned_pawn;
      generateUnpinnedCheckBlackPawnLegalMoves(position, check_ray, move_list);

      position.piece_bitboard[11] = pinned_pawn;
      generatePinnedCheckBlackPawnLegalMoves(position, check_ray, move_list);

      position.piece_bitboard[11] = pawn_bitboard;
    }else{
      position.piece_bitboard[5] = unpinned_pawn;
      generateUnpinnedCheckWhitePawnLegalMoves(position, check_ray, move_list);

      position.piece_bitboard[5] = pinned_pawn;
      generatePinnedCheckWhitePawnLegalMoves(position, check_ray, move_list);

      position.piece_bitboard[5] = pawn_bitboard;
    }

    // generate king moves
    uint64_t king_movement_mask = pseudomoves_lookup_table[king_pseudomoves_lookup_table_offsets + king_square];
    king_movement_mask &= ~(position.occupied_squares_bitboard[position.turn]);
    king_movement_mask &= ~(attacked_squares);
   
    uint64_t captures = king_movement_mask & position.occupied_squares_bitboard[!position.turn];
    uint64_t not_captures = king_movement_mask ^ captures;
   
    while(captures){
      int square_to = popLSB(captures);
      uint16_t move = (king_square << 10) | (square_to << 4) | 0x4;

      move_list.moves[move_list.head++] = move;
    }

    while(not_captures){
      int square_to = popLSB(not_captures);
      uint16_t move = (king_square << 10) | (square_to << 4);

      move_list.moves[move_list.head++] = move;
    }

  }else{ // DOUBLE CHECK
    
    // generate king moves
    uint64_t king_movement_mask = pseudomoves_lookup_table[king_pseudomoves_lookup_table_offsets + king_square];
    king_movement_mask &= ~(position.occupied_squares_bitboard[position.turn]);
    king_movement_mask &= ~(attacked_squares);
   
    uint64_t captures = king_movement_mask & position.occupied_squares_bitboard[!position.turn];
    uint64_t not_captures = king_movement_mask ^ captures;
   
    while(captures){
      int square_to = popLSB(captures);
      uint16_t move = (king_square << 10) | (square_to << 4) | 0x4;

      move_list.moves[move_list.head++] = move;
    }

    while(not_captures){
      int square_to = popLSB(not_captures);
      uint16_t move = (king_square << 10) | (square_to << 4);

      move_list.moves[move_list.head++] = move;
    }
  }
}

uint64_t getAttackedSquares(Position& position){

  uint64_t attacked_squares = 0;

  int piece_offset = 3 + (position.turn ? -3 : 3);
  int friendly_offset = 3 + (position.turn ? 3 : -3);
  uint64_t occupancy = (position.occupied_squares_bitboard[0] | position.occupied_squares_bitboard[1]) & ~(position.piece_bitboard[friendly_offset]);

  // generate rook and queen moves
  uint64_t rook_and_queen_bitboard = position.piece_bitboard[piece_offset + 1] | position.piece_bitboard[piece_offset + 4];

  while (rook_and_queen_bitboard){
    int square_from = popLSB(rook_and_queen_bitboard);
    attacked_squares |= pseudomoves_lookup_table[rook_pseudomoves_lookup_table_offsets[square_from] + _pext_u64(occupancy, rook_relevant_occupancy_mask[square_from])];
  }

  // generate bishop and queen moves
  uint64_t bishop_and_queen_bitboard = position.piece_bitboard[piece_offset + 1] | position.piece_bitboard[piece_offset + 2];

  while (bishop_and_queen_bitboard){
    int square_from = popLSB(bishop_and_queen_bitboard);
    attacked_squares |= pseudomoves_lookup_table[bishop_pseudomoves_lookup_table_offsets[square_from] + _pext_u64(occupancy, bishop_relevant_occupancy_mask[square_from])];
  }

  // generate knights moves
  uint64_t knight_bitboard = position.piece_bitboard[piece_offset + 3];

  while (knight_bitboard){
    int square_from = popLSB(knight_bitboard);
    attacked_squares |= pseudomoves_lookup_table[knight_pseudomoves_lookup_table_offsets + square_from];
  }

  // generate king moves
  uint64_t king_bitboard = position.piece_bitboard[piece_offset];
  attacked_squares |= pseudomoves_lookup_table[king_pseudomoves_lookup_table_offsets + popLSB(king_bitboard)];

  // generate pawn moves  
  uint64_t pawn_bitboard = position.piece_bitboard[piece_offset + 5];
  uint64_t pawn_turn_mask = -position.turn; // 0xffffffffffffffff if black turn and 0x0 if white turn 

  // PAWN RIGHT CAPTURE
  uint64_t a_file_mask = 0x0101010101010101; 
  uint64_t h_file_mask = 0x8080808080808080;

  // black right capture
  attacked_squares |= ((pawn_bitboard & ~h_file_mask) >> 7) & ~pawn_turn_mask;

  // white right capture
  attacked_squares |= ((pawn_bitboard & ~h_file_mask) << 9) & pawn_turn_mask;

  // PAWN LEFT CAPTURE
  // black left capture
  attacked_squares |= ((pawn_bitboard & ~a_file_mask) >> 9) & ~pawn_turn_mask;

  // white left capture
  attacked_squares |= ((pawn_bitboard & ~a_file_mask) << 7) & pawn_turn_mask;

  return attacked_squares;
}