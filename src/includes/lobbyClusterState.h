#pragma once

#include <clusterState.h>
#include <vector>
#include <uniformWrappers.h>

class Player;
class PlayerManager;
class RenderableDome;
class SyncablePlayer;

class LobbyClusterState : public ClusterState {
 public:
  LobbyClusterState(sgct::Engine *gEngine,  GameConfig *gameConfig, PlayerManager *playerManager);
  LobbyClusterState(sgct::Engine *gEngine, GameConfig *gameConfig);
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
  sgct::SharedVector<Player> *sharedPlayers;
  sgct::SharedString playerName;
  sgct::SharedInt timer;

  RenderableDome *dome;
  int domeGrid;
  int domeLogo;
  Uniform<float> *timeUni;

  glm::vec3 playerListAnchor;
  const glm::vec3 DEFAULT_PLAYER_LIST_ANCHOR = glm::vec3(-2.8f, 7.5f, 2.15f);
};
