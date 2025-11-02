#include "graphics_and_ui.h" // For the function declarations
#include "colors.h"
#include <stdio.h>           // Needed for printf and scanf
#include <stdlib.h>          // For system("cls") and other stuff
#include <ctype.h>           // For toupper() and isalpha()
#include <windows.h>         // For Sleep() on Windows
#include <conio.h> // for _kbhit() and _getch()


// Clears the screen!
void clearScreen() 
{
#ifdef _WIN32
	system("cls");
#else
	system("clear");
#endif
}

// prints in color!
void printc(const char* color, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	printf("%s", color); // Set the color

	
	vprintf(format, args); // Correctly print with variable arguments
	va_end(args);

	printf(COLOR_RESET); // Reset color back to default
}

// prints text slowly like in an RPG, and if the player presses any key it immediately finishes printing
void printSlow(const char* color, const char* text, int delayMilliseconds)
{
	printf("%s", color); // Set the color first (e.g., RED, BLUE, etc.)

	bool skip = false; // Flag to check if the player pressed any key and wants to skip the animation

	for (int i = 0; text[i] != '\0'; i++) // Go through each character in the text until the end
	{
		printf("%c", text[i]); // Print the current character
		fflush(stdout);        // Make sure it appears immediately (without buffering delay)

		if (!skip) // If the user didn't press anything yet
		{
			// Check if a key was pressed
			if (_kbhit())
			{
				_getch(); // Read and discard the pressed key (otherwise it'll stay stuck in input)
				skip = true; // Activate skip mode (from now on, no delay between letters)
			}
			else
			{
				Sleep(delayMilliseconds); // If no key was pressed, wait between characters (for the slow effect)
			}
		}
	}

	printf(COLOR_RESET); // After the full line is printed, reset the color back to default
}

// draws the board header (A B C D E ....)
void drawBoardHeader()
{
	/*
	 * Draws the column headers for both the player's and the enemy's boards.
	 *
	 * 1. Prints the title headers: "Your Board" and "Enemy Board".
	 * 2. Loops through the board size to print the A–J (or more) column letters above each board.
	 * 3. Uses numberToChar() to convert numeric indices into alphabetical column labels.
	 *
	 * Example Output:
	 *         Your Board                   Enemy Board
	 *    A B C D E F G H I J           A B C D E F G H I J
	 */

	// Print the top title for both boards
	printc(WHITE,"        Your Board                   Enemy Board");

	// Print padding before column headers
	printf("    \n   ");

	// Print column headers (A-J) for the player's board
	for (int i = 0; i < BOARDSIZE; i++)
	{
		char horziontalHeader = numberToChar(i + 1);
		printc(WHITE,"%c ", horziontalHeader);
	}

	// Print spacing between boards
	printf("          ");

	// Print column headers (A-J) for the enemy's board
	for (int i = 0; i < BOARDSIZE; i++)
	{
		char horziontalHeader = numberToChar(i + 1);
		printc(WHITE,"%c ", horziontalHeader);
	}

	// Move to the next line after headers are printed
	printf("\n");
}

