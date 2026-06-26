CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -march=native -O3 -flto

SFMLFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

TARGET = ChessEngine
TEST = PerftTest

SRC = $(wildcard src/*.cpp) \
      $(wildcard src/Board/*.cpp) \
      $(wildcard src/Core/*.cpp) \
      $(wildcard src/Eval/*.cpp) \
      $(wildcard src/Move/*.cpp) \
      $(wildcard src/MoveGen/*.cpp) \
      $(wildcard src/Search/*.cpp) \
      $(wildcard src/UCI/*.cpp)

SRC_TEST = $(wildcard src/Benchmark/*.cpp) \
			$(wildcard src/Board/*.cpp) \
      $(wildcard src/Core/*.cpp) \
      $(wildcard src/Eval/*.cpp) \
      $(wildcard src/Move/*.cpp) \
      $(wildcard src/MoveGen/*.cpp) \
      $(wildcard src/Search/*.cpp) \
      $(wildcard src/UCI/*.cpp)

OBJ = $(SRC:.cpp=.o)
OBJ_TEST = $(SRC_TEST:.cpp=.o)

all: $(TARGET) $(TEST)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) $(SFMLFLAGS)

$(TEST): $(OBJ_TEST)
	$(CXX) $(OBJ_TEST) -o $(TEST)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
	rm -f $(OBJ_TEST) $(TEST)

run: $(TARGET)
	./$(TARGET)

test: $(TEST)
	./$(TEST)