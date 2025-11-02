#include "types.h"
#include "colors.h"
#include "Save&load.h"
#include "graphics_and_ui.h"

// Points bounes
#define BASE_BOUNES_EASY 100
#define BASE_BOUNES_MEDUIM 200
#define BASE_BOUNES_HARD 500
#define BASE_BOUNES_NIGHTMARE 1000

#define BOUNES_UNUSED_TURNS 5
#define BOUNES_HITSTEAK 20
#define BOUNES_ACURRACY_NEEDED_EXTRA 55.0f
#define BOUNES_ACURRACY_NEEDED 50.0f
#define BOUNES_ACURRACY_EXTRA_AMOUNT 200
#define BOUNES_ACURRACY_AMOUNT 100

void flushInputBuffer()
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
}

bool getIntInput(const char* prompt, int* output, const char* color)
{
    printc(color, prompt);
    int result = scanf_s("%d", output);
    if (result != 1)
    {
        flushInputBuffer(); // clear junk input
        return false;
    }
    flushInputBuffer(); // clear newline
    return true;
}
// Converts a Rank enum into a human-readable string
const char* getRankName(enum Rank rank)
// Returns the Rank as a string (for printing nicely)
// Purpose:
// Takes an enum value (Rank) and returns the associated Rank name as a string,
// so it can be printed to the player (instead of showing numbers).
{
    switch (rank)
    {
    case Scout:
        return "Deck Swabber";           // Beginner rank
    case Cannoneer:
        return "Cannoneer";              // Mid-rank, artillery expert
    case Captain:
        return "Captain";                // Ship commander
    case Legendary:
        return "Legendary Pirate Lord";  // Ultimate rank
    default:
        return "Unknown";                // Fallback if invalid
    }
}

bool findPlayerName(const char* searchName, Player* foundPlayer)
// Searches the players file for a player by name
// If found, fills the Player struct and returns true
// If not found, returns false
{

    FILE* file = NULL;
    Player tempPlayer;

    if (fopen_s(&file, "players.txt", "r") != 0 || file == NULL) // if we cant open the file for some reason
    {
        printc(RED, "\n[!] Error opening the crew manifest!\n\n");
        return false;
    }

    // Read each player from file
    while (fscanf_s(file, "%s %d %d", tempPlayer.name, (unsigned)sizeof(tempPlayer.name), &tempPlayer.rank, &tempPlayer.score) == 3)
    {
        if (strcmp(tempPlayer.name, searchName) == 0)
        {
            // Match found! Copy the temp player to output
            *foundPlayer = tempPlayer;
            fclose(file);
            return true;
        }
    }

    fclose(file);
    return false;
}

void addNewPlayer(const char* playerName)
// Adds a new player to the crew manifest.
//
// Purpose:
// - Create a new Player struct.
// - Set default starting values (rank = Scout, score = 0).
// - Open "players.txt" in append mode.
// - Write the new player's info at the end of the file.
// - Confirm success to the player.
//
// Inputs:
// - name: Name of the new player (chosen by user).
{
    Player newPlayer;
    FILE* file = NULL;

    // Set up new player
    strcpy_s(newPlayer.name, sizeof(newPlayer.name), playerName);
    newPlayer.rank = Scout;
    newPlayer.score = 0;

    // Open the file for appending
    if (fopen_s(&file, "players.txt", "a") != 0 || file == NULL)
    {
        printc(RED, "\n[!] Couldn't scrawl yer name on the manifest!\n\n");
        return;
    }

    // Write the new player's data
    fprintf_s(file, "%s %d %d\n", newPlayer.name, newPlayer.rank, newPlayer.score);

    fclose(file); // Always close the file

    // Welcome message
    printSlow(BRIGHT_GREEN, "\n[+] Welcome aboard, sailor!\n\n", TYPE_FAST);
}

// Handles the main menu
// ==============================

void printLoginScreen()
{
    printc(BLUE, "\n===================================");
    printSlow(BRIGHT_CYAN, "\n        PLUNDERCELLS", TYPE_FAST);
    printc(BLUE, "\n===================================\n");

    // Menu options
    printSlow(YELLOW, "\n[1] Join as New Sailor", TYPE_SUPERFAST);
    printSlow(YELLOW, "\n[2] Returnin' Crew Member", TYPE_SUPERFAST);
    printSlow(YELLOW, "\n[3] See LeaderBoard", TYPE_SUPERFAST);
}

int getPlayerMenuChoice()
// Gets the player's menu choice from input
// Returns the number entered by the player
{
    int choice;
    while (!getIntInput("\n\nWhat's yer pick, matey? ", &choice,CYAN))
    {
        printc(RED, "[!] Invalid input! Enter a number.\n");
    }
    return choice;
}

