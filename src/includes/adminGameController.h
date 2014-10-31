#pragma once

#include <socketGameController.h>

class GameConfig;
class Player;

class AdminGameController : public SocketGameController {
public:
  AdminGameController(GameEngine *ge, Webserver *ws, ActionResolver *ar, DataSerializationBuilder *dsb, std::string subProtocol, GameConfig *gameConfig);
  void performActions();
protected:
  void handleAction(int sessionId, ClientAction action);
  int numberOfPlayers;
  GameConfig *gameConfig;
  float wormWidth;
  float wormSpeed;
  int countdown;
  void addPlayers(DataSerializationBuilder*, std::vector<Player*>);
  void addSettings(DataSerializationBuilder*);
};