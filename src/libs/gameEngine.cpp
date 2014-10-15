#include <gameEngine.h>
#include <iostream>

GameEngine::GameEngine(WormTracker* wt) {
  state = Intro;
  idCounter = 0;
}

int GameEngine::connectPlayer() {
  return idCounter++;
}

bool GameEngine::disconnectPlayer(int playerId) {
  return false;
}

void GameEngine::turnLeft(int playerId, bool turn) {
  std::cout << "left turn " << (turn ? "true" : "false") << std::endl;
}

void GameEngine::turnRight(int playerId, bool turn) {
  std::cout << "right turn " << (turn ? "true" : "false") << std::endl;
}

bool GameEngine::setName(int playerId, std::string name) {
  return false;
}

void GameEngine::startMoving(int playerId) {

}

void GameEngine::startLobby() {

}

void GameEngine::startGame() {
  std::cout << "Game started" << std::endl;
}

void GameEngine::endGame() {
  std::cout << "Game ended" << std::endl;
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
