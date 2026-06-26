#pragma once

#include <string>
#include "../Core/enums.hpp"

enum CastlingRights : uint8_t {
  WK = 1,
  WQ = 2,
  BK = 4,
  BQ = 8
};

struct alignas(64) Position {

  // board representation
  int8_t board[64];
  uint64_t piece_bitboard[12] = {0}; // a bitboard for each piece
  
  // auxiliary bitboards
  uint64_t occupied_squares_bitboard[2] = {0}; // 0 for white, 1 for black
  uint64_t pin_ray_bitboards[64] = {0};
  uint8_t castling_bitmap[64];

  // flags
  uint8_t castling_rights = 0;
  int enpassant_square = -1;
  int half_moves = 0;
  uint64_t zobrist_key = 0;

  int turn = 0; // 0 for white, 1 for black

  int semi_moves_number = 0;
  int moves_number = 0;

  Position(std::string fen_string = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"){

    // inizializzazione castling bitmap
    for(int i = 0; i < 64; i++){
      castling_bitmap[i] = WK | WQ | BK | BQ;
    }

    castling_bitmap[0] &= ~(WQ);
    castling_bitmap[4] &= ~(WK | WQ);
    castling_bitmap[7] &= ~(WK);
    castling_bitmap[56] &= ~(BQ);
    castling_bitmap[60] &= ~(BK | BQ);
    castling_bitmap[63] &= ~(BK);

    // inizializzazione bitboard
    int board_index = 0;
    int string_index = 0;

    // lettura posizione pezzi
    while(board_index < 64){

      char c = fen_string[string_index++];

      if(c == '/'){
        continue;
      }

      if(c >= '0' && c <= '9'){
        for(int i = 0; i < c - '0'; i++){
          board[((7 - (board_index / 8)) * 8) + (board_index % 8)] = NO_PIECE;
          board_index++;
        }
        continue;
      }

      int square = (7 - (board_index / 8)) * 8 + board_index % 8;

      int piece_index = NO_PIECE;

      switch (tolower(c)){
        case 'k': piece_index = KING; break;
        case 'q': piece_index = QUEEN; break;
        case 'b': piece_index = BISHOP; break;
        case 'n': piece_index = KNIGHT; break;
        case 'r': piece_index = ROOK; break;
        case 'p': piece_index = PAWN; break;
      }

      if(islower(c)){
        piece_index += 6;
      }

      piece_bitboard[piece_index] |= (1ULL << square);
      board[square] = piece_index;

      if(islower(c)){
        occupied_squares_bitboard[BLACK] |= (1ULL << square);
      }else{
        occupied_squares_bitboard[WHITE] |= (1ULL << square);
      }
      
      board_index++;
    }

    // space
    string_index++;

    // lettura turno
    char c_turn = fen_string[string_index++];
    if(c_turn == 'w'){
      turn = WHITE;
    }else{
      turn = BLACK;
    }

    // space
    string_index++;

    // lettura diritti di arrocco
    char cr = fen_string[string_index++];
    if(cr == '-'){
      castling_rights = 0;
      string_index++;
    }else{
      while(cr != ' '){
        switch (cr){
          case 'K':
            castling_rights |= WK;
            break;
          case 'Q':
            castling_rights |= WQ;
            break;
          case 'k':
            castling_rights |= BK;
            break;
          case 'q':
            castling_rights |= BQ;
            break;  
          default:
            break;
        }
        cr = fen_string[string_index++];
      }
    }

    char en_p = fen_string[string_index++];
    if(en_p == '-'){
      enpassant_square = -1;
    }else{
      int row = fen_string[string_index++] - '0' - 1;
      int column = en_p - 'a';

      int offset_enp = turn ? 1 : -1;

      enpassant_square = (row + offset_enp) * 8 + column;
    }

    //std::cout << enpassant_square << std::endl;

    string_index++;

    // semimosse e mosse
    
  };
};