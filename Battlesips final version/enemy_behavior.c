#include "types.h"
#include "gameplay.h"
#include "enemy_behavior.h"
#include "graphics_and_ui.h"
#include "colors.h"
#include <stdio.h>           

#define PEEKS_AFTER_TRIES_HARD 5
#define PEEKS_AFTER_TRIES_NIGHTMARE 3

// ==============================================
// Core AI Traits
// ==============================================

/**
 * Checks if a tile is near any wreckage (#).
 * Used to avoid wasting shots in areas likely to be already sunk ships.
 */
bool isNearWreckage(Board* board, int row, int col)

{
	for (int dy = -1; dy <= 1; dy++)
	{
		for (int dx = -1; dx <= 1; dx++)
		{
			if (dy == 0 && dx == 0)
			{
				continue; // Don't check the center itself
			}

			int newRow = row + dy;
			int newCol = col + dx;

			if (newRow >= 0 && newRow < BOARDSIZE && newCol >= 0 && newCol < BOARDSIZE) {
				if (board->displayBoard[newRow][newCol] == '#')
				{
					return true; // Wreckage detected nearby!
				}
			}
		}
	}
	return false; // Safe, no wreckage around
}

/**
 * Randomly shoot at a valid unexplored tile (~).
 * Avoids shooting into wreckage (#), hits (X), and misses (O).
 */
bool randomShoot(Board* playerBoard, int* inputRow, int* inputCol)
{
	int attempts = 0;
	while (attempts < BOARDSIZE * BOARDSIZE)
	{
		int row = rand() % BOARDSIZE;
		int col = rand() % BOARDSIZE;

		if (playerBoard->displayBoard[row][col] != 'X' &&
			playerBoard->displayBoard[row][col] != 'O' &&
			playerBoard->displayBoard[row][col] != '#' &&
			!isNearWreckage(playerBoard, row, col))
		{
			*inputRow = row;
			*inputCol = col;
			return true; // Found good target
		}
		attempts++;
	}
	return false; // No good target found (shouldn't happen)
}

/**
 * After a hit, tries to attack adjacent tiles (up, down, left, right).
 * Marks directions already tried to avoid repeating.
 */
bool huntAdjacent(Board* enemyBoard, Board* playerBoard, int* inputRow, int* inputCol)
{
	AIState* ai = &enemyBoard->Aistate;

	int startX = ai->lastHitX;
	int startY = ai->lastHitY;

	for (int dir = 0; dir < 4; dir++)
	{
		if (ai->triedDirections[dir]) continue; // Already tried this direction

		ai->triedDirections[dir] = true; // Mark direction as tried

		switch (dir)
		{
		case 0: *inputRow = startY + 1; *inputCol = startX; break; // down
		case 1: *inputRow = startY - 1; *inputCol = startX; break; // up
		case 2: *inputRow = startY; *inputCol = startX + 1; break; // right
		case 3: *inputRow = startY; *inputCol = startX - 1; break; // left
		}

		// Check valid shot
		if (*inputRow >= 0 && *inputRow < BOARDSIZE &&
			*inputCol >= 0 && *inputCol < BOARDSIZE &&
			playerBoard->displayBoard[*inputRow][*inputCol] != 'X' &&
			playerBoard->displayBoard[*inputRow][*inputCol] != 'O' &&
			playerBoard->displayBoard[*inputRow][*inputCol] != '#')
		{
			return true; // Found good adjacent tile
		}
	}

	return false; // No adjacent tiles available
}

/**
 * After 2 hits in a row, continues attacking along detected ship direction.
 * Tries to sink the whole ship by moving straight.
 */
