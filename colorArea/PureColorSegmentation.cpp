#include "stdafx.h"
#include "PureColorSegmentation.h"

using namespace std;
using namespace cv;

namespace colorArea
{
	PureColorSegmentation::PureColorSegmentation()
	{
	}

	PureColorSegmentation::~PureColorSegmentation()
	{
	}

	bool PureColorSegmentation::segmentation(const cv::Mat & src)
	{
		srcImage = &src;

		maskImage = Mat::zeros(src.rows + 2, src.cols + 2, CV_32SC1);

		//…Ë÷√mask±ﬂΩÁ
		for (int i = 0; i < maskImage.rows; ++i)
		{
			maskImage.at<int>(i, 0) = -1;
			maskImage.at<int>(i, maskImage.cols - 1) = -1;
		}
		for (int i = 1; i < maskImage.cols; ++i)
		{
			maskImage.at<int>(0, i) = -1;
			maskImage.at<int>(maskImage.rows - 1, i) = -1;
		}

		srcSpace = src.cols;
		maskSpace = maskImage.cols;

		index = 0;

		floodFill(200, 200);


		return false;
	}

	void PureColorSegmentation::floodFill(int x, int y)
	{
		ColorStruct<unsigned char> *srcPoint = (ColorStruct<unsigned char> *)(srcImage->ptr<Vec3b>(y)) + x;
		int *maskPoint = (int *)(maskImage.ptr<int>(y + 1)) + x + 1;
		currentColor = *srcPoint;
		++index;
		floodFillScanline(srcPoint, maskPoint);
	}

	void PureColorSegmentation::floodFillScanline(const ColorStruct<unsigned char> *seed, int *maskSeed)
	{
		*maskSeed = index;

		const ColorStruct<unsigned char> *miny, *maxy;
		int *minMasky, *maxMasky;
		for (miny = seed - srcSpace, minMasky = maskSeed - maskSpace; ; miny -= srcSpace, minMasky -= maskSpace)
		{
			if (*minMasky == 0)
			{
				if (*miny == currentColor)
				{
					*minMasky = index;
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
		for (maxy = seed + srcSpace, maxMasky = maskSeed + maskSpace; ; maxy += srcSpace, maxMasky += maskSpace)
		{
			if (*maxMasky == 0)
			{
				if (*maxy == currentColor)
				{
					*maxMasky = index;
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}

		int *imask;
		const ColorStruct<unsigned char> *j;
		for (imask = minMasky + maskSpace, j = miny + srcSpace; imask < maxMasky; imask += maskSpace, j += srcSpace)
		{
			if (*(imask - 1) == 0)
			{
				if (*(j - 1) == currentColor)

				{
					floodFillScanline((j - 1), (imask - 1));
				}
			}
		}
		for (imask = minMasky + maskSpace, j = miny + srcSpace; imask < maxMasky; imask += maskSpace, j += srcSpace)
		{
			if (*(imask + 1) == 0)
			{
				if (*(j + 1) == currentColor)
				{
					floodFillScanline((j + 1), (imask + 1));
				}
			}
		}
	}
}
