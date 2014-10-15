#pragma once

class GameEngine;

class GameController {
 public:
  virtual ~GameController() {};
  virtual void performActions() = 0;
};
