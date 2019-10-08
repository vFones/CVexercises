#include <iostream>
#include <opencv2/opencv.hpp>

cv::Mat distanceTransform(cv::Mat &src);

int main(int argc, char **argv)
{
  if(argc < 2)
  {
    std::cerr << "image path not found" << std::endl;
    exit(EXIT_FAILURE);
  }
  cv::Mat src = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);
  if(src.empty())
  {
    std::cerr << "Image not valid" << std::endl;
    exit(EXIT_FAILURE);
  }
  cv::imshow("Source", src);

  cv::Mat dst = distanceTransform(src);
  cv::imshow("Distance Transform", dst);
  cv::waitKey(0);

  return 0;
}


cv::Mat distanceTransform(cv::Mat &src)
{
  cv::Mat dt = src.clone();
  //threshold for binary conversion
  cv::threshold(dt, dt, 127, 255, cv::THRESH_BINARY_INV);
  
  /*
  for(auto i=0;i<3;i++)
    cv::dilate(dt, dt,  cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(11,11)));
  
  for(auto i=0; i<3;i++)
    cv::erode(dt, dt,  cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(11,11)));
  */


  //start from top and check up and left pixels
  for(auto i=0; i<dt.rows; i++)
  {
    for(auto j=0; j<dt.cols; j++)
    {
      int U, L;
      U=L=0;
      if(i-1 >= 0)
        U = dt.at<uchar>(i-1, j);
      if(j-1 >= 0)
        L = dt.at<uchar>(i, j-1);
      
      //choose minimum and apply to result matrix, incrementing by one
      dt.at<uchar>(i,j) = 1 + cv::min(cv::min(U, L), (int)dt.at<uchar>(i,j));
    }
  }

  //do the same but start from bottom
  for(auto i=dt.rows; i>=0; i--)
  {
    for(auto j=dt.cols; j>=0; j--)
    {
      int D, R;
      D=R=0;

      if(i+1 < dt.rows)
        D = dt.at<uchar>(i+1,j);
      if(j+1 < dt.cols)
        R = dt.at<uchar>(i, j+1);
      
      dt.at<uchar>(i,j) = 1 + cv::min(cv::min(D,R), (int)dt.at<uchar>(i,j));
    }
  }
  
  //now convert to a gray scale image and return
  cv::normalize(dt, dt, 0, 255, cv::NORM_MINMAX);

  return dt;
}