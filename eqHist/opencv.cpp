#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

vector<float> getHist(Mat src);
Mat equalizeHist(Mat src, vector<float> hist);


int main(int argc, char** argv )
{
    if ( argc != 2 )
    {
        cout << "usage: opencv <Image_Path>" << endl;;
        exit(-1);
    }
    Mat image, image_gray;

    image = imread( argv[1], COLOR_BGR2GRAY);
    // opening as greyscale image
    
    Mat dst(image.size(), image.type());

    if ( !image.data )
    {
        cout << "No image data" << endl;;
        exit(-1);
    }

    imshow("Start", image);
    
    cvtColor(image, image_gray, COLOR_BGR2GRAY);

    //process
    cout << "getting histogram" << endl;
    vector<float> histogram = getHist(image_gray);
       
    cout << "equalizing histogram" << endl;
    dst = equalizeHist(image_gray, histogram);

    imshow("Result", dst);
    
    waitKey(0);
    return 0;
}


Mat equalizeHist(Mat src, vector<float> histogram)
{
  Mat dst = src.clone();

  auto nm = src.rows * src.cols;//MN = rows*cols
  auto L = 256;                 //L = color level -> 256 in that case  
  
  cout << "initializing new vector" << endl;
  vector<float> newHist(256, 0);

  cout << "setting new histogram" << endl;
  newHist.at(0) = (histogram.at(0)/nm) * (L-1);

  //equalize with round(Sk = L-1 sum(j=0 to k) nj/MN)
  //nj = single value of old histogram
  //S-k will be new histogram

  for(auto k=1; k<L; k++)
  {
    //cout << newHist.at(k) << endl;
    newHist.at(k) = (( float(histogram.at(k)/(nm)) ) * (L-1) ) + newHist.at(k-1);
  }


  for(auto i=0; i < src.rows; i++)
    for(auto j=0; j < src.cols; j++)
      dst.at<uchar>(i,j) = (int)round(newHist.at( src.at<uchar>(i,j) ));

  return dst;
}


vector<float> getHist(Mat src)
{
  vector<float> histogram(256, 0);

  for(auto i=0; i < src.rows; i++)
    for(auto j=0; j < src.cols; j++)
      histogram.at( src.at<uchar>(i,j) )++;

    return histogram;
}
