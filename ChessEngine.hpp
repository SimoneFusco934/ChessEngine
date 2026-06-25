#pragma once 

#include <immintrin.h>
#include "ChessUtilities.hpp"
 
size_t rookRelevantOccupancyMask(int square);
size_t bishopRelevantOccupancyMask(int square);

uint64_t rookPseudolegalMoves(int square, uint64_t occupancy);
uint64_t bishopPseudolegalMoves(int square, uint64_t occupancy);

void fillPseudomovesLookupTable();

void generateLegalMoves(Position& position, MoveList& move_list);
void generateUnpinnedPawnLegalMoves(Position& position, MoveList& move_list);
void generatePinnedPawnLegalMoves(Position& position, MoveList& move_list);

uint64_t getAttackedSquares(Position& position);

uint64_t calculateMoves(int square, int piece, const Position& position);
void makeMove(Position& position, uint16_t move, MoveStack& move_stack);
void unmakeMove(Position& position, MoveStack& move_stack);

void makeMoveCopy(Position& position, uint16_t move);