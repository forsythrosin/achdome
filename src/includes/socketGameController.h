#pragma once

#include <gameController.h>
#include <gameEngine.h>
#include <webserver.h>
#include <dataSerializationBuilder.h>
#include <actionResolver.h>

class SocketGameController : public GameController {
public:
  SocketGameController(GameEngine *ge, Webserver *ws, ActionResolver *ar, DataSerializationBuilder *dsb, std::string subProtocol);
  virtual ~SocketGameController();
  virtual void performActions();
protected:
  Webserver *webServer;
  ActionResolver *actionResolver;
  DataSerializationBuilder *dataSerializationBuilder;
  std::string subProtocol;
  void onClose(int sessionId);
  virtual void handleAction(int sessionId, ClientAction action) = 0;
  GameEngine::State currentState;
  GameEngine::State prevState;
};
