#include "types.h"
#include "graphics_and_ui.h"
#include "enemy_behavior.h"
#include "gameplay.h"
#include "colors.h"
#include "Save&load.h"
#include <time.h> // for srand

int main()
{
    srand(time(NULL)); // Randomize numbers for the game

    Board playerBoard;
    Board enemyBoard;

    // Step 1: Create the player struct
    Player currentPlayer = playerLoginMenu();

    // Step 2: Create a new Stats struct for this battle
    gameStats battleStats = { 0 };  // Initialize all fields to zero

    // Step 3: Get difficulty from the player
    enum compLV LV = selectLV(currentPlayer.rank);

    // Step 4: Initialize AI
    AIState ai = { .hunting = false, .Lv = LV, .lastHitX = -1, .lastHitY = -1, .currentDirection = -1, .usedSemiCheat = false,0,0 };
    enemyBoard.Aistate = ai;

    // Step 5: Initialize boards
    gameInitialize(&playerBoard);
    gameInitialize(&enemyBoard);

    // Step 6: Setup ships
    setUpShips(&playerBoard, &enemyBoard);

    // Step 7: Battle phase (PASS battleStats pointer)
    AttackPhase(&playerBoard, &enemyBoard, &battleStats);

    // After battle ends, check who won:
    if (endGameCheck(&enemyBoard)) // Enemy's ships all sunk
    {
        printEndScreen(true);
        endGame(&currentPlayer, &battleStats, LV);
        ShowScoreBoard();
    }
    else {
        printEndScreen(false);
    }

    return 0;
}
