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

  this->clientMessages = new boost::lockfree::queue<QueueElement*>(300);
}
Webserver::~Webserver(){
  webserverThread.join();
  delete clientMessages;
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
  socketServer.stop();
}


void Webserver::addBroadcast(std::string message){
  for(auto sessionInfo : sessionIdToInfo){
    auto session = sessionInfo.second->session;
    server::connection_ptr connection = socketServer.get_con_from_hdl(session);
    connection->send(message);
  }
}
void Webserver::addMessage(int sessionId, std::string message){
  auto sessionInfo = sessionIdToInfo.at(sessionId);
  server::connection_ptr connection = socketServer.get_con_from_hdl(sessionInfo->session);
  connection->send(message);
}

void Webserver::onOpen(connection_hdl handle){
  int sessionId = nextId++;
  SessionInfo *sessionInfo = new SessionInfo();
  sessionInfo->sessionId = sessionId;
  sessionInfo->session = handle;
  sessionIdToInfo.insert({sessionId, sessionInfo});
  sessionHandleToInfo.insert({handle, sessionInfo});
}

void Webserver::onMessage(connection_hdl handle, server::message_ptr message){
  auto sessionInfo = sessionHandleToInfo.at(handle);
  int sessionId = sessionInfo->sessionId;
  auto queueElement = new QueueElement();
  queueElement->message = message->get_payload();
  queueElement->sessionId = sessionId;

  clientMessages->push(queueElement);
}

void Webserver::onHttp(connection_hdl handle){
  server::connection_ptr con = socketServer.get_con_from_hdl(handle);

  // Set status to 200 rather than the default error code
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

bool Webserver::readClientMessage(int &sessionId, std::string &message){
  QueueElement *elem;
  if(clientMessages->pop(elem)){
    sessionId = elem->sessionId;
    message = elem->message;
    delete elem;
    return true;
  }
  return false;
}


void Webserver::onClose(connection_hdl handle){
  auto sessionIt = sessionHandleToInfo.find(handle);
  SessionInfo *sessionInfo = sessionIt->second;
  sessionHandleToInfo.erase(sessionIt);
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
