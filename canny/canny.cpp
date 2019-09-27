#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void suppression(Mat src, Mat thetas, Mat suppressed);
void histeresis(Mat dst, int ht, int lt);

int main(int argc, char** argv )
{
  if ( argc != 6 )
  {
    cout << "usage: opencv <Image_Path> <blur mask size> <sigma> <high> <low>" << endl;
    return -1;
  }
  Mat image;
  image = imread( argv[1], IMREAD_GRAYSCALE);
  // opening as greyscale image

  if ( !image.data )
  {
    cout << "No image data" << endl;
    return -1;
  }

  imshow("Lenna", image);
  
  int size = atoi(argv[2]);
  float sigma = atof(argv[3]);
  int canny_lt = atoi(argv[4]);
  int canny_ht = atoi(argv[5]);
  
  cout << "Using kernel size and sigma -> " << size << " " << sigma << endl;
  Mat blurred;
  cv::GaussianBlur(image, blurred, cv::Size(size,size), sigma);

  Mat gx, gy;

  cv::Sobel(blurred, gx, CV_32F, 1, 0);
  cv::Sobel(blurred, gy, CV_32F, 0, 1);

  Mat mag, mag_8U; 
  cv::magnitude(gx, gy, mag);
  mag.convertTo(mag_8U, CV_8U);
  
  Mat thetas;
  cv::phase(gx, gy, thetas, true);
   
  Mat suppressed;
  mag_8U.copyTo(suppressed);
  suppression(mag_8U, thetas, suppressed); 

  Mat thresholded(suppressed.size(), CV_8U);
  suppressed.copyTo(thresholded);

  histeresis(thresholded, canny_ht, canny_lt);
  imshow("My Canny", thresholded);

  Mat cvcanny(image.size(), CV_8U);

  //true so calculate magnitudo with squares
  cv::Canny(blurred, cvcanny, canny_lt, canny_ht, 3, true);
  imshow("OpenCV Canny", cvcanny);

  waitKey(0);
  return 0;
}

void histeresis(Mat dst, int ht, int lt)
{
  vector<Point> strong_edges;
  for(auto i = 0; i < dst.rows; i++)
  {
    for(auto j = 0; j < dst.cols; j++)
    {
      if(dst.at<uchar>(i,j) > ht)
      {
        dst.at<uchar>(i,j) = 255;
        strong_edges.push_back(Point(i,j));
      }
      else
        if(dst.at<uchar>(i,j) < lt)
          dst.at<uchar>(i,j) = 0;
    }
  }

  while(strong_edges.size())
  {
    Point edge = strong_edges.front();
    for(auto i = -1; i <= 1; i++)
    {
      for(auto j = -1; j <= 1; j++)
      {
        Point point(i+edge.x, j+edge.y);
        if( point.x >= 0 && point.y >= 0 && point.x < dst.rows && point.y < dst.cols )
        {
          if( dst.at<uchar>(point.x, point.y) != 0 && dst.at<uchar>(point.x, point.y) != 255 )
          {
            dst.at<uchar>(point.x, point.y) = 255;
            strong_edges.push_back(point);
          }
        }
      }
    }
    strong_edges.pop_back();
  }

  for(auto it = dst.begin<uchar>(); it < dst.end<uchar>(); it++)
    if(*it != 0 && *it != 255)
      *it=0;

}

void suppression(Mat src, Mat thetas, Mat suppressed)
{
  cout << "suppressing" << endl;
  vector<int> vec;
  for(auto i=0;i<src.rows;i++)
  { 
    for(auto j=0;j<src.cols;j++)
    {
      int edges = saturate_cast<int>(thetas.at<float>(i,j)/45.0)%4;
      int prev = 0, next = 0, magn = 0;
      
      switch(edges)
      {
        case 0:
          if(j-1 >=0)
            prev = src.at<uchar>(i,j-1);
          if(j+1 < src.cols)
            next = src.at<uchar>(i,j+1);
        break;
        case 1:
          if(i-1>=0 && j-1 >= 0)
            prev = src.at<uchar>(i-1,j-1);
          if(i+1 < src.rows && j+1 < src.cols)
            next = src.at<uchar>(i+1,j+1);
        break;
        case 2:
          if(i-1 >= 0)
           prev = src.at<uchar>(i-1,j);
          if(i+1 < src.cols)
           next = src.at<uchar>(i+1,j);
        break;
        case 3:
          if(i-1 >= 0 && j+1 < src.cols)
            prev = src.at<uchar>(i-1,j+1);
          if(i+1 < src.rows && j-1 >= 0)
            next = src.at<uchar>(i+1, j-1);
        break;
      }

      magn = src.at<uchar>(i,j);
      
      if(prev > magn || next > magn)
       suppressed.at<uchar>(i,j)=0;
    }
  }
  cout << "suppressed" << endl;
}
