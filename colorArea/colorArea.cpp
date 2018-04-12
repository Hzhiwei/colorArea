// colorArea.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
//#include "AreaSegmentation.h"
#include "PureColorSegmentation.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace colorArea;

int main()
{
	cv::VideoCapture camera;

	//try
	//{
	//	camera.open(0);
	//	std::cout << "camera open succeed" << std::endl;
	//}
	//catch(...)
	//{
	//	std::cout << "camera open failed" << std::endl;
	//}

	//while (true)
	//{
	//	cv::Mat frame;
	//	camera >> frame;
	//	//cv::imshow("image", frame);
	//	CS.FFTtest(frame);
	//	cv::waitKey(30);
	//}
	

	cv::Mat src = cv::imread("image/pure5.png");
	//ColorSegmentation CS;
	//CS.Segmentation1(src);
	//CS.FFTtest(src);
	//CS.DrawRect(src, cv::RotatedRect(cv::Point2f(300, 300), cv::Size2f(500, 500), 45), cv::Scalar(255, 0, 0));
	//cv::imshow("src", src);
	PureColorSegmentation pcs;

	pcs.segmentation(src);

	cv::imshow("src", src);
	//cv::imwrite("edge.png", src);

	while (cv::waitKey(30));

    return 0;
}

