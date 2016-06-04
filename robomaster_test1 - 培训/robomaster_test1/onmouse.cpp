#include <opencv2\opencv.hpp>
#include <iostream>
#include "cv.h"
#include "highgui.h"

using namespace cv;
using namespace std;

bool selectObject = false;//代表是否在选要跟踪的初始目标，true表示正在用鼠标选择
Point origin;//用于保存鼠标选择第一次单击时点的位置
Rect selection;//用于保存鼠标选择的矩形框
Mat frame;

void onMouse( int event, int x, int y, int, void* )
{
    if( selectObject )//只有当鼠标左键按下去时才有效，然后通过if里面代码就可以确定所选择的矩形区域selection了
    {
        selection.x = MIN(x, origin.x);//矩形左上角顶点坐标
        selection.y = MIN(y, origin.y);
        selection.width = std::abs(x - origin.x);//矩形宽
        selection.height = std::abs(y - origin.y);//矩形高

        selection &= Rect(0, 0, frame.cols, frame.rows);//用于确保所选的矩形区域在图片范围内
    }

    switch( event )
    {
    case CV_EVENT_LBUTTONDOWN:
        origin = Point(x,y);
        selection = Rect(x,y,0,0);//鼠标刚按下去时初始化了一个矩形区域
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
	//打开摄像头
	VideoCapture cap;
	cap.open("RM.avi");	//等同于VideoCapture cap（1）

	//检查是否打开
	if (!cap.isOpened())
	{
		cout << "NO OPEN" << endl;
		return -1;
	}

	namedWindow("frame", 1);
	setMouseCallback( "frame", onMouse, 0 );//消息响应机制
	while (1)
	{	
		//从cap中读一帧存入frame
		cap >> frame;
		//如果未读到图像
		if (frame.empty())break;



		if( selectObject && selection.width > 0 && selection.height > 0 )
        {
            Mat roi(frame, selection);
            bitwise_not(roi, roi);//bitwise_not为将每一个bit位取反
        }

		imshow("frame", frame);
		//等待33ms，如果按ESC则退出
		if (waitKey(33) == 27)break;

	}
	return 0;
}