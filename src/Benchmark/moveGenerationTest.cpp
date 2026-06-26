#include <iostream>
#include <chrono>
#include <iomanip>
#include "../Board/position.hpp"
#include "../Move/move_list.hpp"
#include "../Move/move_stack.hpp"
#include "../Move/make_move.hpp"
#include "../Move/unmake_move.hpp"
#include "../MoveGen/movegen.hpp"
#include "../MoveGen/initLUT.hpp"

#define DEPTH 7
#define FEN_STRING "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

std::string moveToString(uint16_t move){

  int square_from = move >> 10;
  int square_to = (move & 0x3f0) >> 4;

  int start_row = square_from / 8;
  int start_column = square_from % 8;

  int end_row = square_to / 8;
  int end_column = square_to % 8;

  char start_column_letter = start_column + 97;
  char end_column_letter = end_column + 97;
  start_row++; end_row++;

  std::string str = start_column_letter + std::to_string(start_row) + "-" + end_column_letter + std::to_string(end_row);

  return str;
}

// SINGLE COUNT PERFT
uint64_t perftSingle(Position& position, MoveList& move_list, MoveStack& move_stack, int depth){

  if(depth == 0){
      return 1ULL;
  } 

  uint64_t nodes = 0;

  uint64_t parents_moves = move_list.head;

  generateLegalMoves(position, move_list);

  for(int i = parents_moves; i < move_list.head; i++){

    int move = move_list.moves[i];

    makeMove(position, move, move_stack);

    uint64_t child_nodes = perftSingle(position, move_list, move_stack, depth - 1);

    if(depth == DEPTH){
      std::cout << "Nodi figli di " << moveToString(move) << ": " << child_nodes << std::endl;
    }

    nodes += child_nodes;

    unmakeMove(position, move_stack);
  }

  move_list.head = parents_moves;

  return nodes;
}


// BULK COUNT PERFT
uint64_t perftBulk(Position& position, MoveList& move_list,  MoveStack& move_stack, int depth){

  uint64_t nodes = 0;

  uint64_t parents_moves = move_list.head;

  generateLegalMoves(position, move_list);

  if(depth == 1){
    nodes += (move_list.head - parents_moves);
    move_list.head = parents_moves;
    return nodes;
  } 

  for(int i = parents_moves; i < move_list.head; i++){

    int move = move_list.moves[i];

    makeMove(position, move, move_stack);

    uint64_t child_nodes = perftBulk(position, move_list, move_stack, depth - 1);

    //if(depth == DEPTH){
    //  std::cout << "Nodi figli di " << moveToString(move) << ": " << child_nodes << std::endl;
    //}

    nodes += child_nodes;

    unmakeMove(position, move_stack);
  }

  move_list.head = parents_moves;

  return nodes;
}

// KIWI
// r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 

int main(){
  
  Position position(FEN_STRING);
  MoveList move_list;
  MoveStack move_stack;

  fillPseudomovesLookupTable();

  auto start = std::chrono::high_resolution_clock::now();

  uint64_t nodes = perftBulk(position, move_list, move_stack, DEPTH);

  auto end = std::chrono::high_resolution_clock::now();

  auto duration =
  std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  double nps = nodes / (duration.count() / 1000.0);

  std::cout << "Nodes: " << nodes << std::endl;
  std::cout << "Time: " << duration.count() << " ms" << std::endl;
  std::cout << "NPS: " << std::fixed << std::setprecision(0) << nps << std::endl;
}


