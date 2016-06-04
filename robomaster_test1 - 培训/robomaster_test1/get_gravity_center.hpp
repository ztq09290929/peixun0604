#include <iostream>
#include <math.h>
#include <opencv2/opencv.hpp>



void get_gravity_center(cv::Mat inverse_project, cv::Point& center_point, float& m00)
{
    assert(false == inverse_project.empty());
    assert(CV_8UC1 == inverse_project.type());

	

    float u_avg = 0;
    float v_avg = 0;

    float u_sum = 0;
    float v_sum = 0;

    m00 = 0;

    for(int i = 0;i < inverse_project.rows;i++)
    {
        uchar* ptr = inverse_project.ptr<uchar>(i);
        for(int j = 0;j < inverse_project.cols;j++)
        {
            u_sum += ptr[j] * j;
            v_sum += ptr[j] * i;
            m00 += ptr[j];
        }
    }

	m00 = m00 > 30 ? m00 : 30;

    //std::cout << u_sum / m00 << std::endl;

    center_point.x = static_cast<int>(u_sum / m00);
    center_point.y = static_cast<int>(v_sum / m00);


}



void camshift_iteration(cv::Mat invert_img, cv::Rect& bounding_box, int max_iteration_times = 10, int eps_pixel = 5)
{
    cv::Point center_point;
    float m00;
    for(int i = 0;i < max_iteration_times;i++)
    {
        get_gravity_center(invert_img(bounding_box), center_point, m00);
        
        float disx = bounding_box.width / 2 - center_point.x;
        float disy = bounding_box.height / 2 - center_point.y;

        if(std::sqrt(disx * disx + disy * disy) < eps_pixel)
        {
            break;
        }

		bounding_box.width = std::max(static_cast<int>(2 * sqrt(m00 / 256)),50 );
		bounding_box.height = bounding_box.width * 1.2;

		bounding_box.x += center_point.x - bounding_box.width / 2;
        bounding_box.y += center_point.y - bounding_box.height / 2;


		bounding_box = bounding_box & cv::Rect(0, 0, invert_img.cols, invert_img.rows);
    }
}


