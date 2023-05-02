#include <iostream>
#include <string>
#include "chevbot_state.h"
#include "chevbot_identifier.h"

int main() {
	//std::string video_path = "E:\\Zhong_Yang_Research\\MicroRobot\\video\\091918\\strct_2a_5.avi";
	std::string video_path = "D:\\Zhong_Yang_Research\\MicroRobot\\video\\chevbot_video\\old_design\\Video1-22.avi";
	ChevBotIdentifier identifier(video_path.c_str(), 501);
	ChevBotState current_state;
	while (!identifier.isVideoEnd() && !identifier.isKeyPressStop()) {
		int64 t0 = cv::getTickCount();
		current_state = identifier.getCurrentState();
		identifier.showCurrentFrame();
		std::cout << "x: " << current_state.x << "   " << "y: " << current_state.y << "   " << "angle: " << current_state.angle << std::endl;
		int64 t1 = cv::getTickCount();
		double secs = (t1 - t0) / cv::getTickFrequency();
		std::cout << "processing time: " << secs << std::endl;
	}
	return 0;
}