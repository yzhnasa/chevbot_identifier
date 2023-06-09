// ChevBotIdentification.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "chevbot_identifier.h"

ChevBotIdentifier::ChevBotIdentifier(std::string video_path, unsigned int frame_number) : frame_number(frame_number){
	element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
	stream = cv::VideoCapture(video_path);
	stream >> current_frame;
	//cv::imshow("current_frame", current_frame);
	if (!stream.isOpened()) {
		std::cerr << "Cannot open a camera or file." << std::endl;
		exit(-1);
	}
	background = cv::imread("background.png", cv::IMREAD_GRAYSCALE);
	if (background.empty()) {
		temporalMedianBackgroundEstimation();
	}
	//cv::GaussianBlur(background, background, cv::Size(5, 5), 0, 0, cv::BORDER_DEFAULT);
	//cv::imshow("background", background);
	//cv::waitKey(0);
}

void ChevBotIdentifier::temporalMedianBackgroundEstimation() {
	//cv::Mat frame;
	cv::Mat median_frame;
	std::vector<cv::Mat> frames;
	std::vector<uchar> frames_pixels;
	stream >> median_frame;
	//cv::cvtColor(median_frame, median_frame, cv::CV_BGR2GRAY);
	cv::cvtColor(median_frame, median_frame, cv::COLOR_BGR2GRAY);
	cv::GaussianBlur(median_frame, median_frame, cv::Size(5, 5), 0, 0, cv::BORDER_DEFAULT);
	frames.push_back(median_frame.clone());
	for (uint i = 1; i < frame_number; i++) {
		stream >> current_frame;
		//cv::cvtColor(current_frame, current_frame, CV_BGR2GRAY);
		cv::cvtColor(current_frame, current_frame, cv::COLOR_BGR2GRAY);
		cv::GaussianBlur(median_frame, median_frame, cv::Size(5, 5), 0, 0, cv::BORDER_DEFAULT);
		frames.push_back(current_frame.clone());
	}
	for (uint i = 0; i < frames[0].rows; i++) {
		for (uint j = 0; j < frames[0].cols; j++) {
			for (uint k = 0; k < frame_number; k++) {
				frames_pixels.push_back(frames[k].at<uchar>(i, j));
			}
			std::sort(frames_pixels.begin(), frames_pixels.end());
			median_frame.at<uchar>(i, j) = frames_pixels[std::round(frame_number / 2)];
			frames_pixels.clear();
		}
	}
	background = median_frame;
	cv::imwrite("background.png", background);
}

void ChevBotIdentifier::identifyAFrame() {
	stream >> current_frame;
	cv::Mat frame = current_frame.clone();
	try {
		//cv::cvtColor(frame, frame, CV_BGR2GRAY);
		cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
	}
	catch (cv::Exception & e) {
		std::cerr << e.msg << std::endl;
	}
	
	cv::GaussianBlur(frame, frame, cv::Size(3, 3), 0, 0, cv::BORDER_DEFAULT);
	frame = frame - background;
	cv::equalizeHist(frame, frame);
	cv::imshow("frame", frame);
	//cv::threshold(~frame, frame, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	cv::threshold(~frame, frame, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
	cv::erode(frame, frame, element);
	cv::morphologyEx(frame, frame, cv::MORPH_CLOSE, element);
	cv::morphologyEx(frame, frame, cv::MORPH_OPEN, element);
	cv::dilate(frame, frame, element);
	frame = ~frame;
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(frame, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);
	cv::RotatedRect largest_rect = cv::RotatedRect();
	cv::RotatedRect second_largest_rect = cv::RotatedRect();
	cv::RotatedRect current_rect = cv::RotatedRect();
	float largest_area = 0;

	for (unsigned int i = 0; i < contours.size(); i++) {
		current_rect = cv::minAreaRect(cv::Mat(contours[i]));
		if (current_rect.size.area() > largest_area) {
			largest_area = current_rect.size.area();
			second_largest_rect = largest_rect;
			largest_rect = current_rect;
		}
	}
	chev_bot_rect = second_largest_rect;
}

//void ChevBotIdentifier::

bool ChevBotIdentifier::isVideoEnd() {
	//if (stream.get(CV_CAP_PROP_POS_FRAMES) < stream.get(CV_CAP_PROP_FRAME_COUNT))
	if (stream.get(cv::CAP_PROP_POS_FRAMES) < stream.get(cv::CAP_PROP_FRAME_COUNT))
		return false;
	return true;
}

bool ChevBotIdentifier::isKeyPressStop() {
	if (cv::waitKey(30) >= 0)
		return true;
	return false;
}

ChevBotState ChevBotIdentifier::getCurrentState() {
	identifyAFrame();
	ChevBotState current_state;
	current_state.x = chev_bot_rect.center.x;
	current_state.y = chev_bot_rect.center.y;
	current_state.angle = chev_bot_rect.angle;
	return current_state;
}

void ChevBotIdentifier::showCurrentFrame() {
	cv::Point2f rect_points[4];
	chev_bot_rect.points(rect_points);
	for (int j = 0; j < 4; j++)
		cv::line(current_frame, rect_points[j], rect_points[(j + 1) % 4], cv::Scalar(0, 255, 0), 5, 8);
	cv::imshow("current_frame", current_frame);
}

ChevBotIdentifier::~ChevBotIdentifier() {}