// draws a single board, it twice to draw the players board and the enemys board
void drawSingleBoard(Board* board, int rowIndex, bool hideShips)
{
/*
 * Draws a single row of the board at the given row index.
 *
 * Parameters:
 * - board: Pointer to the Board structure to draw from.
 * - rowIndex: The row to be printed.
 * - hideShips: If true, ship positions will be hidden (used for enemy board).
 *
 * Logic:
 * - Iterates over each column in the row.
 * - If a ship exists at the position:
 *     - If the cell was hit (marked 'X') and the ship is fully damaged, mark it as '#'.
 *     - Otherwise, show 'X' for hit or 'S' for ship (unless hidden).
 * - If no ship is present, display the current character (usually '~' or 'O').
 *
 * Symbols:
 * - 'S' = Ship (only shown if hideShips == false)
 * - 'X' = Hit
 * - '#' = Sunk (all parts of ship hit)
 * - '~' = Water / hidden ship
 */

	for (int columIndex = 0; columIndex < BOARDSIZE; columIndex++)
	{
		// If there is a ship in this cell
		if (board->shipBoard[rowIndex][columIndex] != NULL)
		{
			// If it's already marked as hit
			if ((board->displayBoard[rowIndex][columIndex] == 'X') || (board->displayBoard[rowIndex][columIndex] == '#'))
			{
				// If the ship is fully hit, mark it as sunk
				if (board->shipBoard[rowIndex][columIndex]->hits == board->shipBoard[rowIndex][columIndex]->size)
				{
					board->displayBoard[rowIndex][columIndex] = '#';
				}
				else
				{
					board->displayBoard[rowIndex][columIndex] = 'X';
				}
			}
			else
			{
				// If ship isn't hit yet, show it unless hiding is requested
				if (hideShips == false)
				{
					board->displayBoard[rowIndex][columIndex] = 'S';
				}
				else
				{
					board->displayBoard[rowIndex][columIndex] = '~';
				}
			}

		}

		// Print the symbol for the current cell
		char symbol = board->displayBoard[rowIndex][columIndex];

		switch (symbol)
		{
		case '~':
			printc(BLUE, "~ "); // BLUE ~
			break;
		case 'S':
			printc(GREEN, "S "); // GREEN S
			break;
		case 'X':
			printc(RED, "X "); // RED X
			break;
		case '#':
			printc(GRAY, "# "); // GRAY ~
			break;
		case 'O':
			printc(CYAN, "O "); // CYAN O
			break;
		default:
			printf("%c ", symbol); // default print the symbol on the array
			break;
		}

	}
}

// Updates the board each turn
void updateBoard(Board* playerBoard, Board* enemyBoard)
{
	/*
	 * Updates and prints the current state of both the player's and the enemy's boards.
	 *
	 * 1. Clears the screen and prints column headers for both boards.
	 * 2. Loops through each row and prints:
	 *    - The player board: Shows ship positions and attack status.
	 *    - The enemy board: Shows hits, misses, and sunken ships.
	 *
	 * Display Symbols:
	 * - '~': Water (unrevealed)
	 * - 'S': Ship
	 * - 'X': Hit
	 * - 'O': Miss
	 * - '#': Entire ship destroyed
	 */

	clearScreen();
	drawBoardHeader();

	// Displaying the board themselves
	for (int i = 0; i < BOARDSIZE; i++)
	{
		//Player side
		printf("%2d ", i); // Player's vertical header
		drawSingleBoard(playerBoard, i, false);

		// Enemy side
		printf("       ");
		printf("%2d ", i); // Enemy's vertical header
		drawSingleBoard(enemyBoard, i, true);

		printf("\n"); // Move to next row after both boards are printed
	}
}

// prints the messages for each action in the game
void printMessage(enum MSG msg)
{
	// This function turns the MSG to anctual printable line of dialoge 
	// We also use this function to understand if an attack missed, hit, or there was a certen error
	printf("\n");
	switch (msg)
	{
	case MSG_HIT:
		Sleep(1000);
		printSlow(RED,"HIT!",TYPE_FAST);  // For attack that hit a part of a ship 
		break;

	case MSG_MISS:
		Sleep(1000);
		printSlow(YELLOW,"MISS!",TYPE_FAST);  // For attack that missed 
		break;

	case MSG_SUNK:
		Sleep(1000);
		printSlow(RED,"A Ship has Sunk!", TYPE_FAST);  // For attack that sunk a ship
		break;

	case MSG_ERROR_OUT_OF_BOUNDS:
		printc(YELLOW,"Invalid position (out of bounds)"); // if the user putted on a position that does not exsist on the board (in the attack or setup Phase)
		break;

	case MSG_ERROR_IN_RANGE:
		printc(YELLOW,"Invalid position (In range of another ship)"); // if the user tryied setting up a ship that is in range of another ship
		break;
	case MSG_PLACE_SHIP_SUCCESS:
		printc(GREEN,"Ship has been placed!"); // message that appear if a ship was placed succesfully
		break;
	case MSG_EMPTY:
		printf("\n"); // an empty line for the enemy ship
		break;
	case MSG_ALREADY_ATTACKED:
		printc(YELLOW,"Attack already has been made there!"); // Message that appear if the user tries to attack the same place twice
		break;
	default:
		printf("\n");

	}
	printf("\n");
}

