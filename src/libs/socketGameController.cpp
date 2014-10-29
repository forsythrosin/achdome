#include <socketGameController.h>
#include <gameEngine.h>
#include <chrono>
#include <thread>

SocketGameController::SocketGameController(GameEngine *ge, Webserver *ws, ActionResolver *ar, DataSerializationBuilder *dsb, std::string subProtocol) {
  gameEngine = ge;
  webServer = ws;
  actionResolver = ar;
  dataSerializationBuilder = dsb;
  this->subProtocol = subProtocol;
}

SocketGameController::~SocketGameController() {

}

void SocketGameController::performActions() {
  int sessionId;
  std::string message;
  while (webServer->readClientMessage(subProtocol, sessionId, message)) {
    ClientAction action;
    if (actionResolver->resolve(message, action)) {
      handleAction(sessionId, action);
    }
  }
}