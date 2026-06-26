#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

#include "MoveGen/initLUT.hpp"
#include "Core/pop_lsb.hpp"
#include "Board/position.hpp"
#include "Move/move_list.hpp"
#include "Move/move_stack.hpp"
#include "MoveGen/movegen.hpp"
#include "Move/make_move.hpp"
#include "Move/unmake_move.hpp"

#define WINDOW_SIZE 800.0f
#define TILE_SIZE 100.0f

void drawBitboard(sf::RenderWindow& window, uint64_t piece_bitboard, sf::Sprite& piece_sprite);
void drawMoves(sf::RenderWindow& window, uint64_t valid_moves_bitmap, sf::CircleShape& move_sprite);

int player = 0; // 0 for white and 1 for black

void drawBitboard(sf::RenderWindow& window, uint64_t piece_bitboard, sf::Sprite& piece_sprite, int invisible_square){

  if(invisible_square != -1){
    piece_bitboard = piece_bitboard & ~(1ULL << invisible_square);
  }

  while (piece_bitboard){

    int square = popLSB(piece_bitboard);

    if(square != invisible_square){
    int column = square % 8;
    int row = 7 - (square / 8);

    piece_sprite.setPosition(column * TILE_SIZE, row * TILE_SIZE);

    window.draw(piece_sprite);}
  }
}

void drawMoves(sf::RenderWindow& window, uint64_t valid_moves_bitmap, sf::CircleShape& move_sprite){

  while (valid_moves_bitmap){
    int square = popLSB(valid_moves_bitmap);

    int column = square % 8;
    int row = 7 - (square / 8);

    move_sprite.setPosition(column * TILE_SIZE + TILE_SIZE / 4, row * TILE_SIZE + TILE_SIZE / 4);

    window.draw(move_sprite);
  }
}

