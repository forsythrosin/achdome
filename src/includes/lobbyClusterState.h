#pragma once

#include <clusterState.h>
#include <vector>
class RenderablePanel;
class Player;
class PlayerManager;

class LobbyClusterState : public ClusterState {
 public:
  LobbyClusterState(sgct::Engine *gEngine);
  LobbyClusterState(sgct::Engine *gEngine, PlayerManager *playerManager);
  ~LobbyClusterState();

  void attach();
  void detach();

  void preSync();
  void draw();
  void encode();
  void decode();

 private:
  glm::mat4 getMVP(int offset);

  PlayerManager *playerManager;
  sgct::SharedVector<Player*> *sharedPlayers;

  const glm::vec3 LIST_UPPER_LEFT_ANCHOR = glm::vec3(-2.8f, 7.5f, 2.15f);
};
