#include <game.h>
#include <player.h>
#include <gamePlayer.h>
#include <wormTracker.h>
#include <playerManager.h>

Game::Game(PlayerManager *playerManager, WormTracker *wormTracker) {
  std::vector<Player*> players = playerManager->getConnectedPlayers();
  
  for (Player *p : players) {
    gamePlayers[p->getId()] = new GamePlayer(p);
  }
  
  playerManager->addEventListener(this);
}

bool Game::start() {
  wormTracker->clear();
  for (auto iter : gamePlayers) {
    GamePlayer *gamePlayer = iter.second;
    wormTracker->createWormHead(gamePlayer->getId(), glm::vec3(0.0, 1.0, 0.0), glm::vec3(1.0, 0.0, 0.0));
    
  }
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

}

void Game::onPlayerDisconnect(int playerId) {

}


bool Game::isAlive(int playerId) {
  return true;
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

}




std::string Game::getCountry(int playerId) {
  return "";
}

glm::vec2 Game::getPosition(int playerId) {
  return glm::vec2(0.0); // phi, theta
}

bool Game::startMoving(int playerId) {
  return true;
}

