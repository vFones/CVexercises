#include <stdio.h>
#include <algorithm>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void medianBlur(Mat, int);
Mat med(Mat, int);

int main(int argc, char** argv )
{
    if ( argc != 3 )
    {
        cout << "usage: opencv <Image_Path> <mask size>" << endl;
        return -1;
    }
 
    int sm = atoi(argv[2]);
    if(sm < 3)
    {
      cout << "mask too small" <<endl;
      exit(-2);
    }

    Mat image = imread( argv[1], 0); // opening as greyscale image 
    if(!image.data)
    {
        cout << "No image data" << endl;
        return -1;
    }   

    //Mat image_grey;
    //cvtColor(image, image_grey, COLOR_BGR2GRAY);

    Mat imgmed(image.size(), image.type());
    Mat imgmedian(image.size(), image.type());


    imshow("Start", image);


    cout << "applying filter" << endl;
    imgmed = med(image, sm);
    medianBlur(image, imgmedian, sm);

    cout << "fin" << endl; 
    imshow("MED", imgmed);
    imshow("MEDIAN", imgmedian);
    waitKey(0);
    return 0;
}

Mat med(Mat image, int sm)
{
  int med = 0;
  int cont = 0;
  Mat dst(image.size(), image.type());
  for(auto i = 0; i < image.rows; i++)
  {
    for(auto j = 0; j < image.cols; j++)
    {
      med = 0;
      cont = 0;
      for(auto r = 0; r < sm; r++)
      {
        for(auto c = 0; c < sm; c++)
        {
          //cout << "Mask: " << r << "," << c << endl;
          if(!(i+r-1 < 0 || j+c-1 < 0 || i+r >= image.rows || j+c >= image.cols))
          { 
            //cout << "Inserting in vector pixel (r+i-1, c+j-1): " << r+i-1 << "," << c+j-1 << endl;
            med = med + image.at<uchar>(r+i, c+j);
            cont++;
          }
        }
      }
      //cout << med << " " << cont << endl;
      dst.at<uchar>(i,j) = med/cont;
      //cout << "med inserted in new matrix " << dst.at<uchar>(i,j) << endl;
    }
  }
  return dst;
}

void medianBlur(cv::Mat &src, cv::Mat &dst, int size_kernel)
{
  auto k = size_kernel/2;
  std::vector<uchar> tmpVector;
  
  for(auto x = 0; x < src.rows; x++)
  {
    for(auto y = 0; y < src.cols; y++)
    {

      for(auto i = -k; i <= k; i++)
      {
        for(auto j = -k; j <= k; j++)
        {
          auto tmpx = x-i;
          auto tmpy = y-j;
          if(tmpx > 0 && tmpy > 0 && tmpx <= src.rows && tmpy <= src.cols)
            tmpVector.push_back(src.at<uchar>(tmpx, tmpy));
        }
      }

      sort(tmpVector.begin(), tmpVector.end());
      int med = tmpVector.at(tmpVector.size()/2);
      
      
      dst.at<uchar>(x, y) = cv::saturate_cast<uchar>(med);
      tmpVector.clear();
    }
  }
  return;
}