bool followShipDirection(Board* enemyBoard, Board* playerBoard, int* inputRow, int* inputCol)
{
	AIState* ai = &enemyBoard->Aistate;

	if (ai->currentDirection != -1) {
		int dx = 0, dy = 0;

		// Convert direction number to dx/dy
		switch (ai->currentDirection) {
		case 0: dy = -1; break; // up
		case 1: dy = 1;  break; // down
		case 2: dx = 1;  break; // right
		case 3: dx = -1; break; // left
		}

		// === Try forward from secondHit ===
		int nextX = ai->secondHitX + dx;
		int nextY = ai->secondHitY + dy;

		if (nextX >= 0 && nextX < BOARDSIZE &&
			nextY >= 0 && nextY < BOARDSIZE &&
			playerBoard->displayBoard[nextY][nextX] != 'X' &&
			playerBoard->displayBoard[nextY][nextX] != 'O' &&
			playerBoard->displayBoard[nextY][nextX] != '#')
		{
			*inputCol = nextX;
			*inputRow = nextY;
			return true;
		}

		// === Forward failed, try backward ===
		if (!ai->reversedOnce)
		{
			ai->reversedOnce = true;
			ai->secondHitX = ai->lastHitX;
			ai->secondHitY = ai->lastHitY;

			int reverseX = ai->secondHitX - dx;
			int reverseY = ai->secondHitY - dy;

			if (reverseX >= 0 && reverseX < BOARDSIZE &&
				reverseY >= 0 && reverseY < BOARDSIZE &&
				playerBoard->displayBoard[reverseY][reverseX] != 'X' &&
				playerBoard->displayBoard[reverseY][reverseX] != 'O' &&
				playerBoard->displayBoard[reverseY][reverseX] != '#')
			{
				// Flip direction
				if (ai->currentDirection == 0) ai->currentDirection = 1;
				else if (ai->currentDirection == 1) ai->currentDirection = 0;
				else if (ai->currentDirection == 2) ai->currentDirection = 3;
				else if (ai->currentDirection == 3) ai->currentDirection = 2;

				ai->secondHitX = reverseX;
				ai->secondHitY = reverseY;

				*inputCol = reverseX;
				*inputRow = reverseY;
				return true;
			}
		}

		// === Both directions failed, give up ===
		ai->hunting = false;
		ai->currentDirection = -1;
		ai->reversedOnce = false;
		ai->lastHitX = -1;
		ai->lastHitY = -1;
		ai->secondHitX = -1;
		ai->secondHitY = -1;
		for (int i = 0; i < 4; i++) ai->triedDirections[i] = false;
	}

	return false;
}

/**
 * In Nightmare difficulty, AI "cheats" and finds visible ship parts ('S').
 * 90% chance to hit them immediately.
 */
bool perfectTargeting(Board* playerBoard, int* inputRow, int* inputCol)
{
	for (int y = 0; y < BOARDSIZE; y++)
	{
		for (int x = 0; x < BOARDSIZE; x++)
		{
			if (playerBoard->displayBoard[y][x] == 'S')
			{
				// 90% chance to actually target it
				if ((rand() % 100) < 90)
				{
					*inputRow = y;
					*inputCol = x;
					return true;
				}
			}
		}
	}
	return false; // Didn't find a ship
}

/**
 * If enemy is down to one ship, cheats once to peek at player's ship locations.
 * Only happens once per game (fair "panic" mechanic).
 */
bool semiCheatOnLastShip(Board* enemyBoard, Board* playerBoard, int* inputRow, int* inputCol)
{
	AIState* ai = &enemyBoard->Aistate;

	if (ai->usedSemiCheat)
	{
		return false; // Already used, no more cheating!
	}

	if (ai->shipSunk < TOTAL_SHIPS - 1)
	{
		return false;
	}

	// Now we are allowed to semi-cheat
	int candidatesRow[BOARDSIZE * BOARDSIZE];
	int candidatesCol[BOARDSIZE * BOARDSIZE];
	int candidatesCount = 0;

	// Search player ship board for still-alive ship parts
	for (int row = 0; row < BOARDSIZE; row++)
	{
		for (int col = 0; col < BOARDSIZE; col++)
		{
			if (playerBoard->shipBoard[row][col] != NULL &&
				playerBoard->displayBoard[row][col] == '~') // Only if not hit yet!
			{
				candidatesRow[candidatesCount] = row;
				candidatesCol[candidatesCount] = col;
				candidatesCount++;
			}
		}
	}

	if (candidatesCount > 0)
	{
		int choice = rand() % candidatesCount;
		*inputRow = candidatesRow[choice];
		*inputCol = candidatesCol[choice];
		ai->usedSemiCheat = true; // Mark as used!
		return true; // Successful semi-cheat attack
	}

	return false; // No good ship parts found
}

