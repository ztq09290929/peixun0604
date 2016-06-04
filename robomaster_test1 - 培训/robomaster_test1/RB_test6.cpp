/*------------------------------------------------------------------------------------------*\
   使用图像HSV颜色空间的色调H，用反投影法和CamShift法来查找目标物体,鼠标选中物体作为初始
\*------------------------------------------------------------------------------------------*/
#include <iostream>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\video\tracking.hpp>
#include "objectFinder.h"
#include "colorhistogram.h"
#include "get_gravity_center.hpp"
#include "time.h"

using namespace std;
using namespace cv;

///全局变量
Mat colorhist;	//回调函数参数——颜色直方图
Mat imageROI;	//回调函数参数——样本图像中提取的目标区域
int maxbin;
bool paused = false; //判断是否为暂停状态
bool selectObject = false;//代表是否在用鼠标选要跟踪的初始目标，true表示正在用鼠标选择

Point origin;//用于保存鼠标选择第一次单击时点的位置
Rect selection = Rect(0,0,0,0);//用于保存鼠标选择的矩形框
Mat frame,frame0;//最终的视频图片

int cnt = 0;

///声明：求取颜色直方图的回调函数
void on_colorhist(int,void*);

///声明：鼠标回调函数
void onMouse( int event, int x, int y, int, void* );


int main()
{
	///创建滑动条，可以调整饱和度阈值****************************************************/
	namedWindow("track");
	setMouseCallback( "track", onMouse, 0 );//鼠标消息响应机制

	///**********************************************************************************/


	///打开视频**************************************************************************/
	VideoCapture cap;
	//cap.open("RM2.avi");	
	cap.open(0);
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
		clock_t t1, t2;
		
		//double t = (double)getTickCount();
		if(!paused)
		{
			//从cap中读一帧存入frame
			cap >> frame0;
			//如果未读到图像
			if (frame0.empty())break;
			
		}

		frame0.copyTo(frame);
		t1 = clock();
		if(selection.width > 0 && selection.height > 0 &&!paused &&!selectObject)
		{
			///使用camshift查找 **************************************************************/

			Mat hsv,result;//hsv图像，result概率图
			ObjectFinder finder;
			vector<Mat> v;	//存放三个通道的向量
			int ch[1]={0};	//find传入参数，决定统计哪个通道

			///输入上面得到的1D直方图，并归一化
			finder.setHistogram(colorhist);
			///将frame转化为hsv通道
			//cvtColor(frame, hsv, CV_BGR2HSV);
			BGR2HSV(frame, hsv);

			///得到输入图像hsv通道的反投影图像result
			result= finder.find(hsv,0.0f,180.0f,ch,1,maxbin);

			///定义搜索终止条件
			//TermCriteria criteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 );
			/////开始CamShift查找
			//RotatedRect trackBox = CamShift(result, selection, criteria); 
			camshift_iteration(result, selection, 10, 5);

			//std::cout << "sleft  " << selection << std::endl;
			
            ///防止矩形框小于1                       
			if( selection.area() <= 1 )   //是通过max_num_of_trees_in_the_forest  
                {
                    int cols = result.cols, rows = result.rows, r = (MIN(cols, rows) + 5)/6;
                    selection = Rect(selection.x - r, selection.y - r,
                                       selection.x + r, selection.y + r) &
                                  Rect(0, 0, cols, rows);//Rect函数为矩阵的偏移和大小，即第一二个参数为矩阵的左上角点坐标，第三四个参数为矩阵的宽和高
                }
			///画出trackBox的内切椭圆
			//ellipse(frame, trackBox, Scalar(255,255,255), 2, CV_AA );//跟踪的时候以椭圆为代表目标
			cv::rectangle(frame, selection, cv::Scalar(0, 0, 255), 2);

			ostringstream file_name;
			file_name <<"result/"<< (++cnt) << ".jpg";
			cv::imwrite(file_name.str(), frame(selection));
		}




		///鼠标画矩形框******************************************************************/
		if( selectObject && selection.width > 0 && selection.height > 0 )
        {
				Mat roi(frame, selection);
				bitwise_not(roi, roi);//bitwise_not为将每一个bit位取反
        }

		///显示画面**********************************************************************/
		imshow("track",frame);
		/*t = ((double)getTickCount() - t)/getTickFrequency();
		cout << "Times passed in seconds: " << t << endl;*/
		///暂停，判断键盘输入等***********************************************************/
		char c = (char)waitKey(1);
		if( c == 27 ) break;          //退出键
            
        switch(c)
        {
			case 'p':       //暂停跟踪交替
				paused = !paused;
				break;
			default:
            ;
        }

		t2 = clock();
		std::cout << "当前帧率： " << 1 / (0.0001+(double)(t2 - t1) / CLOCKS_PER_SEC) << std::endl;;
	}
	return 0;
}

///函数：求取颜色直方图的回调函数
void on_colorhist(int,void*)
{
	//创建对象hc用来求取颜色直方图colorhist
	ColorHistogram hc;
	//计算颜色直方图colorhist
	colorhist= hc.getHueHistogram(imageROI,maxbin);
}

///函数：鼠标回调函数
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
		paused = false;
		///获取所选区域，输入到回调函数，得到初始colorhist
		imageROI = frame0(selection);///注意，ROI区域取自frame0，因为frame在鼠标选择时颜色被取反了
		on_colorhist(0,0);
        break;
    }
}