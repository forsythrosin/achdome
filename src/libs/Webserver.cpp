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

Webserver *Webserver::mInstance = NULL;
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

	if (!strcmp(&file[n - 4], ".ico"))
		return "image/x-icon";

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
            std::string *message;
            boost::shared_lock< boost::shared_mutex > lock(*sessionInfo->mtx);
			if (sessionInfo->messages->empty() && sessionInfo->messages->pop(message)){
                int n = sprintf(gSendBuffer, "%s\n", message->c_str());
                libwebsocket_write(wsi, reinterpret_cast<unsigned char *>(gSendBuffer), n, LWS_WRITE_TEXT);
            }
        }
        break;
        case LWS_CALLBACK_ESTABLISHED:{
            int sessionId = Webserver::instance()->generateSessionIndex();
            sessionInfo->messages = new boost::lockfree::queue<std::string*>(5);
            sessionInfo->sessionId = sessionId;
            sessionInfo->wsi = wsi;
            sessionInfo->mtx = new boost::shared_mutex();
            Webserver::instance()->addSession(sessionId, sessionInfo);
        }
        break;
        case LWS_CALLBACK_CLOSED:{
          {
            boost::unique_lock< boost::shared_mutex > lock(*sessionInfo->mtx);
            Webserver::instance()->removeSession(sessionInfo->sessionId);
			delete sessionInfo->messages;
          }
		delete sessionInfo->mtx;
        }
        break;
        case LWS_CALLBACK_RECEIVE:{
            //libwebsocket_write(wsi, reinterpret_cast<unsigned char *>(in), len, LWS_WRITE_TEXT);

            // log what we recieved and what we're going to send as a response.
            //printf("received data: %s\n", (char *)in);

      			Webserver *webserver = Webserver::instance();
      			if (webserver->mWebsocketCallback) {
      				const char *msg = reinterpret_cast<const char *>(in);
      				webserver->mWebsocketCallback(msg);
      			}
        }
        break;
	}
    return 0;
}

Webserver::Webserver()
{
	boost::unique_lock< boost::shared_mutex > lock(serverMutex);
	mInstance = this;
	mMainThreadPtr = NULL;
	mWebsocketCallback = nullptr;
	mRunning = false;
    mSessionIndex = 0;
    sessionsWaitingForWrite = new boost::lockfree::queue<int>(100);
}

Webserver::~Webserver()
{
    mRunning = false;
    mWebsocketCallback = nullptr;
	boost::unique_lock< boost::shared_mutex > lock(serverMutex);
	delete sessionsWaitingForWrite;
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
    boost::unique_lock< boost::shared_mutex > lock(serverMutex);
    mRunning = true;
    mPort = port;
    mTimeout = timeout_ms;
  	mMainThreadPtr = new (std::nothrow) tthread::thread(mainLoop, this);
  	memset(gSendBuffer, 0, SendBufferLength);
}

bool Webserver::isRunning()
{
  bool tmpBool;
	tmpBool = mRunning;
	return tmpBool;
}

void Webserver::setRunning(bool state)
{
    mRunning = state;
}

void Webserver::setCallback(Webserver::WebsocketCallback cb)
{
	mWebsocketCallback = cb;
}

unsigned int Webserver::generateSessionIndex()
{
    //session index 0 is invalid all clients having an index > 0 is ok

    unsigned int tmpUi;
    boost::unique_lock< boost::shared_mutex > lock(serverMutex);
    mSessionIndex++;
	   tmpUi = mSessionIndex;
    return tmpUi;
}


void Webserver::addBroadcast(std::string broadcast) {
    boost::shared_lock< boost::shared_mutex > lock(serverMutex);
    for(auto session : sessions){
        session.second->messages->push(new std::string(broadcast));
        this->sessionsWaitingForWrite->push(session.second->sessionId);
    }
}

void Webserver::addMessage(int sessionId, std::string message) {
  boost::shared_lock< boost::shared_mutex > lock(serverMutex);
  auto sessionIt = sessions.find(sessionId);
	if (sessionIt != this->sessions.end()){
		sessionIt->second->messages->push(new std::string(message));
		sessionsWaitingForWrite->push(sessionIt->second->sessionId);
	}
}

void Webserver::addSession(int sessionId, SessionInfo *session) {
    boost::unique_lock< boost::shared_mutex > lock(serverMutex);
    this->sessions.insert({sessionId, session});
}

bool Webserver::removeSession(int sessionId){
    boost::unique_lock< boost::shared_mutex > lock(serverMutex);
    auto sessionIt = sessions.find(sessionId);
    if(sessionIt == this->sessions.end()){
        return false;
    }
    this->sessions.erase(sessionId);
    return true;
}

SessionInfo* Webserver::getSession(int sessionId){
    auto sessionIt = sessions.find(sessionId);
    SessionInfo* result = NULL;
    if(sessionIt != this->sessions.end()){
        result = sessionIt->second;
    }
    return result;
}
std::queue<int> Webserver::getSessionsWaitingForWrite(){
	std::queue<int> writingSessions;
	while (!sessionsWaitingForWrite->empty()){
		int sessionId;
		sessionsWaitingForWrite->pop(sessionId);
		writingSessions.push(sessionId);
	}
	return writingSessions;
}

void mainLoop(void * arg)
{
	Webserver * parent = reinterpret_cast<Webserver *>(arg);

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
		while (!sessions.empty()){
			int sessionId = sessions.front();
			sessions.pop();
			auto sessionInfo = Webserver::instance()->getSession(sessionId);
			if (sessionInfo){
				libwebsocket_callback_on_writable(context, sessionInfo->wsi);
			}
		}
		
		
		libwebsocket_service(context, parent->getTimeout()); //5 ms -> 200 samples / s
	}

	libwebsocket_context_destroy(context);
	return;
}
