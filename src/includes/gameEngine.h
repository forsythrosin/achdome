#include <string>
#include <vector>
#include <glm/glm.hpp>

class WormTracker;

enum GameState {
  Intro,
  Lobby,
  Game,
  Results
};


class GameEngine {
 public:
  GameEngine(WormTracker *wt);
  int connectPlayer();
  bool disconnectPlayer(int playerId);
  void disconnectAll();

  void turnLeft(int playerId, bool turn);
  void turnRight(int playerId, bool turn);
  void startMoving(int playerId);
  bool setName(int playerId, std::string name);

  void startLobby();
  void startGame();
  void endGame();
  void showResults();

  std::string getName(int playerId);
  bool isAlive(int playerId);
  int getKiller(int playerId);
  std::vector<int> getKills(int playerId);
  bool hasStartedMoving(int playerId);
  glm::vec3 getColor(int playerId);

  std::string getCountry();
  glm::vec2 getPosition(); // phi, theta

  void tick();

  /* TODO: get spawn position */
  GameState getGameState();


 private:
  WormTracker *wormTracker;
  GameState state;
  int idCounter;
};
