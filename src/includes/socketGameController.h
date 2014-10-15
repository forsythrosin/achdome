#include <gameController.h>
#include <Webserver.h>
#include <actionResolver.h>

class GameEngine;

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
};
