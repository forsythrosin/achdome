#include <playerLobbyTile.h>

#include <player.h>
#include <renderer.h>
#include <renderablePanel.h>
#include <algorithm>

PlayerLobbyTile::PlayerLobbyTile(Player *player, int offset, Renderer *renderer)
    : player(player), offset(offset), renderer(renderer) {

  panel = new RenderablePanel(
    glm::vec3(-3.0, 7.5, 2.5 - offset),
    6.0,
    0.5
  );
  panel->setColor(player->getColor());
  panelId = renderer->addRenderable(panel, GL_TRIANGLES, "uiPanelShader.vert", "uiPanelShader.frag", false);

  glm::mat4 mvp = renderer->getMVP();
  glm::mat4 translate = glm::translate(
    glm::mat4(1.0f),
    glm::vec3(-2.8f, 7.5f, 2.15f - offset)
  );
  glm::mat4 rot_back = glm::rotate(glm::mat4(1.0f), 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
  glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
  textPos = mvp*translate*rot_back*scale;
}

PlayerLobbyTile::~PlayerLobbyTile() {
  renderer->removeRenderable(panelId);
  delete panel;
}

void PlayerLobbyTile::render() {
  renderer->render(panelId);
  std::string playerName = player->getName();
  std::transform(playerName.begin(), playerName.end(), playerName.begin(), ::toupper);
  sgct_text::print3d(
    sgct_text::FontManager::instance()->getFont("Comfortaa", 50),
    textPos,
    player->getColor(),
    playerName.c_str()
  );
}
