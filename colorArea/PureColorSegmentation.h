#pragma once

#include <opencv2/opencv.hpp>


namespace colorArea
{
	template<typename T>
	class ColorStruct
	{
	public:
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

		template<typename _T>
		inline ColorStruct<T> & operator=(const ColorStruct<_T> & Param)
		{
			b = Param.b;
			g = Param.g;
			r = Param.r;
			return *this;
		}

		inline bool operator==(const ColorStruct<T> & Param) const
		{
			return !memcmp(this, &Param, sizeof(ColorStruct<T>));
		}

		template<typename _T>
		inline ColorStruct<T> operator%(const ColorStruct<_T> & Param) const
		{
			return ColorStruct<T>(
				b >= Param.b ? b - Param.b : Param.b - b,
				g >= Param.g ? g - Param.g : Param.g - g,
				r >= Param.r ? r - Param.r : Param.r - r);
		}

		template<typename _T>
		inline int distance(const ColorStruct<_T> & Param) const
		{
			ColorStruct<T> temp = operator%(Param);
			return temp.b * temp.b + temp.g * temp.g + temp.r * temp.r;
		}

	};

	class PureColorSegmentation
	{
	public:
		PureColorSegmentation();
		~PureColorSegmentation();

	public:
		int segmentation( cv::Mat &src);
		void floodFillAll();
		void floodFill(int x, int y);
		void floodFillScanline(const ColorStruct<unsigned char> *seed, int *maskSeed);
		std::vector<std::vector<cv::Point>> edgeTrackAll(void);
		std::vector<cv::Point> edgeTrack(cv::Point seed);
		void drawEdge(cv::Mat &dst, const std::vector<std::vector<cv::Point>> &edge, int index, cv::Scalar color);
		inline bool floodFillCheckAndCal(const ColorStruct<unsigned char> *color);
		inline void addPointToAverage(const ColorStruct<unsigned char> *color);

	private:
		cv::Mat maskImage;
		std::vector<ColorStruct<unsigned char>> indexColor;
		const cv::Mat *srcImage = nullptr;
		int srcSpace, maskSpace;
		int counter;
		int index;
		ColorStruct<float> averageColor;
		/*
		surroundDiffer为中心点的八邻域像素地址于中心点的差，surroundDiffer[8] = surroundDiffer[0]，方便计算

		0/8	1	2
		7	s	3
		6	5	4
		*/
		int surroundPointerDiffer[9];

		int surroundIndexDiffer[9][2];
	};

	inline bool PureColorSegmentation::floodFillCheckAndCal(const ColorStruct<unsigned char>* color)
	{
		//return color->distance(averageColor) < 7000;

		if (averageColor.distance(*color) < 2500)
		{
			addPointToAverage(color);
			return true;
		}
		else
		{
			return false;
		}
	}

	inline void PureColorSegmentation::addPointToAverage(const ColorStruct<unsigned char> *color)
	{
		++counter;
		averageColor.b += (color->b - averageColor.b) / counter;
		averageColor.g += (color->g - averageColor.g) / counter;
		averageColor.r += (color->r - averageColor.r) / counter;
	}
}