Player handleNewPlayerCreation()
// Handles the process of creating a new player
// - Asks for a new name
// - Checks if name is free
// - If name is taken, offers to log in instead
// - Returns a fully loaded Player struct
{
    Player newPlayer;
    char enteredName[50];
    
    while (true)
    {
        // Ask for the player's chosen name
        printSlow(CYAN, "What be yer name, sailor? ", TYPE_FAST);
        scanf_s("%s", enteredName, (unsigned)sizeof(enteredName));
        
        // Check if the name already exists
        if (!findPlayerName(enteredName, &newPlayer))
        {
            // Name is free: create new player
            addNewPlayer(enteredName);
            findPlayerName(enteredName, &newPlayer); // Load the new player
            printSlow(GREEN, "\nWelcome aboard, ye scallywag!\n", TYPE_FAST);
            system("pause");
            break;
        }
        else
        {
            // Name already exists
            printSlow(BRIGHT_RED, "\n[!] That name is already taken!\n", TYPE_FAST);

            // Offer choices
            printSlow(YELLOW, "\n[1] Log into this name", TYPE_SUPERFAST);
            printSlow(YELLOW, "\n[2] Choose a different name", TYPE_SUPERFAST);

            int nameChoice = 0;
            printSlow(CYAN, "\n\nYer choice: ", TYPE_SUPERFAST);
            scanf_s("%d", &nameChoice);

            if (nameChoice == 1)
            {
                // Log into the existing player
                system("pause");
                break;
            }
            else
            {
                // Try entering another name
                clearScreen();
            }
        }
    }

    return newPlayer;
}

Player handleReturningPlayerLogin()
// Handles the process of logging into an existing player
// - Asks for a name
// - Checks if player exists
// - If not found, offers to create a new player
// - Returns a fully loaded Player struct
{
    Player returningPlayer;
    char enteredName[50];

    while (true)
    {
        // Ask for the existing player name
        printSlow(CYAN, "What name ye be sailin' under? ", TYPE_FAST);
        scanf_s("%s", enteredName, (unsigned)sizeof(enteredName));

        // Try to find the player
        if (findPlayerName(enteredName, &returningPlayer))
        {
            // Found! Welcome back.
            char buffer[80];
            sprintf_s(buffer, sizeof(buffer), "\n[+] Ahoy, %s %s!\n", getRankName(returningPlayer.rank), returningPlayer.name);
            printSlow(BRIGHT_GREEN, buffer, TYPE_SUPERFAST);
            system("pause");
            break;
        }
        else
        {
            // No such player found
            printSlow(RED, "\n[!] No such sailor aboard!\n", TYPE_SUPERFAST);

            // Offer to create a new one
            printSlow(YELLOW, "\n[1] Create a new sailor", TYPE_SUPERFAST);
            printSlow(YELLOW, "\n[2] Try enterin' again", TYPE_SUPERFAST);

            int nameChoice = 0;
            printSlow(CYAN, "\n\nYer choice: ", TYPE_SUPERFAST);
            scanf_s("%d", &nameChoice);

            if (nameChoice == 1)
            {
                // Create new player
                addNewPlayer(enteredName);
                findPlayerName(enteredName, &returningPlayer);
                char buffer[80];
                sprintf_s(buffer, sizeof(buffer), "\n[+] Welcome %s %s!\n", getRankName(returningPlayer.rank), enteredName);
                printSlow(GREEN, buffer, TYPE_SUPERFAST);
                system("pause");
                break;
            }
            else
            {
                // Try entering a different name
                clearScreen();
            }
        }
    }

    return returningPlayer;
}

Player playerLoginMenu()
{
    enum MenuOption
    {
        MENU_NEW_PLAYER = 1,
        MENU_RETURNING_PLAYER,
        MENU_VIEW_LEADERBOARD
    };

    Player player;
    bool loggedIn = false;

    while (!loggedIn)
    {
        clearScreen();
        printLoginScreen(); // A small helper to print the title and options

        int menuChoice = getPlayerMenuChoice();

        switch (menuChoice)
        {
        case MENU_NEW_PLAYER:
            player = handleNewPlayerCreation();
            loggedIn = true;
            break;

        case MENU_RETURNING_PLAYER:
            player = handleReturningPlayerLogin();
            loggedIn = true;
            break;

        case MENU_VIEW_LEADERBOARD:
            ShowScoreBoard();
            break;

        default:
            printSlow(RED, "\n[!] Invalid choice, try again.\n", TYPE_SUPERFAST);
            break;
        }
    }

    return player;
}

