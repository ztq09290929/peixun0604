/*------------------------------------------------------------------------------------------*\
   使用图像HSV颜色空间的色调H，用反投影法和meanShift法来查找目标物体,鼠标点击来选择物体，并时刻输出目标位置
\*------------------------------------------------------------------------------------------*/
#include <iostream>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\video\tracking.hpp>
#include "objectFinder.h"
#include "colorhistogram.h"

using namespace std;
using namespace cv;

///全局变量
int minSat=115;	//回调函数参数——饱和度阈值
Mat colorhist;	//回调函数参数——颜色直方图
Mat imageROI;	//回调函数参数——样本图像中提取的目标区域

bool paused = false; //判断是否为暂停状态
bool selectObject = false;//代表是否在用鼠标选要跟踪的初始目标，true表示正在用鼠标选择

Point origin;//用于保存鼠标选择第一次单击时点的位置
Rect selection = Rect(0,0,0,0);//用于保存鼠标选择的矩形框
Mat frame,frame0;//最终的视频图片



///声明：求取颜色直方图的回调函数
void on_colorhist(int,void*);

///声明：鼠标回调函数
void onMouse( int event, int x, int y, int, void* );


int main()
{
	///创建滑动条，可以调整饱和度阈值****************************************************/
	namedWindow("track");
	setMouseCallback( "track", onMouse, 0 );//鼠标消息响应机制
	createTrackbar( "minSat", "track", &minSat, 255, on_colorhist );//最后一个参数为0代表没有调用滑动拖动的响应函数

	///**********************************************************************************/


	///打开视频**************************************************************************/
	VideoCapture cap;
	//cap.open("RM1.avi");	
	cap.open(1);
	//检查是否打开
	if (!cap.isOpened())
	{
		cout << "AVI NO OPEN" << endl;
		return -1;
	}
	///***********************************************************************************/


	///进入处理循环***********************************************************************/
	while (1)
	{	
		
		if(!paused)
		{
			//从cap中读一帧存入frame
			cap >> frame0;
			//如果未读到图像
			if (frame0.empty())break;
			
		}

		frame0.copyTo(frame);

		if(selection.width > 0 && selection.height > 0 &&!paused &&!selectObject)
		{
			///使用minshift查找 **************************************************************/

			Mat hsv,result;//hsv图像，result概率图
			ObjectFinder finder;
			vector<Mat> v;	//存放三个通道的向量
			int ch[1]={0};	//find传入参数，决定统计哪个通道

			///输入上面得到的1D直方图，并归一化
			finder.setHistogram(colorhist);
			///将frame转化为hsv通道
			cvtColor(frame, hsv, CV_BGR2HSV);
			///分解出S通道，即饱和度
			split(hsv,v);

			///识别低饱和度像素
			threshold(v[1],v[1],minSat,255,CV_THRESH_BINARY);

			///得到输入图像hsv通道的反投影图像result
			result= finder.find(hsv,0.0f,180.0f,ch,1);

			///除去低饱和度的像素
			bitwise_and(result,v[1],result);

			///定义搜索终止条件
			TermCriteria criteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 3, 1 );
			///开始CamShift查找
			//cv::meanShift(result,selection,criteria);
			
			rectangle(frame, selection, Scalar(255,255,255),2,8);
			///输出trackBox中心坐标
			cout<<"location = "<<"("<<selection.x+15<<","<<selection.y+15<<")"<<endl;
		}

		///点击鼠标时画红色圆形******************************************************************/
		if( selectObject )
        {
				circle(frame,Point(origin.x,origin.y),10,Scalar(0,0,255),2,8);
        }

		///显示画面**********************************************************************/
		imshow("track",frame);
		
		///暂停，判断键盘输入等***********************************************************/
		char c = (char)waitKey(33);
		if( c == 27 ) break;          //退出键
            
        switch(c)
        {
			case 'p':       //暂停跟踪交替
				paused = !paused;
				break;
			default:
            ;
        }
	}
	return 0;
}

///函数：求取颜色直方图的回调函数
void on_colorhist(int,void*)
{
	//创建对象hc用来求取颜色直方图colorhist
	ColorHistogram hc;
	//计算颜色直方图colorhist
	colorhist= hc.getHueHistogram(imageROI,minSat);
}

///函数：鼠标回调函数
void onMouse( int event, int x, int y, int, void* )
{
    if( selectObject )//只有当鼠标左键按下去时才有效，然后通过if里面代码就可以确定所选择的矩形区域selection了
    {
		///初始目标框为30*30的正方形
        selection.x = origin.x - 15;//矩形左上角顶点坐标
        selection.y = origin.y - 15;
        selection.width = 30;//矩形宽
        selection.height = 30;//矩形高

        selection &= Rect(0, 0, frame.cols, frame.rows);//用于确保所选的矩形区域在图片范围内
    }

    switch( event )
    {
    case CV_EVENT_LBUTTONDOWN:
        origin = Point(x,y);
        selectObject = true;
        break;
    case CV_EVENT_LBUTTONUP:
        selectObject = false;
		///获取所选区域，输入到回调函数，得到初始colorhist
		imageROI = frame0(selection);///注意，ROI区域取自frame0，因为frame在鼠标选择时颜色被取反了
		on_colorhist(0,0);
		///画出初始目标框
		rectangle(frame0, selection, Scalar(255,255,255),2,8);
        break;
    }
}