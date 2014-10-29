#pragma once

#include <gameController.h>
#include <utility>
#include <queue>

class GameEngine;

typedef std::pair<int, int> KeyEvent;

class KeyboardGameController : public GameController {
 public:
  KeyboardGameController(GameEngine *ge);
  ~KeyboardGameController();
  void performActions();
  void processKeyEvent(int key, int action);
 private:
  std::queue<KeyEvent> eventQueue;
  void perform(KeyEvent e);
  void startGame();
  void connectPlayer();
  void startMoving();
  void turnLeft(bool turn);
  void turnRight(bool turn);
  GameEngine *gameEngine;
  int playerId;
};
