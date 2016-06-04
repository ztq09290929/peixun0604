/*------------------------------------------------------------------------------------------*\
   ʹ��ͼ��HSV��ɫ�ռ��ɫ��H���÷�ͶӰ����meanShift��������Ŀ������,�������ѡ�����壬��ʱ�����Ŀ��λ��
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

///ȫ�ֱ���
int minSat=115;	//�ص����������������Ͷ���ֵ
Mat colorhist;	//�ص���������������ɫֱ��ͼ
Mat imageROI;	//�ص�����������������ͼ������ȡ��Ŀ������

bool paused = false; //�ж��Ƿ�Ϊ��ͣ״̬
bool selectObject = false;//�����Ƿ��������ѡҪ���ٵĳ�ʼĿ�꣬true��ʾ���������ѡ��

Point origin;//���ڱ������ѡ���һ�ε���ʱ���λ��
Rect selection = Rect(0,0,0,0);//���ڱ������ѡ��ľ��ο�
Mat frame,frame0;//���յ���ƵͼƬ



///��������ȡ��ɫֱ��ͼ�Ļص�����
void on_colorhist(int,void*);

///���������ص�����
void onMouse( int event, int x, int y, int, void* );


int main()
{
	///���������������Ե������Ͷ���ֵ****************************************************/
	namedWindow("track");
	setMouseCallback( "track", onMouse, 0 );//�����Ϣ��Ӧ����
	createTrackbar( "minSat", "track", &minSat, 255, on_colorhist );//���һ������Ϊ0����û�е��û����϶�����Ӧ����

	///**********************************************************************************/


	///����Ƶ**************************************************************************/
	VideoCapture cap;
	//cap.open("RM1.avi");	
	cap.open(1);
	//����Ƿ��
	if (!cap.isOpened())
	{
		cout << "AVI NO OPEN" << endl;
		return -1;
	}
	///***********************************************************************************/


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

		if(selection.width > 0 && selection.height > 0 &&!paused &&!selectObject)
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
			TermCriteria criteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 3, 1 );
			///��ʼCamShift����
			//cv::meanShift(result,selection,criteria);
			
			rectangle(frame, selection, Scalar(255,255,255),2,8);
			///���trackBox��������
			cout<<"location = "<<"("<<selection.x+15<<","<<selection.y+15<<")"<<endl;
		}

		///������ʱ����ɫԲ��******************************************************************/
		if( selectObject )
        {
				circle(frame,Point(origin.x,origin.y),10,Scalar(0,0,255),2,8);
        }

		///��ʾ����**********************************************************************/
		imshow("track",frame);
		
		///��ͣ���жϼ��������***********************************************************/
		char c = (char)waitKey(33);
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

///��������ȡ��ɫֱ��ͼ�Ļص�����
void on_colorhist(int,void*)
{
	//��������hc������ȡ��ɫֱ��ͼcolorhist
	ColorHistogram hc;
	//������ɫֱ��ͼcolorhist
	colorhist= hc.getHueHistogram(imageROI,minSat);
}

///���������ص�����
void onMouse( int event, int x, int y, int, void* )
{
    if( selectObject )//ֻ�е�����������ȥʱ����Ч��Ȼ��ͨ��if�������Ϳ���ȷ����ѡ��ľ�������selection��
    {
		///��ʼĿ���Ϊ30*30��������
        selection.x = origin.x - 15;//�������ϽǶ�������
        selection.y = origin.y - 15;
        selection.width = 30;//���ο�
        selection.height = 30;//���θ�

        selection &= Rect(0, 0, frame.cols, frame.rows);//����ȷ����ѡ�ľ���������ͼƬ��Χ��
    }

    switch( event )
    {
    case CV_EVENT_LBUTTONDOWN:
        origin = Point(x,y);
        selectObject = true;
        break;
    case CV_EVENT_LBUTTONUP:
        selectObject = false;
		///��ȡ��ѡ�������뵽�ص��������õ���ʼcolorhist
		imageROI = frame0(selection);///ע�⣬ROI����ȡ��frame0����Ϊframe�����ѡ��ʱ��ɫ��ȡ����
		on_colorhist(0,0);
		///������ʼĿ���
		rectangle(frame0, selection, Scalar(255,255,255),2,8);
        break;
    }
}