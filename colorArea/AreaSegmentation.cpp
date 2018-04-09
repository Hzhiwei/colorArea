#include "stdafx.h"
#include "AreaSegmentation.h"

using namespace cv;
using namespace std;

namespace colorArea
{
	ColorSegmentation::ColorSegmentation() :
		MinArea(400)
	{
	}

	ColorSegmentation::~ColorSegmentation()
	{
	}

	void ColorSegmentation::setMinArea(unsigned int area)
	{
	}

	bool ColorSegmentation::Segmentation(const cv::Mat & src) const
	{
		if (src.empty())
		{
			return false;
		}

		cv::imshow("src", src);
		cv::waitKey(30);

		//高斯模糊
		cv::Mat fuzzyImg;
		cv::GaussianBlur(src, fuzzyImg, cv::Size(25, 25), 0, 0);

		cv::imshow("fuzzyImg", fuzzyImg);
		cv::waitKey(30);

		//通道分离
		std::vector<cv::Mat> channelImg;
		cv::split(fuzzyImg, channelImg);

		for (int i = 0; i < fuzzyImg.channels(); ++i)
		{
			cv::imshow(std::string("channelImg-") + std::to_string(i), channelImg[i]);
		}
		cv::waitKey(30);

		//边缘查找
		std::vector<cv::Mat> cannyImg;
		cannyImg.resize(fuzzyImg.channels());

		for (int i = 0; i < fuzzyImg.channels(); ++i)
		{
			cv::Canny(channelImg[i], cannyImg[i], 20, 20, 3);
			cv::imshow(std::string("cannyImg-") + std::to_string(i), cannyImg[i]);
		}
		cv::waitKey(30);



		return true;
	}

	bool ColorSegmentation::Segmentation1(const cv::Mat & src) const
	{
		cv::imshow("src", src);
		cv::waitKey(30);

		cv::Mat small;
		cv::resize(src, small, cv::Size(320, 180));
		cv::imshow("small", small);
		cv::waitKey(30);

		//高斯模糊
		cv::Mat fuzzyImg;
		cv::GaussianBlur(small, fuzzyImg, cv::Size(15, 15), 0, 0);
		cv::imshow("fuzzyImg", fuzzyImg);
		cv::waitKey(30);

		//边缘查找
		cv::Mat cannyImg;
		cv::Canny(fuzzyImg, cannyImg, 30, 60, 3);
		cv::imshow("cannyImg", cannyImg);
		cv::waitKey(30);
		cv::Mat cannyShowImage;
		showEdge(fuzzyImg, cannyShowImage, cannyImg);
		cv::imshow("cannyShowImage", cannyShowImage);
		cv::waitKey(30); 

		//掩膜
		cv::Mat maskImage;
		cannyImg.convertTo(maskImage, CV_16UC1);
		copyMakeBorder(maskImage, maskImage, 1, 1, 1, 1, BORDER_CONSTANT, Scalar(0));

		return false;
	}

	bool ColorSegmentation::Segmentation2(const cv::Mat & src) const
	{
		return false;
	}

