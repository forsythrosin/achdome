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

#define SendBufferLength 32

struct libwebsocket_context;

Webserver * Webserver::mInstance = NULL;
tthread::mutex gSendMutex;
char gSendBuffer[SendBufferLength];

void mainLoop(void * arg);
static int nullHttp(struct libwebsocket_context * context, struct libwebsocket *wsi,
                    enum libwebsocket_callback_reasons reason,  void *user, void *in, size_t len);
static int echoCallback(struct libwebsocket_context * context, struct libwebsocket *wsi,
                        enum libwebsocket_callback_reasons reason, void *user, void *in, size_t len);

struct SessionInfo{
    int sessionId;
};

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

			/* this server has no concept of directories */
			if (strchr(data + 1, '/'))
			{
				libwebsockets_return_http_status(context, wsi,
					HTTP_STATUS_FORBIDDEN, NULL);
				return -1;
			}

			/* if a legal POST URL, let it continue and accept data */
			if (lws_hdr_total_length(wsi, WSI_TOKEN_POST_URI))
				return 0;

			/* if not, send a file the easy way */
            //fprintf(stderr, "data: %s\n", data);
			if (strcmp(data, "/"))
			{

				strncat(buf, data+1, sizeof(buf) - strlen(buf) - 2);
			}
			else /* default file to serve */
			{
				//strcat(buf, "D:/Projects/ixel_tools/websocket_test/bin/");
				strcat(buf, webUtils::absolutePathToResource("socket.html").c_str());
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
	switch (reason){
        case LWS_CALLBACK_SERVER_WRITEABLE: {
            if (Webserver::instance()->hasWaitingBroadcasts()) {
                gSendMutex.lock();
                std::string message = Webserver::instance()->getNextBroadcast();
                int n = sprintf(gSendBuffer, "%s\n", message.c_str());
                libwebsocket_write(wsi, reinterpret_cast<unsigned char *>(gSendBuffer), n, LWS_WRITE_TEXT);
                gSendMutex.unlock();
            };
        }
        break;
        case LWS_CALLBACK_ESTABLISHED:{
            gSendMutex.lock();
            int sessionId = Webserver::instance()->generateSessionIndex();
            std::cout << "New session connected with id: " <<sessionId << std::endl;
            reinterpret_cast<SessionInfo*>(user)->sessionId = sessionId;
            Webserver::instance()->addClient(sessionId);
            gSendMutex.unlock();
        }
        break;
        case LWS_CALLBACK_CLOSED:{
            gSendMutex.lock();
            SessionInfo *sessionInfo = reinterpret_cast<SessionInfo*>(user);
            std::cout << "Closed session with id: " << sessionInfo->sessionId << std::endl;
            gSendMutex.unlock();
        }
        break;
        case LWS_CALLBACK_RECEIVE:{
            //libwebsocket_write(wsi, reinterpret_cast<unsigned char *>(in), len, LWS_WRITE_TEXT);

            // log what we recieved and what we're going to send as a response.
            //printf("received data: %s\n", (char *)in);

            if (Webserver::instance()->mWebMessageCallbackFn)
                Webserver::instance()->mWebMessageCallbackFn(reinterpret_cast<const char *>(in), len);


            Webserver::instance()->addBroadcast(reinterpret_cast<const char *>(in));
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
		libwebsocket_service(context, parent->getTimeout()); //5 ms -> 200 samples / s
        if(Webserver::instance()->hasWaitingBroadcasts()){
            libwebsocket_callback_on_writable_all_protocol(&protocols[1]);
        }
	}

	libwebsocket_context_destroy(context);
	return;
}

bool Webserver::hasWaitingBroadcasts() {
    return this->waitingBroadcasts.size() > 0;
}

std::string Webserver::getNextBroadcast() {
    auto message = this->waitingBroadcasts.back();
    this->waitingBroadcasts.pop_back();
    return message;
}

void Webserver::addBroadcast(std::string broadcast) {
    this->waitingBroadcasts.push_back(broadcast);
}

void Webserver::addClient(int sessionId) {
    this->clients.insert({sessionId, "bla"});
}

bool Webserver::removeClient(int sessionId){
    this->clients.erase(sessionId);
}
