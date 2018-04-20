#include <cstdio>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sys/time.h>
#include <unistd.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "i2c_d6t.h"

using namespace std;
using namespace cv;

int main()
{
    D6T *sensorPtr = 0;
    int address = 0x0a;
    std::string i2c_bus = "/dev/i2c-1";
    uint8_t iType = 1; // D6T-44L-01

    sensorPtr = new D6T(i2c_bus, address, iType);
    int16_t *measurements = 0;

    char value_c[256];
    cv::Mat frame, src_video;

    /* Initialize thermal sensor display point */
    int start_point_x, start_point_y;
    int end_point_x, end_point_y;

    int color_r, color_g, color_b;

    int put_text_point_x, put_text_point_y;
   
    /* Initialize overlay processing */
    double alphaValue = 0.3;
    double betaValue;
    cv::Mat dst_image;

    /* Initialize camera processing */
    cv::VideoCapture capture(0);
    if (!capture.isOpened())
    {
        return -1;
    }

    uint file_cnt = 0;
    uint shutter_cnt = 0;
    stringstream ss;
    string file_name;
    while (1)
    {
        /* Measure */
        measurements = sensorPtr->measure();

        /* Capture */
        for (int i = 0; i < 10; i++) capture >> frame;
        src_video = frame;

        float temp_cel[16];
        for (int i = 0; i < 15; i++)
        {
            temp_cel[i] = (float)measurements[i + 1] * 0.1f;
        }
        temp_cel[15] = (float)measurements[16] * 0.1f;

        /* csv output */
        //ss << "./label/" << file_cnt << ".csv";
        //file_name = ss.str();
        //ss.str("");
        //ss.clear(stringstream::goodbit);

        //ofstream log;
        //log.open(file_name, ios::trunc);
        //for (int i = 0; i < 16; i++) {
        //    // sprintf(value_c, "%.5f", temp_cel[i]);
        //    log << temp_cel[i];
        //    if (i != 15) {
        //        log << ",";
        //    }
        //}
        //log.close();

        /* generated thermal image */
        cv::Mat thermal_img = cv::Mat::zeros(800, 800, CV_8UC3);
        int loop_cnt = 0;
        for (int i = 4; i > 0; i--)
        {
            for (int j = 0; j < 4; j++)
            {
                start_point_x = j * 200;
                start_point_y = (i - 1) * 200;
                end_point_x = (j + 1) * 200;
                end_point_y = i * 200;

                // std::cout << temp_cel[cnt] << std::endl;
                color_r = 120 - (int)((temp_cel[loop_cnt] - 10.0f) / 30.0f * 255.0f);
                color_r = std::pow(color_r, 2);
                color_g = 0;
                color_b = 0;
                rectangle(thermal_img, Point(start_point_x, start_point_y), Point(end_point_x, end_point_y), Scalar(color_b, color_g, color_r), -1, CV_AA);

                sprintf(value_c, "%.2f", temp_cel[loop_cnt]);
                put_text_point_x = start_point_x;
                put_text_point_y = start_point_y + 50;
                putText(thermal_img, value_c, Point(put_text_point_x, put_text_point_y), FONT_HERSHEY_SIMPLEX, 1.5, Scalar(255, 255, 255), 2, CV_AA);

                loop_cnt++;
            }
        }

        /* camera processing */
        flip(src_video, src_video, 1);

        /* image output */
        //ss << "./image/" << file_cnt << ".png";
        //file_name = ss.str();
        //ss.str("");
        //ss.clear(stringstream::goodbit);
        //imwrite(file_name, src_video); 

        /* overlay processing */
        cvtColor(src_video, src_video, CV_RGBA2RGB);
        resize(src_video, src_video, Size(), 800.0/src_video.cols, 800.0/src_video.rows);
        cvtColor(thermal_img, thermal_img, CV_RGBA2RGB);
        //resize(thermal_img, thermal_img, cv::Size(), 1, 0.75);

        //cout << src_video.size() << endl;
        //cout << thermal_img.size() << endl;

        betaValue = (1.0 - alphaValue);
        addWeighted(thermal_img, alphaValue, src_video, betaValue, 0, dst_image);

        /* overlay image output */
        //ss << "./overlay/" << file_cnt << ".png";
        //file_name = ss.str();
        //ss.str("");
        //ss.clear(stringstream::goodbit);

        //imwrite(file_name, dst_image); 

        /* display */
        namedWindow("drawing", CV_WINDOW_AUTOSIZE);
        imshow("drawing", dst_image);
        waitKey(1);

	/* for overlay image output */
        //if (shutter_cnt == 2) {
        //    sleep(10);
        //    shutter_cnt = 0;
        //} else {
        //    shutter_cnt++;
        //}

        //file_cnt++;
        //if (file_cnt == 3000) break;
    }
    return 0;
}
