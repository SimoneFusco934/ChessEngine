#include "ChessUtilities.hpp"

int pieceAtSquare(int square, Position& position){

  uint64_t square_bit = (1ULL << square);
  int piece_type = -1;
 
  if(position.occupied_squares_bitboard[0] & square_bit){
    for(int i = 0; i < 6; i++){
      if(position.piece_bitboard[i] & square_bit){
        piece_type = i;
      }
    }
  }else if(position.occupied_squares_bitboard[1] & square_bit){
    for(int i = 6; i < 12; i++){
      if(position.piece_bitboard[i] & square_bit){
        piece_type = i;
      }
    }
  }

  return piece_type;
}

int popLSB(uint64_t &bb) {
  int index = __builtin_ctzll(bb); // conta gli zeri finali
  bb &= bb - 1;                    // rimuove il bit meno significativo
  return index;
}

std::string utf8(char32_t cp)
{
    std::string s;

    s += char(0xE0 | ((cp >> 12) & 0x0F));
    s += char(0x80 | ((cp >> 6) & 0x3F));
    s += char(0x80 | (cp & 0x3F));

    return s;
}