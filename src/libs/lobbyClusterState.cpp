#include <lobbyClusterState.h>
#include <player.h>
#include <playerManager.h>
#include <algorithm>
#include <renderableDome.h>
#include <texture2D.h>
#include <textTexture2D.h>
#include <renderableLobbyName.h>
#include <stdio.h>

LobbyClusterState::LobbyClusterState(sgct::Engine *gEngine, GameConfig *gameConfig) : LobbyClusterState(gEngine, gameConfig, nullptr) {
  attached = false;
}

LobbyClusterState::LobbyClusterState(sgct::Engine *gEngine, GameConfig *gameConfig, PlayerManager *playerManager) : ClusterState(gEngine, gameConfig) {
  this->playerManager = playerManager;
  sharedPlayers = new sgct::SharedVector<Player>(100);
  // playerName.setVal("");
  timer.setVal(0);
  setPlayerListAnchor(DEFAULT_PLAYER_LIST_ANCHOR);
  dome = new RenderableDome(50, 20);
  renderablePanel = new RenderableLobbyName();
  font = new Font("fonts/Comfortaa-Light.ttf", 100);
  text = new TextTexture2D(font, "wat");

  attached = false;
}

LobbyClusterState::~LobbyClusterState() {
  delete dome;
  delete sharedPlayers;
}

void LobbyClusterState::attach() {
  domeLogo = renderer->addRenderable(dome, GL_TRIANGLES, "domeShader.vert", "domeLogoShader.frag", true);
  panel = renderer->addRenderable(renderablePanel, GL_TRIANGLES, "uiPanelShader.vert", "textShader.frag", false);

  timer.setVal(0);
  timeUni = new Uniform<float>("time");
  textUni = new Uniform<Texture2D*>("text");
  textUni->setTextureLocation(10, GL_TEXTURE10);

  renderer->setUniform(panel, textUni);
  renderer->setUniform(domeLogo, timeUni);

  attached = true;
}

void LobbyClusterState::detach() {
  renderer->removeRenderable(domeLogo);
  renderer->removeRenderable(panel);

  delete timeUni;
  timeUni = nullptr;

  attached = false;
}

void LobbyClusterState::draw() {
  timeUni->set(timer.getVal());


  // render grid lines
  renderer->render(domeLogo);

  auto players = sharedPlayers->getVal();
  int totalPlayers = players.size();
  for (int offset = 0; offset < totalPlayers; ++offset) {
    Player &player = players.at(offset);
    std::string playerName = player.getName();
    std::transform(playerName.begin(), playerName.end(), playerName.begin(), ::toupper);

    text->setText(playerName);
    Texture2D *texture = text->getTexture();
    textUni->set(texture);

    float ratio = ((float)texture->getWidth())/texture->getHeight();
    renderablePanel->setColor(player.getColor());
    renderablePanel->update(offset, totalPlayers, ratio);
    renderer->render(panel);
  }
}

void LobbyClusterState::setPlayerListAnchor(glm::vec3 anchor) {
  playerListAnchor = anchor;
}

glm::mat4 LobbyClusterState::getMVP(int offset) {
  glm::mat4 mvp = renderer->getMVP();
  glm::mat4 translate = glm::translate(
    glm::mat4(1.0f),
    playerListAnchor + glm::vec3(0.0, 0.0, offset)
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
    auto connectedPlayers = std::vector<Player>();
    for(auto *p : playerManager->getConnectedPlayers()){
      connectedPlayers.push_back(*p);
    }
    sharedPlayers->setVal(connectedPlayers);

    double now = sgct::Engine::instance()->getTime();
    double diff = now - startTime;
    timer.setVal(diff*60);
  }
}

/**
 * Only master:
 * Encode vector with SyncablePlayer:s then encode each player name separately
 */
void LobbyClusterState::encode() {
  sgct::SharedData *data = sgct::SharedData::instance();

  data->writeVector(sharedPlayers);
  data->writeInt(&timer);
}

/**
 * Only slaves:
 * Decode vector with SyncablePlayer:s then decode each player name separately
 */
void LobbyClusterState::decode() {
  sgct::SharedData *data = sgct::SharedData::instance();

  data->readVector(sharedPlayers);
  data->readInt(&timer);
}
