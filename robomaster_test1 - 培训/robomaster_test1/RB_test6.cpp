/*------------------------------------------------------------------------------------------*\
   ʹ��ͼ��HSV��ɫ�ռ��ɫ��H���÷�ͶӰ����CamShift��������Ŀ������,���ѡ��������Ϊ��ʼ
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

///ȫ�ֱ���
Mat colorhist;	//�ص���������������ɫֱ��ͼ
Mat imageROI;	//�ص�����������������ͼ������ȡ��Ŀ������
int maxbin;
bool paused = false; //�ж��Ƿ�Ϊ��ͣ״̬
bool selectObject = false;//�����Ƿ��������ѡҪ���ٵĳ�ʼĿ�꣬true��ʾ���������ѡ��

Point origin;//���ڱ������ѡ���һ�ε���ʱ���λ��
Rect selection = Rect(0,0,0,0);//���ڱ������ѡ��ľ��ο�
Mat frame,frame0;//���յ���ƵͼƬ

int cnt = 0;

///��������ȡ��ɫֱ��ͼ�Ļص�����
void on_colorhist(int,void*);

///���������ص�����
void onMouse( int event, int x, int y, int, void* );


int main()
{
	///���������������Ե������Ͷ���ֵ****************************************************/
	namedWindow("track");
	setMouseCallback( "track", onMouse, 0 );//�����Ϣ��Ӧ����

	///**********************************************************************************/


	///����Ƶ**************************************************************************/
	VideoCapture cap;
	//cap.open("RM2.avi");	
	cap.open(0);
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
		clock_t t1, t2;
		
		//double t = (double)getTickCount();
		if(!paused)
		{
			//��cap�ж�һ֡����frame
			cap >> frame0;
			//���δ����ͼ��
			if (frame0.empty())break;
			
		}

		frame0.copyTo(frame);
		t1 = clock();
		if(selection.width > 0 && selection.height > 0 &&!paused &&!selectObject)
		{
			///ʹ��camshift���� **************************************************************/

			Mat hsv,result;//hsvͼ��result����ͼ
			ObjectFinder finder;
			vector<Mat> v;	//�������ͨ��������
			int ch[1]={0};	//find�������������ͳ���ĸ�ͨ��

			///��������õ���1Dֱ��ͼ������һ��
			finder.setHistogram(colorhist);
			///��frameת��Ϊhsvͨ��
			//cvtColor(frame, hsv, CV_BGR2HSV);
			BGR2HSV(frame, hsv);

			///�õ�����ͼ��hsvͨ���ķ�ͶӰͼ��result
			result= finder.find(hsv,0.0f,180.0f,ch,1,maxbin);

			///����������ֹ����
			//TermCriteria criteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 );
			/////��ʼCamShift����
			//RotatedRect trackBox = CamShift(result, selection, criteria); 
			camshift_iteration(result, selection, 10, 5);

			//std::cout << "sleft  " << selection << std::endl;
			
            ///��ֹ���ο�С��1                       
			if( selection.area() <= 1 )   //��ͨ��max_num_of_trees_in_the_forest  
                {
                    int cols = result.cols, rows = result.rows, r = (MIN(cols, rows) + 5)/6;
                    selection = Rect(selection.x - r, selection.y - r,
                                       selection.x + r, selection.y + r) &
                                  Rect(0, 0, cols, rows);//Rect����Ϊ�����ƫ�ƺʹ�С������һ��������Ϊ��������Ͻǵ����꣬�����ĸ�����Ϊ����Ŀ�͸�
                }
			///����trackBox��������Բ
			//ellipse(frame, trackBox, Scalar(255,255,255), 2, CV_AA );//���ٵ�ʱ������ԲΪ����Ŀ��
			cv::rectangle(frame, selection, cv::Scalar(0, 0, 255), 2);

			ostringstream file_name;
			file_name <<"result/"<< (++cnt) << ".jpg";
			cv::imwrite(file_name.str(), frame(selection));
		}




		///��껭���ο�******************************************************************/
		if( selectObject && selection.width > 0 && selection.height > 0 )
        {
				Mat roi(frame, selection);
				bitwise_not(roi, roi);//bitwise_notΪ��ÿһ��bitλȡ��
        }

		///��ʾ����**********************************************************************/
		imshow("track",frame);
		/*t = ((double)getTickCount() - t)/getTickFrequency();
		cout << "Times passed in seconds: " << t << endl;*/
		///��ͣ���жϼ��������***********************************************************/
		char c = (char)waitKey(1);
		if( c == 27 ) break;          //�˳���
            
        switch(c)
        {
			case 'p':       //��ͣ���ٽ���
				paused = !paused;
				break;
			default:
            ;
        }

		t2 = clock();
		std::cout << "��ǰ֡�ʣ� " << 1 / (0.0001+(double)(t2 - t1) / CLOCKS_PER_SEC) << std::endl;;
	}
	return 0;
}

///��������ȡ��ɫֱ��ͼ�Ļص�����
void on_colorhist(int,void*)
{
	//��������hc������ȡ��ɫֱ��ͼcolorhist
	ColorHistogram hc;
	//������ɫֱ��ͼcolorhist
	colorhist= hc.getHueHistogram(imageROI,maxbin);
}

///���������ص�����
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
		///��ȡ��ѡ�������뵽�ص��������õ���ʼcolorhist
		imageROI = frame0(selection);///ע�⣬ROI����ȡ��frame0����Ϊframe�����ѡ��ʱ��ɫ��ȡ����
		on_colorhist(0,0);
        break;
    }
}