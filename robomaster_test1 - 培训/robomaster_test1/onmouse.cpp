#include <opencv2\opencv.hpp>
#include <iostream>
#include "cv.h"
#include "highgui.h"

using namespace cv;
using namespace std;

bool selectObject = false;//�����Ƿ���ѡҪ���ٵĳ�ʼĿ�꣬true��ʾ���������ѡ��
Point origin;//���ڱ������ѡ���һ�ε���ʱ���λ��
Rect selection;//���ڱ������ѡ��ľ��ο�
Mat frame;

void onMouse( int event, int x, int y, int, void* )
{
    if( selectObject )//ֻ�е�����������ȥʱ����Ч��Ȼ��ͨ��if�������Ϳ���ȷ����ѡ��ľ�������selection��
    {
        selection.x = MIN(x, origin.x);//�������ϽǶ�������
        selection.y = MIN(y, origin.y);
        selection.width = std::abs(x - origin.x);//���ο�
        selection.height = std::abs(y - origin.y);//���θ�

        selection &= Rect(0, 0, frame.cols, frame.rows);//����ȷ����ѡ�ľ���������ͼƬ��Χ��
    }

    switch( event )
    {
    case CV_EVENT_LBUTTONDOWN:
        origin = Point(x,y);
        selection = Rect(x,y,0,0);//���հ���ȥʱ��ʼ����һ����������
        selectObject = true;
        break;
    case CV_EVENT_LBUTTONUP:
        selectObject = false;
        //if( selection.width > 0 && selection.height > 0 )
        //    trackObject = -1;
        break;
    }
}
int main()
{
	//������ͷ
	VideoCapture cap;
	cap.open("RM.avi");	//��ͬ��VideoCapture cap��1��

	//����Ƿ��
	if (!cap.isOpened())
	{
		cout << "NO OPEN" << endl;
		return -1;
	}

	namedWindow("frame", 1);
	setMouseCallback( "frame", onMouse, 0 );//��Ϣ��Ӧ����
	while (1)
	{	
		//��cap�ж�һ֡����frame
		cap >> frame;
		//���δ����ͼ��
		if (frame.empty())break;



		if( selectObject && selection.width > 0 && selection.height > 0 )
        {
            Mat roi(frame, selection);
            bitwise_not(roi, roi);//bitwise_notΪ��ÿһ��bitλȡ��
        }

		imshow("frame", frame);
		//�ȴ�33ms�������ESC���˳�
		if (waitKey(33) == 27)break;

	}
	return 0;
}