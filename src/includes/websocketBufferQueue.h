#ifndef __CLIENT_STATE_MAPPER__
#define __CLIENT_STATE_MAPPER__

#include <queue>

#include "clientStates.h"
#include "Webserver.h"
#include <functional>
#include <boost/lockfree/queue.hpp>

typedef std::string QueueElement;

class WebsocketBufferQueue {
public:
    WebsocketBufferQueue(Webserver *server);
    ~WebsocketBufferQueue();
    void pushElement(QueueElement);
    QueueElement* pop();
    bool empty();
private:
    boost::lockfree::queue<QueueElement*> *queue;
};

#endif