// converst a number to a char
char numberToChar(int num) {
	/*
	This function turns a number to a char
	*/
	if (num >= 1 && num <= 26) {
		return 'A' + (num - 1);  // 'A' is 65, so we add (num - 1) to get the right letter.
	}
	return '?';  // Return '?' if the number is not between 1 and 26.
}

// gets the player input and makes sure the input is valid
bool GetPlayerInput(int* inputRow, int* inputCol)
/**
 * Prompts the player to enter board coordinates (e.g., B3 or 3B) and validates them.
 *
 * Parameters:
 * - inputRow: Pointer to store the row index.
 * - inputCol: Pointer to store the column index.
 *
 * Returns:
 * - true if the input is valid and within board bounds.
 * - false if input format is invalid or out of bounds.
 */
{
	char firstChar, secondChar;

	printSlow(BRIGHT_CYAN,"\nCommander, input target coordinates (e.g., B3 or 3B): ", TYPE_SUPERFAST);
	Sleep(300);

	// Try to read exactly two characters (e.g., 'B' and '3' or '3' and 'B')
	int correctInput = scanf_s(" %c%c", &firstChar, (unsigned)sizeof(firstChar), &secondChar, (unsigned)sizeof(secondChar));

	//DEBUG MESSAGE
	if (firstChar == 'R' && secondChar == 'R')
	{
		*inputRow = -1;
		*inputCol = -1;
		return true;
	}

	// if the user did not enter two input like CC or 222 we return false
	if (correctInput != 2) {
		printMessage(MSG_ERROR_OUT_OF_BOUNDS);
		while (getchar() != '\n'); // flush invalid input
		return false;
	}

	// Make sure the user does not enter more than two inputs
	int extraChar = getchar();
	if (extraChar != '\n') {
		// Extra input detected (e.g., "E100"), invalid input
		printMessage(MSG_ERROR_OUT_OF_BOUNDS);
		while (extraChar != '\n' && extraChar != EOF) {
			extraChar = getchar();
		}
		return false;
	}


	/// we detect here what wich char is a letter and wich is a digit
	char columnLetter;
	char rowDigitChar;

	// Detect order: letter-number or number-letter
	if (isalpha(firstChar) && isdigit(secondChar)) {
		columnLetter = firstChar;
		rowDigitChar = secondChar;
	}
	else if (isdigit(firstChar) && isalpha(secondChar)) {
		columnLetter = secondChar;
		rowDigitChar = firstChar;
	}
	else {
		// Invalid input format (e.g., "##", "AA", "55")
		printMessage(MSG_ERROR_OUT_OF_BOUNDS);
		return false;
	}

	// Convert input to 0-based index
	*inputCol = toupper(columnLetter) - 'A';  // e.g., 'B' → 1
	*inputRow = rowDigitChar - '0';           // e.g., '3' → 3

	// Check if the indices are within board bounds
	if (*inputCol < 0 || *inputCol >= BOARDSIZE || *inputRow < 0 || *inputRow >= BOARDSIZE) {
		printMessage(MSG_ERROR_OUT_OF_BOUNDS);
		return false;
	}

	return true;
}

// gets the players orientation (V\H)
char getPlayerOrientation() {
	/**
 * Prompts the player to input a ship orientation.
 *
 * Behavior:
 * - Repeatedly asks the player to enter 'H' (Horizontal) or 'V' (Vertical).
 * - Converts input to uppercase to ensure case-insensitive comparison.
 * - Validates the input and only returns once a correct orientation is received.
 *
 * Returns:
 * - 'H' if the player chose horizontal.
 * - 'V' if the player chose vertical.
 */
	char orientation;
	while (true) {
		printSlow(BRIGHT_CYAN,"\nChoose your ship alignment, Captain! [H]orizontal or [V]ertical: ",TYPE_SUPERFAST);
		scanf_s(" %c", &orientation, 1);  // Read a single character
		orientation = toupper(orientation);// Convert to uppercase

		if (orientation == 'H' || orientation == 'V') {
			return orientation; // Valid input, return it
		}

		// If input is invalid, notify the player and flush leftover input
		printSlow(YELLOW,"Invalid orientation. Try again.	\n",TYPE_SUPERFAST);
		while (getchar() != '\n'); // flush buffer
	}
}