// ==============================

// Prints the mission options depending on the player's rank
void printAvailableMissions(enum Rank playerRank)
{
    printSlow(CYAN, "\n[1] Easy", TYPE_FAST);

    if (playerRank >= Cannoneer)
        printSlow(CYAN, "\n[2] Medium", TYPE_FAST);
    else
        printSlow(GRAY, "\n[X] Medium (Locked)", TYPE_FAST);

    if (playerRank >= Captain)
        printSlow(CYAN, "\n[3] Hard", TYPE_FAST);
    else
        printSlow(GRAY, "\n[X] Hard (Locked)", TYPE_FAST);

    if (playerRank == Legendary)
        printSlow(BRIGHT_RED, "\n[4] NIGHTMARE", TYPE_FAST);
    else
        printSlow(GRAY, "\n[X] NIGHTMARE (Locked)", TYPE_FAST);
}

enum compLV selectLV(enum Rank playerRank)
// Displays and handles the mission difficulty selection
// Based on the player's current Rank
{
    int choice = 0;
    bool validChoice = false;

    while (!validChoice)
    {
        clearScreen();
        printc(BLUE, "\n==============================");
        printSlow(BRIGHT_CYAN, "\n     Select Your Mission", TYPE_FAST);
        printc(BLUE, "\n==============================\n");

        // Show available missions based on player rank
        printAvailableMissions(playerRank);

        // Get player choice
        while (!getIntInput("\n\nEnter your choice: ", &choice,CYAN))
        {
            printSlow(RED, "\n[!] Invalid input! Enter a number.\n", TYPE_SUPERFAST);
        }

        // Validate choice: only allow options up to rank+1
        if (choice <= playerRank + 1 && choice >= 1)
        {
            validChoice = true;

            switch (choice)
            {
            case 1:
                printSlow(BRIGHT_GREEN, "\nLevel selected: Easy\nGood luck!\n", TYPE_FAST);
                break;
            case 2:
                printSlow(BRIGHT_GREEN, "\nLevel selected: Medium\nGood luck!\n", TYPE_FAST);
                break;
            case 3:
                printSlow(BRIGHT_GREEN, "\nLevel selected: Hard\nGood luck!\n", TYPE_FAST);
                break;
            case 4:
                printSlow(BRIGHT_GREEN, "\nLevel selected: Nightmare\nGood luck!\n", TYPE_FAST);
                printSlow(BRIGHT_RED, "You will need it...\n", TYPE_SLOW + 30);
                break;
            }
            SLEEP(1);
            system("pause");

            // Return the corresponding difficulty
            return (enum compLV)(choice - 1); // Because EASY=0, MEDIUM=1, HARD=2, NIGHTMARE=3
        }
        else
        {
            printSlow(BRIGHT_RED, "\nYou haven't unlocked that level yet!\n", TYPE_FAST);
            system("pause");
        }
    }

    // Should never reach here
    return EASY;
}

bool isTopPlayer(const Player* p)
{
    FILE* fp;
    Player tempPlayer;

    fopen_s(&fp, "players.txt", "r");
    if (fp == NULL)
    {
        printc(RED, "\n[!] Couldn't open crew manifest for checking top score!\n");
        return false;
    }

    while (fscanf_s(fp, "%s %d %d", tempPlayer.name, (unsigned)sizeof(tempPlayer.name), &tempPlayer.rank, &tempPlayer.score) == 3)
    {
        if (strcmp(tempPlayer.name, p->name) != 0) // Don't compare to yourself
        {
            if (tempPlayer.score >= p->score)
            {
                fclose(fp);
                return false; // Someone else has same or higher score
            }
        }
    }

    fclose(fp);
    return true; // No one beat you
}

// Calculates the total points earned based on stats and difficulty
int calculateVictoryPoints(gameStats* stats, enum compLV difficulty)
{
    int basePoints = 0;
    int turnBonus = 0;
    int hiStreakBonus = 0;
    int accuracyBonus = 0;
    float accuracy = 0.0f;

    // Base points for winning
    switch (difficulty)
    {
    case EASY:
        basePoints = BASE_BOUNES_EASY;
        break;
    case MEDUIM:
        basePoints = BASE_BOUNES_MEDUIM;
        break;
    case HARD:
        basePoints = BASE_BOUNES_HARD;
        break;
    case NIGHTMARE:
        basePoints = BASE_BOUNES_NIGHTMARE;
        break;
    }

    // Bonus for unused turns
    int maxTurns = BOARDSIZE * BOARDSIZE;
    int unusedTurns = maxTurns - stats->numOfTurns;
    if (unusedTurns > 0)
    {
        turnBonus = unusedTurns * BOUNES_UNUSED_TURNS;
    }

    // Bonus for best hit streak
    hiStreakBonus = stats->bestHitStreak * BOUNES_HITSTEAK;

    // Bonus for accuracy
    int totalShots = stats->numOfHits + stats->numOfMiss;
    if (totalShots > 0)
    {
        accuracy = ((float)stats->numOfHits / (float)totalShots) * 100.0f;

        if (accuracy >= BOUNES_ACURRACY_NEEDED_EXTRA)
            accuracyBonus = BOUNES_ACURRACY_EXTRA_AMOUNT;
        else if (accuracy >= BOUNES_ACURRACY_NEEDED)
            accuracyBonus = BOUNES_ACURRACY_AMOUNT;
    }

    // Return total points
    return basePoints + turnBonus + hiStreakBonus + accuracyBonus;
}

