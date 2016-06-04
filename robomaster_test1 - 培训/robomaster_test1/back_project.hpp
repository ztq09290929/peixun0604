#pragma once
#include "opencv2/imgproc.hpp"
#include <opencv2/core/utility.hpp>
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"

void back_project( cv::Mat &img, cv::Mat &hish,const double MAX_VALUE, cv::Mat& img_out )
{
	// img_out = cv::Mat(img);
	/*int gap = 181 / hish.cols;
	uchar* org_p = img.data;
	uchar *new_p = img_out.data;
	for (unsigned int i = 0; i < img.cols*img.rows; i++)
	{
		double value = hish.data[(*org_p) / gap];
		uchar norm_value = (uchar)((value / MAX_VALUE) * 255);
		*new_p = norm_value;
		org_p++;
		new_p++;
	}*/

	
	
	int gap = 1;
	assert(img.empty() == false);
	assert(img.type() == CV_8UC1);
	assert(hish.empty() == false);
	assert(hish.type() == CV_32SC1);
	
	img_out = cv::Mat(img.size(), img.type());
	int* histptr = hish.ptr<int>(0);
	for (int i = 0; i < img.rows; i++)
	{
		uchar* ptr = img.ptr<uchar>(i);
		uchar* ptr_out = img_out.ptr<uchar>(i);
		for (int j = 0; j < img.cols; j++)
		{
			double value = histptr[ptr[j] / gap];
			int norm_value = static_cast<int>(value / MAX_VALUE *255);
			ptr_out[j] = norm_value;
		}
	}

}