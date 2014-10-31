#include <gameEngine.h>
#include <wormTracker.h>
#include <game.h>
#include <tween.h>
#include <tweener.h>
#include <cassert>

GameEngine::GameEngine(WormTracker* wt, PlayerManager* pm, GameConfig *gameConfig) {
  state = State::LOBBY;
  playerManager = pm;
  currentGame = nullptr;
  countdownSecondsLeft = 0;
  nextPlayerId = 0;
  nextGameId = 0;
  wormTracker = wt;
  this->gameConfig = gameConfig;
}

/**
 * Connect a player and return the new identifier
 */
int GameEngine::connectPlayer() {
  return playerManager->connectPlayer();
}

/**
 * Disconnect a player.
 */
bool GameEngine::disconnectPlayer(int playerId) {
  return playerManager->disconnectPlayer(playerId);
}

/**
 * Get current game participants.
 */
std::vector<int> GameEngine::getCurrentGameParticipants() {
  return currentGame->getParticipants();
}

std::vector<Player*> GameEngine::getPlayers() {
  return playerManager->getConnectedPlayers();
}

/**
 * Turn left.
 */
bool GameEngine::turnLeft(int playerId, bool turn) {
  if (state == State::GAME && currentGame != nullptr) {
    return currentGame->turnLeft(playerId, turn);
  }
  return false;
}

/**
 * Turn right.
 */
bool GameEngine::turnRight(int playerId, bool turn) {
  if (state == State::GAME && currentGame != nullptr) {
    return currentGame->turnRight(playerId, turn);
  }
  return false;
}

/**
 * Start moving.
 */
bool GameEngine::startMoving(int playerId) {
  if (state == State::GAME && currentGame != nullptr) {
    return currentGame->startMoving(playerId);
  }
  return false;
}

/**
 * Set name.
 */
bool GameEngine::setName(int playerId, std::string name) {
  return playerManager->setName(playerId, name);
}


/**
 * Start Lobby.
 */
bool GameEngine::startLobby() {
  if (state == State::INTRO || state == State::GAME_OVER || state == State::TOURNAMENT_OVER) {
    if (currentGame != nullptr) {
      wormTracker->clear();
      currentGame->end();
      delete currentGame;
      currentGame = nullptr;
    }
    std::cout << "Start lobby state." << std::endl;
    state = State::LOBBY;
    return true;
  } else {
    return false;
  }
}


/**
 * Start Countdown.
 */
void GameEngine::startCountdown() {
  std::cout << "Start countdown state." << std::endl;
  gameIndexInTournament++;
  assert(gameIndexInTournament < nGamesInTournament);

  float duration = gameConfig->countdownDuration;
  currentGame = new Game(nextGameId++, playerManager, wormTracker);
  state = State::COUNTDOWN;

  countdownSecondsLeft = duration;
  Tween countdownTween(countdownSecondsLeft, [this, duration](double t) {
      this->countdownSecondsLeft = duration - t*duration;
    }, [this]() {
      this->startGame();
    });
  
  Tweener::getInstance()->startTween(countdownTween);
}

/**
 * Start tournament
 */
bool GameEngine::startTournament(int nGames) {
  if (state != State::LOBBY || currentGame != nullptr) {
    return false;
  }
  nGamesInTournament = nGames;
  gameIndexInTournament = -1; // game not yet started.
  startCountdown();
}

/**
 * Start Game.
 */
void GameEngine::startGame() {
  assert(state == COUNTDOWN);
  assert(currentGame != nullptr);

  currentGame->start();
  state = State::GAME;
}

/**
 * End game.
 */
bool GameEngine::endGame() {
  if (currentGame == nullptr) {
    return false;
  }
  if (gameIndexInTournament < nGamesInTournament - 1) {
    startGameOver();
  } else {
    startTournamentOver();
  }
  return true;
}


void GameEngine::startGameOver() {
  assert(state == State::GAME);
  assert(currentGame != nullptr);

  float duration = gameConfig->gameOverDuration;
  state = State::GAME_OVER;
  std::cout << "Start game over state." << std::endl;

  gameOverSecondsLeft = duration;
  Tween gameOverTween(gameOverSecondsLeft, [this, duration](double t) {
      this->gameOverSecondsLeft = duration - t*duration;
    }, [this]() {
      this->startCountdown();
    });

  Tweener::getInstance()->startTween(gameOverTween);
}


void GameEngine::startTournamentOver() {
  assert(state == State::GAME);
  state = State::TOURNAMENT_OVER;
  std::cout << "Start tournament over state.";
}


std::string GameEngine::getName(int playerId) {
  return playerManager->getName(playerId);
}

bool GameEngine::isAlive(int playerId) {
  return currentGame->isAlive(playerId);
}

bool GameEngine::isInCurrentGame(int playerId) {
  if (!currentGame) return false;
  return currentGame->isParticipating(playerId);
}

int GameEngine::getKiller(int playerId) {
  return currentGame->getKiller(playerId);
}

std::vector<int> GameEngine::getKills(int playerId) {
  return currentGame->getKills(playerId);
}

bool GameEngine::hasStartedMoving(int playerId) {
  return currentGame->hasStartedMoving(playerId);
}

glm::vec4 GameEngine::getColor(int playerId) {
  return playerManager->getColor(playerId);
}


glm::vec2 GameEngine::getPosition(int playerId) {
  return currentGame->getPosition(playerId);
}

float GameEngine::getSecondsLeftInCountdown() {
  return countdownSecondsLeft;
}

void GameEngine::tick() {
  if (state == State::GAME && currentGame != nullptr) {
    currentGame->tick();
    if (currentGame->isOver()) {
      endGame();
    }
  }
}

GameEngine::State GameEngine::getGameState() {
  return state;
}
