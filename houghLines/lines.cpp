#include <iostream>
#include <opencv2/opencv.hpp>

cv::Mat houghLines(cv::Mat source);
void findLinePolar(float rho, double theta, cv::Point &p1, cv::Point &p2);


int main(int argc, char **argv)
{
  if(argc < 3)
  {
    std::cerr << "exe, path img, threshold 1" << std::endl;
    exit(EXIT_FAILURE);
  }
  cv::Mat src = cv::imread(argv[1], 0);
  if(src.empty())
  {
    std::cerr << "Image not valid" << std::endl;
    exit(EXIT_FAILURE);
  }
  cv::imshow("Source", src);
  
  cv::Mat gx, gy, blur, canny;
  //getting edges with canny
  cv::GaussianBlur(src, blur, cv::Size(5, 5), 2.4);
  cv::Canny(blur, canny, 20, 100);

  cv::imshow("Canny", canny);
  cv::Mat votes = houghLines(canny);

  cv::Mat hl = src.clone();
  cv::cvtColor(hl, hl, cv::COLOR_GRAY2BGR);

  //draw lines using votes matrix
  for(int r = 0; r < votes.rows; r++)
  {
    for(int t = 0; t < 180; t++)
    {
      if(votes.at<int>(r, t) > atoi(argv[2]))
      {
        cv::Point p1, p2;
        findLinePolar(r-(sqrt(pow(src.rows, 2) * pow(src.cols, 2))), ((t-90)*CV_PI)/180, p1, p2);
        cv::line(hl, p1, p2, cv::Scalar(0, 0, 200));
      }
    }
  }

  cv::imshow("Hough per lines", hl);

  cv::waitKey(0);
  return 0;
}

cv::Mat houghLines(cv::Mat source)
{
  int diagonal = sqrt(pow(source.rows, 2) * pow(source.cols, 2));
  //using a matrix with rows enough big
  cv::Mat votes = cv::Mat::zeros(diagonal*2, 180, CV_32S);

  //start by voting
  for(int x = 0; x < source.rows; x++)
  {
    for(int y = 0; y < source.cols; y++)
    {
      if( source.at<uchar>(x,y) != 0)
      {
        for(int t = 0; t < 180; t++)
        {
          double theta = (t-90) * CV_PI/180;
          double rho = cvRound( y * cos(theta) + x * sin(theta));
          rho += diagonal;
          votes.at<int>(rho, t)++;
        }
      }
    }
  }
  return votes;
}


void findLinePolar(float rho, double theta, cv::Point &p1, cv::Point &p2)
{
  //go back to cartesian coords
  p1.x = cvRound(cos(theta)*rho + 1000*(-sin(theta)));
  p1.y = cvRound(sin(theta)*rho + 1000*(cos(theta)));
  p2.x = cvRound(cos(theta)*rho - 1000*(-sin(theta)));
  p2.y = cvRound(sin(theta)*rho - 1000*(cos(theta)));
}
