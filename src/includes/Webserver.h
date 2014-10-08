/*
Copyright (c) 2014 Miroslav Andel IXEL AB
miroslav.andel@ixel.se
All rights reserved.
*/

#ifndef _WEB_SERVER_
#define _WEB_SERVER_

#include "external/tinythread.h"

#include <vector>
#include <deque>
#include <queue>
#include <map>
#include <functional>
#include <picojson/picojson.h>
#include <boost/thread/shared_mutex.hpp>
#include <boost/lockfree/queue.hpp>
struct libwebsocket;

struct SessionInfo{
    int sessionId;
    boost::lockfree::queue<std::string*> *messages;
    boost::shared_mutex *mtx;
    libwebsocket *wsi;
};


class Webserver {
public:
    typedef std::function<void(std::string)> WebsocketCallback;


    Webserver();
	~Webserver();
	void start(int port, int timeout_ms = 5);
	bool isRunning();
	void setRunning(bool state);
	void setCallback(WebsocketCallback cb);
	static Webserver * instance() { return mInstance; }
    inline int getPort() { return mPort; }
    inline int getTimeout() { return mTimeout; }
    unsigned int generateSessionIndex();

    WebsocketCallback mWebsocketCallback;

	void addBroadcast(std::string broadcast);
	void addMessage(int sessionId, std::string broadcast);
    void addSession(int sessionId, SessionInfo *session);
    bool removeSession(int sessionId);
    boost::lockfree::queue<int>* getSessionsWaitingForWrite(){return sessionsWaitingForWrite;};
    SessionInfo *getSession(int sessionId);
private:
	static Webserver * mInstance;

	bool mRunning;
    int mPort;
    int mTimeout; //in ms
    unsigned int mSessionIndex;
	tthread::thread * mMainThreadPtr;
    std::map<int, SessionInfo*> sessions;
    boost::shared_mutex serverMutex;
    boost::lockfree::queue<int> *sessionsWaitingForWrite;

};

#endif