// Prints the detailed Victory Report showing bonuses and total score
void printVictoryReport(gameStats* stats, enum compLV difficulty, int totalPoints)
{
    clearScreen();
    printSlow(BRIGHT_CYAN, "\n===  VICTORY REPORT ===\n", TYPE_SUPERFAST);

    int basePoints = 0;
    float accuracy = 0.0f;

    // Base points section (repeat calculation for display only)
    switch (difficulty)
    {
    case EASY:
        basePoints = BASE_BOUNES_EASY;
        break;
    case MEDUIM:
        basePoints = BASE_BOUNES_MEDUIM;
        break;
    case HARD:
        basePoints = BASE_BOUNES_HARD;
        break;
    case NIGHTMARE:
        basePoints = BASE_BOUNES_NIGHTMARE;
        break;
    }

    int maxTurns = BOARDSIZE * BOARDSIZE;
    int unusedTurns = maxTurns - stats->numOfTurns;
    int turnBonus = 0;
    if (unusedTurns > 0)
    {
        turnBonus = unusedTurns * BOUNES_UNUSED_TURNS;
    }


    int hiStreakBonus = stats->bestHitStreak * BOUNES_HITSTEAK;

    int totalShots = stats->numOfHits + stats->numOfMiss;
    int accuracyBonus = 0;

    if (totalShots > 0)
    {
        accuracy = ((float)stats->numOfHits / (float)totalShots) * 100.0f;

        if (accuracy >= BOUNES_ACURRACY_NEEDED_EXTRA)
            accuracyBonus = BOUNES_ACURRACY_EXTRA_AMOUNT;
        else if (accuracy >= BOUNES_ACURRACY_NEEDED)
            accuracyBonus = BOUNES_ACURRACY_AMOUNT;
    }

    // Print all the bonus details
    char buffer[120];
    sprintf_s(buffer, sizeof(buffer), "\nBase Points: +%d", basePoints);
    printSlow(BRIGHT_CYAN, buffer, TYPE_SUPERFAST);

    sprintf_s(buffer, sizeof(buffer), "\nTurn Bonus: +%d", turnBonus);
    printSlow(BRIGHT_CYAN, buffer, TYPE_SUPERFAST);

    sprintf_s(buffer, sizeof(buffer), "\nBest Hit Streak Bonus: +%d", hiStreakBonus);
    printSlow(BRIGHT_CYAN, buffer, TYPE_SUPERFAST);

    sprintf_s(buffer, sizeof(buffer), "\nAccuracy Bonus: +%d (Accuracy: %.1f%%)", accuracyBonus, accuracy);
    printSlow(BRIGHT_CYAN, buffer, TYPE_SUPERFAST);

    sprintf_s(buffer, sizeof(buffer), "\n\nTOTAL POINTS EARNED: +%d\n", totalPoints);
    printSlow(BRIGHT_CYAN, buffer, TYPE_SUPERFAST);
}

// Handles player rank up if they beat the proper difficulty
void handleRankUp(Player* p, enum compLV difficulty)
{
    bool rankedUp = false;

    if (p->rank == Scout && difficulty == EASY)
    {
        p->rank = Cannoneer;
        rankedUp = true;
    }
    else if (p->rank == Cannoneer && difficulty == MEDUIM)
    {
        p->rank = Captain;
        rankedUp = true;
    }
    else if (p->rank == Captain && difficulty == HARD)
    {
        p->rank = Legendary;
        rankedUp = true;
    }

    if (rankedUp)
    {
        printSlow(BRIGHT_YELLOW, "\n==  RANK UP! ==\n", TYPE_FAST);
        printSlow(BRIGHT_CYAN, "Ye've earned the title of: ", TYPE_FAST);

        char buffer[100];
        sprintf_s(buffer, sizeof(buffer), "%s\n", getRankName(p->rank));
        printSlow(BRIGHT_GREEN, buffer, TYPE_SLOW);
    }
}

