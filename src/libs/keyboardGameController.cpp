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
    switch (action) {
    case GLFW_PRESS:
      turnLeft(true);
      break;
    case GLFW_RELEASE:
      turnLeft(false);
      break;
    }
    break;
  case GLFW_KEY_RIGHT:
    switch (action) {
    case GLFW_PRESS:
      turnRight(true);
      break;
    case GLFW_RELEASE:
      turnRight(false);
      break;
    }
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
