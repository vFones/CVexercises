#include <stdio.h>
#include <algorithm>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Mat median(Mat, int);
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

    Mat image_grey;
    cvtColor(image, image_grey, COLOR_BGR2GRAY);

    //Mat imgmed(image_grey.size(), image_grey.type());
    Mat imgmedian(image_grey.size(), image_grey.type());


    imshow("Start", image);


    cout << "applying filter" << endl;
    //imgmed = med(image_grey, sm);
    imgmedian = median(image_grey, sm);

    cout << "fin" << endl; 
    //imshow("MED", imgmed);
    imshow("MEDIAN", imgmedian);
    waitKey(0);
    return 0;
}

Mat med(Mat image, int sm)
{
  int med = 0;
  Mat dst(image.size(), image.type());
  for(auto i = 0; i < image.rows; i++)
  {
    for(auto j = 0; j < image.cols; j++)
    {
      med = 0;
      auto cont = 0;
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
      //cout << "assigning med" << endl;
      dst.at<uchar>(i,j) = med/cont;
      //cout << "med inserted in new matrix " << dst.at<uchar>(i,j) << endl;
    }
  }
  return dst;
}

Mat median(Mat image, int sm)
{
  auto v = 0;
  Mat dst(image.size(), image.type());
  vector <int>vmedian;
  for(auto i = 0; i < image.rows; i++)
  {
    for(auto j = 0; j < image.cols; j++)
    {
      for(auto r = 0; r < sm; r++)
      {
        for(auto c = 0; c < sm; c++)
        {
          //cout << "Mask: " << r << "," << c << endl;
          if(!(i+r-1 < 0 || j+c-1 < 0 || i+r >= image.rows || j+c >= image.cols))
          {
            //cout << "Inserting in vector pixel (r+i-1, c+j-1): " << r+i-1 << "," << c+j-1 << endl;
            v = image.at<uchar>(r+i, c+j);
            vmedian.push_back(v);
          }
        }
      }
      sort(vmedian.begin(), vmedian.end());
      auto median = vmedian.at( vmedian.size()/2 ); 
      //cout << "assigning median " << median;
      //cout << " to this pixel: " << image.at<uchar>(i,j) << endl;
      dst.at<uchar>(i,j) = median; 
      //cout << "median inserted in new matrix " << dst.at<uchar>(i,j) << endl;
      vmedian.clear();
    }
  }
  return dst;
}

