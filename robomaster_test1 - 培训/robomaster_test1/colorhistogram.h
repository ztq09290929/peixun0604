#if !defined COLHISTOGRAM
#define COLHISTOGRAM

#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include "BGR2HSV.hpp"
#include "GetHist.hpp"

using namespace cv;
using namespace std;

class ColorHistogram {

  private:

 //   int histSize[1];
	//float hranges[2];
 //   const float* ranges[1];
 //   int channels[1];

  public:

	ColorHistogram() {

		///构造函数的初始化
		//histSize[0]= 181;
		//hranges[0]= 0.0;    // BRG range
		//hranges[1]= 180.0;
		//ranges[0]= hranges; // all channels have the same range 
		//channels[0]= 0;		// the three channels 
	}

/*************************************************************************************/
	/// BGR转换至HSV，并计算h通道的1维直方图,带饱和度阈值
	Mat getHueHistogram(const Mat &image,int & _maxbin) {

		cv::Mat hist;

		///转换HSV色彩空间
		Mat hsv;
		//cvtColor(image, hsv, CV_BGR2HSV);
		BGR2HSV(image, hsv);

		GetHist(hsv, hist, 181, _maxbin);
		/////使用掩码
		//Mat mask1(image.rows,image.cols,CV_8UC1,cv::Scalar(255));

		//// Prepare arguments for a 1D hue histogram
		//hranges[0]= 0.0;
		//hranges[1]= 180.0;
		//channels[0]= 0; // the hue channel 
		//histSize[0]=181;
		//// Compute histogram
		//calcHist(&hsv, 
		//	1,			// histogram of 1 image only
		//	channels,	// the channel used
		//	mask1,	///使用掩码，去除低饱和度像素
		//	hist,		// the resulting histogram
		//	1,			// it is a 1D histogram
		//	histSize,	// number of bins
		//	ranges		// pixel value range
		//);

		return hist;
	}
/****************************************************************************************/
};
#endif
