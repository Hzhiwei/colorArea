#pragma once

#include <opencv2/opencv.hpp>


namespace colorArea
{
	template<typename T>
	class ColorStruct
	{
		T b;
		T g;
		T r;

	public:
		ColorStruct() {}

		ColorStruct(T _b, T _g, T _r)
		{
			b = _b;
			g = _g;
			r = _r;
		}

		bool operator==(const ColorStruct<T> & Param) const
		{
			return !memcmp(this, &Param, sizeof(ColorStruct<T>));
		}
	};

	class PureColorSegmentation
	{
	public:
		PureColorSegmentation();
		~PureColorSegmentation();

	public:
		int segmentation( cv::Mat &src);
		void floodFillAll(const cv::Mat &src);
		void floodFill(int x, int y);
		void floodFillScanline(const ColorStruct<unsigned char> *seed, int *maskSeed);
		std::vector<std::vector<cv::Point>> edgeTrackAll(void);
		std::vector<cv::Point> edgeTrack(cv::Point seed);
		void drawEdge(cv::Mat &dst, const std::vector<std::vector<cv::Point>> &edge, int index, cv::Scalar color);
		inline bool floodFillCheckAndCal(const ColorStruct<unsigned char> *color);

	private:
		cv::Mat maskImage;
		std::vector<ColorStruct<unsigned char>> indexColor;
		const cv::Mat *srcImage = nullptr;
		int srcSpace, maskSpace;
		int index;
		ColorStruct<unsigned char> currentColor;
		//surroundDiffer为中心点的八邻域像素地址于中心点的差，surroundDiffer[8] = surroundDiffer[0]，方便计算
		/*
		0/8	1	2
		7	s	3
		6	5	4
		*/
		int surroundPointerDiffer[9];

		int surroundIndexDiffer[9][2];
	};

	inline bool PureColorSegmentation::floodFillCheckAndCal(const ColorStruct<unsigned char>* color)
	{
		return *color == currentColor;
	}
}