/**
 * After missing too many times in a row, cheats once to find a ship.
 * Resets miss counter after cheating.
 */
bool peekAfterMissStreak(Board* enemyBoard, Board* playerBoard, int* inputRow, int* inputCol, int missThreshold)
{
	AIState* ai = &enemyBoard->Aistate;

	// Threshold for angry cheat
	if (ai->missStreak < missThreshold)
		return false; // Not angry yet

	// Cheat time: look for ship parts
	int candidatesRow[BOARDSIZE * BOARDSIZE];
	int candidatesCol[BOARDSIZE * BOARDSIZE];
	int candidatesCount = 0;

	for (int row = 0; row < BOARDSIZE; row++)
	{
		for (int col = 0; col < BOARDSIZE; col++)
		{
			if (playerBoard->shipBoard[row][col] != NULL &&
				playerBoard->displayBoard[row][col] == '~')
			{
				candidatesRow[candidatesCount] = row;
				candidatesCol[candidatesCount] = col;
				candidatesCount++;
			}
		}
	}

	if (candidatesCount > 0)
	{
		int choice = rand() % candidatesCount;
		*inputRow = candidatesRow[choice];
		*inputCol = candidatesCol[choice];

		ai->missStreak = 0; // Reset miss counter after cheating
		return true;
	}

	return false; // No ship parts found (extremely rare)
}

// ==============================================
// Difficulty AI Pipelines
// ==============================================

/**
 * Easy AI behavior.
 * Only uses random shooting with no hunting or tactics.
 */
bool tryEasyDifficulty(Board* enemyBoard, Board* playerBoard, int* inputRow, int* inputCol)
{
	return randomShoot(playerBoard, inputRow, inputCol);
}

/**
 * Medium AI behavior.
 * Tries hunting after a hit, prefers clusters (3x3), otherwise random.
 */
bool tryMediumDifficulty(Board* enemyBoard, Board* playerBoard, int* inputRow, int* inputCol)
{
	return 
		huntAdjacent(enemyBoard, playerBoard, inputRow, inputCol) ||
		randomShoot(playerBoard, inputRow, inputCol);
}

/**
 * Hard AI behavior.
 * Smarter hunting: follows ship direction, cheats after long miss streaks,
 * prefers big clusters (5x5), falls back to hunting or random.
 */
bool tryHardDifficulty(Board* enemyBoard, Board* playerBoard, int* inputRow, int* inputCol)
{
	AIState* ai = &enemyBoard->Aistate;

	return
		followShipDirection(enemyBoard, playerBoard, inputRow, inputCol) ||
		huntAdjacent(enemyBoard, playerBoard, inputRow, inputCol) ||
		semiCheatOnLastShip(enemyBoard, playerBoard, inputRow, inputCol) ||
		peekAfterMissStreak(enemyBoard, playerBoard, inputRow, inputCol, PEEKS_AFTER_TRIES_HARD) ||
		randomShoot(playerBoard, inputRow, inputCol);

}

/**
 * Nightmare AI behavior.
 * Brutal and aggressive: targets known ships, follows direction, cheats often,
 * prioritizes clusters, always maximizes pressure.
 */
bool tryNightmareDifficulty(Board* enemyBoard, Board* playerBoard, int* inputRow, int* inputCol)
{
	AIState* ai = &enemyBoard->Aistate;

	return 
		perfectTargeting(playerBoard, inputRow, inputCol) ||
		followShipDirection(enemyBoard, playerBoard, inputRow, inputCol) ||
		semiCheatOnLastShip(enemyBoard, playerBoard, inputRow, inputCol) ||
		peekAfterMissStreak(enemyBoard, playerBoard, inputRow, inputCol, PEEKS_AFTER_TRIES_NIGHTMARE) || // Nightmare cheats faster
		huntAdjacent(enemyBoard, playerBoard, inputRow, inputCol) ||
		randomShoot(playerBoard, inputRow, inputCol);
}


