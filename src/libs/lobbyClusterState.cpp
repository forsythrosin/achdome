#include <lobbyClusterState.h>
#include <player.h>
#include <playerLobbyTile.h>
#include <playerManager.h>
#include <algorithm>


LobbyClusterState::LobbyClusterState(sgct::Engine *gEngine, GameConfig *gameConfig) : LobbyClusterState(gEngine, gameConfig, nullptr) {
  attached = false;
}

LobbyClusterState::LobbyClusterState(sgct::Engine *gEngine, GameConfig *gameConfig, PlayerManager *playerManager) : ClusterState(gEngine, gameConfig) {
  this->playerManager = playerManager;
  sharedPlayers = new sgct::SharedVector<Player*>(100);
  setPlayerListAnchor(DEFAULT_PLAYER_LIST_ANCHOR);

  attached = false;
}

LobbyClusterState::~LobbyClusterState() {
  delete sharedPlayers;
}

void LobbyClusterState::attach() {
  attached = true;
}

void LobbyClusterState::detach() {
  attached = false;
}

void LobbyClusterState::preSync() {
  if (playerManager != nullptr) {
    sharedPlayers->setVal(playerManager->getConnectedPlayers());
  }
}

void LobbyClusterState::draw() {
  std::vector<Player*> players = sharedPlayers->getVal();

  for (int offset = 0; offset < players.size(); ++offset) {
    Player *player = players.at(offset);
    std::string playerName = player->getName();
    std::transform(playerName.begin(), playerName.end(), playerName.begin(), ::toupper);

    sgct_text::print3d(
      sgct_text::FontManager::instance()->getFont("Comfortaa", 50),
      getMVP(offset),
      player->getColor(),
      playerName.c_str()
    );
  }
}

void LobbyClusterState::setPlayerListAnchor(glm::vec3 anchor) {
  playerListAnchor = anchor;
}

glm::mat4 LobbyClusterState::getMVP(int offset) {
  glm::mat4 mvp = renderer->getMVP();
  glm::mat4 translate = glm::translate(
    glm::mat4(1.0f),
    playerListAnchor - glm::vec3(0.0, 0.0, offset)
  );
  glm::mat4 rot_back = glm::rotate(glm::mat4(1.0f), 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
  glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
  return mvp*translate*rot_back*scale;
}

void LobbyClusterState::encode() {
  sgct::SharedData::instance()->writeVector(sharedPlayers);
}

void LobbyClusterState::decode() {
  sgct::SharedData::instance()->readVector(sharedPlayers);
}
