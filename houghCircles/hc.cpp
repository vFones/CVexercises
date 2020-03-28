#include <iostream>
#include <opencv2/opencv.hpp>

cv::Mat houghCircle(cv::Mat &canny, cv::Mat &src, int minrad, int maxrad, int threshold);


int main(int argc, char **argv)
{
  if(argc < 5)
  {
    std::cerr << "exe, path, min radius, max one, votes threshold" << std::endl;
    exit(EXIT_FAILURE);
  }
  cv::Mat src = cv::imread(argv[1], cv::IMREAD_COLOR);
  if(src.empty())
  {
    exit(EXIT_FAILURE);
  }
  cv::imshow("SRC", src);
  cv::cvtColor(src, src, cv::COLOR_BGR2GRAY);
  
  //making a gaussian blur before canny
  cv::Mat blur;
  cv::GaussianBlur(src, blur, cv::Size(5,5), 2.4);
  cv::Mat canny;
  cv::Canny(blur, canny, 100, 150);
  cv::imshow("Canny", canny);

  int minradius = atoi(argv[2]);
  int maxradius = atoi(argv[3]);
  int threshold = atoi(argv[4]);
  
  std::cout << "threshold " << threshold << std::endl;

  cv::Mat hough = houghCircle(canny, src, minradius, maxradius, threshold);
  
  cv::imshow("Circles", hough);
  
  cv::waitKey(0);

  return 0;
}


cv::Mat houghCircle(cv::Mat &canny, cv::Mat &src, int minrad, int maxrad, int threshold)
{
  //creating a 3D matrix with size of A and B (polar coords) and the Radius of circles
  int size[] = {src.rows, src.cols, (maxrad-minrad)+1};
  cv::Mat acc(3, size, cv::IMREAD_GRAYSCALE, cv::Scalar(0));

  cv::Mat hough = src.clone();

  cv::cvtColor(hough, hough,cv::COLOR_GRAY2BGR);


  //start voting
  for(int i = 0; i < src.rows; i++)
  {
    for(int j = 0; j < src.cols; j++)
    {
      if(canny.at<uchar>(i,j) != 0)
      {
        for(int r = minrad; r < maxrad; r++)
        {
          for(int t = 0; t < 360; t++)
          {
            int a = cvRound(i - r * (cos(t * CV_PI/180)));
            int b = cvRound(j - r * (sin(t * CV_PI/180)));
            if(a >= 0 && a < src.rows && b >= 0 && b < src.cols)
              acc.at<uchar>(a, b, r-minrad)++;
          }
        }
      }
    }
  }

  std::cerr << "Accumulator created" << std::endl;

  //if any of the votes are enough big than write down to hough matrix and return
  for(int x = 0; x < src.rows; x++)
  {
    for(int y = 0; y < src.cols; y++)
    {
      for(int r = 0; r < maxrad-minrad; r++)
      {
        if(acc.at<uchar>(x, y, r) > threshold)
          cv::circle(hough, cv::Point(y, x), r + minrad, cv::Scalar(0, 0, 255), 1);
      }
    }
  }

  std::cout << "fin." << std::endl;
  
  return hough;
}
