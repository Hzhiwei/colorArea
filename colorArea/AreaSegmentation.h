#pragma once

#include <vector>
#include <opencv2/opencv.hpp>

namespace colorArea
{
	class AreaSegmentation
	{
	public:
		AreaSegmentation();
		~AreaSegmentation();

		void SetColorList(const std::vector<cv::Vec3b> &List);
		void SetColorList(int span);
		bool ColorSegmentation(const cv::Mat &src, cv::Mat &dst) const;

	private:
		inline void CalculateColor(const unsigned char * src, unsigned char * dst) const;
		
	private:
		unsigned char * ColorList = nullptr;
		int ColorListLength = 0;
	};

	inline void AreaSegmentation::CalculateColor(const unsigned char * src, unsigned char * dst) const
	{
		int distance, minDistance = 195075, minIndex;
		int x, y, z;
		int temp;
		for (int i = 0; i < ColorListLength; ++i)
		{
			temp = i * 3;
			x = src[0] - ColorList[temp];
			y = src[1] - ColorList[temp + 1];
			z = src[2] - ColorList[temp + 2];
			distance = x * x + y * y + z * z;
			if (distance < minDistance)
			{
				minDistance = distance;
				minIndex = i;
			}
		}
		temp = minIndex * 3;
		dst[0] = ColorList[temp];
		dst[1] = ColorList[temp + 1];
		dst[2] = ColorList[temp + 2];
	}
}
