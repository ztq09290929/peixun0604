/*------------------------------------------------------------------------------------------*\
   ʹ��ͼ��HSV��ɫ�ռ��ɫ��H���÷�ͶӰ����camshift��������Ŀ������
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

int minSat=115;	//�ص����������������Ͷ���ֵ
Mat colorhist;	//�ص���������������ɫֱ��ͼ
Mat imageROI;	//�ص�����������������ͼ������ȡ��Ŀ������

bool paused = false; //�ж��Ƿ�Ϊ��ͣ״̬
bool selectObject = false;//�����Ƿ��������ѡҪ���ٵĳ�ʼĿ�꣬true��ʾ���������ѡ��
Point origin;//���ڱ������ѡ���һ�ε���ʱ���λ��
Rect selection;//���ڱ������ѡ��ľ��ο�
Mat frame;//���յ���ƵͼƬ

///���ص�����
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
		paused = false;
        break;
    }
}

///��ȡ��ɫֱ��ͼ�Ļص�����
void on_colorhist(int,void*)
{
	//��������hc������ȡ��ɫֱ��ͼcolorhist
	ColorHistogram hc;
	//������ɫֱ��ͼcolorhist
	colorhist= hc.getHueHistogram(imageROI,minSat);
}

int main()
{

	///����ͼ�񣬻�ȡROI*****************************************************************/
	Mat image;

	image= imread("fs1.jpg");
	if (!image.data){cout<<"error";return -1;}; 

	//����ROI���򣬲����þ��ο����
	imageROI= image(Rect(268,223,30,35));  
	rectangle(image, Rect(268,223,30,35),Scalar(255,255,255));

	//��ʾͼ��
	namedWindow("Image");
	imshow("Image",image);
	///**********************************************************************************/


	///���������������Ե������Ͷ���ֵ****************************************************/
	namedWindow("track");
	setMouseCallback( "track", onMouse, 0 );//�����Ϣ��Ӧ����
	createTrackbar( "minSat", "track", &minSat, 255, on_colorhist );//���һ������Ϊ0����û�е��û����϶�����Ӧ����
	///����Ĭ��ֵ���õ���ʼcolorhist
	on_colorhist(0,0);
	///**********************************************************************************/


	///����Ƶ**************************************************************************/
	VideoCapture cap;
	cap.open("RMFS.avi");	
	//����Ƿ��
	if (!cap.isOpened())
	{
		cout << "AVI NO OPEN" << endl;
		return -1;
	}
	///***********************************************************************************/

	Rect trackwindow(290,260,30,35);//��ʼ���ο�

	Mat frame0;
	///���봦��ѭ��***********************************************************************/
	while (1)
	{	
		if(!paused)
		{
			//��cap�ж�һ֡����frame
			cap >> frame0;
			//���δ����ͼ��
			if (frame0.empty())break;
			
		}
		frame0.copyTo(frame);
		if(!paused)
		{
			///ʹ��minshift���� **************************************************************/

			Mat hsv,result;//hsvͼ��result����ͼ
			ObjectFinder finder;
			vector<Mat> v;	//�������ͨ��������
			int ch[1]={0};	//find�������������ͳ���ĸ�ͨ��

			///��������õ���1Dֱ��ͼ������һ��
			finder.setHistogram(colorhist);
			///��frameת��Ϊhsvͨ��
			cvtColor(frame, hsv, CV_BGR2HSV);
			///�ֽ��Sͨ���������Ͷ�
			split(hsv,v);

			///ʶ��ͱ��Ͷ�����
			threshold(v[1],v[1],minSat,255,CV_THRESH_BINARY);

			///�õ�����ͼ��hsvͨ���ķ�ͶӰͼ��result
			result= finder.find(hsv,0.0f,180.0f,ch,1);

			///��ȥ�ͱ��Ͷȵ�����
			bitwise_and(result,v[1],result);

			///����������ֹ����
			TermCriteria criteria(TermCriteria::EPS,10,1);

			cout << "meanshift= " << cv::meanShift(result,trackwindow,criteria) << endl;
			
			rectangle(frame, trackwindow, Scalar(255,255,255),2,8);
			//if( trackwindow.area() <= 1 )                                                     
   //             {  
   //                 int cols = result.cols, rows = result.rows, r = (MIN(cols, rows) + 5)/6;  
   //                 trackwindow = Rect(trackwindow.x - r, trackwindow.y - r,  
   //                                    trackwindow.x + r, trackwindow.y + r) &  
   //                               Rect(0, 0, cols, rows);//Rect����Ϊ�����ƫ�ƺʹ�С������һ��������Ϊ��������Ͻǵ����꣬�����ĸ�����Ϊ����Ŀ�͸�  
   //             }  
			//RotatedRect trackBox = CamShift(result,trackwindow,criteria);
			//
			//ellipse( frame, trackBox, Scalar(255,255,255), 2, CV_AA );			
		}

		///��껭���ο�******************************************************************/
		if( selectObject && selection.width > 0 && selection.height > 0 )
        {
            Mat roi(frame, selection);
            bitwise_not(roi, roi);//bitwise_notΪ��ÿһ��bitλȡ��
        }

		///��ʾ����**********************************************************************/
		imshow("track",frame);

		///��ͣ���жϼ��������***********************************************************/
		char c = (char)waitKey(100);
		if( c == 27 ) break;          //�˳���
            
        switch(c)
        {
			case 'p':       //��ͣ���ٽ���
				paused = !paused;
				break;
			default:
            ;
        }
	}
	return 0;
}