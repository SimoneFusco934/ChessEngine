#pragma once

#include "../Board/position.hpp"
#include "../Move/move_list.hpp"

void generateUnpinnedWhitePawnLegalMoves(Position& position, MoveList& move_list);
void generatePinnedWhitePawnLegalMoves(Position& position, MoveList& move_list);
void generateUnpinnedCheckWhitePawnLegalMoves(Position& position, uint64_t check_ray, MoveList& move_list);
void generatePinnedCheckWhitePawnLegalMoves(Position& position, uint64_t check_ray, MoveList& move_list);

void generateUnpinnedBlackPawnLegalMoves(Position& position, MoveList& move_list);
void generatePinnedBlackPawnLegalMoves(Position& position, MoveList& move_list);
void generateUnpinnedCheckBlackPawnLegalMoves(Position& position, uint64_t check_ray, MoveList& move_list);
void generatePinnedCheckBlackPawnLegalMoves(Position& position, uint64_t check_ray, MoveList& move_list);

void generateLegalMoves(Position& position, MoveList& move_list);

uint64_t getAttackedSquares(Position& position);