int main() {

  sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE, WINDOW_SIZE), "Chess");
  window.setFramerateLimit(60);

  sf::Cursor pointer_cursor;
  if(!pointer_cursor.loadFromSystem(sf::Cursor::Hand)){
    std::cout << "Errore nel caricamento cursore pointer." << std::endl;
  }
  sf::Cursor arrow_cursor;
  if(!arrow_cursor.loadFromSystem(sf::Cursor::Arrow)){
    std::cout << "Errore nel caricamento cursore arrow." << std::endl;
  }

  sf::RectangleShape tile_sprite[64];
  sf::Texture piece_texture;
  if (!piece_texture.loadFromFile("./sprites/chess_pieces_texture.png")){
    std::cout << "Errore nel caricamento sprite." << std::endl;
  }
  sf::Sprite piece_sprite[12];

  // tiles setup
  for(int i = 0; i < 64; i++){

    tile_sprite[i].setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
    tile_sprite[i].setPosition((i % 8) * TILE_SIZE, std::floor(i / 8) * TILE_SIZE);

    if(((i / 8) + (i % 8)) % 2 == 0){
      tile_sprite[i].setFillColor(sf::Color(240, 217, 181));
    }else{
      tile_sprite[i].setFillColor(sf::Color(181, 135, 98));
    }
  }

  // piece setup
  for(int i = 0; i < 12; i++){
    piece_sprite[i].setScale(sf::Vector2f(0.15625f, 0.15625f));
    piece_sprite[i].setTexture(piece_texture);
    
    if(i < 6){
      piece_sprite[i].setTextureRect(sf::IntRect((i % 6) * 640, 0, 640, 640));
    }else{
      piece_sprite[i].setTextureRect(sf::IntRect((i % 6) * 640, 640, 640, 640));
    }
  }

  // mouse pointer variable
  bool hovering = false;
  int mouse_hovering_square;

  // piece movement variables
  bool is_dragging = false;
  int start_square;
  int piece_to_move;

  int invisible_square = -1;

  uint64_t valid_moves_bitmap = 0;
  sf::CircleShape circle_move(25.f);
  circle_move.setFillColor(sf::Color(200, 0, 0));

  Position position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  MoveList move_list;
  MoveStack move_stack;

  fillPseudomovesLookupTable();

  // game rendering loop
  while (window.isOpen()){

    sf::Event event;
    while(window.pollEvent(event)){

      if(event.type == sf::Event::Closed){
        window.close();
      }

      if(event.type == sf::Event::MouseButtonPressed){
        if(event.mouseButton.button == sf::Mouse::Left){

          // check bounds
          int start_row = 7 - std::floor((event.mouseButton.y / TILE_SIZE));
          int start_column = event.mouseButton.x / TILE_SIZE;

          if(start_row < 0 || start_row > 7 || start_column < 0 || start_column > 7){
            continue;
          }

          start_square = start_row * 8 + start_column;

          if((position.occupied_squares_bitboard[position.turn] & (1ULL << start_square))){

            is_dragging = true;
            invisible_square = start_square;

            piece_to_move = position.board[start_square];
 
            generateLegalMoves(position, move_list);

            /*DEBUG
            for(int i = 0; i < move_list.head; i++){
              int start_square = move_list.moves[i] >> 10;
              int end_square = (move_list.moves[i] >> 4) & 0x3f;

              int start_row = start_square / 8;
              int start_column = start_square % 8;

              int end_row = end_square / 8;
              int end_column = end_square % 8;


              char start_column_letter = start_column + 97;
              char end_column_letter = end_column + 97;
              start_row++; end_row++;

              int piece_char = pieceAtSquare(start_square, position) + 9812;
              std::cout << "Move: " << utf8(piece_char) << end_column_letter << end_row << std::endl;
              std::cout << "Flags: " << (int)(move_list.moves[i] & 0xf) << std::endl;

              std::cout << "Move: " << start_column_letter << start_row << "-" << end_column_letter << end_row << std::endl;
              std::cout << "Flags: " << (int)(move_list.moves[i] & 0xf) << std::endl;
                  
            }

            std::cout << std::endl << std::endl;
            */      

            for(int i = 0; i < move_list.head; i++){
              if(((move_list.moves[i] & 0xfc00) >> 10) == start_square){
                valid_moves_bitmap |= (1ULL << ((move_list.moves[i] >> 4) & 0x3f));
              }
            }
          }
        }
      }

      if(event.type == sf::Event::MouseButtonReleased){
        if(event.mouseButton.button == sf::Mouse::Left && is_dragging){

          // check bounds
          int end_row = 7 - std::floor(event.mouseButton.y / TILE_SIZE);
          int end_column = event.mouseButton.x / TILE_SIZE;

          int end_square = end_row * 8 + end_column;

          if(!(end_row < 0 || end_row > 7 || end_column < 0 || end_column > 7 || !((1ULL << end_square) & valid_moves_bitmap))){

            for(int i = 0; i < move_list.head; i++){
              if(((move_list.moves[i] >> 10) == start_square && ((move_list.moves[i] & 0x3f0) >> 4) == end_square)){

                // check promotion
                if(move_list.moves[i] & 0x8){

                  int piece_to_prom = -1;
                  char prom;

                  std::cout << "Quale pezzo utilizzare per la promozione? [q/r/b/n]: " << std::endl;
                  std::cin >> prom;

                  switch (prom){
                    case 'q':
                      piece_to_prom = 0;
                      break;
                    case 'b':
                      piece_to_prom = 1;
                      break;
                    case 'n':
                      piece_to_prom = 2;
                      break;
                    case 'r':
                      piece_to_prom = 3;
                      break;
                    default:
                      break;
                  }

                  move_list.moves[i] = (move_list.moves[i] & 0xfffc) | piece_to_prom;
                }
                
                makeMove(position, move_list.moves[i], move_stack);

                break;
              }
            }
          }

          move_list.head = 0;
          is_dragging = false;
          valid_moves_bitmap = 0;
          invisible_square = -1;
        }
      }

      if(event.type == sf::Event::KeyPressed){

        if(event.key.code == sf::Keyboard::BackSpace){
          unmakeMove(position, move_stack);
        }
      }
    }

    sf::Vector2i mouse_local_position = sf::Mouse::getPosition(window);
    mouse_hovering_square = (7 - std::floor(mouse_local_position.y / TILE_SIZE)) * 8 + (mouse_local_position.x / TILE_SIZE);

    // clear the window with black color
    window.clear(sf::Color::Black);

    // draw tiles
    for(int i = 0; i < 64; i++){
      window.draw(tile_sprite[i]);
    }

    // draw pieces
    for(int i = 0; i < 12; i++){
      drawBitboard(window, position.piece_bitboard[i], piece_sprite[i], invisible_square);
    }

    // draw moves
    drawMoves(window, valid_moves_bitmap, circle_move);

    // check if mouse is hovering over a piece
    if((position.occupied_squares_bitboard[position.turn]) & (1ULL << mouse_hovering_square)){
      hovering = true;
    }else{
      hovering = false;
    }

    if(is_dragging){
      hovering = true;
      piece_sprite[piece_to_move].setPosition(sf::Vector2f(mouse_local_position.x - TILE_SIZE/2, mouse_local_position.y - TILE_SIZE/2));
      window.draw(piece_sprite[piece_to_move]);
    }

    if(hovering){
      window.setMouseCursor(pointer_cursor);
    }else{
      window.setMouseCursor(arrow_cursor);
    }

    // end the current frame
    window.display();
  }

  return 0;
}