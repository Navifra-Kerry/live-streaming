#ifndef LIVE_SUB_SYSTEM_H
#define LIVE_SUB_SYSTEM_H

#include "Poco/AutoPtr.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/Util/Subsystem.h"
#include "services/webcam/WebcamService.h"
using services::webcam::WebcamService;

namespace LiveStream{

class WebServerDispatcher;
class LiveSubSystem: public Poco::Util::Subsystem
{
public:
	LiveSubSystem();
	~LiveSubSystem();

	void cancelInit();

	// Subsystem
	void initialize(Poco::Util::Application& app);		
	void uninitialize();
	void defineOptions(Poco::Util::OptionSet& options);
	const char* name() const;

private:
    Poco::AutoPtr<Poco::Net::HTTPServerParams> _httpServerParams;
	Poco::SharedPtr<WebcamService> _webcamService;
	Poco::AutoPtr<WebServerDispatcher> _webServerDispatcher;
	
	Poco::Net::HTTPServer * _httpServer;
    bool              _cancelInit;
};
}

#endif // LIVE_SUB_SYSTEM_H
