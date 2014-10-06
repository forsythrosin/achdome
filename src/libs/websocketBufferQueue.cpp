#include "websocketBufferQueue.h"
#include <functional>
#include <thread>
#include <mutex>

WebsocketBufferQueue::WebsocketBufferQueue(Webserver *server){

    auto func = Webserver::WebsocketCallback([&](std::string str){
        this->pushElement(str);
    });
    server->setCallback(func);
}

void WebsocketBufferQueue::pushElement(QueueElement element) {
    std::mutex mutex;
    std::lock_guard<std::mutex> lock(mutex);
    this->queue.enqueue(element);
}

QueueElement WebsocketBufferQueue::pop() {
    QueueElement elem;
    queue.try_dequeue(elem);
    return elem;
}

bool WebsocketBufferQueue::empty(){
    return this->queue.peek() == NULL;
}