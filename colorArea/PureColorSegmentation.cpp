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

		maskImage = Mat::zeros(src.rows + 2, srcImage->cols + 2, CV_32SC1);

		//设置mask边界
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

		index = 0;

		indexColor.clear();
		//0号不使用，直接丢弃
		indexColor.push_back(ColorStruct<unsigned char>(0, 0, 0));

		for (int i = 0; i < srcImage->cols; ++i)
		{
			for (int j = 0; j < srcImage->rows; ++j)
			{
				if (maskImage.at<int>(j + 1, i + 1) == 0)
				{
					floodFill(i, j);
				}
			}
		}

		imshow("mask", maskImage);

		return true;
	}

	//漫水填充
	void PureColorSegmentation::floodFill(int x, int y)
	{
		ColorStruct<unsigned char> *srcPoint = (ColorStruct<unsigned char> *)(srcImage->ptr<Vec3b>(y)) + x;
		int *maskPoint = (int *)(maskImage.ptr<int>(y + 1)) + x + 1;
		currentColor = *srcPoint;
		index += 1;
		indexColor.push_back(currentColor);
		floodFillScanline(srcPoint, maskPoint);
	}

	//线扫描找出种子点的连通区域
	void PureColorSegmentation::floodFillScanline(const ColorStruct<unsigned char> *seed, int *maskSeed)
	{
		*maskSeed = index;

		const ColorStruct<unsigned char> *miny, *maxy;
		int *minMasky, *maxMasky;
		for (miny = seed - srcImage->cols, minMasky = maskSeed - maskImage.cols; ; miny -= srcImage->cols, minMasky -= maskImage.cols)
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
		for (maxy = seed + srcImage->cols, maxMasky = maskSeed + maskImage.cols; ; maxy += srcImage->cols, maxMasky += maskImage.cols)
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
		for (imask = minMasky + maskImage.cols, j = miny + srcImage->cols; imask < maxMasky; imask += maskImage.cols, j += srcImage->cols)
		{
			if (*(imask - 1) == 0)
			{
				if (*(j - 1) == currentColor)

				{
					floodFillScanline((j - 1), (imask - 1));
				}
			}
		}
		for (imask = minMasky + maskImage.cols, j = miny + srcImage->cols; imask < maxMasky; imask += maskImage.cols, j += srcImage->cols)
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
