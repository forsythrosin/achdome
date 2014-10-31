#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <playerManager.h>
#include "gameConfig.h"

class WormTracker;
class Game;
class Tween;

class GameEngine {
 public:

  enum State {
    INTRO = 0,
    LOBBY = 1,
    COUNTDOWN = 2,
    GAME = 3,
    GAME_OVER = 4,
    TOURNAMENT_OVER = 5
  };

  GameEngine(WormTracker *wt, PlayerManager *pm, GameConfig *gameConfig);
  // Player related:
  int connectPlayer();
  bool disconnectPlayer(int playerId);
  void disconnectAll();
  std::string getName(int playerId);
  glm::vec4 getColor(int playerId);

  // Player/GamePlayer related:
  bool isInCurrentGame(int playerId);
  std::vector<int> getCurrentGameParticipants();
  glm::vec2 getPosition(int playerId); // phi, theta

  // GamePlayer related:
  bool turnLeft(int playerId, bool turn);
  bool turnRight(int playerId, bool turn);
  bool startMoving(int playerId);
  bool setName(int playerId, std::string name);
  std::vector<Player*> getPlayers();
  bool isAlive(int playerId);
  int getKiller(int playerId);
  std::vector<int> getKills(int playerId);
  bool hasStartedMoving(int playerId);

  // State related:
  State getGameState();
  bool startLobby();
  bool startTournament(int nGames);
  bool endGame();
  bool endTournament();

  float getSecondsLeftInCountdown();
  float getSecondsLeftInGameOver();

  void tick();

 private:
  void startCountdown();
  void startGame();
  void startGameOver();
  void startTournamentOver();

  int nextPlayerId;
  int nextGameId;
  State state;
  Game *currentGame;
  PlayerManager *playerManager;
  WormTracker *wormTracker;
  GameConfig *gameConfig;
  Tween *countdownTween;
  float countdownSecondsLeft;
  float gameOverSecondsLeft;

  int gameIndexInTournament;
  int nGamesInTournament;
  int getGameIndexInTournament();

};
