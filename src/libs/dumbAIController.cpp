#include <dumbAIController.h>

DumbAIController::DumbAIController(GameEngine *ge, unsigned int nPlayers) : GameController(ge){
  if(nPlayers > 100){
    this->numPlayers = 100;
  }
  else{
    this->numPlayers = nPlayers;
  }
}

void DumbAIController::performActions(){
  currentState = gameEngine->getGameState();
  if(currentState != prevState){
    switch (currentState) {
    case GameEngine::LOBBY: {
      if (prevState != GameEngine::GAME) {
        for(auto i = 0u; i < numPlayers; i++){
          players.push_back(gameEngine->connectPlayer());
        }
      }
      break;
    }

    case GameEngine::GAME: {
      for(auto playerId : players){
        gameEngine->startMoving(playerId);
      }
      break;
    }
    case GameEngine::TOURNAMENT_OVER: {
      for(auto playerId : players){
        gameEngine->disconnectPlayer(playerId);
      }
      players.clear();
    }
    }
  }

  prevState = currentState;
}
