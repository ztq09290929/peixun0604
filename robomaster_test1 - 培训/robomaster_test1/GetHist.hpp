//CopyRights @÷‹ºÕ«ø
#include<iostream>
#include "opencv2/opencv.hpp"

void GetHist(cv::Mat & _inputImg, cv::Mat & _hist, const int _bin, int & _maxBinNum)
{
	_hist = cv::Mat::zeros(1, _bin,CV_32SC1);
	int* histptr = _hist.ptr<int>(0);
	for (int i = 0; i < _inputImg.rows; i++)
	{
		uchar *ptr = _inputImg.ptr<uchar>(i);
		for (int j = 0; j < _inputImg.cols; j++)
		{
			histptr[ptr[j]] += 1;
		}
	}
	_maxBinNum = 0;
	for (int i = 0; i < _bin; i++)
	{
		//std::cout << histptr[i] << std::endl;
		_maxBinNum = _maxBinNum > histptr[i] ? _maxBinNum : histptr[i];
	}
}
