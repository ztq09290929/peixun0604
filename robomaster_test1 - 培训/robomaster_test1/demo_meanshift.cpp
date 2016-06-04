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
	///读入图像，获取ROI
	Mat image= imread("fs1.jpg");
	if (!image.data){cout<<"error";return -1;}; 

	///定义ROI区域，并且用矩形框出来
	Mat imageROI= image(Rect(268,223,30,35));  
	rectangle(image, Rect(268,223,30,35),Scalar(255,255,255));

	///显示图像
	namedWindow("Image");
	imshow("Image",image);

	waitKey();

	///设定饱和度阈值65，创建对象hc，获得h通道的1维直方图colorhist
	int minSat=115;
	ColorHistogram hc;
	MatND colorhist= hc.getHueHistogram(imageROI,minSat);
	
///***************************************** 使用minshift查找 ***************************************************/
	///创建对象finder
	ObjectFinder finder;
	///输入上面得到的1D直方图，并归一化
	finder.setHistogram(colorhist);

	///读入待检测图像，并转化为HSV通道
	image= imread("fs1.jpg");
	Mat hsv;
	cvtColor(image, hsv, CV_BGR2HSV);

	///分解出S通道，即饱和度
	vector<Mat> v;
	split(hsv,v);

	///识别低饱和度像素
	threshold(v[1],v[1],minSat,255,cv::THRESH_BINARY);
	namedWindow("Saturation");
	imshow("Saturation",v[1]);

	waitKey();

	///得到输入图像hsv通道的反投影图像result
	int ch[1]={0};
	Mat result= finder.find(hsv,0.0f,180.0f,ch,1);

	///除去低饱和度的像素
	bitwise_and(result,v[1],result);
	namedWindow("Result Hue and raw");
	imshow("Result Hue and raw",result);

	waitKey();
	///矩形框初始位置
	Rect rect(250,250,30,35);
	rectangle(image, rect, Scalar(0,0,255));

	//TermCriteria模板类，这个类是作为迭代算法的终止条件的，该类变量需要3个参数，一个是类型，
	//第二个参数为迭代的最大次数，最后一个是特定的阈值。
	//类型有CV_TERMCRIT_ITER、CV_TERMCRIT_EPS、CV_TERMCRIT_ITER+CV_TERMCRIT_EPS，
	//分别代表着迭代终止条件为达到最大迭代次数终止，迭代到阈值终止，或者两者都作为迭代终止条件。
	//以上的宏对应的c++的版本分别为TermCriteria::COUNT、TermCriteria::EPS，这里的COUNT也可以写成MAX_ITER。
	TermCriteria criteria(TermCriteria::EPS,10,1);
	cout << "meanshift= " << cv::meanShift(result,rect,criteria) << endl;
	///	最终估计的目标位置
	rectangle(image, rect, Scalar(255,255,255));

	// Display image
	namedWindow("Image 2 result");
	imshow("Image 2 result",image);

	waitKey();
	return 0;
}