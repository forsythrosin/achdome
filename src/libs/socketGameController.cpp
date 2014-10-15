#include <socketGameController.h>
#include <gameEngine.h>

SocketGameController::SocketGameController(GameEngine *ge, Webserver *ws, ActionResolver *ar) {
  gameEngine = ge;
  webServer = ws;
  actionResolver = ar;
}

SocketGameController::~SocketGameController() {

}

void SocketGameController::performActions() {
  int sessionId;
  int playerId;
  std::string message;
  while (webServer->read(sessionId, message)) {
    ClientAction action;
    if (actionResolver->resolve(message, action)) {
      switch (action.type) {
      case ClientAction::START_GAME:
        gameEngine->startGame();
        break;
      case ClientAction::REGISTER:
        playerId = gameEngine->connectPlayer();
        sessionIds.insert({ playerId, sessionId });
        playerIds.insert({ sessionId, playerId });
        webServer->addMessage(sessionId, "{\"message\":\"started\"}");
        break;
      case ClientAction::UNREGISTER:
        if(playerIds.count( sessionId ) != 0){
          playerId = playerIds.at(sessionId);
          playerIds.erase(sessionId);
          gameEngine->disconnectPlayer(playerId);
          sessionIds.erase(playerId);
        }
        break;
      case ClientAction::START_MOVING:
        playerId = playerIds.at(sessionId);
        gameEngine->startMoving(playerId);
        break;
      case ClientAction::LEFT_DOWN:
        playerId = playerIds.at(sessionId);
        gameEngine->turnLeft(playerId, true);
        break;
      case ClientAction::LEFT_UP:
        playerId = playerIds.at(sessionId);
        gameEngine->turnLeft(playerId, false);
        break;
      case ClientAction::RIGHT_DOWN:
        playerId = playerIds.at(sessionId);
        gameEngine->turnRight(playerId, true);
        break;
      case ClientAction::RIGHT_UP:
        playerId = playerIds.at(sessionId);
        gameEngine->turnRight(playerId, false);
        break;
      }
    }
  }
}
