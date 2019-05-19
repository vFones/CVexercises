#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void sobel(Mat src, Mat sobelized);
void suppression(Mat src, Mat suppressed);

int main(int argc, char** argv )
{
    if ( argc != 2 )
    {
        printf("usage: opencv <Image_Path>\n");
        return -1;
    }
    Mat image, sobelized, isterized;
    image = imread( argv[1], 0);
    // opening as greyscale image

    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }

    imshow("Start", image);
    image.copyTo(sobelized);

    sobel(image, sobelized);
    imshow("SOBEL", sobelized);
    
    Mat suppressed(sobelized.size(), sobelized.type());
    suppression(sobelized, suppressed);
    imshow("SUPPRESSED", suppressed);

    waitKey(0);
    return 0;
}

int maxVector(vector <int> vec)
{
  auto max = 0;
  for(auto i: vec)
    if (i > max)
      max = i;

  return max;
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
    vMax = maxVector(vec);
    for(auto m=0; m<vec.size(); m++)
      if(vec.at(m) < vMax)
        vec.at(m)=0;

      suppressed.at<uchar>(i,j) = saturate_cast<uchar>(vMax);
    }
  }
  cout << "suppressed" << endl;
}


void sobel(Mat src, Mat sobelized)
{
  int gx[3][3] = {{-1,0,1}, {-2,0,2}, {-1,0,1}};
  int gy[3][3] = {{-1,-2,-1},{0,0,0},{1,2,1}};

  cout << "applying sobel" << endl;

  for(auto i=0;i<src.rows;i++)
  {  
    for(auto j=0;j<src.cols;j++)
    {
      auto sobX=0, sobY=0;
      auto gradient = 0;
      for(auto k=0; k<3;k++)
      {
        for(auto l=0; l<3; l++)
        {
          if( !(i+k-1<0 || j+l-1 <0 || i+k>src.rows || j+l > src.cols) )
          {
            sobX += src.at<uchar>(i+k,j+l) * gx[k][l];
            sobY += src.at<uchar>(i+k,j+l) * gx[k][l];
          }
        }
      }
      gradient = abs(sobX) + abs(sobY);
      sobelized.at<uchar>(i,j) = saturate_cast<uchar>(gradient);
    }  
  }
  cout << "Finished sobel" << endl;
}
