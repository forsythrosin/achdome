#include <gameController.h>
#include <gameEngine.h>
#include <webserver.h>
#include <dataSerializationBuilder.h>
#include <actionResolver.h>

class SocketGameController : public GameController {
public:
  SocketGameController(GameEngine *ge, Webserver *ws, ActionResolver *ar, DataSerializationBuilder *dsb);
  ~SocketGameController();
  void performActions();
private:
  GameEngine *gameEngine;
  Webserver *webServer;
  ActionResolver *actionResolver;
  DataSerializationBuilder *dataSerializationBuilder;
  std::map<int, int> sessionIds;
  std::map<int, int> playerIds;
  std::map<int, bool> lives;
  GameEngine::State currentState;
};
