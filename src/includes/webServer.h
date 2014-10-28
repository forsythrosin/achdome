#pragma once
#include <memory>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <boost/lockfree/queue.hpp>
#include <thread>
typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

struct SessionInfo{
  int sessionId;
  connection_hdl session;
};

struct QueueElement{
  int sessionId;
  std::string message;
};

class Webserver{
public:
  Webserver();
  ~Webserver();
  void start(int port);
  bool readClientMessage(int &sessionId, std::string &message);
  void addBroadcast(std::string message);
  void addMessage(int sessionId, std::string message);
private:
  void startServer(int port);
  void onOpen(connection_hdl handle);
  void onClose(connection_hdl handle);
  void onMessage(connection_hdl handle, server::message_ptr message);
  void onHttp(connection_hdl handle);

  std::map<int, SessionInfo*> sessionIdToInfo;
  std::map<connection_hdl, SessionInfo*> sessionHandleToInfo;
  server socketServer;

  std::thread webserverThread;

  std::atomic_int	 nextId;

  boost::lockfree::queue<QueueElement*> *clientMessages;
};
