#pragma once

#include <cstdint>

#define MAX_DEPTH 512

struct alignas(64) MoveList {

  uint16_t moves[MAX_DEPTH * 128] = {0};

  // DEBUG
  //uint16_t moves[2000000] = {0};

  int head = 0;

};