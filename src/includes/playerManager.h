#include <string>
#include <vector>
#include <map>
#include <glm/glm.hpp>

class Player;
class PlayerEventListener;

class PlayerManager {
 public:
  PlayerManager();

  int connectPlayer();
  bool disconnectPlayer(int playerId);
  bool isConnected(int playerId);
  bool setName(int playerId, std::string name);
  std::string getName(int playerId);
  bool setColor(int playerId, glm::vec3 color);
  glm::vec3 getColor(int playerId);

  void addEventListener(PlayerEventListener *pm);
  void removeEventListener(PlayerEventListener *pm);
  int getPlayerIdByName(std::string name);
  std::vector<Player*> getConnectedPlayers();


private:
  std::map<int, Player*> players;
  std::string generatePlayerName();
  int nextPlayerId = 0;
std::vector<PlayerEventListener*> eventListeners;
};