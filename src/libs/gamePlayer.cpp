#include <gamePlayer.h>
#include <player.h>
#include <vector>

GamePlayer::GamePlayer(Player *p) {
  player = p;
}

std::vector<int> GamePlayer::getKillIds() {

}

int GamePlayer::getKiller() {

}


int GamePlayer::getId() {
  return player->getId();
}

bool GamePlayer::isAlive() {
  return alive;
}
