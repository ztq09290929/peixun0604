/*------------------------------------------------------------------------------------------*\
   使用图像HSV颜色空间的色调H，用反投影法和meanshift法来查找目标物体
\*------------------------------------------------------------------------------------------*/

#include <iostream>
#include <vector>

#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\video\tracking.hpp>

#include "objectFinder.h"
#include "colorhistogram.h"

using namespace std;
using namespace cv;
int main()
{
	//打开摄像头
	VideoCapture cap("RMFS.avi");


	//检查是否打开
	if (!cap.isOpened())
	{
		cout << "NO OPEN" << endl;
		return -1;
	}

	namedWindow("frame", 1);

	while (1)
	{	

		Mat frame;
		//从cap中读一帧存入frame
		cap >> frame;
		//如果未读到图像
		if (frame.empty())break;

		imshow("frame", frame);
		//等待33ms，如果按ESC则退出
		if (waitKey(100) == 27)break;

	}
	return 0;
}