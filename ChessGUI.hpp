#pragma once 

#include <SFML/Graphics.hpp>

void drawBitboard(sf::RenderWindow& window, uint64_t piece_bitboard, sf::Sprite& piece_sprite);
void drawMoves(sf::RenderWindow& window, uint64_t valid_moves_bitmap, sf::CircleShape& move_sprite);

 