// Handles end-of-game victory scoring, reporting, and rank progression
void endGame(Player* p, gameStats* stats, enum compLV difficulty)
{
    int oldScore = p->score;
    enum Rank oldRank = p->rank;

    // 1. Calculate total points based on performance
    int totalPoints = calculateVictoryPoints(stats, difficulty);

    // 2. Show detailed Victory Report
    printVictoryReport(stats, difficulty, totalPoints);

    // 3. Compare with old score and update if new record
    if (totalPoints > oldScore)
    {
        p->score = totalPoints;
        updatePlayerInFile(p);

        if (isTopPlayer(p))
        {
            printSlow(GREEN, "\n === NEW WORLD RECORD!!! ===\n", TYPE_FAST);
        }
        else
        {
            printSlow(GREEN, "\n === NEW PERSONAL RECORD! ===\n", TYPE_FAST);
        }
    }
    else
    {
        printSlow(YELLOW, "\n[!] Ye fought bravely, but didn't beat yer old record.\n", TYPE_SUPERFAST);
    }

    // 4. Handle Rank Up (based on difficulty beaten)

    handleRankUp(p, difficulty);

    if (p->score != oldScore || p->rank != oldRank)
    {
        updatePlayerInFile(p);
    }

    system("pause");
}

// Updates the player's saved data inside players.txt
void updatePlayerInFile(Player* p)
{
    FILE* originalFile = NULL;
    FILE* tempFile = NULL;
    Player tempPlayer;

    // Open files
    if (fopen_s(&originalFile, "players.txt", "r") != 0 || originalFile == NULL ||
        fopen_s(&tempFile, "temp.txt", "w") != 0 || tempFile == NULL)
    {
        printc(RED, "\n[!] Error opening files for updating!\n");
        return;
    }

    // Copy players one by one, updating the target player
    while (fscanf_s(originalFile, "%s %d %d", tempPlayer.name, (unsigned)sizeof(tempPlayer.name),
        &tempPlayer.rank, &tempPlayer.score) == 3)
    {
        if (strcmp(tempPlayer.name, p->name) == 0)
        {
            // Found the player: write updated info
            fprintf(tempFile, "%s %d %d\n", p->name, p->rank, p->score);
        }
        else
        {
            // Other players: copy without change
            fprintf(tempFile, "%s %d %d\n", tempPlayer.name, tempPlayer.rank, tempPlayer.score);
        }
    }

    fclose(originalFile);
    fclose(tempFile);

    // Replace old file with updated temp file
    if (remove("players.txt") != 0)
    {
        printc(RED, "\n[!] Error removing old player file!\n");
    }
    else if (rename("temp.txt", "players.txt") != 0)
    {
        printc(RED, "\n[!] Error renaming updated player file!\n");
    }
}

// Sorts a list of players in descending order by score (simple bubble sort)
void sortPlayersByScore(Player* list, int count)
{
    for (int i = 0; i < count - 1; ++i)
    {
        for (int j = 0; j < count - i - 1; ++j)
        {
            if (list[j].score < list[j + 1].score)
            {
                // Swap players
                Player temp = list[j];
                list[j] = list[j + 1];
                list[j + 1] = temp;
            }
        }
    }
}

// Displays the top players sorted by score
void ShowScoreBoard()
{
    Player playerList[100]; // Max number of players
    int playerCount = 0;

    FILE* file = NULL;
    if (fopen_s(&file, "players.txt", "r") != 0 || file == NULL)
    {
        printc(RED, "\n[!] Error opening players.txt for reading leaderboard!\n");
        return;
    }

    // Load all players from file
    while (fscanf_s(file, "%s %d %d",
        playerList[playerCount].name, (unsigned)sizeof(playerList[playerCount].name),
        &playerList[playerCount].rank,
        &playerList[playerCount].score) == 3)
    {
        playerCount++;
    }

    fclose(file);

    // Sort players by score
    sortPlayersByScore(playerList, playerCount);

    // Display the sorted leaderboard
    clearScreen();
    printc(BRIGHT_CYAN, "\n=== HALL OF LEGENDS ===\n\n");

    for (int i = 0; i < playerCount; ++i)
    {
        char buffer[120];
        sprintf_s(buffer, sizeof(buffer), "[%d] %s | %s | %d Points\n",
            i + 1, playerList[i].name, getRankName(playerList[i].rank), playerList[i].score);

        printc(YELLOW, buffer);
    }

    system("pause");
}
