#pragma once
#include "types.h"

// sets up the boards when first lunching the game
void gameInitialize(Board* board);

// checks if the enter coords are valid (no attacking twice, no ships that are in range of each other etc.)
bool checkForValidCoords(int x, int y, char orientation, int size);

// checks if the ships dont touch eachother
bool isInRangeOfShip(int x, int y, char orientation, int size, Board* TargetBoard);

// checks if we are not near a sunken ship
bool isNearWreckage(Board* board, int row, int col);

// add a ship to a board, returns the correct message if susseful or failed
enum MSG addShip(Board* targetBoard, Ship* ship, int x, int y);

// attacks a board in a given coord, returns the correct msg for hit\miss
enum MSG attack(Board* targetBoard, int x, int y);

// picks a random coord
void getRandomEmptyTile(Board* board, int* inputRow, int* inputCol);

// picks a random orientation (V\H)
char GetRandomOrientation();

// places all the ships at random
void autoPlaceRemainingShips(Board* board, int shipsRemaining, int startIndex);

// Set up phase
void setUpShips(Board* playerBoard, Board* enemyBoard);

// checks if there are no remaing ships on the given board
bool endGameCheck(Board* board);

// handles the enemy turn in the attack phase
void EnemyAttack(Board* enemyBoard, Board* playerBoard);

// handles the players turn in the attack phase
bool PlayerAttack(Board* enemyBoard, Board* playerBoard, gameStats* gameStats);

// handles the attack phase
bool AttackPhase(Board* playerBoard, Board* enemyBoard, gameStats* gameStats);

// prints the victory\lose screen
void printEndScreen(bool PlayerWon);

