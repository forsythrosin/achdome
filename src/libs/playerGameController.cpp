#include <playerGameController.h>

void PlayerGameController::performActions() {
  SocketGameController::performActions();

  if (currentState != prevState) {
    // Game state changed
    int sessionId;
    int playerId;
    std::string name;
    glm::vec4 color;
    std::string sendMessage;
    switch (currentState) {
    case GameEngine::INTRO: {
      sendMessage = dataSerializationBuilder->add("message", "register")->build();
      webServer->addBroadcast(sendMessage);
      std::cout << "State changed to Intro" << std::endl;
      break;
    }

    case GameEngine::LOBBY: {
      if (prevState == GameEngine::GAME) {
        // If coming from game, loop through players already registered and send "registered" message
        for (auto it = playerIds.begin(); it != playerIds.end(); it++) {
          sessionId = it->first;
          playerId = it->second;
          name = gameEngine->getName(playerId);
          color = gameEngine->getColor(playerId);
          sendMessage =
            dataSerializationBuilder
            ->add("message", "registered")
            ->add("data", dataSerializationBuilder->group()
                  ->add("id", playerId)
                  ->add("name", name)
                  ->add("color", color)
                  )
            ->build();
          webServer->addMessage(sessionId, sendMessage);
        }
      }
      std::cout << "State changed to Lobby" << std::endl;
      break;
    }

    case GameEngine::COUNTDOWN: {
      float time = gameEngine->getSecondsLeftInCountdown();
      DataSerializationBuilder *players = dataSerializationBuilder->group();
      dataSerializationBuilder
        ->add("message", "countdown")
        ->add("data", dataSerializationBuilder->group()
              ->add("time", time)
              ->add("players", players)
              );



      // Gets poitions only of players using this controller
      std::vector<int> activeIds = gameEngine->getCurrentGameParticipants();
      for (int id : activeIds) {
        glm::vec4 color = gameEngine->getColor(id);
        glm::vec2 position = gameEngine->getPosition(id);
        players
          ->add(std::to_string(id), dataSerializationBuilder->group()
                ->add("color", color)
                ->add("position", dataSerializationBuilder->group()
                      ->add("phi", position.x)
                      ->add("theta", position.y)
                      )
                );
      }
      sendMessage = dataSerializationBuilder->build();
      for (auto it = playerIds.begin(); it != playerIds.end(); it++) {
        sessionId = it->first;
        playerId = it->second;
        if (gameEngine->isInCurrentGame(playerId)) {
          webServer->addMessage(sessionId, sendMessage);
        }
      }
      break;
    }

    case GameEngine::GAME: {
      sendMessage = dataSerializationBuilder->add("message", "notMoving")->build();
      for (auto it = playerIds.begin(); it != playerIds.end(); it++) {
        sessionId = it->first;
        playerId = it->second;
        if (gameEngine->isInCurrentGame(playerId)) {
          webServer->addMessage(sessionId, sendMessage);
        }
      }

      std::cout << "State changed to Game" << std::endl;
      break;
    }
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
      }
      else {
        lives.insert({ playerId, isAlive });
      }
    }
  }

}

void PlayerGameController::handleAction(int sessionId, ClientAction action) {
  int playerId;
  std::string name;
  glm::vec4 color;
  std::string sendMessage;
  switch (action.type) {
  case ClientAction::REGISTER:
    playerId = gameEngine->connectPlayer();
    if (action.data.count("name") > 0) {
      gameEngine->setName(playerId, action.data.at("name"));
    }
    sessionIds.insert({playerId, sessionId});
    playerIds.insert({sessionId, playerId});
    name = gameEngine->getName(playerId);
    color = gameEngine->getColor(playerId);
    sendMessage =
      dataSerializationBuilder
      ->add("message", "registered")
      ->add("data", dataSerializationBuilder->group()
            ->add("id", playerId)
            ->add("name", name)
            ->add("color", color)
            )
      ->build();
    webServer->addMessage(sessionId, sendMessage);
    break;
  case ClientAction::UNREGISTER:
    if (playerIds.count(sessionId) != 0){
      playerId = playerIds.at(sessionId);
      playerIds.erase(sessionId);
      if (gameEngine->disconnectPlayer(playerId)) {
        sendMessage =
          dataSerializationBuilder
          ->add("message", "register")
          ->build();
        webServer->addMessage(sessionId, sendMessage);
      }
      sessionIds.erase(playerId);
      std::cout << "Unregister player " << playerId << std::endl;
    }
    break;
  case ClientAction::START_MOVING:
    playerId = playerIds.at(sessionId);
    if (gameEngine->startMoving(playerId)) {
      sendMessage = dataSerializationBuilder->add("message", "moving")->build();
      webServer->addMessage(sessionId, sendMessage);
    }
    else {
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
