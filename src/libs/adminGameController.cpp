#include <adminGameController.h>

void AdminGameController::performActions() {
  SocketGameController::performActions();

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

  if (action.data.count("password") == 0 || action.data.at("password") != "setPasswordInConfig") {
    std::cout << "Refused admin " << sessionId << std::endl;
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
    dataSerializationBuilder
      ->add("message", "authenticated");
    if (currentState == GameEngine::GAME) {
      dataSerializationBuilder
        ->add("data", dataSerializationBuilder->group()
          ->add("started", 1)
        );
    } else if (currentState == GameEngine::LOBBY) {
      dataSerializationBuilder
        ->add("data", dataSerializationBuilder->group()
          ->add("ready", 1)
        );
    }
    sendMessage = dataSerializationBuilder
      ->build();
    webServer->addMessage(sessionId, sendMessage);
    break;
  case ClientAction::START_GAME:
    // TODO: Change this enum to START_TOURNAMENT and use user-defined number of games.
    std::cout << "Game started by admin " << sessionId << std::endl;
    gameEngine->startTournament(3);
    break;
  case ClientAction::END_GAME:
    std::cout << "Game ended by admin " << sessionId << std::endl;
    gameEngine->endGame();
    break;
  }
}
