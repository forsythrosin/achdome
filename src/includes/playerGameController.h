#pragma once

#include <socketGameController.h>

class PlayerGameController : public SocketGameController {
public:
  PlayerGameController(GameEngine *ge, Webserver *ws, ActionResolver *ar, DataSerializationBuilder *dsb, std::string subProtocol) : SocketGameController(ge, ws, ar, dsb, subProtocol) {};
  void performActions();
protected:
  void handleAction(int sessionId, ClientAction action);
  std::map<int, int> sessionIds;
  std::map<int, int> playerIds;
  std::map<int, bool> lives;
  void addGameResults(DataSerializationBuilder* builder, std::vector<int> playerIds);
  void addTournamentResults(DataSerializationBuilder* builder, std::vector<int> playerIds);
};