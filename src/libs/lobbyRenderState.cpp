#include <lobbyRenderState.h>
#include <renderablePanel.h>

LobbyRenderState::LobbyRenderState(sgct::Engine *gEngine) : RenderState(gEngine) {
}

LobbyRenderState::~LobbyRenderState() {
}

void LobbyRenderState::init() {
  font = new sgct_text::Font("Arial", 1.0f);
  RenderablePanel *panel0 = new RenderablePanel(
    glm::vec3(-3.0, 7.5, 2.5),
    6.0,
    1.0
  );
  panel0->setColor(glm::vec4(0.5, 0.5, 0.5, 1.0));
  int panel0Id = renderer->addRenderable(panel0, GL_TRIANGLES, "uiPanelShader.vert", "uiPanelShader.frag", false);

  RenderablePanel *panel1 = new RenderablePanel(
    glm::vec3(-3.0, 7.5, 1.0),
    6.0,
    1.0
  );
  panel1->setColor(glm::vec4(0.5, 0.5, 0.5, 1.0));
  int panel1Id = renderer->addRenderable(panel1, GL_TRIANGLES, "uiPanelShader.vert", "uiPanelShader.frag", false);

  panels.push_back(panel0Id);
  panels.push_back(panel1Id);
}

void LobbyRenderState::preSync() {
}

void LobbyRenderState::draw() {
  for (auto p : panels) {
    renderer->render(p);
  }

  // sgct_text::print(
  //   font,
  //   1.0f,
  //   1.0f,
  //   "Ach %s",
  //   "Dome"
  // );
}

void LobbyRenderState::encode() {
}
void LobbyRenderState::decode() {
}
