#pragma once

#include "types.h" // Because we need 'Board' type for drawing functions
#include <stdbool.h> // For functions returning bool

// Clears the screen
void clearScreen();

// prints in color!
void printc(const char* color, const char* format, ...);

// prints slowly like in an rpg!
void printSlow(const char* color, const char* text, int delayMilliseconds);

// Draws the title headers (A B C D ...) 
void drawBoardHeader();

// Draws a single row of the board
void drawSingleBoard(Board* board, int rowIndex, bool hideShips);

// Updates and redraws the entire player and enemy boards
void updateBoard(Board* playerBoard, Board* enemyBoard);

// Prints game messages (like "Hit!" or "Miss!")
void printMessage(enum MSG msg);

// Converts a number to a corresponding letter (e.g., 1 -> A, 2 -> B)
char numberToChar(int num);

// Gets player input (e.g., B3 or 3B), returns true if valid
bool GetPlayerInput(int* inputRow, int* inputCol);

// Gets player input for ship orientation (H or V)
char getPlayerOrientation();

// Swaps input if the player entered something like 5C instead of C5
void swap(char* inputColChar, int* inputRow);

