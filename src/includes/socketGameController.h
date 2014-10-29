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
  GameEngine *gameEngine;
  Webserver *webServer;
  ActionResolver *actionResolver;
  DataSerializationBuilder *dataSerializationBuilder;
  std::string subProtocol;
  virtual void handleAction(int sessionId, ClientAction action) = 0;
};
