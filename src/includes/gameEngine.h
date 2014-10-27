#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <playerManager.h>

class WormTracker;
class Game;


class GameEngine {
 public: 

  enum State {
    INTRO,
    LOBBY,
    GAME
  };

  GameEngine(WormTracker *wt, PlayerManager *pm);
  int connectPlayer();
  bool disconnectPlayer(int playerId);
  void disconnectAll();

  bool turnLeft(int playerId, bool turn);
  bool turnRight(int playerId, bool turn);
  bool startMoving(int playerId);
  bool setName(int playerId, std::string name);

  void startLobby();
  void startGame();
  void endGame();

  std::string getName(int playerId);
  bool isAlive(int playerId);
  bool isInCurrentGame(int playerId);
  int getKiller(int playerId);
  std::vector<int> getKills(int playerId);
  bool hasStartedMoving(int playerId);
  glm::vec3 getColor(int playerId);

  std::string getCountry(int playerId);
  glm::vec2 getPosition(int playerId); // phi, theta
  
  void tick();

  /* TODO: get spawn position */
  State getGameState();
    
 private:
  int nextPlayerId;
  State state;
  Game *currentGame;
  PlayerManager *playerManager;
  WormTracker *wormTracker;
};
