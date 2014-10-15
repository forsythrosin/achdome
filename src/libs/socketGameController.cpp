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
    Action action;
    if (actionResolver->resolve(message, action)) {
      switch (action.type) {
      case Action::START_GAME:
        gameEngine->startGame();
        break;
      case Action::REGISTER:
        playerId = gameEngine->connectPlayer();
        sessionIds.insert({ playerId, sessionId });
        playerIds.insert({ sessionId, playerId });
        break;
      case Action::UNREGISTER:
        if(playerIds.count( sessionId ) != 0){
          playerId = playerIds.at(sessionId);
          playerIds.erase(sessionId);
        }
        gameEngine->disconnectPlayer(playerId);
        sessionIds.erase(playerId);
        break;
      case Action::START_MOVING:
        playerId = playerIds.at(sessionId);
        gameEngine->startMoving(playerId);
        break;
      case Action::LEFT_DOWN:
        playerId = playerIds.at(sessionId);
        gameEngine->turnLeft(playerId, true);
        break;
      case Action::LEFT_UP:
        playerId = playerIds.at(sessionId);
        gameEngine->turnLeft(playerId, false);
        break;
      case Action::RIGHT_DOWN:
        playerId = playerIds.at(sessionId);
        gameEngine->turnRight(playerId, true);
        break;
      case Action::RIGHT_UP:
        playerId = playerIds.at(sessionId);
        gameEngine->turnRight(playerId, false);
        break;
      }
    }
  }
}
