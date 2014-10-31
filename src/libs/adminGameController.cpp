#include <adminGameController.h>
#include <gameConfig.h>
#include <player.h>

AdminGameController::AdminGameController(GameEngine *ge, Webserver *ws, ActionResolver *ar, DataSerializationBuilder *dsb, std::string subProtocol, GameConfig *gameConfig) : SocketGameController(ge, ws, ar, dsb, subProtocol) {
  numberOfPlayers = 0;
  
  this->gameConfig = gameConfig;
  wormWidth = gameConfig->wormWidth;
  wormSpeed = gameConfig->wormSpeed;
  countdown = gameConfig->countdown;
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

  if (wormWidth != gameConfig->wormWidth || wormSpeed != gameConfig->wormSpeed || countdown != gameConfig->countdown) {
    // Settings changed
    wormWidth = gameConfig->wormWidth;
    wormSpeed = gameConfig->wormSpeed;
    countdown = gameConfig->countdown;

    std::string message =
      dataSerializationBuilder
        ->add("message", "settingsChanged")
        ->add("data", dataSerializationBuilder->group()
          ->add("settings", dataSerializationBuilder->group()
            ->add("wormWidth", wormWidth)
            ->add("wormSpeed", wormSpeed)
            ->add("countdown", countdown)
          )
        )->build();
    webServer->addBroadcast(message, subProtocol);
  }

  if (currentState != prevState) {
    // Game state changed
    std::string sendMessage;
    switch (currentState) {
    case GameEngine::INTRO:

      break;
    case GameEngine::LOBBY:
      sendMessage = dataSerializationBuilder->add("message", "lobby")->build();
      webServer->addBroadcast(sendMessage, subProtocol);
      break;
    case GameEngine::GAME:
      sendMessage = dataSerializationBuilder->add("message", "gameStarted")->build();
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
  case ClientAction::AUTHENTICATE_ADMIN:
    std::cout << "Authenticated admin " << sessionId << std::endl;
    data = dataSerializationBuilder->group();
    dataSerializationBuilder
      ->add("message", "authenticated")
      ->add("data", data);
    addSettings(data);
    addPlayers(data, gameEngine->getPlayers());
    if (currentState == GameEngine::GAME) {
      data->add("started", 1);
    } else if (currentState == GameEngine::LOBBY) {
      data->add("ready", 1);
    }
    sendMessage = dataSerializationBuilder->build();
    webServer->addMessage(sessionId, sendMessage);
    break;
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
    if (action.ints.count("countdown") > 0) {
      std::cout << "countdown = " << action.ints.at("countdown") << std::endl;
      gameConfig->countdown = action.ints.at("countdown");
    }
    break;
  case ClientAction::START_GAME:
    std::cout << "Game started by admin " << sessionId << std::endl;
    gameEngine->startCountdown();
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
    ->add("countdown", gameConfig->countdown)
  );
}