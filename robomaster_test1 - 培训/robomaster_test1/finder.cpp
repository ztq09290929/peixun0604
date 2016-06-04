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
	//������ͷ
	VideoCapture cap("RMFS.avi");


	//����Ƿ��
	if (!cap.isOpened())
	{
		cout << "NO OPEN" << endl;
		return -1;
	}

	namedWindow("frame", 1);

	while (1)
	{	

		Mat frame;
		//��cap�ж�һ֡����frame
		cap >> frame;
		//���δ����ͼ��
		if (frame.empty())break;

		imshow("frame", frame);
		//�ȴ�33ms�������ESC���˳�
		if (waitKey(100) == 27)break;

	}
	return 0;
}