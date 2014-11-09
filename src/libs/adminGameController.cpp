#include <adminGameController.h>
#include <gameConfig.h>
#include <player.h>

AdminGameController::AdminGameController(GameEngine *ge, Webserver *ws, ActionResolver *ar, DataSerializationBuilder *dsb, std::string subProtocol, GameConfig *gameConfig) : SocketGameController(ge, ws, ar, dsb, subProtocol) {
  numberOfPlayers = 0;
  
  this->gameConfig = gameConfig;
  wormWidth = gameConfig->wormWidth;
  wormSpeed = gameConfig->wormSpeed;
  turnSpeed = gameConfig->turnSpeed;
  countdown = gameConfig->countdown;
  minTimeBetweenGaps = gameConfig->minTimeBetweenGaps;
  maxTimeBetweenGaps = gameConfig->maxTimeBetweenGaps;
  minTimeInGap = gameConfig->minTimeInGap;
  maxTimeInGap = gameConfig->maxTimeInGap;
};

void AdminGameController::performActions() {
  SocketGameController::performActions();

  std::vector<Player*> connectedPlayers = gameEngine->getPlayers();
  if (numberOfPlayers != connectedPlayers.size()) {
    // Players connected/disconnected
    numberOfPlayers = connectedPlayers.size();
    DataSerializationBuilder *data = dataSerializationBuilder->group();
    dataSerializationBuilder
      ->add("message", "playerListUpdated")
      ->add("data", data);
    addPlayers(data, connectedPlayers);
    std::string message = dataSerializationBuilder->build();
    webServer->addBroadcast(message, subProtocol);
  }

  if (
        wormWidth != gameConfig->wormWidth ||
        wormSpeed != gameConfig->wormSpeed ||
        turnSpeed != gameConfig->turnSpeed ||
        countdown != gameConfig->countdown ||
        minTimeBetweenGaps != gameConfig->minTimeBetweenGaps ||
        maxTimeBetweenGaps != gameConfig->maxTimeBetweenGaps ||
        minTimeInGap != gameConfig->minTimeInGap ||
        maxTimeInGap != gameConfig->maxTimeInGap
     ) {
    // Settings changed
    wormWidth = gameConfig->wormWidth;
    wormSpeed = gameConfig->wormSpeed;
    turnSpeed = gameConfig->turnSpeed;
    countdown = gameConfig->countdown;
    minTimeBetweenGaps = gameConfig->minTimeBetweenGaps;
    maxTimeBetweenGaps = gameConfig->maxTimeBetweenGaps;
    minTimeInGap = gameConfig->minTimeInGap;
    maxTimeInGap = gameConfig->maxTimeInGap;

    DataSerializationBuilder *data = dataSerializationBuilder->group();
    dataSerializationBuilder
      ->add("message", "settingsChanged")
      ->add("data", data);
    addSettings(data);
    std::string message = dataSerializationBuilder->build();
    webServer->addBroadcast(message, subProtocol);
  }

  if (currentState != prevState) {
    DataSerializationBuilder *data = dataSerializationBuilder->group();

    // Game state changed
    std::string sendMessage;
    switch (currentState) {
    case GameEngine::INTRO:

      break;
    case GameEngine::LOBBY:
      addPlayers(data, gameEngine->getPlayers());
      sendMessage = dataSerializationBuilder
        ->add("message", "lobby")
        ->add("data", data)
        ->build();
      webServer->addBroadcast(sendMessage, subProtocol);
      break;
    case GameEngine::COUNTDOWN:
      addPlayers(data, gameEngine->getPlayers());
      sendMessage = dataSerializationBuilder
        ->add("message", "countdown")
        ->add("data", data)
        ->build();
      webServer->addBroadcast(sendMessage, subProtocol);
      break;
    case GameEngine::GAME:
      addPlayers(data, gameEngine->getPlayers());
      sendMessage = dataSerializationBuilder
        ->add("message", "gameStarted")
        ->add("data", data)
        ->build();
      webServer->addBroadcast(sendMessage, subProtocol);
      break;
    case GameEngine::GAME_OVER:
      addPlayers(data, gameEngine->getPlayers());
      sendMessage = dataSerializationBuilder
        ->add("message", "gameOver")
        ->add("data", data)
        ->build();
      webServer->addBroadcast(sendMessage, subProtocol);
      break;
    case GameEngine::TOURNAMENT_OVER:
      addPlayers(data, gameEngine->getPlayers());
      sendMessage = dataSerializationBuilder
        ->add("message", "tournamentOver")
        ->add("data", data)
        ->build();
      webServer->addBroadcast(sendMessage, subProtocol);
      break;
    }
  }
}

