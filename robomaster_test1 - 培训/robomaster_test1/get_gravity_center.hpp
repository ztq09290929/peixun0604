#include <iostream>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <xmmintrin.h>
#include <iostream>
#include <time.h>



void get_gravity_center(cv::Mat input_inverse_project, cv::Point& center_point, float& m00)
{
	assert(false == input_inverse_project.empty());
	assert(CV_8UC1 == input_inverse_project.type());

	

    float u_avg = 0;
    float v_avg = 0;

	float u_sum[4] = { 0 };
	float v_sum[4] = { 0 };
	float m00_sum[4] = { 0 };

	float m10 = 0;
	float m01 = 0;

    m00 = 0;

	__m128* u_sum_ptr = (__m128*)u_sum;
	__m128* v_sum_ptr = (__m128*)v_sum;
	__m128* m00_sum_ptr = (__m128*)m00_sum;

	input_inverse_project.convertTo(input_inverse_project, CV_32FC1);

	

	cv::Mat inverse_project(input_inverse_project.rows, std::ceil(static_cast<float>(input_inverse_project.cols) / 4) * 4, CV_32FC1, cv::Scalar(0));

	input_inverse_project.copyTo(inverse_project(cv::Rect(0, 0, input_inverse_project.cols, input_inverse_project.rows)));


    for(int i = 0;i < inverse_project.rows;i++)
    {
        __m128* ptr = (__m128*)inverse_project.ptr<float>(i);

        for(int j = 0;j < inverse_project.cols;j+=4,++ptr)
        {
            __m128 temp = _mm_mul_ps(*ptr, _mm_set_ps(j,j+1,j+2,j+3));
			*u_sum_ptr = _mm_add_ps(*u_sum_ptr, temp);

			temp = _mm_mul_ps(*ptr, _mm_set_ps1(i));
			*v_sum_ptr = _mm_add_ps(*v_sum_ptr, temp);

			*m00_sum_ptr = _mm_add_ps(*m00_sum_ptr, *ptr);
			
        }
    }

	for (int i = 0; i <4; ++i)
	{
		m10 += u_sum[i];
		m01 += v_sum[i];
		m00 += m00_sum[i];
	}

	m00 = m00 > 30 ? m00 : 30;

    //std::cout << u_sum / m00 << std::endl;

    center_point.x = static_cast<int>(m10 / m00);
    center_point.y = static_cast<int>(m01 / m00);


}



void camshift_iteration(cv::Mat invert_img, cv::Rect& bounding_box, int max_iteration_times = 10, int eps_pixel = 5)
{
    cv::Point center_point;
    float m00;
    for(int i = 0;i < max_iteration_times;i++)
    {
		clock_t t1, t2;

		t1 = clock();
        get_gravity_center(invert_img(bounding_box), center_point, m00);
		t2 = clock();

		std::cout << t2 - t1 << std::endl;
        
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


