#include <playerManager.h>
#include <player.h>
#include <sstream>
#include <glm/ext.hpp>

PlayerManager::PlayerManager(ColorTheme* ct) {
  colorTheme = ct;
}

PlayerManager::~PlayerManager(){
  for(auto pIt : players){
    delete pIt.second;
  }
}

int PlayerManager::connectPlayer() {
  int id = nextPlayerId++;
  std::string playerName = generatePlayerName();
  glm::vec4 color = colorTheme->playerColor(id);

  Player *p = new Player(players.size(), color, playerName);
  players[id] = p;

  std::cout << '"' << playerName << '"' << " connected with id = " << id << std::endl;

  return id;
}

bool PlayerManager::disconnectPlayer(int playerId) {
  if (players.find(playerId) == players.end()) {
    return false;
  }
  if (!players[playerId]->isConnected()) {
    return false;
  }
  Player *player = players[playerId];
  player->disconnect();

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


glm::vec4 PlayerManager::getColor(int playerId) {
  if (players.find(playerId) == players.end()) {
    return glm::vec4(0.0);
  }
  return players[playerId]->getColor();
}

bool PlayerManager::setColor(int playerId, glm::vec4 color) {
  if (players.find(playerId) == players.end()) {
    return false;
  }
  players[playerId]->setColor(color);
  return true;
}

bool PlayerManager::setPointsInGame(int playerId, int gameId, int points) {
  if (players.find(playerId) == players.end()) {
    return false;
  }
  players[playerId]->setPointsInGame(gameId, points);
  return true;
}

int PlayerManager::getPointsInGame(int playerId, int gameId) {
  if (players.find(playerId) == players.end()) {
    return -1;
  }
  return players[playerId]->getPointsInGame(gameId);
}

int PlayerManager::getPointsInGames(int playerId, std::vector<int> gameIds) {
  if (players.find(playerId) == players.end()) {
    return -1;
  }
  int totalPoints = 0;
  bool participated = false;
  for (int gameId : gameIds) {
    int p = players[playerId]->getPointsInGame(gameId);
    if (p != -1) {
      totalPoints += p;
      participated = true;
    }
  }

  if (participated) {
    return totalPoints;
  } else {
    return -1;
  }
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

/**
 * Return all player ids in map.
 */
std::vector<int> PlayerManager::getPlayerIds() {
  std::vector<int> vect;
  for (auto it : players) {
    vect.push_back(it.first);
  }
  return vect;
}
