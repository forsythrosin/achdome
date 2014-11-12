#pragma once

#include <gameController.h>
#include <vector>
#include <gameEngine.h>

class DumbAIController : public GameController{
public:
  DumbAIController(GameEngine *ge, unsigned int numPlayers);
  virtual ~DumbAIController() {};
  void performActions();
private:
  unsigned int numPlayers;
  std::vector<int> players;
  GameEngine::State currentState;
  GameEngine::State prevState;
};
