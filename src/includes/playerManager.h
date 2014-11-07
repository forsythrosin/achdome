#pragma once

#include <string>
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include "colorTheme.h"

class Player;
class PlayerEventListener;

class PlayerManager {
 public:
  PlayerManager(ColorTheme*);
  ~PlayerManager();
  int connectPlayer();
  bool disconnectPlayer(int playerId);
  bool isConnected(int playerId);
  bool setName(int playerId, std::string name);
  std::string getName(int playerId);
  bool setColor(int playerId, glm::vec4 color);
  glm::vec4 getColor(int playerId);

  void addEventListener(PlayerEventListener *pm);
  void removeEventListener(PlayerEventListener *pm);
  int getPlayerIdByName(std::string name);
  std::vector<Player*> getConnectedPlayers();
  std::vector<int> getPlayerIds();

  bool setPointsInGame(int playerId, int gameId, int points);
  int getPointsInGame(int playerId, int gameId);
  int getPointsInGames(int playerId, std::vector<int> gameIds);
private:
  std::map<int, Player*> players;
  std::string generatePlayerName();
  int nextPlayerId = 0;
  std::vector<PlayerEventListener*> eventListeners;
  ColorTheme *colorTheme;
};
