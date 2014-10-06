#ifndef __CLIENT_STATE_MAPPER__
#define __CLIENT_STATE_MAPPER__

#include <queue>

#include "clientStates.h"
#include "Webserver.h"
#include <functional>
#include <readerwriterqueue/readwriterqueue.h>

typedef std::string QueueElement;

class WebsocketBufferQueue {
public:
    WebsocketBufferQueue(Webserver *server);
    void pushElement(QueueElement);
    QueueElement pop();
    bool empty();
private:
    moodycamel::ReaderWriterQueue<QueueElement> queue;

};

#endif