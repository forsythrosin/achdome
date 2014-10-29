#pragma once

#include <renderState.h>
#include <vector>
class RenderablePanel;
class Player;
class PlayerLobbyTile;

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
  std::vector<Player*> cachedPlayers;
  sgct::SharedVector<Player*> players;
  std::vector<PlayerLobbyTile*> tiles;
};