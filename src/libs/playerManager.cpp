#include <playerManager.h>
#include <player.h>
#include <string>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

PlayerManager::PlayerManager() {
  std::random_device rd;
  randGen = std::mt19937(rd());
}

int PlayerManager::connectPlayer() {
  int id = nextPlayerId++;
  std::string playerName = generatePlayerName();

  // Generate color in HSV space and convert to RGB
  std::uniform_real_distribution<float> dis(0.0, 1.0);
  float v = 0.9f + 0.1f * dis(randGen); // [0.9,1.0[
  float s = 0.4f + (1 - v) * 0.3f; // [0.4,0.7[
  float h = dis(randGen) * 360.f; // [0.0,360.0[
  glm::vec3 color = glm::rgbColor(glm::vec3(h,s,v));

  Player *p = new Player(players.size(), color, playerName);
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
  if (players.find(playerId) == players.end()) {
    return false;
  }
  players[playerId]->setName(name);
  return true;
}


std::string PlayerManager::getName(int playerId) {
  if (players.find(playerId) == players.end()) {
    return "";
  }
  return players[playerId]->getName();
}


glm::vec3 PlayerManager::getColor(int playerId) {
  if (players.find(playerId) == players.end()) {
    return glm::vec3(0.0);
  }
  return players[playerId]->getColor();
}




bool PlayerManager::setColor(int playerId, glm::vec3 color) {
  if (players.find(playerId) == players.end()) {
    return false;
  }
  players[playerId]->setColor(color);
  return true;
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
