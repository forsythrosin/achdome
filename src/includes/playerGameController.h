#pragma once

#include <socketGameController.h>

class PlayerGameController : public SocketGameController {
public:
  PlayerGameController(GameEngine *ge, Webserver *ws, ActionResolver *ar, DataSerializationBuilder *dsb, std::string subProtocol);
  void performActions();
protected:
  void handleAction(int sessionId, ClientAction action);
  GameEngine::State currentState;
  std::map<int, int> sessionIds;
  std::map<int, int> playerIds;
  std::map<int, bool> lives;
};