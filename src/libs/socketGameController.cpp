#include <socketGameController.h>
#include <gameEngine.h>
#include <chrono>
#include <thread>

SocketGameController::SocketGameController(GameEngine *ge, Webserver *ws, ActionResolver *ar, DataSerializationBuilder *dsb, std::string subProtocol) : GameController(ge){
  webServer = ws;
  actionResolver = ar;
  dataSerializationBuilder = dsb;
  this->subProtocol = subProtocol;
  currentState = gameEngine->getGameState();
}

SocketGameController::~SocketGameController() {

}

void SocketGameController::performActions() {
  prevState = currentState;
  currentState = gameEngine->getGameState();
  ClientMessage cm;
  while (webServer->readClientMessage(subProtocol, cm)) {
    ClientAction action;
    switch (cm.type) {
    case ClientMessage::Type::MESSAGE:
      if (actionResolver->resolve(cm.message, action)) {
        handleAction(cm.sessionId, action);
      }
      break;
    case ClientMessage::Type::CLOSED:
      onClose(cm.sessionId);
      break;
    }
  }
}

void SocketGameController::onClose(int sessionId) {
  ClientAction ca;
  ca.type = ClientAction::Type::UNREGISTER;
  handleAction(sessionId, ca);
}
