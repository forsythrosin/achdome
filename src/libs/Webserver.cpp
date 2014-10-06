/*
Copyright (c) 2014 Miroslav Andel IXEL AB
miroslav.andel@ixel.se
All rights reserved.
*/

#include "Webserver.h"
#include <stdio.h>
#include <stdlib.h>
#include <libwebsockets.h> // websocket lib
#include <webUtils.h>
#include <iostream>
#include <cstring>

#define SendBufferLength 32

Webserver * Webserver::mInstance = NULL;
tthread::mutex gSendMutex;
char gSendBuffer[SendBufferLength];

void mainLoop(void * arg);
static int nullHttp(struct libwebsocket_context * context, struct libwebsocket *wsi,
                    enum libwebsocket_callback_reasons reason,  void *user, void *in, size_t len);
static int echoCallback(struct libwebsocket_context * context, struct libwebsocket *wsi,
                        enum libwebsocket_callback_reasons reason, void *user, void *in, size_t len);



// protocol types for websockets
static struct libwebsocket_protocols protocols[] =
        {
                {
                        "http-only",
                        nullHttp,
                        0
                },
                {
                        "sgct",
                        echoCallback,
                        sizeof(SessionInfo)
                },
                {
                        NULL, NULL, 0
                }
        };

const char * get_mimetype(const char *file)
{
	int n = strlen(file);

	fprintf(stderr, "File lenght: %d\n", n);

	if (n < 5)
		return NULL;

	/*if (!strcmp(&file[n - 4], ".ico"))
		return "image/x-icon";*/

	if (!strcmp(&file[n - 4], ".png"))
		return "image/png";

    if (!strcmp(&file[n - 4], ".jpg"))
		return "image/png";

	if (!strcmp(&file[n - 5], ".html"))
		return "text/html";

	if (!strcmp(&file[n - 4], ".css"))
		return "text/css";

	if (!strcmp(&file[n - 5], ".less"))
		return "text/css";

	if (!strcmp(&file[n - 3], ".js"))
		return "text/javascript";

	return NULL;
}

//callbacks
static int nullHttp(
	struct libwebsocket_context * context,
	struct libwebsocket *wsi,
	enum libwebsocket_callback_reasons reason,
	void *user, void *in, size_t len)
{
	char buf[1024];
	memset(buf, 0, 1024);
	const char *mimetype;

	switch (reason)
	{
		case LWS_CALLBACK_HTTP:
		{
			if (len < 1)
			{
				libwebsockets_return_http_status(context, wsi,
					HTTP_STATUS_BAD_REQUEST, NULL);
				return -1;
			}

			const char * data = reinterpret_cast<const char *>(in);

			/* if a legal POST URL, let it continue and accept data */
			if (lws_hdr_total_length(wsi, WSI_TOKEN_POST_URI))
				return 0;

			/* if not, send a file the easy way */
			if (strcmp(data, "/"))
			{
                auto pathToFile = webUtils::absolutePathToResource(data+1);
				strcat(buf, pathToFile.c_str());
			}
			else /* default file to serve */
			{
				//strcat(buf, "D:/Projects/ixel_tools/websocket_test/bin/");
				strcat(buf, webUtils::absolutePathToResource("index.html").c_str());
			}
			buf[sizeof(buf)-1] = '\0';

			/* refuse to serve files we don't understand */
			mimetype = get_mimetype(buf);
			if (!mimetype)
			{
				lwsl_err("Unknown mimetype for %s\n", buf);
				libwebsockets_return_http_status(context, wsi,
					HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE, NULL);
				return -1;
			}

			if (libwebsockets_serve_http_file(context, wsi, buf,
				mimetype, NULL))
				return -1; /* through completion or error, close the socket */
		}
	break;
	}


	return 0;
}

static int echoCallback(struct libwebsocket_context * context,struct libwebsocket *wsi,
	                    enum libwebsocket_callback_reasons reason, void *user, void *in, size_t len){
	// reason for callback
    SessionInfo *sessionInfo = reinterpret_cast<SessionInfo*>(user);
    switch (reason){
        case LWS_CALLBACK_SERVER_WRITEABLE: {
            gSendMutex.lock();
            if(!sessionInfo->messages->empty()){
                std::string message = sessionInfo->messages->front();
                sessionInfo->messages->pop();
                int n = sprintf(gSendBuffer, "%s\n", message.c_str());
                libwebsocket_write(wsi, reinterpret_cast<unsigned char *>(gSendBuffer), n, LWS_WRITE_TEXT);
            }
            gSendMutex.unlock();
        }
        break;
        case LWS_CALLBACK_ESTABLISHED:{
            gSendMutex.lock();
            int sessionId = Webserver::instance()->generateSessionIndex();
            sessionInfo->messages = new std::queue<std::string>();
            sessionInfo->sessionId = sessionId;
            sessionInfo->wsi = wsi;
            Webserver::instance()->addSession(sessionId, sessionInfo);
            gSendMutex.unlock();
        }
        break;
        case LWS_CALLBACK_CLOSED:{
            gSendMutex.lock();
            Webserver::instance()->removeSession(sessionInfo->sessionId);
            delete sessionInfo->messages;
            gSendMutex.unlock();
        }
        break;
        case LWS_CALLBACK_RECEIVE:{
            //libwebsocket_write(wsi, reinterpret_cast<unsigned char *>(in), len, LWS_WRITE_TEXT);

            // log what we recieved and what we're going to send as a response.
            //printf("received data: %s\n", (char *)in);

            if (Webserver::instance()->mWebMessageCallbackFn)
                Webserver::instance()->mWebMessageCallbackFn(reinterpret_cast<const char *>(in), len);
        }
        break;
	}
    return 0;
}



