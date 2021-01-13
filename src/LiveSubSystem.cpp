#include "Poco/Util/Application.h"
#include "Poco/Exception.h"
#include "LiveSubSystem.h"
#include "Network/WebServerDispatcher.h"
#include "Network/WebServerRequestHandlerFactory.h"
#include "Network/MediaTypeMapper.h"
#include "services/webcam/WebcamService.h"
#include "Network/router/VideoStreamingRequestHandlerFactory.h"

using services::webcam::WebcamService;

namespace LiveStream {

LiveSubSystem::LiveSubSystem():
	_cancelInit(false)
{
}


LiveSubSystem::~LiveSubSystem()
{

}

void LiveSubSystem::cancelInit()
{
	_cancelInit = true;
}

void LiveSubSystem::initialize(Poco::Util::Application& app)
{
	if (_cancelInit) return;	

    _httpServerParams = new Poco::Net::HTTPServerParams();
    _httpServerParams->setMaxQueued(app.config().getInt("web.server.MaxQueued", 250));
    _httpServerParams->setMaxThreads(app.config().getInt("web.server.MaxThreads", 1));

    WebServerDispatcher::Config dispconfig;

    dispconfig.corsAllowedOrigin = app.config().getString("web.server.host","http://localhost") + ":" + app.config().getString("web.server.port", "5000");
    Poco::AutoPtr<MediaTypeMapper> mime = new MediaTypeMapper();
    mime->add("html", "text/html");
    mime->add("ico", "image/x-icon");
    mime->add("css", "text/css");
    mime->add("jpeg", "image/jpeg");
    mime->add("jpg", "image/jpeg");
    mime->add("png", "image/png");
    mime->add("json", "application/json");
    mime->add("svg", "image/svg+xml");

    dispconfig.pMediaTypeMapper = std::move(mime);

    _webServerDispatcher = new WebServerDispatcher(dispconfig);
    _webServerDispatcher->threadPool().addCapacity(50);

    WebServerDispatcher::VirtualPath vPath;
    vPath.path = "/";
    vPath.cors.allowOrigin = "*";
    vPath.resource = app.config().getString("web.server.Public", "build/");
    _webServerDispatcher->addVirtualPath(vPath);

    _webcamService = new WebcamService();
    _webcamService->StartRecording();

    WebServerDispatcher::VirtualPath webcam;
    webcam.cors.allowOrigin = "*";
    webcam.cors.enable = true;
    webcam.path = "/api/webcam";
    webcam.pFactory = new infrastructure::video_streaming::VideoStreamingRequestHandlerFactory(_webcamService);
    _webServerDispatcher->addVirtualPath(webcam);
    

    _httpServer = new Poco::Net::HTTPServer(new WebServerRequestHandlerFactory(*_webServerDispatcher, false), _webServerDispatcher->threadPool(), 
        Poco::Net::ServerSocket(Poco::UInt16(app.config().getInt("web.server.port", 3000))), _httpServerParams);

    _httpServer->start();
	app.logger().information("Startup complete.");
}
	
void LiveSubSystem::uninitialize()
{   
    if(_webcamService->IsRecording()) {
        _webcamService->StopRecording();
    }
    _httpServer->stop();
    delete _httpServer;
    Poco::Util::Application::instance().logger().information("Shutdown complete.");
	
}

void LiveSubSystem::defineOptions(Poco::Util::OptionSet& options)
{

}

const char* LiveSubSystem::name() const
{
	return "LIVE-STREAMING SUBSYSTEM";
}

}