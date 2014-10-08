#include <gameController.h>

class GameEngine;

class SocketGameController : public GameController {
 public:
  SocketGameController(GameEngine *ge, int port);
  ~SocketGameController();
  void performActions();
};
