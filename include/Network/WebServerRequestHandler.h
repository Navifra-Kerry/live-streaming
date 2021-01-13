#ifndef SERVER_REQUEST_HANDELER
#define SERVER_REQUEST_HANDELER

#include "Poco/Net/HTTPRequestHandler.h"

namespace LiveStream {

class WebServerDispatcher;
class WebServerRequestHandler : public Poco::Net::HTTPRequestHandler
{
public:
	WebServerRequestHandler(WebServerDispatcher& dispatcher, bool secure);

	~WebServerRequestHandler();

	void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);

private:
	WebServerDispatcher& _dispatcher;
	bool _secure;
};

}

#endif // SERVER_REQUEST_HANDELER
