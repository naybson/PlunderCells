#include "types.h"
#include "gameplay.h"
#include "enemy_behavior.h"
#include "graphics_and_ui.h"
#include "colors.h"


bool checkForValidCoords(int x, int y, char orientation, int size)

/**
* Checks whether the ship placement is within the valid boundaries of the board.
*
* Parameters:
* - x, y: Starting coordinates for the ship placement.
* - orientation: 'H' for horizontal, 'V' for vertical.
* - size: The length of the ship.
*
* Returns:
* - true if the ship fits within the board's boundaries in the given orientation.
* - false if the placement goes out of bounds or the orientation is invalid.
*
* Note:
* This function only checks if the ship fits on the board.
* It does NOT check if the ship overlaps with any existing ships.
*/
{
	if (orientation == 'H' || orientation == 'V') // Ensure orientation is either horizontal or vertical
	{
		// Check if starting position is inside the board limits
		if (x >= BOARDSIZE || x < 0 || y >= BOARDSIZE || y < 0)
		{
			return false; // out of bounds: invalid coords
		}

		// Check if the ship exceeds horizontal bounds
		if (orientation == 'H' && (x + size) > BOARDSIZE)
		{
			return false; // Ship goes out of bounds horizontally
		}

		// Check if the ship exceeds vertical bounds
		if (orientation == 'V' && (y + size) > BOARDSIZE)
		{
			return false; // Ship goes out of bounds Vertically
		}

		return true; // Ship placement is Valid!
	}
	return false; // invalid orientation
}

bool isInRangeOfShip(int x, int y, char orientation, int size, Board* TargetBoard)
{
	/**
	* Checks if placing a ship at the given coordinates would result in a collision.
	*
	* Parameters:
	* - x, y: Starting coordinates where the ship would be placed.
	* - orientation: 'H' for horizontal or 'V' for vertical placement.
	* - size: The length of the ship.
	* - TargetBoard: The board we're checking placement against.
	*
	* Returns:
	* - true if any part of the ship would collide with an existing ship.
	* - false if the space is free for placement.
	*/

	/**
	* Checks if placing a ship at the given coordinates would result in a collision
	* OR be adjacent (even diagonally) to an existing ship.
	*/

	for (int i = 0; i < size; i++)
	{
		int xi, yi;

		if (orientation == 'H')
		{
			xi = x + i;
			yi = y;
		}
		else if (orientation == 'V')
		{
			xi = x;
			yi = y + i;
		}
		else {
			return true; // Invalid orientation
		}

		// Now check the surrounding 3x3 area around (xi, yi)
		for (int dy = -1; dy <= 1; dy++)
		{
			for (int dx = -1; dx <= 1; dx++)
			{
				int nx = xi + dx;
				int ny = yi + dy;

				// Ignore out-of-bounds
				if (nx < 0 || nx >= BOARDSIZE || ny < 0 || ny >= BOARDSIZE)
					continue;

				// If any nearby cell (including diagonals) contains a ship, reject placement
				if (TargetBoard->shipBoard[ny][nx] != NULL)
					return true;
			}
		}
	}

	// No nearby ships found — placement is valid
	return false;
}

void gameInitialize(Board* board)
{
	/**
	 * Initializes the board for a new game.
	 *
	 * Responsibilities:
	 * - Clears the ship and display boards (sets all tiles to empty water).
	 * - Initializes all ships (small, medium, and large) with their default values.
	 *
	 * Parameters:
	 * - board: A pointer to the Board struct to be initialized.
	 *
	 * Details:
	 * - shipBoard is filled with NULL to indicate no ships are placed.
	 * - displayBoard is filled with '~' to visually represent empty water.
	 * - shipsPerPlayer is populated with ships of the correct size and default stats.
	 */

	 // Initialize each cell on the board to empty (no ship, water tile)
	for (int i = 0; i < BOARDSIZE; i++)
	{
		for (int j = 0; j < BOARDSIZE; j++)
		{
			board->shipBoard[i][j] = NULL; // No ship present
			board->displayBoard[i][j] = '~'; // Empty water for display
		}
	}

	// Track the next available index in the ship array
	int currentShip = 0;

	// Create all small ships
	for (int i = 0; i < SMALL_SHIP_NUM; i++, currentShip++)
	{
		board->shipsPerPlayer[currentShip].size = SMALL_SHIP_SIZE;
		board->shipsPerPlayer[currentShip].hits = 0;
		board->shipsPerPlayer[currentShip].orientation = 'H';
	}
	// Create all meduim ships
	for (int i = 0; i < MEDUIM_SHIP_NUM; i++, currentShip++)
	{
		board->shipsPerPlayer[currentShip].size = MEDUIM_SHIP_SIZE;
		board->shipsPerPlayer[currentShip].hits = 0;
		board->shipsPerPlayer[currentShip].orientation = 'H';
	}
	// Create all large ships
	for (int i = 0; i < LARGE_SHIP_NUM; i++, currentShip++)
	{
		board->shipsPerPlayer[currentShip].size = LARGE_SHIP_SIZE;
		board->shipsPerPlayer[currentShip].hits = 0;
		board->shipsPerPlayer[currentShip].orientation = 'H';
	}

}

