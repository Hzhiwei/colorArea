#include "stdafx.h"
#include "PureColorSegmentation.h"
#include <vector>
#include <set>

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

	int PureColorSegmentation::segmentation( cv::Mat & src)
	{
		floodFillAll(src);
		vector<vector<Point>> edge = edgeTrackAll();
		drawEdge(src, edge, -1, Scalar(0, 0, 0));

		return indexColor.size() - 1;
	}

	//根据颜色分块，块关系由maskImage提供
	void PureColorSegmentation::floodFillAll(const cv::Mat & src)
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
	}

	//漫水填充
	void PureColorSegmentation::floodFill(int x, int y)
	{
		ColorStruct<unsigned char> *srcPoint = (ColorStruct<unsigned char> *)(srcImage->ptr<Vec3b>(y)) + x;
		int *maskPoint = (int *)(maskImage.ptr<int>(y + 1)) + x + 1;
		averageColor = *srcPoint;
		index += 1;
		floodFillScanline(srcPoint, maskPoint);
		indexColor.push_back(averageColor);
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
				if (floodFillCheckAndCal(miny))
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
				if (floodFillCheckAndCal(maxy))
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
				if (floodFillCheckAndCal(j - 1))

				{
					floodFillScanline((j - 1), (imask - 1));
				}
			}
		}
		for (imask = minMasky + maskImage.cols, j = miny + srcImage->cols; imask < maxMasky; imask += maskImage.cols, j += srcImage->cols)
		{
			if (*(imask + 1) == 0)
			{
				if (floodFillCheckAndCal(j + 1))
				{
					floodFillScanline((j + 1), (imask + 1));
				}
			}
		}
	}

	vector<vector<Point>> PureColorSegmentation::edgeTrackAll(void)
	{
		surroundPointerDiffer[0] = -maskImage.cols - 1;
		surroundPointerDiffer[1] = -maskImage.cols;
		surroundPointerDiffer[2] = -maskImage.cols + 1;
		surroundPointerDiffer[3] = 1;
		surroundPointerDiffer[4] = maskImage.cols + 1;
		surroundPointerDiffer[5] = maskImage.cols;
		surroundPointerDiffer[6] = maskImage.cols - 1;
		surroundPointerDiffer[7] = -1;
		surroundPointerDiffer[8] = -maskImage.cols - 1;

		surroundIndexDiffer[0][0] = -1;
		surroundIndexDiffer[0][1] = -1;
		surroundIndexDiffer[1][0] = 0;
		surroundIndexDiffer[1][1] = -1;
		surroundIndexDiffer[2][0] = 1;
		surroundIndexDiffer[2][1] = -1;
		surroundIndexDiffer[3][0] = 1;
		surroundIndexDiffer[3][1] = 0;
		surroundIndexDiffer[4][0] = 1;
		surroundIndexDiffer[4][1] = 1;
		surroundIndexDiffer[5][0] = 0;
		surroundIndexDiffer[5][1] = 1;
		surroundIndexDiffer[6][0] = -1;
		surroundIndexDiffer[6][1] = 1;
		surroundIndexDiffer[7][0] = -1;
		surroundIndexDiffer[7][1] = 0;

		set<int> repository;
		vector<vector<Point>> edgeList;

		repository.insert(-1);
		for (int i = 0; i < maskImage.cols; ++i)
		{
			for (int j = 0; j < maskImage.rows; ++j)
			{
				if (repository.find(maskImage.at<int>(j, i)) == repository.end())
					//if判断是速度瓶颈（开O2优化运行时间可以从600ms降到35ms）
				{
					repository.insert(maskImage.at<int>(j, i));
					edgeList.push_back(edgeTrack(Point(i, j)));
				}
			}
		}

		return edgeList;
	}

	//根据种子点查找轮廓，种子点必须位于轮廓上。
	//轮廓位置为区域内点上
	//轮廓方向顺时针
	vector<Point> PureColorSegmentation::edgeTrack(Point seed)
	{
		vector<Point> edgePointList;

		edgePointList.push_back(seed);

		const int *seedPointer = (int *)maskImage.data + maskImage.cols * seed.y + seed.x;
		const int *currentPointer = seedPointer;
		int nextIndex = -1;

		//查找种子点后的第一个分界点
		for (int i = 0; i < 8; ++i)
		{
			if ((*(currentPointer + surroundPointerDiffer[i]) != *currentPointer)
				&& (*(currentPointer + surroundPointerDiffer[i + 1]) == *currentPointer))
			{
				nextIndex = (i + 1) % 8;
				break;
			}
		}

		if (nextIndex == -1)
		{
			--edgePointList[0].x;
			--edgePointList[0].y;
			return edgePointList;
		}

		while (currentPointer + surroundPointerDiffer[nextIndex] != seedPointer)
		{
			edgePointList.push_back(Point(edgePointList.back().x + surroundIndexDiffer[nextIndex][0], edgePointList.back().y + surroundIndexDiffer[nextIndex][1]));

			currentPointer += surroundPointerDiffer[nextIndex];

			for (int i = (nextIndex + 5) % 8; ; i = (i + 1) % 8)
			{
				if (*(currentPointer + surroundPointerDiffer[i]) == *seedPointer)
				{
					nextIndex = i;
					break;
				}
			}
		}

		for(auto i = edgePointList.begin(); i != edgePointList.end(); ++i)
		{
			--i->y;
			--i->x;
		}

		return edgePointList; 
	}

	void PureColorSegmentation::drawEdge(cv::Mat & dst, const std::vector<std::vector<cv::Point>>& edge, int index, cv::Scalar color)
	{
		if (index >= (int)edge.size())
		{
			return;
		}

		ColorStruct<unsigned char> colorData(color.val[0], color.val[1], color.val[2]);

		if (index < 0) 
		{
			for (auto i = edge.begin(); i != edge.end(); ++i)
			{
				for (auto j = i->begin(); j != i->end(); ++j)
				{
					*((ColorStruct<unsigned char> *)dst.data + dst.cols * j->y + j->x) = colorData;
				}
			}
		}
		else
		{
			for (auto i = edge[index].begin(); i != edge[index].end(); ++i)
			{
				*((ColorStruct<unsigned char> *)dst.data + dst.cols * i->y + i->x) = colorData;
			}
		}
	}
}
