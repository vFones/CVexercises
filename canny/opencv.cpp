#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void suppression(Mat src, Mat suppressed);

int main(int argc, char** argv )
{
  if ( argc != 4 )
  {
    printf("usage: opencv <Image_Path>\n");
    return -1;
  }
  Mat image;
  image = imread( argv[1], IMREAD_GRAYSCALE);
  // opening as greyscale image

  if ( !image.data )
  {
    printf("No image data \n");
    return -1;
  }

  imshow("Lenna", image);
  
  int size = atoi(argv[2]);
  float sigma = atof(argv[3]);

  Mat blurred;
  cv::GaussianBlur(image, blurred, cv::Size(size,size), sigma);

  Mat gx, gy;

  cv::Sobel(blurred, gx, CV_32F, 1, 0);
  cv::Sobel(blurred, gy, CV_32F, 0, 1);

  Mat mag, mag_8U; 
  cv::magnitude(gx, gy, mag);
  mag.convertTo(mag_8U, CV_8U);
  imshow("Magnitudo", mag_8U);
  
  Mat thetas;
  cv::phase(gx, gy, thetas, true);
  

  
  waitKey(0);
  return 0;
}


void suppression(Mat src, Mat suppressed)
{
  vector<int> vec;
  for(auto i=0;i<src.rows;i++)
  { 
    for(auto j=0;j<src.cols;j++)
    {
      auto vMax = 0;
      for(auto k=0; k<3;k++)
      {
        for(auto l=0; l<3; l++)
        {
          if( !(i+k-1<0 || j+l-1 <0 || i+k>src.rows || j+l > src.cols) )
            vec.push_back(src.at<uchar>(i+k, j+l)); 
        }
      }
      suppressed.at<uchar>(i,j) = saturate_cast<uchar>(vMax);
    }
  }
  cout << "suppressed" << endl;
}

