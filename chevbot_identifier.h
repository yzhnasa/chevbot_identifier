#ifndef __CHEVBOT_IDENTIFICATION__
#define __CHEVBOT_IDENTIFICATION__

#include <iostream>
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "chevbot_state.h"

class ChevBotIdentifier {
private:
	unsigned int frame_number;
	unsigned int curret_frame_number;
	unsigned int total_frame_number; //Only for video, not for camera.
	cv::Mat background;
	cv::VideoCapture stream;
	cv::Mat current_frame;
	cv::Mat element;
	cv::RotatedRect chev_bot_rect;

	void temporalMedianBackgroundEstimation();
	void identifyAFrame();
public:
	//TODO:capture from camera
	//ChevBotIdentification(unsigned int frame_number);
	ChevBotIdentifier(std::string video_path, unsigned int frame_number);
	bool isVideoEnd();
	bool isKeyPressStop();
	ChevBotState getCurrentState();
	void showCurrentFrame();
	~ChevBotIdentifier();
};

#endif // !__CHEVBOT_IDENTIFICATION__