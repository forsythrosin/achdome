#pragma once

class GameEngine;

class GameController {
public:
  GameController(GameEngine *ge){gameEngine = ge;}
  virtual ~GameController() {};
  virtual void performActions() = 0;
protected:
  GameEngine *gameEngine;
};