enum MSG addShip(Board* targetBoard, Ship* ship, int x, int y)
	/*
	* Attempts to place a ship on the board at a given starting position.
	*
	* 1. First, it checks if the ship's coordinates and orientation are within the board limits
	*    using checkForValidCoords.
	* 2. Then, it checks if the space is already occupied by another ship using isInRangeOfShip.
	* 3. If both checks pass, the ship is added to the shipBoard grid according to its orientation:
	*    - For horizontal ships, it fills cells to the right.
	*    - For vertical ships, it fills cells downward.
	*
	* Returns:
	* - MSG_PLACE_SHIP_SUCCESS if the ship was placed successfully.
	* - MSG_ERROR_IN_RANGE if the placement collides with another ship.
	* - MSG_ERROR_OUT_OF_BOUNDS if the placement is off the board.
	*/
{
	// We check here if the given coords are in bound of the board via the "checkForValidCoords" function, we want this to be true
	if (checkForValidCoords(x, y, ship->orientation, ship->size))
	{
		// We check that the ship dont collide with another ship on the board via the "isInRangeOfShip", we want this to be false
		if (!isInRangeOfShip(x, y, ship->orientation, ship->size, targetBoard))
		{
			targetBoard->shipBoard[y][x] = ship; // We place the first part of the ship on the given coords

			// if the orientation is 'H' we place the other ship parts on cells on the right of where we placed the first part
			// We do this for a number of times according to the ship's size
			if (ship->orientation == 'H')
			{
				for (int i = 0; i < ship->size; i++)
				{
					targetBoard->shipBoard[y][x + i] = ship;
				}
				return MSG_PLACE_SHIP_SUCCESS; // After all parts has been placed, return a success message
			}
			// if the orientation is 'V' we place the other ship parts on cells on the downward of where we placed the first part
			// We do this for a number of times according to the ship's size
			else
				if (ship->orientation == 'V')
				{
					for (int i = 0; i < ship->size; i++)
					{
						targetBoard->shipBoard[y + i][x] = ship;
					}
					return MSG_PLACE_SHIP_SUCCESS; // After all parts has been placed, return a success message
				}
		} return MSG_ERROR_IN_RANGE; // If the ship collide with another one we print a collision error

	} return MSG_ERROR_OUT_OF_BOUNDS; // If the given Coords are out of bound of the board
}

enum MSG attack(Board* targetBoard, int x, int y)
	/*
	 * Handles an attack on the target board at the specified coordinates.
	 *
	 * The function checks whether the selected position has already been attacked
	 * (i.e., marked as 'X', 'O', or '#'). If so, it returns MSG_ALREADY_ATTACKED to prevent repeated moves.
	 *
	 * If the cell contains a ship (i.e., not NULL), it marks the cell as a hit ('X'),
	 * increases the hit counter on the ship, and returns MSG_HIT.
	 *
	 * If the cell is empty (NULL), it marks the cell as a miss ('O') and returns MSG_MISS.
	 *
	 * Parameters:
	 * - targetBoard: Pointer to the board being attacked.
	 * - x: The column index of the attack.
	 * - y: The row index of the attack.
	 *
	 * Returns:
	 * - MSG_ALREADY_ATTACKED if the spot was already hit.
	 * - MSG_HIT if the attack hits a ship.
	 * - MSG_MISS if the attack hits water.
	 */
{
	// Check if this position was already attacked (hit, miss, or sunk)
	if (targetBoard->displayBoard[y][x] == 'X' ||
		targetBoard->displayBoard[y][x] == 'O' ||
		targetBoard->displayBoard[y][x] == '#')
	{

		return MSG_ALREADY_ATTACKED; // return repeated attack message
	}

	// Check if this position hit a ship
	if (targetBoard->shipBoard[y][x] != NULL)
	{
		targetBoard->displayBoard[y][x] = 'X'; // mark hit
		targetBoard->shipBoard[y][x]->hits++;

		if (targetBoard->shipBoard[y][x]->hits >= targetBoard->shipBoard[y][x]->size)
		{
			return MSG_SUNK; // return sunk message
		}
		else {
			return MSG_HIT; // return hit message
		}
	}
	// if the attack did not hit the ship, mark the position as a miss
	else if (targetBoard->shipBoard[y][x] == NULL)
	{
		targetBoard->displayBoard[y][x] = 'O'; // mark miss
		return MSG_MISS; // return miss message
	}
}

