#ifndef BRAIN_H
#define BRAIN_H

#include <string>
#include "../Game/game.h"
#include <cstdlib>
#include <ctime>
#include <set>

class Brain
{
public:
    Brain();                               // Constructor
    int getNextMove(GameState &gamestate); // Returns the next move for the AI
private:
    // Keep current position of the AI in the vision grid
    int playerRow = -1; 
    int playerCol = -1;

    // Keep the last move made by the AI
    int lastMove = 0; 

    // Track last and previous absolute positions to avoid loops and backtracking
    int lastRow = -1;
    int lastCol = -1;
    int prevRow = -1;
    int prevCol = -1;

    // Keep track of tiles previously visited
    std::set<std::pair<int, int>> visitedTiles;

    // Stores the position where the stage is changed to avoid going back into a previous stage
    std::pair<int, int> stageCheckpoint{-1, -1};

    // Determines what stage the AI is currently in
    int checkStage(const std::vector<std::vector<char>> &vision, int playerRow, int playerCol);

    // Handles the movement in maze stages
    int handleMaze(const GameState & gamestate, int playerRow, int playerCol);

    // Checks if a tile at a certain position is able to be moved into
    bool isAllowed(int newRow, int newCol);

    // Handles food collection stage
    int handleFood(const GameState & gamestate, int playerRow, int playerCol);

    // Stores what stage the Ai is in
    int currentStage = 1;

    // Determines flag stage status
    bool hasFlag = false;
    bool flagDelivered = false;

    // Handles flag stage
    int handleFlag(const GameState & gamestate, int playerRow, int playerCol);

    // To keep track of any maze dead ends
    std::set<std::pair<int, int>> deadEndTiles;
};

#endif // BRAIN_H