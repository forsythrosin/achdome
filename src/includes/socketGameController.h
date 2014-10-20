#include <gameController.h>
#include <gameEngine.h>
#include <webserver.h>
#include <actionResolver.h>

class SocketGameController : public GameController {
public:
  SocketGameController(GameEngine *ge, Webserver *ws, ActionResolver *ar);
  ~SocketGameController();
  void performActions();
private:
  GameEngine *gameEngine;
  Webserver *webServer;
  ActionResolver *actionResolver;
  std::map<int, int> sessionIds;
  std::map<int, int> playerIds;
  std::map<int, bool> lives;
  GameEngine::State currentState;
};
