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

  void setPlayerListAnchor(glm::vec3 anchor);

 private:
  glm::mat4 getMVP(int offset);

  PlayerManager *playerManager;
  sgct::SharedVector<Player*> *sharedPlayers;

  glm::vec3 playerListAnchor;
  const glm::vec3 DEFAULT_PLAYER_LIST_ANCHOR = glm::vec3(-2.8f, 7.5f, 2.15f);
};
