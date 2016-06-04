/*------------------------------------------------------------------------------------------*\
   ʹ��ͼ��HSV��ɫ�ռ��ɫ��H���÷�ͶӰ����meanshift��������Ŀ������
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
	///����ͼ�񣬻�ȡROI
	Mat image= imread("fs1.jpg");
	if (!image.data){cout<<"error";return -1;}; 

	///����ROI���򣬲����þ��ο����
	Mat imageROI= image(Rect(268,223,30,35));  
	rectangle(image, Rect(268,223,30,35),Scalar(255,255,255));

	///��ʾͼ��
	namedWindow("Image");
	imshow("Image",image);

	waitKey();

	///�趨���Ͷ���ֵ65����������hc�����hͨ����1άֱ��ͼcolorhist
	int minSat=115;
	ColorHistogram hc;
	MatND colorhist= hc.getHueHistogram(imageROI,minSat);
	
///***************************************** ʹ��minshift���� ***************************************************/
	///��������finder
	ObjectFinder finder;
	///��������õ���1Dֱ��ͼ������һ��
	finder.setHistogram(colorhist);

	///��������ͼ�񣬲�ת��ΪHSVͨ��
	image= imread("fs1.jpg");
	Mat hsv;
	cvtColor(image, hsv, CV_BGR2HSV);

	///�ֽ��Sͨ���������Ͷ�
	vector<Mat> v;
	split(hsv,v);

	///ʶ��ͱ��Ͷ�����
	threshold(v[1],v[1],minSat,255,cv::THRESH_BINARY);
	namedWindow("Saturation");
	imshow("Saturation",v[1]);

	waitKey();

	///�õ�����ͼ��hsvͨ���ķ�ͶӰͼ��result
	int ch[1]={0};
	Mat result= finder.find(hsv,0.0f,180.0f,ch,1);

	///��ȥ�ͱ��Ͷȵ�����
	bitwise_and(result,v[1],result);
	namedWindow("Result Hue and raw");
	imshow("Result Hue and raw",result);

	waitKey();
	///���ο��ʼλ��
	Rect rect(250,250,30,35);
	rectangle(image, rect, Scalar(0,0,255));

	//TermCriteriaģ���࣬���������Ϊ�����㷨����ֹ�����ģ����������Ҫ3��������һ�������ͣ�
	//�ڶ�������Ϊ�����������������һ�����ض�����ֵ��
	//������CV_TERMCRIT_ITER��CV_TERMCRIT_EPS��CV_TERMCRIT_ITER+CV_TERMCRIT_EPS��
	//�ֱ�����ŵ�����ֹ����Ϊ�ﵽ������������ֹ����������ֵ��ֹ���������߶���Ϊ������ֹ������
	//���ϵĺ��Ӧ��c++�İ汾�ֱ�ΪTermCriteria::COUNT��TermCriteria::EPS�������COUNTҲ����д��MAX_ITER��
	TermCriteria criteria(TermCriteria::EPS,10,1);
	cout << "meanshift= " << cv::meanShift(result,rect,criteria) << endl;
	///	���չ��Ƶ�Ŀ��λ��
	rectangle(image, rect, Scalar(255,255,255));

	// Display image
	namedWindow("Image 2 result");
	imshow("Image 2 result",image);

	waitKey();
	return 0;
}