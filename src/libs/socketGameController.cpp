#include <socketGameController.h>
#include <gameEngine.h>
#include <chrono>
#include <thread>

SocketGameController::SocketGameController(GameEngine *ge, Webserver *ws, ActionResolver *ar, DataSerializationBuilder *dsb) {
  gameEngine = ge;
  currentState = ge->getGameState();
  webServer = ws;
  actionResolver = ar;
  dataSerializationBuilder = dsb;
}

SocketGameController::~SocketGameController() {

}

void SocketGameController::performActions() {
  int sessionId;
  int playerId;
  std::string message;
  glm::vec3 color;
  std::string name;
  std::string sendMessage;
  while (webServer->read(sessionId, message)) {
    ClientAction action;
    if (actionResolver->resolve(message, action)) {
      switch (action.type) {
      case ClientAction::START_GAME:
        gameEngine->startGame();
        std::cout << "Game started" << std::endl;
        break;
      case ClientAction::REGISTER:
        playerId = gameEngine->connectPlayer();
        if (action.data.count("name") > 0) {
          gameEngine->setName(playerId, action.data.at("name"));
        }
        sessionIds.insert({ playerId, sessionId });
        playerIds.insert({ sessionId, playerId });
        name = gameEngine->getName(playerId);
        color = gameEngine->getColor(playerId);
        // Mock messages, these sould be built by e.g a messageBuilder
        sendMessage =
          dataSerializationBuilder
            ->add("message", "registered")
            ->add("data", dataSerializationBuilder->group()
              ->add("name", name)
              ->add("color", color)
            )
            ->build();
        webServer->addMessage(sessionId, sendMessage);
        break;
      case ClientAction::UNREGISTER:
        if(playerIds.count( sessionId ) != 0){
          playerId = playerIds.at(sessionId);
          playerIds.erase(sessionId);
          gameEngine->disconnectPlayer(playerId);
          sessionIds.erase(playerId);
          std::cout << "Unregister player " << playerId << std::endl;
        }
        break;
      case ClientAction::START_MOVING:
        playerId = playerIds.at(sessionId);
        if (gameEngine->startMoving(playerId)) {
          // Mock message, this sould be built by e.g a messageBuilder
          webServer->addMessage(sessionId, "{\"message\":\"moving\"}");
        } else {
          std::cout << "Don't start moving player " << playerId << std::endl;
        }
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

  GameEngine::State prevState = currentState;
  currentState = gameEngine->getGameState();
  if (currentState != prevState) {
    // Game state changed
    switch (currentState) {
    case GameEngine::INTRO:
      // Mock message, this sould be built by e.g a messageBuilder
      webServer->addBroadcast("{\"message\":\"register\"}");
      std::cout << "State changed to Intro" << std::endl;
      break;
    case GameEngine::LOBBY:
      if (prevState == GameEngine::GAME) {
        // If coming from game, loop through players already registered and send "registered" message
        for (auto it = playerIds.begin(); it != playerIds.end(); it++) {
          int sessionId = it->first;
          int playerId = it->second;
          name = gameEngine->getName(playerId);
          color = gameEngine->getColor(playerId);
          // Mock messages, these sould be built by e.g a messageBuilder
          webServer->addMessage(sessionId, "{\"message\":\"registered\", \"data\":{\"name\":\"Player\",\"color\":[255,0,0]}}");
        }
      }
      std::cout << "State changed to Lobby" << std::endl;
      break;
    case GameEngine::GAME:
      // Mock message, this sould be built by e.g a messageBuilder
      webServer->addBroadcast("{\"message\":\"countdown\", \"data\":{\"time\":5,\"phi\":0.75,\"theta\":0.75}}");
      std::cout << "Countdown started";
      int time = 5;
      while (time > 0) {
        std::cout << " " << time--;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      }
      std::cout << std::endl;
      webServer->addBroadcast("{\"message\":\"notMoving\"}");
      std::cout << "State changed to Game" << std::endl;
      break;
    }
  }

  if (currentState == GameEngine::GAME) {
    for (auto it = playerIds.begin(); it != playerIds.end(); it++) {
      int sessionId = it->first;
      int playerId = it->second;
      bool isAlive = gameEngine->isAlive(playerId);
      if (lives.count(playerId) != 0) {
        auto it2 = lives.find(playerId);
        bool wasAlive = it2->second;
        if (wasAlive && !isAlive) {
          webServer->addMessage(sessionId, "{\"message\":\"died\", \"data\":{\"name\":\"Tomas\",\"color\":[0,255,0]}}");
        }
        it2->second = isAlive;
      } else {
        lives.insert({playerId, isAlive});
      }
    }
  }
}
