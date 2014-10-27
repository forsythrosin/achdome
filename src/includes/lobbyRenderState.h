#pragma once

#include <renderState.h>
#include <vector>
class RenderablePanel;

class LobbyRenderState : public RenderState {
 public:
  LobbyRenderState(sgct::Engine *gEngine);
  ~LobbyRenderState();

  void init();
  void preSync();
  void draw();
  void encode();
  void decode();

 private:
  sgct_text::Font *font;
  std::vector<int> panels;
};