Webserver::Webserver()
{
	mInstance = this;
	mMainThreadPtr = NULL;
	mWebMessageCallbackFn = NULL;
	mRunning = false;
    mSessionIndex = 0;
}

Webserver::~Webserver()
{
	mMutex.lock();
		mRunning = false;
		mWebMessageCallbackFn = NULL;
	mMutex.unlock();

	if (mMainThreadPtr && mMainThreadPtr->joinable())
	{
		fprintf(stderr, "Waiting for websocket thread to finish...\n");

		mMainThreadPtr->join();
		delete mMainThreadPtr;
		mMainThreadPtr = NULL;
	}
}

void Webserver::start(int port, int timeout_ms)
{
	mRunning = true;
    mPort = port;
    mTimeout = timeout_ms;
	mMainThreadPtr = new (std::nothrow) tthread::thread(mainLoop, this);
	memset(gSendBuffer, 0, SendBufferLength);
}

bool Webserver::isRunning()
{
	bool tmpBool;
	mMutex.lock();
	tmpBool = mRunning;
	mMutex.unlock();
	return tmpBool;
}

void Webserver::setRunning(bool state)
{
	mMutex.lock();
	mRunning = state;
	mMutex.unlock();
}

void Webserver::setCallback(Webserver::WebMessageCallbackFn cb)
{
	mMutex.lock();
	mWebMessageCallbackFn = cb;
	mMutex.unlock();
}

unsigned int Webserver::generateSessionIndex()
{
    //session index 0 is invalid all clients having an index > 0 is ok

    unsigned int tmpUi;
    mMutex.lock();
    mSessionIndex++;
	tmpUi = mSessionIndex;
	mMutex.unlock();
    return tmpUi;
}


void Webserver::addBroadcast(std::string broadcast) {
    mMutex.lock();
    for(auto session : sessions){
        session.second->messages->push(broadcast);
        this->sessionsWaitingForWrite.push_front(session.second->sessionId);
    }
    mMutex.unlock();
}

void Webserver::addSession(int sessionId, SessionInfo *session) {
    mMutex.lock();
    this->sessions.insert({sessionId, session});
    mMutex.unlock();
}

bool Webserver::removeSession(int sessionId){
    mMutex.lock();
    this->sessions.erase(sessionId);
    this->sessionsWaitingForWrite.erase(
            std::find(this->sessionsWaitingForWrite.begin(),
                    this->sessionsWaitingForWrite.end(),
                    sessionId)
    );
    mMutex.unlock();
    return true;
}

SessionInfo* Webserver::getSession(int sessionId){
    mMutex.lock();
    auto sessionIt = sessions.find(sessionId);
    SessionInfo* result = NULL;
    if(sessionIt != this->sessions.end()){
        result = sessionIt->second;
    }
    mMutex.unlock();
    return result;
}

void mainLoop(void * arg)
{
	Webserver * parent = reinterpret_cast<Webserver*>(arg);

	// server url will be ws://localhost:9000
	const char *interface = NULL;
	struct libwebsocket_context *context = NULL;

	// we're not using ssl
	const char *cert_path = NULL;
	const char *key_path = NULL;

	//lws_set_log_level(7, lwsl_emit_syslog);
	//lws_set_log_level(1, lwsl_emit_syslog);

	// no special options
	int opts = 0;

	// create connection struct
	struct lws_context_creation_info info;
	memset(&info, 0, sizeof info);

	info.port = parent->getPort();
	info.iface = interface;
	info.protocols = protocols;
	info.extensions = NULL;
	info.ssl_cert_filepath = cert_path;
	info.ssl_private_key_filepath = key_path;
	info.options = opts;
	info.gid = -1;
	info.uid = -1;

	// create libwebsocket context representing this server
	context = libwebsocket_create_context(&info);

	// make sure it starts
	if (context == NULL)
	{
		parent->setRunning(false);
		fprintf(stderr, "libwebsocket init failed\n");
		return;
	}
	printf("starting server...\n");

	// infinite loop, to end this server send SIGTERM. (CTRL+C)
	while (parent->isRunning())
	{
        auto sessions = Webserver::instance()->getSessionsWaitingForWrite();
        while(sessions.size() > 0){
            auto sessionId = sessions.front();
            sessions.pop_front();
            auto sessionInfo = Webserver::instance()->getSession(sessionId);
            if(sessionInfo){
                libwebsocket_callback_on_writable(context, sessionInfo->wsi);
            }
        }
		libwebsocket_service(context, parent->getTimeout()); //5 ms -> 200 samples / s
	}

	libwebsocket_context_destroy(context);
	return;
}
