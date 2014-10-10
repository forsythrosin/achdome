#include "websocketBufferQueue.h"
#include <functional>
#include <thread>
#include <mutex>

WebsocketBufferQueue::WebsocketBufferQueue(Webserver *server){

    auto func = Webserver::WebsocketCallback([&](std::string str){
        this->pushElement(str);
    });
    server->setCallback(func);

    this->queue = new boost::lockfree::queue<QueueElement*>(100);
}

WebsocketBufferQueue::~WebsocketBufferQueue(){
  delete queue;
}

void WebsocketBufferQueue::pushElement(QueueElement element) {
    this->queue->push(new QueueElement(element));
}

QueueElement* WebsocketBufferQueue::pop() {
    QueueElement *elem;
    if(queue->pop(elem)){
      return elem;
    }
    return nullptr;
}

bool WebsocketBufferQueue::empty(){
    return this->queue->empty();
}
