#include <iostream>
#include <vector>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <time.h>
using namespace std;
using namespace cv;

Mat src, dst;
Mat gx, gy;
float thr;
int cornersize;

typedef struct supprPoint{
  Point p;
  float val;
} SupprPoint;

typedef pair<Point, float> PointValue;

bool mycmp ( PointValue i, PointValue j ){ return ( i.second > j.second ); }

void cornerHarris_demo( int, void* );
void myHarris();

int main(int argc, char** argv )
{
  srand(time(NULL));
  if ( argc != 4 )
  {
    cout << "usage: opencv <Image_Path> <threshold>" << endl;
    return -1;
  }
  src = imread( argv[1], IMREAD_GRAYSCALE);
  // opening as greyscale image

  if ( !src.data )
  {
    cout << "No image data" << endl;
    return -1;
  }
  thr = atof(argv[2]);
  cornersize= atoi(argv[3]);
  GaussianBlur(src, src, cv::Size(5, 5), 2.4);
  cvtColor(src, dst, COLOR_GRAY2BGR);

  imshow( "Harris", src);

  Sobel(src, gx, CV_32F, 1, 0, 3);
  Sobel(src, gy, CV_32F, 0, 1, 3);
  cout << "apply sobel" << endl;
  myHarris();

  waitKey(0);
  return 0;
}


void myHarris()
{
  Mat gx2(gx.size(), gx.type());
  Mat gy2(gy.size(), gy.type());
  Mat gxy(gx.size(), gx.type());
  
  cout << "Calculating gx2, gy2, gxy" << endl;
  for(auto i = 0; i < src.rows; i++)
  {
    for(auto j = 0; j < src.cols; j++)
    {
      gx2.at<float>(i,j) = gx.at<float>(i,j) * gx.at<float>(i,j);
      gy2.at<float>(i,j) = gy.at<float>(i,j) * gy.at<float>(i,j);
      gxy.at<float>(i,j) = gx.at<float>(i,j) * gy.at<float>(i,j);
    }
  }
  
  GaussianBlur(gx2, gx2, cv::Size(5, 5), 2.4);
  GaussianBlur(gy2, gy2, cv::Size(5, 5), 2.4);
  GaussianBlur(gxy, gxy, cv::Size(5, 5), 2.4);
  cout << "Smoothed" << endl;

  Mat R(src.size(), CV_32F);
  float k = 0.04;
  
  float min = numeric_limits<float>::max();
  float max = numeric_limits<float>::min();

  for(auto i = 0; i < src.rows; i++)
  {
    for(auto j = 0; j < src.cols; j++)
    {
      SupprPoint supp;
      float detC = (gx2.at<float>(i,j) * gy2.at<float>(i,j) ) - (gxy.at<float>(i,j) * gxy.at<float>(i,j));
      float trace = (gx2.at<float>(i,j) + gy2.at<float>(i,j));
      R.at<float>(i,j) = detC - k * (trace * trace);
      
      if(R.at<float>(i,j) < min)
        min = R.at<float>(i,j);
      if(R.at<float>(i,j) > max)
        max = R.at<float>(i,j);
    }
  }
  cout << "calculated R mat" << endl;

  vector<PointValue> l;
  for(auto i=0; i < src.rows; i++)
    for(auto j=0; j<src.cols; j++)
    { 
      PointValue tosuppress;
      R.at<float>(i,j) = (R.at<float>(i,j) - min)/(max-min);
      if(R.at<float>(i,j) > thr)
      {
        tosuppress = make_pair(Point(i,j), R.at<float>(i,j));
        l.push_back(tosuppress);
      }
    }

  sort(l.begin(), l.end(), mycmp);
  reverse(l.begin(), l.end());
  
  cout << "suppressing vect: " << l.size() << endl;
  
  for(auto i = 0; i < l.size(); i++)
  {
    for(auto j = i+1; j < l.size(); j++)
    {
      int ix = l.at(i).first.x;
      int iy = l.at(i).first.y;
      int jx = l.at(j).first.x;
      int jy = l.at(j).first.y;
      if(abs(ix - jx) <= cornersize && abs(iy - jy) <= cornersize)
      {
        l.erase(l.begin()+j);
        j--;
      }
    }
  }
  cout << "suppressed" << endl;

  cout << "Drawing circles" << endl;

  for(auto i = 0; i < l.size(); i++)
  {
    circle(dst, Point(l.at(i).first.y, l.at(i).first.x), 8, Scalar(0, 0, 220), 1);
  }
  cout << "fin" << endl;

  imshow("My harris", dst);
}

