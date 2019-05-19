#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void filter(Mat src, String filtername, Mat mgx, Mat mgy, Mat dst);

int main(int argc, char** argv )
{
  int size = 0;
  float sigma = 0.0F;

  if(argc < 3 || argc > 5)
  {
      cout << "usage: opencv <Image_Path> <filter> (<gaussianblur mask size> <sigma>)"<< endl;
      return -1;
  }
  else if(argc > 3)
  {
    size = atoi(argv[3]);
    sigma = atof(argv[4]);
    if(!(size%2))
    {
      cout << "usage: mask must be odd" << endl;
      return -1;
    }
  }

  Mat image = imread( argv[1], IMREAD_GRAYSCALE);
  
  if ( !image.data )
  {
      cout << "No image data" << endl;
      return -1;
  }
  string filtername = argv[2];

  Mat mgx(image.size(), image.type());
  Mat mgy(image.size(), image.type());
  Mat dst(image.size(), image.type());
  imshow("Start", image);

  cout << "Image rows: " << image.rows << endl;
  cout << "Image cols: " << image.cols<< endl;

  cout << "Applying Gaussian Blur with Mask size: " << size;
  cout << "and sigma: " << sigma << endl;

  if(!size)
    GaussianBlur(image, image, Size(size, size), sigma);

  //process
  filter(image, filtername, mgx, mgy, dst);

  imshow("gx", mgx);
  imshow("gy", mgy);
  imshow(filtername, dst);

  waitKey(0);
  return 0;
}


void filter(Mat src, String filtername, Mat mgx, Mat mgy, Mat dst)
{
  int fX[3][3] = {0};
  int fY[3][3] = {0};

  if(filtername == "sobel")
  {
    cout << "Using sobel mask" << endl;
    int sobX[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int sobY[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
    for(auto i = 0; i<3; i++)
    {
      for(auto j=0; j<3;j++)
      {
        fX[i][j] = sobX[i][j]; 
        fY[i][j] = sobY[i][j]; 
      }
    } 
  }
  else if(filtername == "prewitt")
  {
    cout << "Using prewitt mask" << endl;
    int preX[3][3] = {{-1, 0, 1}, {-1, 0, 1}, {-1, 0, 1}};
    int preY[3][3] = {{-1, -1, -1}, {0, 0, 0}, {1, 1, 1}};
    for(auto i = 0; i<3; i++)
    {
      for(auto j=0; j<3;j++)
      {
        fX[i][j] = preX[i][j];
        fY[i][j] = preY[i][j];
      }
    }
  }
  else
    exit(-1);

  int gx = 0, gy=0, sum = 0;
  int i = 0, j = 0;

  cout << "Applying convolution and getting gradients" << endl;

  for(i=0; i < src.rows; i++)
  {
    for(j=0; j < src.cols; j++)
    {
      sum = 0;
      gx = 0;
      gy = 0;
      for(auto k=0; k<3; k++)
      {
        for(auto l=0; l<3; l++)
        {
          if( !(i+k-1 < 0 || j+l-1 < 0 || i+k >= src.rows ||  j+l >= src.cols) )
          {
            gx += src.at<uchar>(i+k, j+l) * fX[k][l];
            gy += src.at<uchar>(i+k, j+l) * fY[k][l];
          }
        }
      }
      mgx.at<uchar>(i, j) = saturate_cast<uchar>(gx);
      mgy.at<uchar>(i, j) = saturate_cast<uchar>(gy);

      sum = abs(gx) + abs(gy);
      dst.at<uchar>(i, j) = saturate_cast<uchar>(sum);
    }
  }
  cout << "finished" << endl; 
}
