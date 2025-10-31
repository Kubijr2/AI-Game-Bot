#include "brain.h"

// Constructor
Brain::Brain()
{
}

// Main decision function to return nextMove
int Brain::getNextMove(GameState &gamestate)
{
    std::vector<std::vector<char>> vision = gamestate.vision;

    int playerRow = -1, playerCol = -1;

    // Locate the player character in the vision grid
    for(size_t row = 0; row < vision.size(); row++)
    {
        for(size_t col = 0; col < vision[row].size(); col++)
        {
            char tile = vision[row][col];
            if(tile == 'v' || tile == '^' || tile == '>' || tile == '<')
            {
                playerRow = static_cast<int>(row);
                playerCol = static_cast<int>(col);
                break;
            }
        }
        if(playerRow != -1)
            break;
    }

    // If the player is not found, do nothing
    if(playerRow == -1 || playerCol == -1)
        return 0;
    
    // Determine what stage the AI is currently in
    int stageType = checkStage(gamestate.vision, playerRow, playerCol);

    // Only update current stage if it is not maze(needed this for some weird bugs)
    if(stageType != currentStage && stageType != 1)
        currentStage = stageType;
    
    // Call function for identified stage
    if(currentStage == 1)
        return handleMaze(gamestate, playerRow, playerCol);
    else if(currentStage == 2)
        return handleFood(gamestate, playerRow, playerCol);
    else if(currentStage == 4)
        return handleFlag(gamestate, playerRow, playerCol);

    // Default return
    return 0;
}

// Checks adjacent tiles to determine the current stage
int Brain::checkStage(const std::vector<std::vector<char>> &vision, int playerRow, int playerCol)
{
    // Look at adjacent tiles
    char right = (playerCol + 1 < static_cast<int>(vision[0].size())) ? vision[playerRow][playerCol + 1] : '+';
    char left = (playerCol - 1 >= 0) ? vision[playerRow][playerCol - 1] : '+';
    char down = (playerRow + 1 < static_cast<int>(vision.size())) ? vision[playerRow + 1][playerCol] : '+';
    char up = (playerRow - 1 >= 0) ? vision[playerRow - 1][playerCol] : '+';

    std::vector<char> adjacent = {right, left, down, up};
    
    // Identify stage based on certain characters in surrounding tiles
    for(char tile : adjacent)
    {
        if(tile == '0') // Food stage
            return 2;
        if(tile == 'A'||tile == 'B') // Flag stage
            return 4;
        if(tile == 'X') // Enemy Stage
            return 5;
        if(tile == 'T') // Trp Stage
            return 6;
    }

    return 1;
}

