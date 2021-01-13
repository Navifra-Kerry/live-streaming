#include "Network/WebServerRequestHandler.h"
#include "Network/WebServerDispatcher.h"


namespace LiveStream {

WebServerRequestHandler::WebServerRequestHandler(WebServerDispatcher& dispatcher, bool secure) :
	_dispatcher(dispatcher),
	_secure(secure)
{
}

WebServerRequestHandler::~WebServerRequestHandler()
{
}


void WebServerRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
	_dispatcher.handleRequest(request, response, _secure);
}

}