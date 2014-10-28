#pragma once
#include <memory>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <boost/lockfree/queue.hpp>
#include <thread>

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

struct websocketConfig : public websocketpp::config::asio {
  // pull default settings from our core config
  typedef websocketpp::config::asio core;

  typedef core::concurrency_type concurrency_type;
  typedef core::request_type request_type;
  typedef core::response_type response_type;
  typedef core::message_type message_type;
  typedef core::con_msg_manager_type con_msg_manager_type;
  typedef core::endpoint_msg_manager_type endpoint_msg_manager_type;

  typedef core::alog_type alog_type;
  typedef core::elog_type elog_type;
  typedef core::rng_type rng_type;
  typedef core::endpoint_base endpoint_base;

  static bool const enable_multithreading = true;

  struct transport_config : public core::transport_config {
    typedef core::concurrency_type concurrency_type;
    typedef core::elog_type elog_type;
    typedef core::alog_type alog_type;
    typedef core::request_type request_type;
    typedef core::response_type response_type;

    static bool const enable_multithreading = true;
  };

  typedef websocketpp::transport::asio::endpoint<transport_config>
    transport_type;

  static const websocketpp::log::level elog_level =
    websocketpp::log::elevel::all;
  static const websocketpp::log::level alog_level =
    websocketpp::log::alevel::all;
};

typedef websocketpp::server<websocketConfig> server;

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
