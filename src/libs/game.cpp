#include <game.h>
#include <player.h>
#include <gamePlayer.h>
#include <wormTracker.h>
#include <playerManager.h>
#include <iostream>

Game::Game(PlayerManager *playerManager, WormTracker *wormTracker) {
  std::vector<Player*> players = playerManager->getConnectedPlayers();
  
  for (Player *p : players) {
    gamePlayers[p->getId()] = new GamePlayer(p);
  }
  
  playerManager->addEventListener(this);
  wormTracker->addEventListener(this);

  this->wormTracker = wormTracker;
}

bool Game::start() {
  time = 0;
  wormTracker->clear();
  std::vector<GamePlayer*> gps;
  for (auto iter : gamePlayers) {
    gps.push_back(iter.second);
  }
  wormTracker->setPlayers(gps);
  return true;
}


bool Game::end() {
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
    
    if (gamePlayers.find(killer) != gamePlayers.end()) {
      gamePlayers[killer]->addKill(id);
    }
  }
  std::cout << "COLLISION!" << std::endl;
}

void Game::onPlayerDisconnect(int playerId) {

}


bool Game::isAlive(int playerId) {
  return true;
}

bool Game::isParticipating(int playerId) {
  return gamePlayers.find(playerId) != gamePlayers.end();
}

int Game::getKiller(int playerId) {
  return -1;
}

bool Game::hasStartedMoving(int playerId) {
  return false;
}

std::vector<int> Game::getKills(int playerId) {
  return std::vector<int>();
}


void Game::tick() {
  wormTracker->tick(time++);
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


std::string Game::getCountry(int playerId) {
  return "";
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
  
  
  return true;
}


