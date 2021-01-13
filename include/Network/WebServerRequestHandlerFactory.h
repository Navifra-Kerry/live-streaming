
#ifndef WEBSERVER_REQUESTHANDLER_FACTORY
#define WEBSERVER_REQUESTHANDLER_FACTORY

#include "Poco/Net/HTTPRequestHandlerFactory.h"

namespace LiveStream {
    
class WebServerRequestHandler;
class WebServerDispatcher;

class WebServerRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
public:
	WebServerRequestHandlerFactory(WebServerDispatcher& dispatcher, bool secure);

	~WebServerRequestHandlerFactory();

	Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request);

private:
	WebServerDispatcher& _dispatcher;
	bool _secure;
};
}

#endif // WEBSERVER_REQUESTHANDLER_FACTORY
