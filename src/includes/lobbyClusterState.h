#pragma once

#include <clusterState.h>
#include <vector>
class RenderablePanel;

class LobbyClusterState : public ClusterState {
 public:
  LobbyClusterState(sgct::Engine *gEngine, GameConfig *gameConfig);
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
};
