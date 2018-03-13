// colorArea.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "AreaSegmentation.h"
#include <opencv2/opencv.hpp>

using namespace colorArea;

int main()
{
	cv::VideoCapture camera;

	AreaSegmentation Seg;

	std::vector<cv::Vec3b> list;
	list.push_back(cv::Vec3b(0, 0, 0));
	list.push_back(cv::Vec3b(255, 255, 255));
	list.push_back(cv::Vec3b(255, 0, 0));
	list.push_back(cv::Vec3b(0, 255, 0));
	list.push_back(cv::Vec3b(0, 0, 255));
	list.push_back(cv::Vec3b(255, 255, 0));
	list.push_back(cv::Vec3b(255, 0, 255));
	list.push_back(cv::Vec3b(0, 255, 255));
	Seg.SetColorList(list);

	try
	{
		camera.open(0);
		std::cout << "camera open succeed" << std::endl;
	}
	catch(...)
	{
		std::cout << "camera open failed" << std::endl;
	}

	while (true)
	{
		cv::Mat frame;
		cv::Mat out;
		camera >> frame;
		Seg.ColorSegmentation(frame, out);
		cv::imshow("image", frame);
		cv::imshow("out", out);
		cv::waitKey(30);
	}

    return 0;
}

