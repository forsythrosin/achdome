#include <gameEngine.h>
#include <wormTracker.h>
#include <game.h>
#include <tween.h>
#include <tweener.h>
#include <cassert>
#include <iostream>
#include <iomanip>

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
  return true;
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
  if (state != State::GAME || currentGame == nullptr) {
    return false;
  }
  wormTracker->saveCollisionBitmapToFile("wormData.ppm");

  // Copy points from GamePlayers to Players.
  std::vector<int> participants = currentGame->getParticipants();
  for (int playerId : participants) {
    playerManager->setPointsInGame(playerId, getCurrentGameId(), currentGame->getPoints(playerId));
  }

  if (gameIndexInTournament < nGamesInTournament - 1) {
    startGameOver();
  } else {
    startTournamentOver();
  }
  return true;
}


void GameEngine::printGameStatistics() {

  std::vector<int> participants = currentGame->getParticipants();
  std::cout << "Results in game " << getCurrentGameId() << ": " << std::endl;
  for (int playerId : participants) {
    std::string name = playerManager->getName(playerId);
    int points = currentGame->getPoints(playerId);
    std::cout << std::setw(20) << std::setfill(' ') << name << ": " << points << std::endl;
  }

}


void GameEngine::printTournamentStatistics() {
  std::vector<int> gameIds = getPlayedGameIdsInTournament();

  std::vector<int> playerIds = playerManager->getPlayerIds();
  std::map<int, int> points;
  for (auto playerId : playerIds) {
    points[playerId] = playerManager->getPointsInGames(playerId, gameIds);
  }
  
  std::cout << "Results in tournament: " << std::endl;
  
  for (auto p : points) {
    int playerId = p.first;
    std::string name = playerManager->getName(playerId);
    int playerPoints = p.second;

    if (playerPoints != -1) {
      std::cout << std::setw(20) << std::setfill(' ') << name << ": " << playerPoints << std::endl;
    }
  }
}


void GameEngine::startGameOver() {
  assert(state == State::GAME);
  assert(currentGame != nullptr);

  float duration = gameConfig->gameOverDuration;
  state = State::GAME_OVER;
  std::cout << "Start game over state." << std::endl;

  printGameStatistics();
  delete currentGame;
  currentGame = nullptr;
  
  gameOverSecondsLeft = duration;
  Tween gameOverTween(gameOverSecondsLeft, [this, duration](double t) {
      this->gameOverSecondsLeft = duration - t*duration;
    }, [this]() {
      this->startCountdown();
    });

  Tweener::getInstance()->startTween(gameOverTween);
}

/**
* End tournament.
*/
bool GameEngine::endTournament() {
  if ((state != State::GAME || currentGame == nullptr) && state != State::GAME_OVER) {
    return false;
  }
  // TODO: Implement this functionality
  std::cout << "End tournament" << std::endl;
  return true;
}


int GameEngine::getCurrentGameId() {
  if (currentGame == nullptr) {
    return -1;
  }
  return currentGame->getId();
}

std::vector<int> GameEngine::getPlayedGameIdsInTournament() {
  assert(currentGame != nullptr);

  std::vector<int> gameIds;

  // get id of the last finished game.
  int lastPlayedGameId = getCurrentGameId();
  if (!currentGame->isOver()) {
    lastPlayedGameId--;
  }
  
  for (int gameId = getCurrentGameId() - gameIndexInTournament; gameId <= lastPlayedGameId; gameId++) {
    gameIds.push_back(gameId);
  }
  return gameIds;
}


void GameEngine::startTournamentOver() {
  assert(state == State::GAME);
  state = State::TOURNAMENT_OVER;
  
  printGameStatistics();
  printTournamentStatistics();
  delete currentGame;
  currentGame = nullptr;

  std::cout << "Start tournament over state.";

  // Start timeout to go to lobby. We may want to change this to a que from admin.
  float duration = gameConfig->gameOverDuration;
  gameOverSecondsLeft = duration;
  Tween tournamentOverTween(gameOverSecondsLeft, [this, duration](double t) {
      this->gameOverSecondsLeft = duration - t*duration;
    }, [this]() {
      this->startLobby();
    });
  Tweener::getInstance()->startTween(tournamentOverTween);
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
