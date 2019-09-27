#include <iostream>
#include <math.h>
#include <opencv2/opencv.hpp>

void MyHoughCircles(cv::Mat &src, cv::Mat &dst, int minradius, int maxradius, int threshold);

int main(int argc, char **argv)
{
  if(argc != 5)
  {
    std::cerr << "less args" << std::endl;
    exit(-1);
  }
  
  cv::Mat image = cv::imread(argv[1], cv::IMREAD_COLOR);
  cv::Mat img_gs = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);

  if( !image.data )
  {
    std::cerr<< "error" << std::endl;
    exit(-1);
  }
  
  int minradius = atoi(argv[2]);
  int maxradius = atoi(argv[3]);
  int threshold = atoi(argv[4]);
  
  cv::Mat blurred;
  cv::Mat cannyed;
  cv::Mat dst;
  std::cout << "Gauss" << std::endl;
  cv::GaussianBlur(img_gs, blurred, cv::Size(5,5), 4);
  std::cout << "Canny" << std::endl;
  cv::Canny(blurred, cannyed, 100, 200, 3);
  
  
  cv::imshow("Orig", image);
  cv::imshow("canny", cannyed);
  cv::imshow("blurr", blurred);
  
  //cv::cvtColor(image, dst, cv::COLOR_GRAY2BGR);
  image.copyTo(dst);
  std::cout << "Hough with min R:" << minradius;
  std:: cout << ", max R: " << maxradius;
  std::cout << ", threshold value: " << threshold << std::endl;

  MyHoughCircles(cannyed, dst, minradius, maxradius, threshold);
  std::cout << "Fin" << std::endl;
  cv::imshow("DST", dst);
  
  cv::waitKey(0);
  return 0;
}


void MyHoughCircles(cv::Mat &src, cv::Mat &dst, int minradius, int maxradius, int threshold)
{
  int size[] = {src.rows, src.cols, (maxradius-minradius)+1};
  
  cv::Mat acc(3, size, cv::IMREAD_GRAYSCALE, cv::Scalar(0));

  std::cout << "applying hough" << std::endl;

  //#pragma omp for
  for(int i = 0; i < src.rows; i++)
  {
    for(int j = 0; j < src.cols; j++)
    {
      if(src.at<uchar>(i, j) >= 250)
      {
        for(int r = minradius; r < maxradius; r++)
        {
          for(int t = 0; t < 360; t++)
          {
            int a = round( i - r*(cos(t * M_PI/180) ));
            int b = round( j - r*(sin(t * M_PI/180) ));
            if( a >= 0 && b >= 0 && a < src.cols && b < src.rows )
              acc.at<uchar>(a, b, r-minradius)++;
          }
        }
      }
    }
  }

  std::cout << acc.size() << "Size acc" << std::endl;
  
  std::vector<cv::Vec3f> cerchi;
  std::cout << "Pushing in vector" << std::endl;
  
  //#pragma omp for
  
  for(int x = 0; x < src.rows; x++)
  {
    for(int y = 0; y < src.cols; y++)
    {
      for(int r = 0; r < maxradius-minradius; r++)
      {
        if(acc.at<uchar>(x, y, r) > threshold)
        {
          cv::Vec3f c(x, y, r+minradius);
          cerchi.push_back(c);
        }
      }
    }
  }
  
  std::cout << "Circles: " << cerchi.size() << std::endl;
  
  //#pragma omp for
  for(int i = 0; i < cerchi.size(); i++)
  {
    cv::Point c(cvRound(cerchi[i][1]), cvRound(cerchi[i][0]));
    int r = cvRound(cerchi[i][2]);
    cv::circle(dst, c, r, cv::Scalar(0, 0, 255), 2);
  }
  return;
}
