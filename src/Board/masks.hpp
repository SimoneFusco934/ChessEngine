#pragma once

#include <cstdint>

namespace Masks {
  inline constexpr uint64_t FILE_A = 0x0101010101010101;
  inline constexpr uint64_t FILE_B = 0x0202020202020202;
  inline constexpr uint64_t FILE_G = 0x4040404040404040; 
  inline constexpr uint64_t FILE_H = 0x8080808080808080;
  
  inline constexpr uint64_t RANK_FIRST = 0x00000000000000ff;
  inline constexpr uint64_t RANK_SECOND = 0x000000000000ff00;
  inline constexpr uint64_t RANK_SEVENTH = 0x00ff000000000000;
  inline constexpr uint64_t RANK_EIGHTH = 0xff00000000000000;
}