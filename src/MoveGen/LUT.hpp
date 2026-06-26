#pragma once

#include <cstdint>

// rook relevant occupancy combinations: 4 * 2**(12) + 24 * 2**(11) + 36 * 2**(10) = 102.400
// bishop relevant occupancy combinations: 2 * 2**(6) + 6 * 2**(7) + 2 * 2**(9) + 22 * 2**(5) = 2.624 * 2 = 5.248
// knight relevant occupancy combinations: 64 * 2**(0) = 64
// king relevant occupancy combinations: 64 * 2**(0) = 64
inline uint64_t pseudomoves_lookup_table[107776]; // 862.208 kb -- 0.862 mb
inline int rook_pseudomoves_lookup_table_offsets[64];
inline int bishop_pseudomoves_lookup_table_offsets[64];
inline int knight_pseudomoves_lookup_table_offsets;
inline int king_pseudomoves_lookup_table_offsets;

inline uint64_t rook_relevant_occupancy_mask[64];
inline uint64_t bishop_relevant_occupancy_mask[64];