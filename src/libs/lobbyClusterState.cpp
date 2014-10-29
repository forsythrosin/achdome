#include <lobbyClusterState.h>
#include <player.h>
#include <playerLobbyTile.h>

LobbyClusterState::LobbyClusterState(sgct::Engine *gEngine) : ClusterState(gEngine) {
  attached = false;
}

LobbyClusterState::~LobbyClusterState() {
  for (auto p : cachedPlayers) {
    delete p;
  }
  for (auto t : tiles) {
    delete t;
  }
}

void LobbyClusterState::attach() {
  cachedPlayers.push_back(new Player(0, glm::vec3(0.5, 0.5, 0.3), "Jonas"));
  cachedPlayers.push_back(new Player(1, glm::vec3(0.3, 0.5, 0.5), "Tomas"));
  cachedPlayers.push_back(new Player(2, glm::vec3(0.5, 0.3, 0.5), "Kalle"));
  cachedPlayers.push_back(new Player(3, glm::vec3(0.3, 0.3, 0.5), "Emil"));

  for (int i = 0; i < cachedPlayers.size(); ++i) {
    tiles.push_back(new PlayerLobbyTile(cachedPlayers.at(i), i, renderer));
  }

  attached = true;
}

void LobbyClusterState::detach() {
  for (auto t : tiles) {
    delete t;
  }
  tiles.clear();
  attached = false;
}


void LobbyClusterState::preSync() {
}

void LobbyClusterState::draw() {
  for (auto t : tiles) {
    t->render();
  }
}

void LobbyClusterState::encode() {
}

void LobbyClusterState::decode() {
}
