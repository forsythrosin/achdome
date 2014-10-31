/*
Copyright (c) 2014 Miroslav Andel IXEL AB
miroslav.andel@ixel.se
All rights reserved.
*/

#include <webUtils.h>
#include <iostream>
#include <fstream>
#include "Webserver.h"

Webserver::Webserver(){
  socketServer.init_asio();
  socketServer.set_reuse_addr(true);
  socketServer.set_open_handler(bind(&Webserver::onOpen, this, ::_1));
  socketServer.set_close_handler(bind(&Webserver::onClose, this, ::_1));
  socketServer.set_message_handler(bind(&Webserver::onMessage, this, ::_1, ::_2));
  socketServer.set_http_handler(bind(&Webserver::onHttp, this, ::_1));
  socketServer.set_socket_init_handler(bind(&Webserver::onSocketInit, this, ::_1, ::_2));
  socketServer.set_validate_handler(bind(&Webserver::validateHandler, this, ::_1));
}
Webserver::~Webserver(){
  socketServer.stop();
  webserverThread.join();
  for (auto cm : clientMessages) {
    delete cm.second;
  }

  for(auto sIt : sessionIdToInfo){
    delete sIt.second;
  }
}
void Webserver::start(int port){
  webserverThread = std::thread(bind(&Webserver::startServer,this, port));
}

void Webserver::startServer(int port){
  std::cout << "Starting server on port " << port << std::endl;
  socketServer.listen(port);
  socketServer.start_accept();
  try {
      socketServer.run();
  } catch (const std::exception & e) {
      std::cout << e.what() << std::endl;
  } catch (websocketpp::lib::error_code e) {
      std::cout << e.message() << std::endl;
  } catch (...) {
      std::cout << "other exception" << std::endl;
  }
}

void Webserver::addBroadcast(std::string message, std::string subProtocol) {
  for (auto sessionInfo : sessionIdToInfo) {
    auto session = sessionInfo.second->session;
    server::connection_ptr connection = socketServer.get_con_from_hdl(session);
    if (subProtocol.empty() || connection->get_subprotocol() == subProtocol) {
      connection->send(message);
    }
  }
}

void Webserver::addMessage(int sessionId, std::string message){
  if (sessionIdToInfo.count(sessionId) > 0) {
    auto sessionInfo = sessionIdToInfo.at(sessionId);
    server::connection_ptr connection = socketServer.get_con_from_hdl(sessionInfo->session);
    connection->send(message);
  }
}

void Webserver::onOpen(connection_hdl handle) {
  auto con = socketServer.get_con_from_hdl(handle);

  SessionInfo *sessionInfo = new SessionInfo();
  sessionInfo->sessionId = nextId++;
  sessionInfo->session = handle;

  boost::shared_lock<boost::shared_mutex> serverLock(serverMutex);

  con->sessionInfo = sessionInfo;
  sessionIdToInfo.insert({sessionInfo->sessionId, sessionInfo});
}

void Webserver::onMessage(connection_hdl handle, server::message_ptr message){
  auto con = socketServer.get_con_from_hdl(handle);

  std::string subp = con->get_subprotocol();
  if (clientMessages.count(subp) == 0) {
    // Create queue for subprotocol
    boost::unique_lock<boost::shared_mutex> lock(serverMutex);
    clientMessages.insert({subp, new boost::lockfree::queue<ClientMessage *>(300)});
  }
  boost::shared_lock<boost::shared_mutex> lock(serverMutex);
  auto messages = clientMessages.at(subp);

  int sessionId = con->sessionInfo->sessionId;
  auto queueElement = new ClientMessage();
  queueElement->type = ClientMessage::Type::MESSAGE;
  queueElement->message = message->get_payload();
  queueElement->sessionId = sessionId;
  messages->push(queueElement);
}

void Webserver::onHttp(connection_hdl handle){
  auto con = socketServer.get_con_from_hdl(handle);

  std::string path = con->get_resource().substr(1);
  if(path.length() == 0){
    // Default file to serve
    path = "index.html";
  }
  auto resourcePath = webUtils::pathToResource(path);
  std::ifstream fin(resourcePath, std::ios::in | std::ios::binary);
  if(!fin.is_open()){
    con->set_status(websocketpp::http::status_code::not_found);
    return;
  }
  std::ostringstream oss;
  oss << fin.rdbuf();
  std::string fileContent =  oss.str();

  con->set_status(websocketpp::http::status_code::ok);
  con->set_body(fileContent);
}

bool Webserver::readClientMessage(std::string subProtocol, ClientMessage &clientMessage){
  if (clientMessages.count(subProtocol) > 0) {
    boost::shared_lock<boost::shared_mutex> lock(serverMutex);
    auto messages = clientMessages.at(subProtocol);
    ClientMessage *elem;
    if (messages->pop(elem)){
      clientMessage.sessionId = elem->sessionId;
      clientMessage.message = elem->message;
      clientMessage.type = elem->type;
      delete elem;
      return true;
    }
  }
  return false;
}


void Webserver::onClose(connection_hdl handle){
  auto con = socketServer.get_con_from_hdl(handle);
  SessionInfo *sessionInfo = con->sessionInfo;

  // Add closed message to queue
  std::string subp = con->get_subprotocol();
  if (clientMessages.count(subp) == 0) {
    // Create queue for subprotocol
    boost::unique_lock<boost::shared_mutex> lock(serverMutex);
    clientMessages.insert({subp, new boost::lockfree::queue<ClientMessage *>(300)});
  }
  boost::shared_lock<boost::shared_mutex> slock(serverMutex);
  auto messages = clientMessages.at(subp);
  int sessionId = sessionInfo->sessionId;
  auto cm = new ClientMessage();
  cm->type = ClientMessage::Type::CLOSED;
  cm->sessionId = sessionId;
  messages->push(cm);

  sessionIdToInfo.erase(sessionInfo->sessionId);
  delete sessionInfo;
}

void Webserver::onSocketInit(connection_hdl hdl, boost::asio::ip::tcp::socket &s) {
  boost::asio::ip::tcp::no_delay option(true);
  s.set_option(option);
}

bool Webserver::validateHandler(connection_hdl hdl) {
  server::connection_ptr con = socketServer.get_con_from_hdl(hdl);
  const std::vector<std::string> & subp_requests = con->get_requested_subprotocols();
  if(subp_requests.size() > 0){
    con->select_subprotocol(subp_requests[0]);
  }
  return true;
}