char GetRandomOrientation()
{
	/*
	* Generate a random orientations
	* Returns either 'H" or 'V'
	*/
	int inputOrientation = (rand() % 2);

	if (inputOrientation % 2 == 0)
	{
		return 'H';
	}
	else
	{
		return 'V';
	}
}

// Finds a random empty tile for ship placement (not used for attacking!)
void getRandomEmptyTile(Board* board, int* inputRow, int* inputCol)
{
	int attempts = 0;
	while (attempts < BOARDSIZE * BOARDSIZE)
	{
		int row = rand() % BOARDSIZE;
		int col = rand() % BOARDSIZE;

		if (board->shipBoard[row][col] == NULL) // We care about SHIPBOARD here!
		{
			*inputRow = row;
			*inputCol = col;
			return;
		}
		attempts++;
	}

	// Failsafe if something goes wrong
	*inputRow = rand() % BOARDSIZE;
	*inputCol = rand() % BOARDSIZE;
}

void autoPlaceRemainingShips(Board* board, int shipsRemaining, int startIndex)
{
	for (int j = 0; j < shipsRemaining; j++)
	{
		int inputRow, inputCol;
		getRandomEmptyTile(board, &inputRow, &inputCol); // Random position
		char inputOrientation = GetRandomOrientation();   // Random orientation
		board->shipsPerPlayer[startIndex + j].orientation = inputOrientation;

		enum MSG result = addShip(board, &board->shipsPerPlayer[startIndex + j], inputCol, inputRow);
		if (result != MSG_PLACE_SHIP_SUCCESS)
		{
			j--; // Retry if placement fails (due to collision or bounds)
		}
	}
}

void setUpShips(Board* playerBoard, Board* enemyBoard)
/**
 * Sets up the ships for both the player and the enemy on their respective boards.
 *
 * Parameters:
 * - playerBoard: Pointer to the player's board.
 * - enemyBoard: Pointer to the enemy's board.
 *
 * Behavior:
 * - Randomly places all enemy ships on the enemy board using GetRandomCords and GetRandomOrientation.
 * - Prompts the player to manually place their ships by providing coordinates and orientation.
 * - Uses addShip() to attempt ship placement, and retries if the placement is invalid.
 * - Displays the board after each valid player placement.
 */
{
	int totalShipsPerPlayer = LARGE_SHIP_NUM + MEDUIM_SHIP_NUM + SMALL_SHIP_NUM;

	// ===========================
	// Place enemy ships randomly
	// ===========================

	autoPlaceRemainingShips(enemyBoard, TOTAL_SHIPS, 0);

	// ===================================
	// Ask player to place ships manually
	// ===================================
	for (int i = 0; i < totalShipsPerPlayer; i++)
	{

		// Ship info
		int shipsRemaining = totalShipsPerPlayer - i;
		int shipSize = playerBoard->shipsPerPlayer[i].size;

		updateBoard(playerBoard, enemyBoard);

		/// Draw the phase headers
		printc(YELLOW,"\n======================================================");
		printc(YELLOW,"\n	    	    Setp up phase	");
		printc(YELLOW,"\n=====================================================");
		printc(BRIGHT_BLUE,"\n Ships remaining to place: %d\n", shipsRemaining);
		printc(GREEN,"Current ship size: %d cells long\n", shipSize);

		/// Give the user a size example
		printc(GREEN,"[");
		for (int s = 0; s < shipSize; s++)
		{
			printc(GREEN,"S");
		}
		printc(GREEN, "]");
		printc(BRIGHT_BLUE,"\n=====================================================");


		// Ask the player for input and check if its valid //
		//=================================================//
		int inputRow, inputCol;
		// Step 1: Get input
		if (!GetPlayerInput(&inputRow, &inputCol))
		{
			i--;
			system("pause");
			continue;
		}

		// Step 2: Debug check
		if (inputRow == -1 && inputCol == -1)
		{
			autoPlaceRemainingShips(playerBoard, shipsRemaining, i);
			return NULL;
		}

		// Step 3: Get orientation
		char inputOrientation = getPlayerOrientation();
		playerBoard->shipsPerPlayer[i].orientation = inputOrientation;

		// Step 4: Try placement
		enum MSG result = addShip(playerBoard, &playerBoard->shipsPerPlayer[i], inputCol, inputRow);
		printMessage(result);

		if (result != MSG_PLACE_SHIP_SUCCESS)
		{
			i--;
			system("pause");
			continue;
		}

	}
	updateBoard(playerBoard, enemyBoard);  // Show final ship setup
}

