#pragma once
#include "types.h"

// ====================
// AI Trait Functions
// ====================

// Shoots randomly at a valid unexplored tile
bool randomShoot(Board* playerBoard, int* inputRow, int* inputCol);

// Hunts around last known hit (nearby tiles)
bool huntAdjacent(Board* enemyBoard, Board* playerBoard, int* inputRow, int* inputCol);

// After 2 hits, follows the ship's direction
bool followShipDirection(Board* enemyBoard, Board* playerBoard, int* inputRow, int* inputCol);

// In Nightmare mode, targets visible ships ('S') directly
bool perfectTargeting(Board* playerBoard, int* inputRow, int* inputCol);

// After losing most ships, cheats once to find a ship
bool semiCheatOnLastShip(Board* enemyBoard, Board* playerBoard, int* inputRow, int* inputCol);

// After many misses, cheats once to find a ship
bool peekAfterMissStreak(Board* enemyBoard, Board* playerBoard, int* inputRow, int* inputCol, int missThreshold);

// Checks if a cell is near wreckage (avoid shooting there)
bool isNearWreckage(Board* board, int row, int col);

// ====================
// Difficulty Handling
// ====================

// AI behavior pipeline for Easy difficulty
bool tryEasyDifficulty(Board* enemyBoard, Board* playerBoard, int* inputRow, int* inputCol);

// AI behavior pipeline for Medium difficulty
bool tryMediumDifficulty(Board* enemyBoard, Board* playerBoard, int* inputRow, int* inputCol);

// AI behavior pipeline for Hard difficulty
bool tryHardDifficulty(Board* enemyBoard, Board* playerBoard, int* inputRow, int* inputCol);

// AI behavior pipeline for Nightmare difficulty
bool tryNightmareDifficulty(Board* enemyBoard, Board* playerBoard, int* inputRow, int* inputCol);

// ====================
// General AI Management
// ====================

// Updates AI memory after each attack (hit, miss, sunk)
void updateAIState(Board* enemyBoard, enum MSG result, int inputRow, int inputCol);

// Main function that handles the enemy's turn
void EnemyAttack(Board* enemyBoard, Board* playerBoard);
