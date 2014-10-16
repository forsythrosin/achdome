#include <gameEngine.h>
#include <iostream>
#include <player.h>
#include <vector>
#include <wormTracker.h>
#include <game.h>

GameEngine::GameEngine(WormTracker* wt, PlayerManager* pm) {
  state = State::INTRO;
  playerManager = pm;
  currentGame = nullptr;
  nextPlayerId = 0;
  wormTracker = wt;
}

/**
 * Connect a player and return the new identifier
 */
int GameEngine::connectPlayer() {
  return playerManager->connectPlayer();
}

/**
 * Disconnect a player.
 */
bool GameEngine::disconnectPlayer(int playerId) {
  return playerManager->disconnectPlayer(playerId);
}

/**
 * Turn left.
 */
bool GameEngine::turnLeft(int playerId, bool turn) {
  if (state == State::GAME && currentGame != nullptr) {
    return currentGame->turnLeft(playerId, turn);
  }
  return false;
}

/**
 * Turn right.
 */
bool GameEngine::turnRight(int playerId, bool turn) {
  if (state == State::GAME && currentGame != nullptr) {
    return currentGame->turnRight(playerId, turn);
  }
  return false;
}

/**
 * Start moving.
 */
bool GameEngine::startMoving(int playerId) {
  if (state == State::GAME && currentGame != nullptr) {
    return currentGame->startMoving(playerId);
  }
  return false;
}

/**
 * Set name.
 */
bool GameEngine::setName(int playerId, std::string name) {
  return playerManager->setName(playerId, name);
}


/**
 * Start Lobby.
 */
void GameEngine::startLobby() {
  state = State::LOBBY;
}

/**
 * Start Lobby.
 */
void GameEngine::startGame() {
  currentGame = new Game(playerManager, wormTracker);
  currentGame->start();
  state = State::GAME;
}

/**
 *
 */
void GameEngine::endGame() {
  currentGame->end();
  delete currentGame;
  currentGame = nullptr;
}


std::string GameEngine::getName(int playerId) {
  return playerManager->getName(playerId);
}

bool GameEngine::isAlive(int playerId) {
  return currentGame->isAlive(playerId);
}

int GameEngine::getKiller(int playerId) {
  return currentGame->getKiller(playerId);
}

std::vector<int> GameEngine::getKills(int playerId) {
  return currentGame->getKills(playerId);
}

bool GameEngine::hasStartedMoving(int playerId) {
  return currentGame->hasStartedMoving(playerId);
}

glm::vec3 GameEngine::getColor(int playerId) {
  return playerManager->getColor(playerId);
}

std::string GameEngine::getCountry(int playerId) {
  return currentGame->getCountry(playerId);
}

glm::vec2 GameEngine::getPosition(int playerId) {
  return currentGame->getPosition(playerId);
}

void GameEngine::tick() {
  if (state == State::GAME && currentGame != nullptr) {
    currentGame->tick();
  }
}

GameEngine::State GameEngine::getGameState() {
  return state;
}