void AdminGameController::handleAction(int sessionId, ClientAction action) {
  std::string sendMessage;
  DataSerializationBuilder *data;

  if (action.strings.count("password") == 0 || action.strings.at("password") != gameConfig->password) {
    std::cout << "Refused admin " << sessionId << std::endl;
    if (action.strings.count("password") > 0) {
      std::cout << "with password " << '"' << action.strings.at("password") << '"' << " != " << '"' << gameConfig->password << '"' << std::endl;
    }
    sendMessage =
      dataSerializationBuilder
      ->add("message", "refused")
      ->build();
    webServer->addMessage(sessionId, sendMessage);
    return;
  }

  switch (action.type) {
  case ClientAction::AUTHENTICATE_ADMIN: {
    std::cout << "Authenticated admin " << sessionId << std::endl;
    data = dataSerializationBuilder->group();
    std::string state = "";
    switch (currentState) {
    case GameEngine::LOBBY:
      state = "lobby";
      break;
    case GameEngine::COUNTDOWN:
      state = "countdown";
      break;
    case GameEngine::GAME:
      state = "gameStarted";
      break;
    case GameEngine::GAME_OVER:
      state = "gameOver";
      break;
    case GameEngine::TOURNAMENT_OVER:
      state = "tournamentOver";
      break;
    }
    dataSerializationBuilder
      ->add("message", state)
      ->add("data", data);
    addSettings(data);
    addPlayers(data, gameEngine->getPlayers());
    sendMessage = dataSerializationBuilder->build();
    webServer->addMessage(sessionId, sendMessage);
    break;
  }
  case ClientAction::UPDATE_SETTINGS:
    std::cout << "Settings updated by admin " << sessionId << std::endl;
    if (action.floats.count("wormWidth") > 0) {
      std::cout << "wormWidth = " << action.floats.at("wormWidth") << std::endl;
      gameConfig->wormWidth = action.floats.at("wormWidth");
    }
    if (action.floats.count("wormSpeed") > 0) {
      std::cout << "wormSpeed = " << action.floats.at("wormSpeed") << std::endl;
      gameConfig->wormSpeed = action.floats.at("wormSpeed");
    }
    if (action.floats.count("turnSpeed") > 0) {
      std::cout << "turnSpeed = " << action.floats.at("turnSpeed") << std::endl;
      gameConfig->turnSpeed = action.floats.at("turnSpeed");
    }
    if (action.ints.count("countdown") > 0) {
      std::cout << "countdown = " << action.ints.at("countdown") << std::endl;
      gameConfig->countdown = action.ints.at("countdown");
    }
    if (action.ints.count("minTimeBetweenGaps") > 0) {
      std::cout << "minTimeBetweenGaps = " << action.ints.at("minTimeBetweenGaps") << std::endl;
      gameConfig->minTimeBetweenGaps = action.ints.at("minTimeBetweenGaps");
    }
    if (action.ints.count("maxTimeBetweenGaps") > 0) {
      std::cout << "maxTimeBetweenGaps = " << action.ints.at("maxTimeBetweenGaps") << std::endl;
      gameConfig->maxTimeBetweenGaps = action.ints.at("maxTimeBetweenGaps");
    }
    if (action.ints.count("minTimeInGap") > 0) {
      std::cout << "minTimeInGap = " << action.ints.at("minTimeInGap") << std::endl;
      gameConfig->minTimeInGap = action.ints.at("minTimeInGap");
    }
    if (action.ints.count("maxTimeInGap") > 0) {
      std::cout << "maxTimeInGap = " << action.ints.at("maxTimeInGap") << std::endl;
      gameConfig->maxTimeInGap = action.ints.at("maxTimeInGap");
    }
    break;
  case ClientAction::START_TOURNAMENT:
    std::cout << "Tournament started by admin " << sessionId << std::endl;
    gameEngine->startTournament(action.ints.at("numberOfGames"));
    break;
  case ClientAction::END_TOURNAMENT:
    std::cout << "Tournament ended by admin " << sessionId << std::endl;
    gameEngine->endTournament();
    break;
  case ClientAction::END_GAME:
    std::cout << "Game ended by admin " << sessionId << std::endl;
    gameEngine->endGame();
    break;
  }
}

void AdminGameController::addPlayers(DataSerializationBuilder *builder, std::vector<Player*> connectedPlayers) {
  DataSerializationBuilder *players = dataSerializationBuilder->group();
  builder->add("players", players);
  for (Player *player : connectedPlayers) {
    players->add(std::to_string(player->getId()), dataSerializationBuilder->group()
      ->add("name", player->getName())
      ->add("color", player->getColor())
      ->add("inGame", gameEngine->isInCurrentGame(player->getId()))
    );
  }
}

void AdminGameController::addSettings(DataSerializationBuilder *builder) {
  builder->add("settings", dataSerializationBuilder->group()
    ->add("wormWidth", gameConfig->wormWidth)
    ->add("wormSpeed", gameConfig->wormSpeed)
    ->add("turnSpeed", gameConfig->turnSpeed)
    ->add("countdown", gameConfig->countdown)
    ->add("minTimeBetweenGaps", gameConfig->minTimeBetweenGaps)
    ->add("maxTimeBetweenGaps", gameConfig->maxTimeBetweenGaps)
    ->add("minTimeInGap", gameConfig->minTimeInGap)
    ->add("maxTimeInGap", gameConfig->maxTimeInGap)
  );
}
