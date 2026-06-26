#include "make_move.hpp"

void makeMove(Position& position, uint16_t move, MoveStack& move_stack){

  int from_square = move >> 10;
  int to_square = (move >> 4) & 0x3f;
  uint8_t flags = move & 0xf;
  int from_piece = position.board[from_square];

  MoveMetadata move_data;
  move_data.move = move;
  move_data.castling_rights = position.castling_rights;
  move_data.enpassant_square = position.enpassant_square;

  // remove piece from its position and add it to the new position
  uint64_t piece_displacement = (1ULL << from_square) | (1ULL << to_square);
  position.piece_bitboard[from_piece] ^= piece_displacement;
  position.occupied_squares_bitboard[position.turn] ^= piece_displacement;

  // BUG: non stai rimuovendo il pedone catturato en passant da board (bug innocuo almenoche non usi unmake move. credo)
  if(flags & 0x4){

    int captured_offset = position.turn ? 8 : -8;
    int captured_end_square = flags == 0x5 ? (to_square + captured_offset) : to_square;

    int captured_piece = position.board[captured_end_square];
    position.piece_bitboard[captured_piece] ^= (1ULL << captured_end_square);
    position.occupied_squares_bitboard[!position.turn] ^= (1ULL << captured_end_square);

    move_data.captured_piece = captured_piece;
  }

  position.board[to_square] = from_piece;

  if(flags == 0x1){
    position.enpassant_square = to_square;
  }else{
    position.enpassant_square = -1;
  }

  if(flags == 0x2){
    int index_offset = 6 + (position.turn ? 0 : -6);
    int king_square = position.turn ? 60 : 4;
    
    uint64_t rook_movement = (1ULL << (king_square + 3)) | (1ULL << (king_square + 1));

    position.piece_bitboard[index_offset + 4] ^= rook_movement;
    position.occupied_squares_bitboard[position.turn] ^= rook_movement;
    position.board[king_square + 3] = -1;
    position.board[king_square + 1] = index_offset + 4;
  }

  if(flags == 0x3){
    int index_offset = 6 + (position.turn ? 0 : -6);
    int king_square = position.turn ? 60 : 4;
    
    uint64_t rook_movement = (1ULL << (king_square - 4)) | (1ULL << (king_square - 1));

    position.piece_bitboard[index_offset + 4] ^= rook_movement;
    position.occupied_squares_bitboard[position.turn] ^= rook_movement;
    position.board[king_square - 4] = -1;
    position.board[king_square - 1] = index_offset + 4;
  }

  //promotions
  if(flags & 0x8){
    position.piece_bitboard[from_piece] ^= (1ULL << to_square);
    
    int index_offset = 6 + (position.turn ? 0 : -6);
    int promotion_piece = (flags & 0x3) + 1;

    position.piece_bitboard[index_offset + promotion_piece] |= (1ULL << to_square);
    position.board[to_square] = index_offset + promotion_piece;
  }

  // add move to stack
  move_stack.prev_moves[position.semi_moves_number] = move_data;

  // update castling rights
  position.castling_rights &= position.castling_bitmap[from_square];
  position.castling_rights &= position.castling_bitmap[to_square];

  // change turn
  position.turn ^= 1;

  // update semimoves
  position.semi_moves_number++;
}