#include <lobbyClusterState.h>
#include <player.h>
#include <playerLobbyTile.h>
#include <playerManager.h>
#include <algorithm>
#include <renderableDome.h>
#include <syncablePlayer.h>

LobbyClusterState::LobbyClusterState(sgct::Engine *gEngine, GameConfig *gameConfig) : LobbyClusterState(gEngine, gameConfig, nullptr) {
  attached = false;
}

LobbyClusterState::LobbyClusterState(sgct::Engine *gEngine, GameConfig *gameConfig, PlayerManager *playerManager) : ClusterState(gEngine, gameConfig) {
  this->playerManager = playerManager;
  sharedPlayers = new sgct::SharedVector<SyncablePlayer>(100);
  playerName.setVal("");
  setPlayerListAnchor(DEFAULT_PLAYER_LIST_ANCHOR);
  dome = new RenderableDome(50, 20);

  attached = false;
}

LobbyClusterState::~LobbyClusterState() {
  delete dome;
  delete sharedPlayers;
}

void LobbyClusterState::attach() {
  domeGrid = renderer->addRenderable(dome, GL_LINES, "domeShader.vert", "domeGridShader.frag", true);
  attached = true;
}

void LobbyClusterState::detach() {
  renderer->removeRenderable(domeGrid);
  attached = false;
}

void LobbyClusterState::draw() {
  // render grid lines
  renderer->render(domeGrid);

  for (int offset = 0; offset < players.size(); ++offset) {
    Player &player = players.at(offset);
    std::string playerName = player.getName();
    std::transform(playerName.begin(), playerName.end(), playerName.begin(), ::toupper);

    sgct_text::print3d(
      sgct_text::FontManager::instance()->getFont("Comfortaa", 50),
      getMVP(offset),
      player.getColor(),
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

/**
 * Only master:
 * Replace internal players vector with connected Players from PlayerManager,
 * convert to SyncablePlayers and set the SyncablePlayer:s to be shared with slaves.
 */
void LobbyClusterState::preSync() {
  if (playerManager != nullptr) {
    players.clear();
    std::vector<SyncablePlayer> syncablePlayers;
    for (Player *p : playerManager->getConnectedPlayers()) {
      players.push_back(*p);
      SyncablePlayer sp(p->getId(), p->getColor());
      syncablePlayers.push_back(sp);
    }
    sharedPlayers->setVal(syncablePlayers);
  }
}

/**
 * Only master:
 * Encode vector with SyncablePlayer:s then encode each player name separately
 */
void LobbyClusterState::encode() {
  sgct::SharedData::instance()->writeVector(sharedPlayers);

  for (auto p : players) {
    playerName.setVal(p.getName());
    sgct::SharedData::instance()->writeString(&playerName);
  }
}

/**
 * Only slaves:
 * Decode vector with SyncablePlayer:s then decode each player name separately
 */
void LobbyClusterState::decode() {
  sgct::SharedData::instance()->readVector(sharedPlayers);

  players.clear();
  for (auto sp : sharedPlayers->getVal()) {
    sgct::SharedData::instance()->readString(&playerName);
    std::string name = playerName.getVal();

    // Create a real Player and add to players
    Player p(sp.getId(), sp.getColor(), name);
    players.push_back(p);
  }
}
