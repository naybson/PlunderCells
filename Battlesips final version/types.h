#pragma once // tells the compiles to include this files only once!

#include <stdio.h>
#include <stdbool.h>

#define SLEEP(seconds) Sleep((seconds) * 1000) // wait for one sec

#define BOARDSIZE 10 // Determens the size of the board

// Type animations
#define TYPE_SLOW 200
#define TYPE_FAST 20
#define TYPE_SUPERFAST 10

// Ship sizes and num
#define SMALL_SHIP_SIZE 2 
#define SMALL_SHIP_NUM 1

#define MEDUIM_SHIP_SIZE 3
#define MEDUIM_SHIP_NUM 4

#define LARGE_SHIP_SIZE 4
#define LARGE_SHIP_NUM 2

#define TOTAL_SHIPS (LARGE_SHIP_NUM + MEDUIM_SHIP_NUM + SMALL_SHIP_NUM)



enum compLV
{
	EASY,
	MEDUIM,
	HARD,
	NIGHTMARE,
	PLAYER
};

enum Rank
{
	Scout,
	Cannoneer,
	Captain,
	Legendary
};

enum MSG
{
	/// enums for all the diffrent messages

	MSG_HIT,
	MSG_MISS,
	MSG_SUNK,
	MSG_ERROR_OUT_OF_BOUNDS,
	MSG_ERROR_IN_RANGE,
	MSG_PLACE_SHIP_SUCCESS,
	MSG_EMPTY,
	MSG_ALREADY_ATTACKED
};



typedef struct {
	bool hunting;
	enum CompLV Lv;
	int lastHitX, lastHitY;
	int currentDirection;   // -1 = not currently targeting
	bool triedDirections[4]; // Track tried directions [down, up, right, left]
	int secondHitX, secondHitY;
	bool reversedOnce;
	bool usedSemiCheat;   // will be true if the ship used its semi cheat ability
	int missStreak; // counts how many consecutive misses
	int shipSunk;
} AIState;

typedef struct {
	int size; // Size of the ship (e.g., 5 for Battleship) 
	int hits; // Number of hits the ship has taken 
	char orientation;  // 'H' for Horizontal, 'V' for Vertical
} Ship;

typedef struct {
	Ship shipsPerPlayer[TOTAL_SHIPS]; // Stores the amouts of ships on the board
	Ship* shipBoard[BOARDSIZE][BOARDSIZE]; // stores information of where the ships are on the map
	char displayBoard[BOARDSIZE][BOARDSIZE]; // Stores infomation of board display
	AIState Aistate;

} Board;

typedef struct
{
	char name[50];
	enum Rank rank;
	int score;

} Player;

typedef struct {
	int numOfTurns;       // How many turns the player used
	int hitStreak;        // Current consecutive hits streak
	int bestHitStreak;    // Best consecutive hits streak
	int numOfHits;        // Total successful hits
	int numOfMiss;        // Total misses
} gameStats;