// Function to handle maze movement(stages 1-3)
int Brain::handleMaze(const GameState & gamestate, int playerRow, int playerCol)
{
    std::vector<std::vector<char>> vision = gamestate.vision;
    int absRow = gamestate.pos[0];
    int absCol = gamestate.pos[1];

    // Track the tiles as they are visited
    visitedTiles.insert({absRow, absCol});

    // Count how many directions are open
    int openPaths = 0;

    // Count Right
    if (playerCol + 1 < static_cast<int>(vision[0].size()) &&
        vision[playerRow][playerCol + 1] != '+' &&
        vision[playerRow][playerCol + 1] != 'D' &&
        vision[playerRow][playerCol + 1] != 'T' &&
        !(vision[playerRow][playerCol + 1] == 'B' && !hasFlag) &&
        isAllowed(absRow, absCol + 1))
        openPaths++;
    
    // Count down
    if (playerRow + 1 < static_cast<int>(vision.size()) &&
        vision[playerRow + 1][playerCol] != '+' &&
        vision[playerRow + 1][playerCol] != 'D' &&
        vision[playerRow + 1][playerCol] != 'T' &&
        !(vision[playerRow + 1][playerCol] == 'B' && !hasFlag) &&
        isAllowed(absRow + 1, absCol))
        openPaths++;

    // Count Up
    if (playerRow - 1 >= 0 &&
        vision[playerRow - 1][playerCol] != '+' &&
        vision[playerRow - 1][playerCol] != 'D' &&
        vision[playerRow - 1][playerCol] != 'T' &&
        !(vision[playerRow - 1][playerCol] == 'B' && !hasFlag) &&
        isAllowed(absRow - 1, absCol))
        openPaths++;

    // Count Left
    if (playerCol - 1 >= 0 &&
        vision[playerRow][playerCol - 1] != '+' &&
        vision[playerRow][playerCol - 1] != 'D' &&
        vision[playerRow][playerCol - 1] != 'T' &&
        !(vision[playerRow][playerCol - 1] == 'B' && !hasFlag) &&
        isAllowed(absRow, absCol - 1))
        openPaths++;

    // Avoid re-entering known dead ends
    if(deadEndTiles.count({absRow, absCol}) > 0 && lastMove != 0)
        return (lastMove == 1) ? 3 : (lastMove == 3) ? 1 : (lastMove == 2) ? 4 : 2;

    // Save checkpoint when changing to a non-maze stage
    if(stageCheckpoint.first == -1 && checkStage(vision, playerRow, playerCol) != 1)
        stageCheckpoint = {absRow, absCol};

    // Try moving right
    if (playerCol + 1 < static_cast<int>(vision[0].size()) &&
        vision[playerRow][playerCol + 1] != '+' &&
        vision[playerRow][playerCol + 1] != 'D' &&
        vision[playerRow][playerCol + 1] != 'T' &&
        !(vision[playerRow][playerCol + 1] == 'B' && !hasFlag) &&
        isAllowed(absRow, absCol + 1))
        return lastMove = 4;

    // Try moving down
    if (playerRow + 1 < static_cast<int>(vision.size()) &&
        vision[playerRow + 1][playerCol] != '+' &&
        vision[playerRow + 1][playerCol] != 'D' &&
        vision[playerRow + 1][playerCol] != 'T' &&
        !(vision[playerRow + 1][playerCol] == 'B' && !hasFlag) &&
        isAllowed(absRow + 1, absCol))
        return lastMove = 3;

    // Try moving up
    if (playerRow - 1 >= 0 &&
        vision[playerRow - 1][playerCol] != '+' &&
        vision[playerRow - 1][playerCol] != 'D' &&
        vision[playerRow - 1][playerCol] != 'T' &&
        !(vision[playerRow - 1][playerCol] == 'B' && !hasFlag) &&
        isAllowed(absRow - 1, absCol))
        return lastMove = 1;

    // Try moving left
    if (playerCol - 1 >= 0 &&
        vision[playerRow][playerCol - 1] != '+' &&
        vision[playerRow][playerCol - 1] != 'D' &&
        vision[playerRow][playerCol - 1] != 'T' &&
        !(vision[playerRow][playerCol - 1] == 'B' && !hasFlag) &&
        isAllowed(absRow, absCol - 1))
        return lastMove = 2;

    if(openPaths <= 1)
    {
        deadEndTiles.insert({absRow, absCol});
    }
    
    // If all directions are visited or blocked, allow revisiting
    if (playerCol + 1 < static_cast<int>(vision[0].size()) &&
        vision[playerRow][playerCol + 1] != '+' &&
        vision[playerRow][playerCol + 1] != 'D' &&
        !(vision[playerRow][playerCol + 1] == 'B' && !hasFlag) &&
        vision[playerRow][playerCol + 1] != 'T')
        return lastMove = 4;

    if (playerRow + 1 < static_cast<int>(vision.size()) &&
        vision[playerRow + 1][playerCol] != '+' &&
        vision[playerRow + 1][playerCol] != 'D' &&
        !(vision[playerRow + 1][playerCol] == 'B' && !hasFlag) &&
        vision[playerRow + 1][playerCol] != 'T')
        return lastMove = 3;

    if (playerRow - 1 >= 0 &&
        vision[playerRow - 1][playerCol] != '+' &&
        vision[playerRow - 1][playerCol] != 'D' &&
        !(vision[playerRow - 1][playerCol] == 'B' && !hasFlag) &&
        vision[playerRow - 1][playerCol] != 'T')
        return lastMove = 1;

    if (playerCol - 1 >= 0 &&
        vision[playerRow][playerCol - 1] != '+' &&
        vision[playerRow][playerCol - 1] != 'D' &&
        !(vision[playerRow][playerCol - 1] == 'B' && !hasFlag) &&
        vision[playerRow][playerCol - 1] != 'T')
        return lastMove = 2;

    return lastMove = 0;
}

