#pragma once

#include "../Board/position.hpp"
#include "../Move/move_stack.hpp"

void makeMove(Position& position, uint16_t move, MoveStack& move_stack);
