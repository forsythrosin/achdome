#pragma once

#include <clusterState.h>
#include <vector>
class RenderablePanel;
class Player;
class PlayerLobbyTile;

class LobbyClusterState : public ClusterState {
 public:
  LobbyClusterState(sgct::Engine *gEngine);
  ~LobbyClusterState();

  void attach();
  void detach();

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
