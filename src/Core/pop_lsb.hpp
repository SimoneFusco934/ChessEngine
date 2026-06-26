#pragma once

#include <cstdint>

inline int popLSB(uint64_t& bb) {
  int index = __builtin_ctzll(bb); // conta gli zeri finali
  bb &= bb - 1;                    // rimuove il bit meno significativo
  return index;
}