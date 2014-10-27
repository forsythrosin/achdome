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
  for (auto iter : gamePlayers) {
    GamePlayer *gamePlayer = iter.second;
    wormTracker->createWormHead(gamePlayer->getId(), glm::vec3(0.0, -M_PI/2, 0.0), glm::vec3(0.0, 0.01, 0));
  }
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


std::string Game::getCountry(int playerId) {
  return "";
}

glm::vec2 Game::getPosition(int playerId) {
  float phi = playerId + 1;
  // phi % (3.14 * 2)
  while (phi > 3.14 * 2) {
    phi -= 3.14 * 2;
  }
  float theta = playerId + 1;
  // theta % (3.14 / 2)
  while (theta > 3.14 / 2) {
    theta -= 3.14 / 2;
  }
  return glm::vec2(phi, theta); // phi, theta
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


