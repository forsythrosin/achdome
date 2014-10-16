#include <playerManager.h>
#include <player.h>
#include <string>
#include <sstream>
#include <glm/glm.hpp>

PlayerManager::PlayerManager() {

}

int PlayerManager::connectPlayer() {
  int id = nextPlayerId++;
  std::string playerName = generatePlayerName();
  Player *p = new Player(players.size(), glm::vec3(1.0, 0.0, 0.0), playerName);
  players[id] = p;
  return id;
}

bool PlayerManager::disconnectPlayer(int playerId) {
  if (players.find(playerId) == players.end()) {
    return false;
  }
  if (!players[playerId]->isConnected()) {
    return false;
  }
  players[playerId]->disconnect();
  return true;
}

std::string PlayerManager::generatePlayerName() {
  int id = nextPlayerId;
  while (true) {
    std::stringstream s;
    s << "Player " << id;
    std::string name = s.str();
    if (getPlayerIdByName(name) == -1) {
      return name;
    }
    id++;
  }
  return "";
}


int PlayerManager::getPlayerIdByName(std::string name) {
  for (auto it = players.begin(); it != players.end(); ++it) {
    int id = it->first;
    Player *player = it->second;
    if (name == player->getName()) {
      return id;
    }
  }
  return -1;
}


bool PlayerManager::setName(int playerId, std::string name) {

}


std::string PlayerManager::getName(int playerId) {
  return "";
}


glm::vec3 PlayerManager::getColor(int playerId) {
  return glm::vec3(0.0);
}




bool PlayerManager::setColor(int playerId, glm::vec3 color) {

}

void PlayerManager::addEventListener(PlayerEventListener *pel) {
  eventListeners.push_back(pel);
}

void PlayerManager::removeEventListener(PlayerEventListener *pel) {
  for (int i = 0; i < eventListeners.size(); i++) {
    if (eventListeners[i] == pel) {
      eventListeners.erase(eventListeners.begin() + i);
    }
  }
}

std::vector<Player*> PlayerManager::getConnectedPlayers() {
  std::vector<Player*> vect;
  for (auto it : players) {
    Player *player = it.second;
    if (player->isConnected()) {
      vect.push_back(player);
    }
  }
  return vect;
}
