#include <stdio.h>
#include <algorithm>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Mat src, dst;
Mat gx, gy;

typedef struct supprPoint{
  Point p;
  float val;
} SupprPoint;

bool mycmp ( SupprPoint i, SupprPoint j ){ return ( i.val > j.val ); }

void cornerHarris_demo( int, void* );
void myHarris();

int main(int argc, char** argv )
{
  if ( argc != 2 )
  {
    cout << "usage: opencv <Image_Path> <blur mask size> <sigma> <high> <low>" << endl;
    return -1;
  }
  src = imread( argv[1], IMREAD_GRAYSCALE);
  // opening as greyscale image

  if ( !src.data )
  {
    cout << "No image data" << endl;
    return -1;
  }
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
  
  vector<SupprPoint> l;
  for(auto i = 0; i < src.rows; i++)
  {
    for(auto j = 0; j < src.cols; j++)
    {
      SupprPoint supp;
      float detC = (gx2.at<float>(i,j) * gy2.at<float>(i,j) ) - (gxy.at<float>(i,j) * gxy.at<float>(i,j));
      float trace = (gx2.at<float>(i,j) + gy2.at<float>(i,j));
      R.at<float>(i,j) = detC - k * (trace * trace);
      if(R.at<float>(i,j) > 0.9)
      {
        supp.p = Point(i,j);
        supp.val = R.at<float>(i,j);
        l.push_back(supp);
      }
    }
  }
  cout << "calculated R mat" << endl;


  cout << "suppressing" << endl;
  sort(l.begin(), l.end(), mycmp);
  reverse(l.begin(), l.end());
  while(l.size())
  {
    SupprPoint tocheck = l.back();
    vector<SupprPoint> tempvect;
    
    for(auto i = -1; i <= 1; i++)
    {
      for(auto j = -1; j <= 1; j++)
      {
        SupprPoint supp;
        if(tocheck.p.x+i > 0 && tocheck.p.y+j > 0 && tocheck.p.x+i < src.rows && tocheck.p.y+j < src.cols)
        {
          supp.p = Point(tocheck.p.x+i,tocheck.p.y+j);
          supp.val = R.at<float>(tocheck.p.x+i, tocheck.p.y+j);
          tempvect.push_back(supp);
        }
      }
    }
    auto max = max_element(tempvect.begin(), tempvect.end(), mycmp);
    for(auto i = 0; i<tempvect.size(); i++)
    { 
      auto tmp = tempvect.at(i);
      if(max->val != tmp.val)
        R.at<float>(tmp.p.x, tmp.p.y) = 0;
    }
    tempvect.clear();
    l.pop_back();
  }
  cout << "suppressed" << endl;


  float t = 0.9;
  for(auto i = 0; i < src.rows; i++)
  {
    for(auto j = 0; j < src.cols; j++)
    {
      //if(R.at<float>(i,j) > t)
        circle(dst, Point(j, i), 3, Scalar(0, 0, 255), 1, 4, 0);
    }
  }
  cout << "showing circles" << endl;

  imshow("My harris", dst);
}

