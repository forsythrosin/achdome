#include <gameEngine.h>

GameEngine::GameEngine(WormTracker* wt) {
  state = Intro;
}

int GameEngine::connectPlayer() {
  return 0;
}

bool GameEngine::disconnectPlayer(int playerId) {
  return false;
}

void GameEngine::turnLeft(int playerId, bool turn) {
  
}

void GameEngine::turnRight(int playerId, bool turn) {
  
}

bool GameEngine::setName(int playerId, std::string name) {
  return false;
}

void GameEngine::startMoving(int playerId) {
  
}

void GameEngine::startLobby() {

}

void GameEngine::startGame() {
  
}

void GameEngine::endGame() {

}

void GameEngine::showResults() {

}

std::string GameEngine::getName(int playerId) {
  return "";
}

bool GameEngine::isAlive(int playerId) {
  return true;
}

int GameEngine::getKiller(int playerId) {
  return -1;
}

std::vector<int> GameEngine::getKills(int playerId) {
  return std::vector<int>();
}

bool GameEngine::hasStartedMoving(int playerId) {
  return false;
}

glm::vec3 GameEngine::getColor(int playerId) {
  return glm::vec3(0.0, 0.0, 0.0);
}

std::string GameEngine::getCountry() {
  return "Sweden";
}

glm::vec2 GameEngine::getPosition() {
  return glm::vec2(0.0, 0.0);
}

void GameEngine::tick() {
  
}

GameState GameEngine::getGameState() {
  return state;
}


