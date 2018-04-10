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
		bool segmentation(const cv::Mat &src);
		void floodFill(int x, int y);
		void floodFillScanline(const ColorStruct<unsigned char> *seed, int *maskSeed);
		inline bool floodFillCheckAndCal(const ColorStruct<unsigned char> *color);

	//private:
		cv::Mat maskImage;
		std::vector<ColorStruct<unsigned char>> indexColor;
		const cv::Mat *srcImage = nullptr;
		int srcSpace, maskSpace;
		int index;
		ColorStruct<unsigned char> currentColor;
	};

	inline bool PureColorSegmentation::floodFillCheckAndCal(const ColorStruct<unsigned char>* color)
	{
		return *color == currentColor;
	}
}

