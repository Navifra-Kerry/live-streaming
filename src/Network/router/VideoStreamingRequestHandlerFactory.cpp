//============================================================================
// Name        : VideoStreamingRequestHandlerFactory.cpp
// Author      : ITM13
// Version     : 1.0
// Description :
//============================================================================
#include "Network/router/VideoStreamingRequestHandlerFactory.h"

#include "Poco\Net\MultipartWriter.h"
#include "Poco\Net\MessageHeader.h"

using Poco::Net::MessageHeader;
using Poco::Net::HTTPResponse;
using Poco::Net::MultipartWriter;

namespace infrastructure {
	namespace video_streaming {
		VideoStreamingRequestHandlerFactory::VideoStreamingRequestHandlerFactory(SharedPtr<WebcamService> webcamService
        ) : webcamService(webcamService) { }

		VideoStreamingRequestHandlerFactory::~VideoStreamingRequestHandlerFactory() {
			//do not delete, since it is a shared pointer
			webcamService = nullptr;
		}

		HTTPRequestHandler* VideoStreamingRequestHandlerFactory::createRequestHandler(const HTTPServerRequest& request) {

			return new VideoStreamingRequestHandler(webcamService);

		}

        		VideoStreamingRequestHandler::VideoStreamingRequestHandler(SharedPtr<WebcamService> webcamService)
			: webcamService(webcamService){
			boundary = "VIDEOSTREAM";
		}

		VideoStreamingRequestHandler::~VideoStreamingRequestHandler() {
			//do not delete, since it is a shared pointer
			webcamService = nullptr;
		}

		void VideoStreamingRequestHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) {
			Poco::Logger& logger = Poco::Logger::get("VideoStreamingRequestHandler");
			logger.information("Video stream request by client: " + request.clientAddress().toString());

			// check if webcam service is running correctly
			if (!webcamService->IsRecording()) {
				logger.warning("No stream available. Closing connection to " + request.clientAddress().toString());
				response.setStatus(HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
				response.send();
				return;
			}

			logger.information("Video streaming started for client " + request.clientAddress().toString());

			response.set("Max-Age", "0");
			response.set("Expires", "0");
			response.set("Cache-Control", "no-cache, private");
			response.set("Pragma", "no-cache");
			response.setContentType("multipart/x-mixed-replace; boundary=--" + boundary);
			response.setChunkedTransferEncoding(false);

			std::ostream& out = response.send();
			int frames = 0;
			//double start = 0.0;
			//double dif = 0.0;

			while (out.good() && webcamService->IsRecording()) {
				//start = CLOCK();

				MultipartWriter writer(out, boundary);

				vector<uchar>* buf = webcamService->GetModifiedImage(); //take ownership

				if (buf->size() == 0) {
					logger.warning("Read empty stream image");
					delete buf;
					continue;
				}

				MessageHeader header = MessageHeader();
				header.set("Content-Length", std::to_string(buf->size()));
				header.set("Content-Type", "image/jpeg");
				writer.nextPart(header);
				out.write(reinterpret_cast<const char*>(buf->data()), buf->size());
				out << "\r\n\r\n";

				delete buf;
				buf = nullptr;

				//dif = CLOCK() - start;
				//printf("Sending: %.2f ms; avg: %.2f ms\r", dif, avgdur(dif));
				++frames;
			}

			printf("Frames: %u", frames);

			logger.information("Video streaming stopped for client " + request.clientAddress().toString());
			//server.HandleClientLostConnection(request.clientAddress());
		}
	}
}