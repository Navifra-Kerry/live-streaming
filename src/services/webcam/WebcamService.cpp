//============================================================================
// Name        : WebcamService.cpp
// Author      : ITM13
// Version     : 1.0
// Description :
//============================================================================
#include "services/webcam/WebcamService.h"
#include <iostream>
#include <iomanip>

#include <Poco\Clock.h>

#include <Poco\Stopwatch.h>
using Poco::Stopwatch;

using Poco::Clock;
using std::cout;

namespace services {
	namespace webcam {
		WebcamService::WebcamService() : capture(VideoCapture()) {
			recordingThread = new Thread("WebCamRecording");
			recordingAdapter = new RunnableAdapter<WebcamService>(*this, &WebcamService::RecordingCore);
			isRecording = false;
			params = { cv::IMWRITE_JPEG_QUALITY, 100 };
			fps = 15;
			delay = 1000 / fps; //in ms
		}

		WebcamService::~WebcamService() {
			if (recordingThread->isRunning()) {
				StopRecording();
			}

			if (capture.isOpened()) {
				capture.release();
			}
		}

		int WebcamService::GetDelay() {
			return delay;
		}

		int WebcamService::GetFPS() {
			return fps;
		}

		void WebcamService::SetModifiedImage(Mat& image) {
			//Stopwatch sw;
			//sw.start();

			Poco::Mutex::ScopedLock lock(modifiedImgMutex); //will be released after leaving scop
			// encode mat to jpg and copy it to content
			cv::imencode(".jpg", image, modifiedImage, params);

			//sw.stop();
			//printf("modified image: %f  ms\n", sw.elapsed() * 0.001);
		}

		vector<uchar>* WebcamService::GetModifiedImage() {
			Poco::Mutex::ScopedLock lock(modifiedImgMutex); //will be released after leaving scop
			vector<uchar> *tempImg = new vector<uchar>(modifiedImage.begin(), modifiedImage.end());
			return tempImg;
		}

		Mat& WebcamService::GetLastImage() {
			Poco::Mutex::ScopedLock lock(lastImgMutex); //will be released after leaving scop
			return lastImage;
		}

		bool WebcamService::StartRecording() {
			Logger& logger = Logger::get("WebcamService");

			capture.open(0, cv::CAP_ANY);
			//capture.open("logger.mp4");

			if (!capture.isOpened()){
				logger.error("No camera available!");
				return false;
			}

			logger.information("starting recording...");

			//camera settings
			capture.set(cv::CAP_PROP_FPS, fps);
			//Possible resolutions : 1280x720, 640x480; 440x330
			//capture.set(cv::CAP_PROP_FRAME_WIDTH, 640);
			//capture.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

			logger.information("Camera settings: ");
			logger.information("FPS: " + std::to_string(capture.get(cv::CAP_PROP_FPS)));
			logger.information("Resolution: " + std::to_string(capture.get(cv::CAP_PROP_FRAME_WIDTH)) + "x" + std::to_string(capture.get(cv::CAP_PROP_FRAME_HEIGHT)));
			logger.information("Codec: " + std::to_string(capture.get(cv::CAP_PROP_FOURCC)));
			logger.information("Format: " + std::to_string(capture.get(cv::CAP_PROP_FORMAT)));

			isRecording = true;
			recordingThread->start(*recordingAdapter);

			logger.information("started recording");

			return true;
		}

		bool WebcamService::StopRecording() {
			Logger& logger = Logger::get("WebcamService");

			logger.information("stopping recording...");

			if (recordingThread->isRunning()) {
				isRecording = false;
				logger.information("recording activity stop requested");
				recordingThread->join();
				logger.information("recording activity stopped successfully");
			}
			else {
				logger.error("recording activity is already stopped!");
			}

			logger.information("stopped recording");

			return true;
		}

		bool WebcamService::IsRecording() {
			return capture.isOpened() && recordingThread->isRunning();
		}

		void WebcamService::RecordingCore() {
			Logger& logger = Logger::get("WebcamService");
			Mat frame;

			//Stopwatch sw;
			Clock clock;
			int newDelay = 0;

			while (isRecording) {
				if (!capture.isOpened()) {
					logger.error("Lost connection to webcam!");
					break;
				}

				//Create image frames from capture
				capture >> frame;

				clock.update();
				if (!frame.empty()) {
						{
							Poco::Mutex::ScopedLock lock(lastImgMutex); //will be released after leaving scop
							SetModifiedImage(frame);
						}

					Notify();
				}
				else {
					logger.warning("Captured empty webcam frame!");
				}

				newDelay = static_cast<int>(delay - clock.elapsed() * 0.001);

				if (newDelay > 0) {
					//webcam can only be queried after some time again
					//according to the FPS rate
					Thread::sleep(newDelay);
				}
			}

			isRecording = false;
		}
	}
}