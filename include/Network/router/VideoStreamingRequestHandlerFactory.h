//============================================================================
// Name        : VideoStreamingRequestHandlerFactory.h
// Author      : ITM13
// Version     : 1.0
// Description :
//============================================================================
#pragma once
#include "../../services/webcam/WebcamService.h"
#include "Poco\Net\HTTPRequestHandlerFactory.h"
#include "Poco\Net\HTTPServerRequest.h"
#include "Poco\SharedPtr.h"
#include "Poco\NotificationQueue.h"
#include "Poco\Logger.h"
#include "Poco\SharedPtr.h"
#include "Poco\BasicEvent.h"
#include "Poco\NotificationQueue.h"
#include "Poco\Net\HTTPServerRequest.h"
#include "Poco\Net\HTTPServerResponse.h"
#include "Poco\Net\HTTPRequestHandler.h"

#include <string>

using std::string;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::HTTPRequestHandler;
using Poco::SharedPtr;
using Poco::NotificationQueue;
using services::webcam::WebcamService;

namespace infrastructure {
	namespace video_streaming {
		class VideoStreamingRequestHandlerFactory : public HTTPRequestHandlerFactory
		{
		public:
			VideoStreamingRequestHandlerFactory(SharedPtr<WebcamService> webcamService);
			~VideoStreamingRequestHandlerFactory();
			HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request);
		private:
			SharedPtr<WebcamService> webcamService;
			string uri;
		};

		class VideoStreamingRequestHandler : public HTTPRequestHandler
		{
		public:
			VideoStreamingRequestHandler(SharedPtr<WebcamService> webcamService);
			~VideoStreamingRequestHandler();
			void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response);
		private:
			SharedPtr<WebcamService> webcamService;
			string boundary;
		};
	}
}