#include "Network/WebServerRequestHandlerFactory.h"
#include "Network/WebServerRequestHandler.h"


namespace LiveStream {

WebServerRequestHandlerFactory::WebServerRequestHandlerFactory(WebServerDispatcher& dispatcher, bool secure) :
	_dispatcher(dispatcher),
	_secure(secure)
{
}


WebServerRequestHandlerFactory::~WebServerRequestHandlerFactory()
{
}


Poco::Net::HTTPRequestHandler* WebServerRequestHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest& request)
{
	return new WebServerRequestHandler(_dispatcher, _secure);
}

}
