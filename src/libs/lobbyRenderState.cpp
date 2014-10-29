#include <lobbyRenderState.h>
#include <renderablePanel.h>
#include <player.h>
#include <playerLobbyTile.h>

LobbyRenderState::LobbyRenderState(sgct::Engine *gEngine) : RenderState(gEngine) {
}

LobbyRenderState::~LobbyRenderState() {
  for (auto p : cachedPlayers) {
    delete p;
  }
  for (auto t : tiles) {
    delete t;
  }
}

void LobbyRenderState::init() {
  cachedPlayers.push_back(new Player(0, glm::vec3(0.5, 0.5, 0.3), "Jonas"));
  cachedPlayers.push_back(new Player(1, glm::vec3(0.3, 0.5, 0.5), "Tomas"));
  cachedPlayers.push_back(new Player(2, glm::vec3(0.5, 0.3, 0.5), "Kalle"));
  cachedPlayers.push_back(new Player(3, glm::vec3(0.3, 0.3, 0.5), "Emil"));

  for (int i = 0; i < cachedPlayers.size(); ++i) {
    tiles.push_back(new PlayerLobbyTile(cachedPlayers.at(i), i, renderer));
  }
}

void LobbyRenderState::preSync() {
}

void LobbyRenderState::draw() {
  for (auto t : tiles) {
    t->render();
  }
}

void LobbyRenderState::encode() {
}

void LobbyRenderState::decode() {
}
