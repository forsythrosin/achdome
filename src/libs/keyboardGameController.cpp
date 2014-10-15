#include <keyboardGameController.h>
#include <gameEngine.h>
#include "sgct.h"

KeyboardGameController::KeyboardGameController(GameEngine *ge) {
  gameEngine = ge;
  playerId = -1;
}

KeyboardGameController::~KeyboardGameController() {

}

void KeyboardGameController::performActions() {
  while (!eventQueue.empty()) {
    KeyEvent e = eventQueue.front();
    perform(e);
    eventQueue.pop();
  }
}

void KeyboardGameController::perform(KeyEvent e) {
  int key = e.first;
  int action = e.second;

  switch (key) {
  case GLFW_KEY_ENTER:
    if (action == GLFW_PRESS) {
      startGame();
      break;
    }
  case GLFW_KEY_UP:
    startMoving();
    break;
  case GLFW_KEY_LEFT:
    turnLeft(action == GLFW_PRESS);
    break;
  case GLFW_KEY_RIGHT:
    turnRight(action == GLFW_PRESS);
    break;
  }
}

void KeyboardGameController::processKeyEvent(int key, int action) {
  eventQueue.push(KeyEvent(key, action));
}


void KeyboardGameController::startGame() {
  if (gameEngine->getGameState() != Game) {
    playerId = gameEngine->connectPlayer();
    gameEngine->startGame();
  }
}


void KeyboardGameController::startMoving() {
  if (playerId != -1) {
    gameEngine->startMoving(playerId);
  }
}

void KeyboardGameController::turnLeft(bool turn) {
  if (playerId != -1) {
    gameEngine->turnLeft(playerId, turn);
  }
}

void KeyboardGameController::turnRight(bool turn) {
  if (playerId != -1) {
    gameEngine->turnRight(playerId, turn);
  }
}
