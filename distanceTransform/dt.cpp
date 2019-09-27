#include <iostream>
#include <math.h>
#include <opencv2/opencv.hpp>

void distanceTransform(cv::Mat src, cv::Mat &dst);

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        std::cerr << "fews args" << std::endl;
        exit(-1);
    }
    cv::Mat raw_img = cv::imread(argv[1], cv::IMREAD_COLOR);
    if(!raw_img.data)
    {
        std::cerr << "error image" << std::endl;
        exit(-2);
    }
    cv::imshow("Raw", raw_img);

    cv::Mat img_bin;
    cv::cvtColor(raw_img, img_bin, cv::COLOR_BGRA2GRAY);
    //cv::GaussianBlur(img_bin, img_bin, cv::Size(5,5), 1.4);
    cv::threshold(img_bin, img_bin, 127, 255, cv::THRESH_BINARY);
    cv::imshow("Binarized", img_bin);

    cv::Mat dt;// (raw_img.size(), CV_8U);
    distanceTransform(img_bin, dt);
    cv::imshow("Distance Transformed", dt);

    cv::waitKey(0);
    return 0;
}

void distanceTransform(cv::Mat src, cv::Mat &dst)
{
    for(int i = 1; i < src.rows; i++)
    {
        for(int j = 1; j < src.cols; j++)
        {
            if(src.at<uchar>(i, j) != 0)
            {
                int N = src.at<uchar>(i-1,j);
                int W = src.at<uchar>(i,j-1);

                src.at<uchar>(i,j) = 1 + std::min(N,W);
            }
        }
    }

    for(int i = src.rows; i >=0; i--)
    {
        for(int j = src.cols; j >=0; j--)
        {
            if(src.at<uchar>(i, j) != 0)
            {
                int S = src.at<uchar>(i+1,j);
                int E = src.at<uchar>(i,j+1);

                src.at<uchar>(i,j) = 1 + std::min( std::min(S,E), (int)src.at<uchar>(i,j));
            }
        }
    }
    cv::normalize(src, dst, 0, 255, cv::NORM_MINMAX);

    return;
}