	bool ColorSegmentation::DrawRect(cv::Mat src, cv::RotatedRect rect, cv::Scalar color)
	{
		if (src.empty())
		{
			return false;
		}
		if (src.type() != CV_8UC3)
		{
			return false;
		}

		//获取顶点
		Point2f pointsf[4];
		rect.points(pointsf);
		Point pointsi[4];
		for (int i = 0; i < 4; ++i)
		{
			pointsi[i].x = pointsf[i].x;
			pointsi[i].y = pointsf[i].y;
		}

		//找出最高点,最低点，最左点，最右点
		int highIndex = 0, lowIndex = 0, leftIndex = 0, rightIndex = 0;
		for (int i = 0; i < 4; ++i)
		{
			if (pointsi[highIndex].y > pointsf[i].y)
			{
				highIndex = i;
			}
			if (pointsi[lowIndex].y < pointsf[i].y)
			{
				lowIndex = i;
			}
			if (pointsi[leftIndex].x > pointsf[i].x)
			{
				leftIndex = i;
			}
			if (pointsi[rightIndex].x < pointsf[i].x)
			{
				rightIndex = i;
			}
		}

		struct colorRamType
		{
			unsigned char b;
			unsigned char g;
			unsigned char r;
		};

		colorRamType colortemp = 
		{
			color.val[0],
			color.val[1],
			color.val[2]
		};

		if ((pointsi[highIndex].y == pointsi[leftIndex].y)
			|| (pointsi[highIndex].y == pointsi[rightIndex].y))
			//正矩形
		{
			int high = pointsi[highIndex].y >= 0 ? pointsi[highIndex].y : 0;
			int low = pointsi[lowIndex].y < src.size().height ? pointsi[lowIndex].y : src.size().height;
			int left = pointsi[leftIndex].x >= 0 ? pointsi[leftIndex].x : 0;
			int right = pointsi[rightIndex].x < src.size().width ? pointsi[rightIndex].x : src.size().width;

			if ((high >= low) || (right <= left))
			{
				return false;
			}

			for (int i = high; i < low; ++i)
			{
				colorRamType *temp = (colorRamType *)src.data + i * src.cols;
				for (int j = left; j < right; ++j)
				{
					*(temp + j) = colortemp;
				}
			}
		}
		else
			//斜矩形
		{
			int *leftSerial = new int[pointsi[lowIndex].y - pointsi[highIndex].y + 2];
			int *rightSerial = new int[pointsi[lowIndex].y - pointsi[highIndex].y + 2];

			float k, b;
			int count;

			count = 0;
			k = (pointsf[highIndex].x - pointsf[leftIndex].x) / (pointsf[highIndex].y - pointsf[leftIndex].y);
			b = pointsf[highIndex].x - k * pointsf[highIndex].y;
			for (int i = pointsi[highIndex].y; i < pointsf[leftIndex].y; ++i)
			{
				leftSerial[count++] = k * i + b;
			}
			k = (pointsf[leftIndex].x - pointsf[lowIndex].x) / (pointsf[leftIndex].y - pointsf[lowIndex].y);
			b = pointsf[leftIndex].x - k * pointsf[leftIndex].y;
			for (int i = pointsi[leftIndex].y; i <= pointsf[lowIndex].y; ++i)
			{
				leftSerial[count++] = k * i + b;
			}
			count = 0;
			k = (pointsf[highIndex].x - pointsf[rightIndex].x) / (pointsf[highIndex].y - pointsf[rightIndex].y);
			b = pointsf[highIndex].x - k * pointsf[highIndex].y;
			for (int i = pointsi[highIndex].y; i < pointsf[rightIndex].y; ++i)
			{
				rightSerial[count++] = k * i + b;
			}
			k = (pointsf[rightIndex].x - pointsf[lowIndex].x) / (pointsf[rightIndex].y - pointsf[lowIndex].y);
			b = pointsf[rightIndex].x - k * pointsf[rightIndex].y;
			for (int i = pointsi[rightIndex].y; i <= pointsf[lowIndex].y; ++i)
			{
				rightSerial[count++] = k * i + b;
			}

			int startIndex, endIndex;
			if (pointsi[highIndex].y < 0)
			{
				startIndex = -pointsi[highIndex].y;
			}
			else
			{
				startIndex = 0;
			}
			if (pointsi[lowIndex].y > src.rows)
			{
				endIndex = pointsi[lowIndex].y - pointsi[highIndex].y + 1 - (pointsi[lowIndex].y - src.rows);
			}
			else
			{
				endIndex = pointsi[lowIndex].y - pointsi[highIndex].y + 1;
			}
			for (int i = startIndex; i < endIndex; ++i)
			{
				int start = leftSerial[i] < 0 ? 0 : leftSerial[i];
				int end = rightSerial[i] >= src.cols ? src.cols : rightSerial[i];

				colorRamType *temp = (colorRamType *)src.data + (pointsi[highIndex].y + i) * src.cols;
				for (int j = start; j <= end; ++j)
				{
					*(temp + j) = colortemp;
				}
			}

			delete leftSerial;
			delete rightSerial;
		}

		return true;
	}

	void ColorSegmentation::showEdge(const cv::Mat & src, cv::Mat & dst, cv::Mat & edge) const
	{
		int size = src.rows * src.cols;
		dst = src;
		for (int i = 0; i < size; ++i)
		{
			if (edge.data[i] != 0)
			{
				dst.data[3 * i] = 0;
				dst.data[3 * i + 1] = 0;
				dst.data[3 * i + 2] = 255;
			}
		}
	}

	void ColorSegmentation::floodFill(cv::Mat & src, cv::Mat & dst, cv::Point seed, cv::Scalar loDiff, cv::Scalar upDiff)
	{
	}

	void ColorSegmentation::floodFill(cv::Mat & src, cv::Mat & dst, cv::Point seed)
	{
	}




}