// ==============================================
// AI State Management
// ==============================================

/**
 * Updates AI memory after an attack.
 * Handles switching into hunting mode, resetting after sunk, counting misses, etc.
 */
void updateAIState(Board* enemyBoard, enum MSG result, int inputRow, int inputCol)
{
	AIState* ai = &enemyBoard->Aistate;

	if (result == MSG_HIT)
	{
		ai->hunting = true;
		ai->missStreak = 0;

		if (ai->lastHitX == -1 && ai->lastHitY == -1)
		{
			ai->lastHitX = inputCol;
			ai->lastHitY = inputRow;
			ai->currentDirection = -1;
			ai->reversedOnce = false;
			for (int i = 0; i < 4; i++) ai->triedDirections[i] = false;
		}
		else if (ai->currentDirection == -1)
		{
			int dx = inputCol - ai->lastHitX;
			int dy = inputRow - ai->lastHitY;

			if (dx == 1) ai->currentDirection = 2;
			else if (dx == -1) ai->currentDirection = 3;
			else if (dy == 1) ai->currentDirection = 1;
			else if (dy == -1) ai->currentDirection = 0;

			ai->secondHitX = inputCol;
			ai->secondHitY = inputRow;
		}
		else
		{
			ai->secondHitX = inputCol;
			ai->secondHitY = inputRow;
		}
	}
	else if (result == MSG_SUNK)
	{
		// Reset hunting memory
		ai->missStreak = 0;
		ai->hunting = false;
		ai->currentDirection = -1;
		ai->reversedOnce = false;
		ai->lastHitX = -1;
		ai->lastHitY = -1;
		ai->secondHitX = -1;
		ai->secondHitY = -1;
		for (int i = 0; i < 4; i++) ai->triedDirections[i] = false;
	}
	else if (result == MSG_MISS)
	{
		ai->missStreak++;
	}
}

/**
 * Handles the full enemy turn:
 * - Picks a move based on AI difficulty
 * - Executes attack
 * - Updates AI memory
 */
void EnemyAttack(Board* enemyBoard, Board* playerBoard)
{
	int inputRow, inputCol;
	bool moveChosen = false;

	switch (enemyBoard->Aistate.Lv)
	{
	case EASY:
		moveChosen = tryEasyDifficulty(enemyBoard, playerBoard, &inputRow, &inputCol);
		break;
	case MEDUIM:
		moveChosen = tryMediumDifficulty(enemyBoard, playerBoard, &inputRow, &inputCol);
		break;
	case HARD:
		moveChosen = tryHardDifficulty(enemyBoard, playerBoard, &inputRow, &inputCol);
		break;
	case NIGHTMARE:
		moveChosen = tryNightmareDifficulty(enemyBoard, playerBoard, &inputRow, &inputCol);
		break;
	}

	if (!moveChosen)
	{
		randomShoot(playerBoard, &inputRow, &inputCol);
	}

	// Display Attack
	printSlow(BRIGHT_RED, "\nEnemy attacks at: ", TYPE_FAST);
	Sleep(1000);

	char colChar = 'A' + inputCol;
	char buffer[50];
	sprintf_s(buffer, sizeof(buffer), "%c%d", colChar, inputRow);
	printSlow(BRIGHT_RED, buffer, TYPE_SLOW);

	if (inputRow >= 0 && inputRow < BOARDSIZE &&
		inputCol >= 0 && inputCol < BOARDSIZE)
	{
		enum MSG result = attack(playerBoard, inputCol, inputRow);
		printMessage(result);
		updateAIState(enemyBoard, result, inputRow, inputCol);
	}

	system("pause");
}