// Prevents AI from revisiting a tile or backtracking past a checkpoint
bool Brain::isAllowed(int newRow, int newCol)
{
    std::pair<int, int> pos(newRow, newCol);

    if(visitedTiles.count(pos) > 0)
        return false;
    if(deadEndTiles.count(pos) > 0)
        return false;
    if(stageCheckpoint.first != -1)
    {
        // Prevent backtracking past the checkpoint
        if(newRow < stageCheckpoint.first || (newRow == stageCheckpoint.first && newCol < stageCheckpoint.second))
            return false;
    }
    return true;
}

// Handles food collection in Stage 2
int Brain::handleFood(const GameState & gamestate, int playerRow, int playerCol)
{
    std::vector<std::vector<char>> vision = gamestate.vision;

    // Check all 4 adjacent tiles for food
    if(playerCol + 1 < static_cast<int>(vision[0].size()) && vision[playerRow][playerCol + 1] == '0')
        return lastMove = 4;
    if(playerRow + 1 < static_cast<int>(vision.size()) && vision[playerRow + 1][playerCol] == '0')
        return lastMove = 3;
    if(playerRow - 1 >= 0 && vision[playerRow - 1][playerCol] == '0')
        return lastMove = 1;
    if(playerCol - 1 >= 0 && vision[playerRow][playerCol - 1] == '0')
        return lastMove = 2;

    // If no adjacent food, move toward the closest visible one
    for(int row = 0; row < static_cast<int>(vision.size()); row++)
    {
        for(int col = 0; col < static_cast<int>(vision[row].size()); col++)
        {
            if(vision[row][col] == '0')
            {
                if(row < playerRow)
                    return lastMove = 1;
                if(row > playerRow)
                    return lastMove = 3;
                if(col < playerCol)
                    return lastMove = 2;
                if(col > playerCol)
                    return lastMove = 4;
            }
        }
    }

    // If no food in vision, just explore like a maze
    return handleMaze(gamestate, playerRow, playerCol);
}

// Handles picking and dropping off flag in Stage 4
int Brain::handleFlag(const GameState & gamestate, int playerRow, int playerCol)
{
    std::vector<std::vector<char>> vision = gamestate.vision;

    // Determine whether AI should be looking for A or B
    char target = 'A';
    if(hasFlag && !flagDelivered)
        target = 'B';

    // Once flag is delivered, return to maze logic
    if(hasFlag && flagDelivered)
        return handleMaze(gamestate, playerRow, playerCol);

    // Check all 4 direcitons for target tile(A or B)
    if(playerCol + 1 < static_cast<int>(vision[0].size()) && vision[playerRow][playerCol + 1] == target)
    {
        if(target == 'A')
        {
            hasFlag = true;
            visitedTiles.clear();
        }
        else if(target == 'B')
        {
            flagDelivered = true;
            visitedTiles.clear();
        }
        return lastMove = 4;
    }
    if(playerRow + 1 < static_cast<int>(vision.size()) && vision[playerRow + 1][playerCol] == target)
    {
        if(target == 'A')
        {
            hasFlag = true;
            visitedTiles.clear();
        }
        else if(target == 'B')
        {
            flagDelivered = true;
            visitedTiles.clear();
        }
        return lastMove = 3;
    }
    if(playerRow - 1 >= 0 && vision[playerRow - 1][playerCol] == target)
    {
        if(target == 'A')
        {
            hasFlag = true;
            visitedTiles.clear();
        }
        else if(target == 'B')
        {
            flagDelivered = true;
            visitedTiles.clear();
        }
        return lastMove = 1;
    }
    if(playerCol - 1 >= 0 && vision[playerRow][playerCol - 1] == target)
    {
        if(target == 'A')
        {
            hasFlag = true;
            visitedTiles.clear();
        }
        else if(target == 'B')
        {
            flagDelivered = true;
            visitedTiles.clear();
        }
        return lastMove = 2;
    }

    // If not adjacent, explore like a maze until it comes into view
    return handleMaze(gamestate, playerRow, playerCol);
}