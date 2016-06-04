#include "BGR2HSV.hpp"
#include <opencv2/opencv.hpp>
#include <time.h>

using namespace cv;
using namespace std;

int main()
{
	cv::Mat img = cv::imread("fs2.jpg");
	cv::Mat img_h,img_h2;
	vector<Mat> v;	//存放三个通道的向量
	cvtColor(img, img_h2, CV_BGR2HSV);
	cv::split(img_h2, v);
	cv::Mat img_h3= v[0];

	clock_t t1, t2;
	t1 = clock();
	BGR2HSV(img, img_h);
	t2 = clock();
	cout << "所用时间: " << (double)(t2 - t1) / CLOCKS_PER_SEC << endl;

	return 0;
}