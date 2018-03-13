#include "stdafx.h"
#include "AreaSegmentation.h"


namespace colorArea
{
	AreaSegmentation::AreaSegmentation()
	{
	}

	AreaSegmentation::~AreaSegmentation()
	{
	}

	void AreaSegmentation::SetColorList(const std::vector<cv::Vec3b> &List)
	{
		if (ColorList != nullptr)
		{
			delete ColorList;
		}
		ColorList = new unsigned char[3 * List.size()];
		ColorListLength = List.size();
		for (int i = 0; i < ColorListLength; ++i)
		{
			ColorList[i * 3] = List[i][0];
			ColorList[i * 3 + 1] = List[i][1];
			ColorList[i * 3 + 2] = List[i][2];
		}
	}

	void AreaSegmentation::SetColorList(int span)
	{
		if (ColorList != nullptr)
		{
			delete ColorList;
		}

		span = span >= 1 ? span : 1;
		span = span <= 256 ? span : 256;
		int delta = 256 / span;

		ColorList = new unsigned char[3 * span * span * span];
		ColorListLength = span * span * span;

		int param1 = span * span;
		int param2 = span;
		for (int i = 0; i < span; ++i)
		{
			int paramx = param1 * i;
			int b = i * delta;
			for (int j = 0; j < span; ++j)
			{
				int g = j * delta;
				int paramy = param2 * j;
				for (int k = 0; k < span; ++k)
				{
					int paramz = (paramx + paramy + k) * 3;
					ColorList[paramz] = b;
					ColorList[paramz + 1] = g;
					ColorList[paramz + 2] = k * delta;
				}
			}
		}
	}

	bool AreaSegmentation::ColorSegmentation(const cv::Mat &src, cv::Mat &dst) const
	{
		if (src.empty())
		{
			return false;
		}

		dst = cv::Mat(src.rows, src.cols, src.type());

		unsigned char *tempSrc = src.data;
		unsigned char *tempDst = dst.data;
		for (int i = src.cols * src.rows; i >= 0; --i)
		{
			CalculateColor(tempSrc, tempDst);
			tempSrc += 3;
			tempDst += 3;
		}

		return true;
	}
}
