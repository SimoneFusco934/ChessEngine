CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -march=native -O3 -march=native -flto

SFMLFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

TARGET = ChessAI

SRC = ChessGUI.cpp ChessEngine.cpp ChessUtilities.cpp

OBJ = $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) $(SFMLFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	./$(TARGET)