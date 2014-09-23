/*
Copyright (c) 2014 Miroslav Andel IXEL AB
miroslav.andel@ixel.se
All rights reserved.
*/

#ifndef _WEB_SERVER_
#define _WEB_SERVER_

#include "external/tinythread.h"
#include <vector>

class Webserver
{
public:
	typedef void(*WebMessageCallbackFn)(const char *, size_t);

	Webserver();
	~Webserver();
	void start(int port, int timeout_ms = 5);
	bool isRunning();
	void setRunning(bool state);
	void setCallback(WebMessageCallbackFn cb);
	static Webserver * instance() { return mInstance; }
    inline int getPort() { return mPort; }
    inline int getTimeout() { return mTimeout; }
    unsigned int generateSessionIndex();

	WebMessageCallbackFn mWebMessageCallbackFn;

    bool hasWaitingBroadcasts();

    std::string getNextBroadcast();

    void addBroadcast(std::string broadcast);

private:
	static Webserver * mInstance;

	bool mRunning;
    int mPort;
    int mTimeout; //in ms
    unsigned int mSessionIndex;
	tthread::mutex mMutex;
	tthread::thread * mMainThreadPtr;
    std::vector<std::string> waitingBroadcasts;
};

#endif