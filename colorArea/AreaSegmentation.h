#pragma once

#include <vector>
#include <opencv2/opencv.hpp>

namespace colorArea
{
	template<typename T>
	struct ColorStruct
	{
		T b;
		T g;
		T r;
	};

	class ColorSegmentation
	{
	public:
		ColorSegmentation();
		~ColorSegmentation();

	public:
		//设置区域最小面积
		void setMinArea(unsigned int area);
		bool Segmentation(const cv::Mat &src) const;
		bool Segmentation1(const cv::Mat &src) const;
		bool Segmentation2(const cv::Mat &src) const;
		bool DrawRect(cv::Mat src, cv::RotatedRect rect, cv::Scalar color);

	private:
		void showEdge(const cv::Mat & src, cv::Mat & dst, cv::Mat & edge) const;
		void floodFill(cv::Mat & src, cv::Mat & dst, cv::Point seed, cv::Scalar loDiff, cv::Scalar upDiff);
		void floodFill(cv::Mat & src, cv::Mat & dst, cv::Point seed);
		inline void floodFillScanline(int x, int y);
		inline void floodFillCheckAndCal(const ColorStruct<unsigned char> *color);
		

	private:
		unsigned int MinArea;
	};

	inline void ColorSegmentation::floodFillScanline(int x, int y)
	{
	}

	inline void ColorSegmentation::floodFillCheckAndCal(const ColorStruct<unsigned char> *color)
	{

	}
}