bool endGameCheck(Board* board)
/*
* This function checks if the One of the player has lost
* we get a board, scan all cells on it and return true if all ships on the board has sunk
*/
{
	for (int i = 0; i < BOARDSIZE; i++)
	{
		for (int j = 0; j < BOARDSIZE; j++)
		{
			if (board->shipBoard[i][j] != NULL && board->shipBoard[i][j]->hits != board->shipBoard[i][j]->size)
			{
				return false;
			}
		}
	}

	return true;
}

bool PlayerAttack(Board* enemyBoard, Board* playerBoard, gameStats* gameStats)
{
	printSlow(GREEN,"\nYour Turn - Fire at Will!",TYPE_FAST);
	int inputRow, inputCol;
	enum MSG result;

	// Get input and validate
	if (!GetPlayerInput(&inputRow, &inputCol))
	{
		system("pause");
		return false; // Retry turn if input was invalid
	}
	// Even if the player inputs RR (debug code) still show an out of bounds error
	if (inputRow == -1 && inputCol == -1)
	{
		printMessage(MSG_ERROR_OUT_OF_BOUNDS);
		system("pause");
		return false;
	}

	// Fire at selected target
	result = attack(enemyBoard, inputCol, inputRow);

	char buffer[100];
	char colChar = 'A' + inputCol; // Turn 0 → A, 1 → B, etc.

	// If the player tries to hit the same player twice, let them retry again
	if (result == MSG_ALREADY_ATTACKED)
	{
		printMessage(MSG_ALREADY_ATTACKED);
		system("pause");
		return false; // Retry same turn if location was already attacked
	}

	sprintf_s(buffer, sizeof(buffer), "You attacked at: %c%d", colChar, inputRow); // One letter + one number
	SLEEP(1);
	printSlow(BRIGHT_CYAN, buffer, TYPE_SUPERFAST); // Print it slowly

	printMessage(result); // Show hit/miss
	system("pause");

	// add game stats if the player hit or missed
	if (result == MSG_HIT || result == MSG_SUNK)
	{
		gameStats->numOfTurns++;
		enemyBoard->Aistate.shipSunk++;
		gameStats->numOfHits++;
		gameStats->hitStreak++;

		if (gameStats->hitStreak > gameStats->bestHitStreak)
		{
			gameStats->bestHitStreak = gameStats->hitStreak;
		}
	}
	else if (result == MSG_MISS)
	{
		gameStats->numOfTurns++;
		gameStats->numOfMiss++;
		gameStats->hitStreak = 0; // Reset streak on miss
	}


	return true; // Switch to enemy's turn
}

bool AttackPhase(Board* playerBoard, Board* enemyBoard, gameStats* gameStats)
{
	/**
	 * Handles the main attack phase of the game loop.
	 *
	 * Parameters:
	 * - playerBoard: Pointer to the player's board (for enemy attacks).
	 * - enemyBoard: Pointer to the enemy's board (for player attacks).
	 *
	 * Overview:
	 * - Alternates turns between player and enemy.
	 * - Player inputs attack coordinates manually.
	 * - Enemy behavior depends on AI level:
	 *     - EASY: Random attacks.
	 *     - MEDIUM: Hunts near previous hit.
	 *     - HARD: Smart directional hunting.
	 *     - NIGHTMARE: (New!) May know ship locations or have unfair advantages.
	 *
	 * Game ends when one board has all ships destroyed.
	 */
	bool gameOver = false;
	bool isPlayerturn = true;  // Keeps track of whose turn it is 

	// Loop until one side has lost all ships
	while (!endGameCheck(playerBoard) && !endGameCheck(enemyBoard))
	{
		// Refresh board for each round
		updateBoard(playerBoard, enemyBoard);

		// Draw Turn Header
		printc(BRIGHT_RED,"\n=====================================================");
		printc(BRIGHT_RED,"\n	    	    Attack phase");
		printc(BRIGHT_RED,"\n=====================================================");

		// === PLAYER'S TURN ===
		if (isPlayerturn)
		{
			if (PlayerAttack(enemyBoard, playerBoard, gameStats)) // if the player managed to finish thier attack it will return true.
			{
				isPlayerturn = false; // after managing to attack give the enemy a chance to attack.
			}
		}
		else // === ENEMY'S TURN ===
		{
			EnemyAttack(enemyBoard, playerBoard);
			isPlayerturn = true;
		}
	}
}
