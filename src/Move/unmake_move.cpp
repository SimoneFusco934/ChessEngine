#include "unmake_move.hpp"

void unmakeMove(Position& position, MoveStack& move_stack){

  position.turn ^= 1;

  MoveMetadata prev_move_data = move_stack.prev_moves[--position.semi_moves_number];
  uint16_t prev_move = prev_move_data.move;
  uint8_t prev_castling_rights = prev_move_data.castling_rights;
  uint8_t prev_captured_piece = prev_move_data.captured_piece;
  int prev_enpassant_square = prev_move_data.enpassant_square;

  int from_square = prev_move >> 10;
  int to_square = (prev_move >> 4) & 0x3f;
  uint8_t flags = prev_move & 0xf;
  int to_piece = position.board[to_square];

  // rimuovi to_piece da to_square e rimettilo in from_square
  uint64_t piece_movement = (1ULL << from_square) | (1ULL << to_square);
  position.piece_bitboard[to_piece] ^= piece_movement;
  position.occupied_squares_bitboard[position.turn] ^= piece_movement;

  // catture
  int captured_offset = position.turn ? 8 : -8;
  int captured_square = flags == 0x5 ? (to_square + captured_offset) : to_square;

  if(flags & 0x4){
    
    int captured_piece = prev_captured_piece;

    position.piece_bitboard[captured_piece] ^= (1ULL << captured_square);
    position.occupied_squares_bitboard[!position.turn] ^= (1ULL << captured_square);

    position.board[captured_square] = prev_captured_piece;
  }

  position.board[from_square] = to_piece;

  // reset en passant
  position.enpassant_square = prev_enpassant_square;

  // arrocco
  if(flags == 0x2){
    int index_offset = 6 + (position.turn ? 0 : -6);
    int king_square = position.turn ? 60 : 4;
    
    uint64_t rook_movement = (1ULL << (king_square + 3)) | (1ULL << (king_square + 1));

    position.piece_bitboard[index_offset + 4] ^= rook_movement;
    position.occupied_squares_bitboard[position.turn] ^= rook_movement;
    position.board[king_square + 3] = index_offset + 4;
    position.board[king_square + 1] = -1;
  }

  if(flags == 0x3){
    int index_offset = 6 + (position.turn ? 0 : -6);
    int king_square = position.turn ? 60 : 4;
    
    uint64_t rook_movement = (1ULL << (king_square - 4)) | (1ULL << (king_square - 1));

    position.piece_bitboard[index_offset + 4] ^= rook_movement;
    position.occupied_squares_bitboard[position.turn] ^= rook_movement;
    position.board[king_square - 4] = index_offset + 4;
    position.board[king_square - 1] = -1;
  }

  // promotions
  if(flags & 0x8){
    position.piece_bitboard[to_piece] ^= (1ULL << from_square);
    
    int index_offset = 6 + (position.turn ? 0 : -6);

    position.piece_bitboard[index_offset + 5] |= (1ULL << from_square);
    position.board[from_square] = index_offset + 5;
  }

  position.castling_rights = prev_castling_rights;

}