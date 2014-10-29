#pragma once

#include <socketGameController.h>

class AdminGameController : public SocketGameController {
public:
  AdminGameController(GameEngine *ge, Webserver *ws, ActionResolver *ar, DataSerializationBuilder *dsb, std::string subProtocol) : SocketGameController(ge, ws, ar, dsb, subProtocol) {};
protected:
  void handleAction(int sessionId, ClientAction action);
};