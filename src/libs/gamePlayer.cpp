#include <gamePlayer.h>
#include <player.h>
#include <vector>
#include <iostream>

GamePlayer::GamePlayer(Player *p) {
  player = p;
  alive = true;
  startedMoving = false;
  killer = -1;
}

std::vector<int> GamePlayer::getKillIds() {
	return std::vector<int>();
}

int GamePlayer::getKiller() {
  return killer;
}


int GamePlayer::getId() {
  return player->getId();
}

bool GamePlayer::isAlive() {
  return alive;
}


bool GamePlayer::kill(int killerId) {
  if (!isAlive()) {
    return false;
  }
  killer = killerId;
}


bool GamePlayer::addKill(int playerId) {
  if (hasKilled(playerId)) {
    return false;
  }
  kills.push_back(playerId);
  return true;
}

bool GamePlayer::hasKilled(int playerId) {
  for (int kill : kills) {
    if (kill == playerId)
      return true;
  }
  return false;
}

bool GamePlayer::startMoving() {
  if (!isAlive()) {
    return false;
  }
  if (!startedMoving) {
    startedMoving = true;
    return true;
  }
  return false;
}
