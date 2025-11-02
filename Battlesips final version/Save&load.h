#pragma once

#include "types.h"

// Rank Functions
const char* getRankName(enum Rank rank);

// Player Management
bool findPlayerName(const char* searchName, Player* foundPlayer);
void addNewPlayer(const char* playerName);
void updatePlayerInFile(Player* p);

// Player Login Menu
Player playerLoginMenu();
void printLoginScreen();
int getPlayerMenuChoice();
Player handleNewPlayerCreation();
Player handleReturningPlayerLogin();

// Game Progression
enum compLV selectLV(enum Rank playerRank);
void printAvailableMissions(enum Rank playerRank);

// Victory System
void endGame(Player* p, gameStats* stats, enum compLV difficulty);
int calculateVictoryPoints(gameStats* stats, enum compLV difficulty);
void printVictoryReport(gameStats* stats, enum compLV difficulty, int totalPoints);
bool isTopPlayer(const Player* p);
void handleRankUp(Player* p, enum compLV difficulty);

// Scoreboard
void ShowScoreBoard();
void sortPlayersByScore(Player* list, int count);
