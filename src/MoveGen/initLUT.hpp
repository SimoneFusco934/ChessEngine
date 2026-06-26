#pragma once

#include <cstddef>
#include <cstdint>

/*
* Generates a Bitboard indicating all the squares 
* the rook could potentially travel to, if the board was completely empty,
* except for the squares at the edge of the board.
* The Bitboard will be used to fill rook_relevant_occupancy_mask.
* Returns the total number of occupancy combinations for that mask.
*/
size_t rookRelevantOccupancyMask(int square);

/*
* Generates a Bitboard indicating all the squares 
* the bishop could potentially travel to, if the board was completely empty,
* except for the squares at the edge of the board.
* The Bitboard will be used to fill bishop_relevant_occupancy_mask.
* Returns the total number of occupancy combinations for that mask.
*/
size_t bishopRelevantOccupancyMask(int square);

/*
* Generates a Bitboard indicating all the squares 
* the rook could potentially go to assuming enemy-only occupancy.
*/
uint64_t rookPseudolegalMoves(int square, uint64_t occupancy);

/*
* Generates a Bitboard indicating all the squares 
* the bishop could potentially go to assuming enemy-only occupancy.
*/
uint64_t bishopPseudolegalMoves(int square, uint64_t occupancy);

/*
* Generates a Bitboard indicating all the squares 
* the knight could potentially go to assuming enemy-only occupancy.
*/
uint64_t knightPseudolegalMoves(int square);

/*
* Generates a Bitboard indicating all the squares 
* the king could potentially go to assuming enemy-only occupancy.
*/
uint64_t kingPseudolegalMoves(int square);

/*
* Fills pseudomoves_lookup_table and the relative offsets.
*/
void fillPseudomovesLookupTable();