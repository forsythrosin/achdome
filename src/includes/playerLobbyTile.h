#pragma once

#include <player.h>
class RenderablePanel;
class Renderer;

class PlayerLobbyTile {
public:
  PlayerLobbyTile(Player *player, int offset, Renderer *renderer);
  ~PlayerLobbyTile();

  void render();

private:
  void init();

  Player *player;
  int offset;
  glm::mat4 textPos;

  RenderablePanel *panel;
  int panelId;
  Renderer *renderer;
};
