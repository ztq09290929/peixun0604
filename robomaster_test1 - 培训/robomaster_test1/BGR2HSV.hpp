#ifndef _BGR2HSV_
#define _BGR2HSV_

#include <opencv2/opencv.hpp>
using namespace std;

int GetMax(int B, int G, int R ,int& idx)
{
	int temp_max = B;
	idx = 0;

	if (G > temp_max)
	{
		temp_max = G;
		idx = 1;
	}

	if (R > temp_max)
	{
		temp_max = R;
		idx = 2;
	}

	return temp_max;
}
int GetMin(int B, int G, int R)
{
	int temp_min = B;

	if (G < temp_min)
		temp_min = G;
	if (R < temp_min)
		temp_min = R;

	return temp_min;
}

void BGR2HSV(cv::Mat _rgb_img, cv::Mat & _h_img)
{
	///定义H通道的图像，大小和RGB一样，单通道，全零
	cv::Mat h_img = cv::Mat::zeros(_rgb_img.rows,_rgb_img.cols,CV_8UC1);

	//#pragma omp parallel for num_threads(2)
	for (int i = 0; i < _rgb_img.rows; ++i)
	{
		cv::Vec3b * pr = _rgb_img.ptr<cv::Vec3b>(i);
		uchar * pr_h = h_img.ptr<uchar>(i);

		//#pragma omp parallel for num_threads(2)
		for (int j = 0; j < _rgb_img.cols; ++j)
		{
			int B = pr[j][0];
			int G = pr[j][1];
			int R = pr[j][2];
			int idx = 0;
			int val_max = GetMax(B, G, R,idx);
			int val_min = GetMin(B, G, R);
			int val_h = 0;
			if (val_max - val_min == 0)
			{
				val_h = 0;
			}
			else if (idx == 2)
			{
				val_h = 60 * (G - B) / (val_max - val_min);
			}
			else if (idx == 1)
			{
				val_h = 120 + 60 * (B - R) / (val_max - val_min);
			}
			else
			{
				val_h = 240 + 60 * (R - G) / (val_max - val_min);
			}

			if (val_h < 0)
			{
				val_h += 360;
			}

			pr_h[j] = val_h / 2;
		}
	}

	_h_img = h_img.clone();
}

#endif // !_RGB2HSV_
