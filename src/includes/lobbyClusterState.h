#pragma once

#include <clusterState.h>
#include <vector>
#include <uniformWrappers.h>

class Font;
class TextTexture2D;
class RenderableLobbyName;
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

  Font *font;
  TextTexture2D *text;

  RenderableLobbyName *renderablePanel;

  RenderableDome *dome;
  int domeLogo;
  int panel;
  Uniform<float> *timeUni;
  Uniform<Texture2D*> *textUni;

  glm::vec3 playerListAnchor;
  const glm::vec3 DEFAULT_PLAYER_LIST_ANCHOR = glm::vec3(0.0f, 7.5f, 1.0f);
};
