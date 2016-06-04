#if !defined OFINDER
#define OFINDER

#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include "back_project.hpp"

using namespace cv;

class ObjectFinder {

  private:

	float hranges[2];
    const float* ranges[1];
    int channels[3];

	MatND histogram;

  public:

	ObjectFinder()  {

		ranges[0]= hranges; // all channels have the same range 
	}
   

	///�������ֱ��ͼ��һ��
	void setHistogram(const cv::Mat& h) {

		histogram= h;
		//normalize(histogram,histogram,1.0);
	}

	///��calcBackProject�����õ�����ͼ������ֵ�����õ����յĶ�ֵͼ��
	///��ɫ��Χ�����룬����֧��������ɫ�ռ�ļ��㣬���ҿ��Լ�ɫ����
	Mat find(cv::Mat& image, float minValue, float maxValue, int *channels, int dim,int _maxbin) {

		Mat result;

		//hranges[0]= minValue;
		//hranges[1]= maxValue;

		//for (int i=0; i<dim; i++)
		//	this->channels[i]= channels[i];

		//calcBackProject(&image,
  //                    1,            // we only use one image at a time
  //                    channels,     // vector specifying what histogram dimensions belong to what image channels
  //                    histogram,    // the histogram we are using
  //                    result,       // the resulting back projection image
  //                    ranges,       // the range of values, for each dimension
  //                    255.0 );        // the scaling factor is chosen such that a histogram value of 1 maps to 255

		back_project(image, histogram, _maxbin, result);
		
		return result;
	}
};
#endif
