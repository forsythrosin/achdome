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
    GAME_OVER = 4
  };

  GameEngine(WormTracker *wt, PlayerManager *pm, GameConfig *gameConfig);
  int connectPlayer();
  bool disconnectPlayer(int playerId);
  void disconnectAll();

  bool turnLeft(int playerId, bool turn);
  bool turnRight(int playerId, bool turn);
  bool startMoving(int playerId);
  bool setName(int playerId, std::string name);

  void startLobby();
  void startCountdown();
  void startGame();
  void endGame();
  
  std::vector<int> getCurrentGameParticipants();
  std::vector<Player*> getPlayers();

  std::string getName(int playerId);
  bool isAlive(int playerId);
  bool isInCurrentGame(int playerId);
  int getKiller(int playerId);
  std::vector<int> getKills(int playerId);
  bool hasStartedMoving(int playerId);
  glm::vec4 getColor(int playerId);

  std::string getCountry(int playerId);
  glm::vec2 getPosition(int playerId); // phi, theta
  float getCountdownSecondsLeft();

  void tick();

  /* TODO: get spawn position */
  State getGameState();

 private:
  void createNewGame();
  int nextPlayerId;
  int nextGameId;
  State state;
  Game *currentGame;
  PlayerManager *playerManager;
  WormTracker *wormTracker;
  GameConfig *gameConfig;
  Tween *countdownTween;
  float countdownSecondsLeft;
};
