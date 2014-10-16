#include <gamePlayer.h>
#include <player.h>
#include <vector>

GamePlayer::GamePlayer(Player *p) {
  player = p;
}

std::vector<int> GamePlayer::getKillIds() {
	return std::vector<int>();
}

int GamePlayer::getKiller() {
	return 0;
}


int GamePlayer::getId() {
  return player->getId();
}

bool GamePlayer::isAlive() {
  return alive;
}
