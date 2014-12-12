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
      //std::cout << "State changed to Intro" << std::endl;
      break;
    }

    case GameEngine::LOBBY: {
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
      //std::cout << "State changed to Lobby" << std::endl;
      break;
    }

    case GameEngine::COUNTDOWN: {
      float time = gameEngine->getSecondsLeftInCountdown();
      DataSerializationBuilder *players = dataSerializationBuilder->group();
      dataSerializationBuilder
        ->add("message", "countdown")
        ->add("data", dataSerializationBuilder->group()
          ->add("round", gameEngine->getGameIndexInTournament() + 1)
          ->add("rounds", gameEngine->getNGamesInTournament())
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

      //std::cout << "State changed to Game" << std::endl;
      break;
    }
    case GameEngine::GAME_OVER: {
      DataSerializationBuilder *data = dataSerializationBuilder->group();
      dataSerializationBuilder
        ->add("message", "gameOver")
        ->add("data", data
          ->add("round", gameEngine->getGameIndexInTournament() + 1)
          ->add("rounds", gameEngine->getNGamesInTournament())
        );
      addGameResults(data, gameEngine->getCurrentGameParticipants());
      std::string message = dataSerializationBuilder->build();
      for (auto it = playerIds.begin(); it != playerIds.end(); it++) {
        sessionId = it->first;
        playerId = it->second;
        if (gameEngine->isInCurrentGame(playerId)) {
          webServer->addMessage(sessionId, message);
        }
      }
      break;
    }
    case GameEngine::TOURNAMENT_OVER: {
      DataSerializationBuilder *data = dataSerializationBuilder->group();
      dataSerializationBuilder
        ->add("message", "tournamentOver")
        ->add("data", data);
      addTournamentResults(data, gameEngine->getCurrentGameParticipants());
      std::string message = dataSerializationBuilder->build();
      for (auto it = playerIds.begin(); it != playerIds.end(); it++) {
        sessionId = it->first;
        playerId = it->second;
        if (gameEngine->isInCurrentGame(playerId)) {
          webServer->addMessage(sessionId, message);
        }
      }
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
          int killerId = gameEngine->getKiller(playerId);
          std::string killerName = gameEngine->getName(killerId);
          glm::vec4 killerColor = gameEngine->getColor(killerId);
          auto data = dataSerializationBuilder->group();
          if (killerId != -1) {
            data
              ->add("killer", dataSerializationBuilder->group()
              ->add("id", killerId)
              ->add("name", killerName)
              ->add("color", killerColor)
            );
          }
          std::string message =
            dataSerializationBuilder
              ->add("message", "died")
              ->add("data", data
                ->add("points", gameEngine->getPointsInGame(playerId))
              )->build();
          webServer->addMessage(sessionId, message);
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
    if (action.strings.count("name") > 0) {
      gameEngine->setName(playerId, action.strings.at("name"));
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
      //std::cout << "Unregister player " << playerId << std::endl;
    }
    break;
  case ClientAction::START_MOVING:
    if (playerIds.count(sessionId) != 0){
      playerId = playerIds.at(sessionId);
      if (gameEngine->startMoving(playerId)) {
        sendMessage = dataSerializationBuilder->add("message", "moving")->build();
        webServer->addMessage(sessionId, sendMessage);
      }
      else {
        //std::cout << "Don't start moving player " << playerId << std::endl;
      }
    }
    break;
  case ClientAction::LEFT_DOWN:
    if (playerIds.count(sessionId) != 0){
      playerId = playerIds.at(sessionId);
      gameEngine->turnLeft(playerId, true);
    }
    break;
  case ClientAction::LEFT_UP:
    if (playerIds.count(sessionId) != 0){
      playerId = playerIds.at(sessionId);
      gameEngine->turnLeft(playerId, false);
    }
    break;
  case ClientAction::RIGHT_DOWN:
    if (playerIds.count(sessionId) != 0){
      playerId = playerIds.at(sessionId);
      gameEngine->turnRight(playerId, true);
    }
    break;
  case ClientAction::RIGHT_UP:
    if (playerIds.count(sessionId) != 0){
      playerId = playerIds.at(sessionId);
      gameEngine->turnRight(playerId, false);
    }
    break;
  default:
    break;
  }
}

void PlayerGameController::addGameResults(DataSerializationBuilder *builder, std::vector<int> ids) {
  DataSerializationBuilder *players = dataSerializationBuilder->group();
  std::sort(ids.begin(), ids.end(), [this](int id1, int id2){ return gameEngine->getPointsInGame(id1) > gameEngine->getPointsInGame(id2); });
  builder->add("players", players);
  for (int i = 0; i < ids.size(); i++) {
    int id = ids[i];
    players->add(std::to_string(i), dataSerializationBuilder->group()
      ->add("player", dataSerializationBuilder->group()
        ->add("id", id)
        ->add("name", gameEngine->getName(id))
        ->add("color", gameEngine->getColor(id))
      )
      ->add("position", i + 1)
      ->add("points", gameEngine->getPointsInGame(id))
    );
  }
}

void PlayerGameController::addTournamentResults(DataSerializationBuilder *builder, std::vector<int> ids) {
  DataSerializationBuilder *players = dataSerializationBuilder->group();
  std::sort(ids.begin(), ids.end(), [this](int id1, int id2){ return gameEngine->getPointsInTournament(id1) > gameEngine->getPointsInTournament(id2); });
  builder->add("players", players);
  for (int i = 0; i < ids.size(); i++) {
    int id = ids[i];
    players->add(std::to_string(i), dataSerializationBuilder->group()
      ->add("player", dataSerializationBuilder->group()
        ->add("id", id)
        ->add("name", gameEngine->getName(id))
        ->add("color", gameEngine->getColor(id))
      )
      ->add("position", i + 1)
      ->add("points", gameEngine->getPointsInTournament(id))
    );
  }
}