// swaps the values incase if the player eneter a fliped input
void swap(char* inputColChar, int* inputRow)
/*
 * If the user enters a swapped input like 5C,
 * this will convert it to C5 correctly.
 */
{
	char digitChar = *inputColChar;
	char letterChar = (char)*inputRow;

	*inputColChar = letterChar;
	*inputRow = digitChar - '0';
}

void printEndScreen(bool PlayerWon)
{
	clearScreen();
	if (!PlayerWon)
	{
		// ANSI escape code for red text

		const char* frames[] =
		{
			// Frame 1 – Abandon ship!

			"+------------------------------------------------------+\n"
			"|                      YOU LOSE!                      |\n"
			"|            Your fleet has been destroyed...         |\n"
			"+------------------------------------------------------+\n"
			"\n"
			"                          .  o ..                      \n"
			"                          o . o o.o                   \n"
			"                               ...oo                  \n"
			"                                 __[]__               \n"
			"                              __|_o_o_o\\__           \n"
			"                          ___|____________|___        \n"
			"                          \\~*~*~*~*~*~*~*~*~/         \n"
			"         ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~  \n"
			"          ~    ~  ~    ~    ~  ~   ~    ~   ~~   ~~   \n"
			"\n"
			"                 >>> ABANDON SHIP! <<<\n"


			// Frame 2 – On fire

			"+------------------------------------------------------+\n"
			"|                      YOU LOSE!                      |\n"
			"|            Your fleet has been destroyed...         |\n"
			"+------------------------------------------------------+\n"
			"\n"
			"                        (   )  o ..   ^ ^             \n"
			"                     (   )  ) ) o.o    ^ ^            \n"
			"                        ( )  ( ) ...oo  ^             \n"
			"                                 __[]__   !! ^ !!     \n"
			"                              __|_x_x_x\\__ !! ^ !!    \n"
			"                          ___|____________|___ ! ! !  \n"
			"                          \\~*~*~*~*~*~*~*~*~/         \n"
			"         ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~  \n"
			"          ~    ~  ~    ~    ~  ~   ~    ~   ~~   ~~   \n"
			"\n"
			"                 >>> FIRE ON DECK! <<<\n"


			// Frame 3 – Going down

			"+------------------------------------------------------+\n"
			"|                      YOU LOSE!                      |\n"
			"|            Your fleet has been destroyed...         |\n"
			"+------------------------------------------------------+\n"
			"\n"
			"                     (   )   (   )   ^^^ ^^           \n"
			"                   (   )     ) )   ^ ^ ^ ^^           \n"
			"                    (   ) (  )     ^^^^ ^             \n"
			"                              __[]__   !! !!          \n"
			"                           __|_x_x_x\\__ !! !! ^       \n"
			"                   ~~~ ___|____________|___ ~~~       \n"
			"             ~~~~~     \\~*~*~*~*~*~*~*~*~/            \n"
			"          ~~~~~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~       \n"
			"           ~  ~    ~    ~   ~~   ~    ~   ~~          \n"
			"\n"
			"                 >>> SHE'S GOING DOWN! <<<\n"
			,

			// Frame 4 – Wreckage

			"+------------------------------------------------------+\n"
			"|                      YOU LOSE!                      |\n"
			"|            Your fleet has been destroyed...         |\n"
			"+------------------------------------------------------+\n"
			"\n"
			"                     ^   ^    ^    ^  ^     ^   ^      \n"
			"                    ^^^   ^^     ^^   ^^  ^^ ^^        \n"
			"                  ^    ^^    ^^^   ^     ^ ^   ^       \n"
			"                        !! !!   !! !!   !!! !!!        \n"
			"                    ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~      \n"
			"                   ~  ~  ~   ~ ~   ~    ~     ~~       \n"
			"\n"
			"              >>> WRECKAGE LOST TO THE SEA <<<\n"

		};
		for (int i = 0; i < 4; ++i)
		{
			clearScreen();
			printc(RED, "%s", frames[i]);
			SLEEP(1);
		}
		SLEEP(2);
	}
	else if (PlayerWon)
	{
		const char* frames[] =
		{
		"+======================================================+\n"
		"|                     VICTORY ACHIEVED!               |\n"
		"|         Your fleet dominates the high seas!         |\n"
		"+======================================================+\n"
		"\n"
		"                             |\n"
		"                             |\n"
		"                            /\\\n"
		"                           /  \\\n"
		"                          /    \\\n"
		"                         *      *\n"
		"\n"
		"                      __/___            \n"
		"                _____/______|           \n"
		"        _______/_____[___]_|__\\_____    \n"
		"        \\              < < <       |    \n"
		"       ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
		"        ~    ~     ~     ~  ~   ~~   ~   ~\n"
		"\n"
		"             >>> ENEMY SIGNALS LOST <<<\n",

			// Frame 2 – Firework rising"
			"+======================================================+\n"
			"|                     VICTORY ACHIEVED!               |\n"
			"|         Your fleet dominates the high seas!         |\n"
			"+======================================================+\n"
			"\n"
			"                             *\n"
			"                            *|*\n"
			"                           * | *\n"
			"                            /|\\\n"
			"                           / | \\\n"
			"                          *  *  *\n"
			"\n"
			"                      __/___            \n"
			"                _____/______|           \n"
			"        _______/_____[___]_|__\\_____    \n"
			"        \\              < < <       |    \n"
			"       ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
			"        ~    ~     ~     ~  ~   ~~   ~   ~\n"
			"\n"
			"             >>> THE SEA IS YOURS <<<\n",

			// Frame 3 – Firework explodes
			"+======================================================+\n"
			"|                     VICTORY ACHIEVED!               |\n"
			"|        Fireworks light the sky above your ship.     |\n"
			"+======================================================+\n"
			"\n"
			"                          .''.   *     *\n"
			"                         :_\\/_:    * * *\n"
			"                     .''.: /\\ :'.   *   \n"
			"                    :_\\/_:'.:::.      *\n"
			"                    : /\\ : :::::     * *\n"
			"                     '..' ':::'       *\n"
			"\n"
			"                      __/___            \n"
			"                _____/______|           \n"
			"        _______/_____[___]_|__\\_____    \n"
			"        \\              < < <       |    \n"
			"       ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
			"        ~    ~     ~     ~  ~   ~~   ~   ~\n"
			"\n"
			"           >>> RETURN TO PORT, HERO <<<\n",

			// Frame 4 – More fireworks
			"+======================================================+\n"
			"|                    ABSOLUTE VICTORY!                |\n"
			"|           Celebration erupts across the fleet!       |\n"
			"+======================================================+\n"
			"\n"
			"               *     .''.       *       *     *\n"
			"             * | *  :_\\/_:    * | *   * | *  * | *\n"
			"              *    : /\\ :     *     *    *    *\n"
			"                 * '..' ':::'     * | *     *\n"
			"\n"
			"                      __/___            \n"
			"                _____/______|           \n"
			"        _______/_____[___]_|__\\_____    \n"
			"        \\              < < <       |    \n"
			"       ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
			"        ~    ~     ~     ~  ~   ~~   ~   ~\n"
			"\n"
			"     >>> MISSION COMPLETE. YOU ARE LEGEND. <<<\n"
		};

		for (int i = 0; i < 3; ++i) {
			clearScreen();
			printc(BRIGHT_GREEN, "%s", frames[i]);  // Bright green for victory
			SLEEP(1);
		}

		SLEEP(2);
	}
}


