#include <game.h>
#include <player.h>
#include <gamePlayer.h>
#include <wormTracker.h>
#include <playerManager.h>
#include <iostream>

Game::Game(int gameId, PlayerManager *playerManager, WormTracker *wormTracker) {
  time = -1;
  this->gameId = gameId;
  this->gameOver = false;

  std::vector<Player*> players = playerManager->getConnectedPlayers();
  
  for (Player *p : players) {
    gamePlayers[p->getId()] = new GamePlayer(p);
  }
  
  playerManager->addEventListener(this);
  wormTracker->addEventListener(this);
  this->playerManager = playerManager;
  this->wormTracker = wormTracker;

  wormTracker->clear();
  std::vector<GamePlayer*> gps;
  for (auto iter : gamePlayers) {
    gps.push_back(iter.second);
  }
  wormTracker->setPlayers(gps);
}

Game::~Game() {
  playerManager->removeEventListener(this);
  wormTracker->removeEventListener(this);
} 

int Game::getId() {
  return gameId;
}


bool Game::start() {
  time = 0;
  return true;
}

bool Game::turnLeft(int playerId, bool turn) {
  if (gamePlayers.find(playerId) == gamePlayers.end()) {
    return false;
  }
  if (!gamePlayers[playerId]->isAlive()) {
    return false;
  }
  return wormTracker->turnLeft(playerId, turn);
}

bool Game::turnRight(int playerId, bool turn) {
  if (gamePlayers.find(playerId) == gamePlayers.end()) {
    return false;
  }
  if (!gamePlayers[playerId]->isAlive()) {
    return false;
  }
  return wormTracker->turnRight(playerId, turn);

}

void Game::onWormCollision (WormCollision wc) {
  int id = wc.collider;
  int killer = wc.collidee;

  if (gamePlayers.find(id) != gamePlayers.end()) {
    wormTracker->stopWormHead(wc.collider);
    gamePlayers[id]->kill(wc.collidee);
    playerManager->setPointsInGame(id, this->gameId, getPoints(id));
    
    if (gamePlayers.find(killer) != gamePlayers.end()) {
      gamePlayers[killer]->addKill(id);
    }
  }

  updateGameOver();
}

void Game::onPlayerDisconnect(int playerId) {

}


bool Game::isAlive(int playerId) {
  if (gamePlayers.find(playerId) == gamePlayers.end()) {
    return false;
  }
  return gamePlayers[playerId]->isAlive();
}

int Game::getNumberOfPlayers() {
  return gamePlayers.size();
}

int Game::getNumberOfPlayersAlive() {
  int nPlayersAlive = 0;
  for (auto iter : gamePlayers) {
    GamePlayer *p = iter.second;
    if (p->isAlive()) {
      nPlayersAlive++;
    }
  }
  return nPlayersAlive;
}

int Game::getNumberOfPlayersStartedMoving() {
  int nPlayersStartedMoving = 0;
  for (auto iter : gamePlayers) {
    GamePlayer *p = iter.second;
    if (p->hasStartedMoving()) {
      nPlayersStartedMoving++;
    }
  }
  return nPlayersStartedMoving;
}

int Game::getNumberOfPlayersMoving() {
  int nPlayersMoving = 0;
  for (auto iter : gamePlayers) {
    GamePlayer *p = iter.second;
    if (p->isMoving()) {
      nPlayersMoving++;
    }
  }
  return nPlayersMoving;
}

void Game::updateGameOver() {
  float nPlayers = getNumberOfPlayers();
  float nStarted = getNumberOfPlayersStartedMoving();
  float nAlive = getNumberOfPlayersAlive();

  float startRatio = nStarted/nPlayers;
  gameOver = nAlive <= 1 && startRatio >= 0.5;
}

bool Game::isParticipating(int playerId) {
  return gamePlayers.find(playerId) != gamePlayers.end();
}

int Game::getKiller(int playerId) {
  if (gamePlayers.find(playerId) == gamePlayers.end()) {
    return -1;
  }
  return gamePlayers[playerId]->getKiller();
}

bool Game::hasStartedMoving(int playerId) {
  if (gamePlayers.find(playerId) == gamePlayers.end()) {
    return false;
  }
  return gamePlayers[playerId]->hasStartedMoving();
}

std::vector<int> Game::getKills(int playerId) {
  if (gamePlayers.find(playerId) == gamePlayers.end()) {
    return std::vector<int>();
  }
  return gamePlayers[playerId]->getKillIds();
}


void Game::tick() {
  if (hasStarted() && !isOver()) {
    time++;
  }
  wormTracker->tick(time);
  for (auto iter : gamePlayers) {
    GamePlayer *p = iter.second;
    p->tick();
  }
}

std::vector<int> Game::getParticipants() {
  std::vector<int> ids;
  for (auto iter : gamePlayers) {
    int id = iter.first;
    GamePlayer *gamePlayer = iter.second;
    ids.push_back(id);
  }
  return ids;
}

glm::vec2 Game::getPosition(int playerId) {
  return wormTracker->getSphericalPosition(playerId);
}

bool Game::startMoving(int playerId) {
  if (gamePlayers.find(playerId) == gamePlayers.end()) {
    return false;
  }
  if (!gamePlayers[playerId]->isAlive()) {
    return false;
  }
  if (hasStartedMoving(playerId)) {
    return false;
  }
  gamePlayers[playerId]->startMoving();
  return wormTracker->startWormHead(playerId);
}

int Game::getPoints(int playerId) {
  if (gamePlayers.find(playerId) == gamePlayers.end()) {
    return -1;
  }
  return gamePlayers[playerId]->getPoints();
}

bool Game::isOver() {
  return gameOver;
}

bool Game::hasStarted() {
  return time >= 0;
}
