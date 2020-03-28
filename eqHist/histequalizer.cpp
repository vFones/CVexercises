#include <iostream>
#include <opencv2/opencv.hpp>

void equalizer(cv::Mat &src, cv::Mat &dst);

int main(int argc, char **argv)
{
  if( argc < 1 )
  {
    std::cout << "usage: opencv <Image_Path>" << std::endl;
    exit(EXIT_FAILURE);
  }

  cv::Mat src = imread(argv[1], cv::IMREAD_GRAYSCALE);
  if(src.empty())
  {
    std::cout << "No image data" << std::endl;
    exit(EXIT_FAILURE);
  }
  cv::imshow("SRC", src);
  cv::Mat dst = src.clone();

  equalizer(src, dst);
  cv::imshow("DST", dst);
  
  cv::waitKey(0);
  return 0;
}

void equalizer(cv::Mat &src, cv::Mat &dst)
{
  std::cout << "Getting old histogram" << std::endl;
  std::vector <float> hist(256, 0);
  auto nm = src.rows * src.cols;//MN = rows*cols
  auto L = 256;                 //L = color level -> 256 in that case  
  
  for(auto it=src.begin<uchar>(); it < src.end<uchar>(); it++)
    hist[*it]++;

  std::cout << "Equalizing" << std::endl;
  std::vector <float> newHist(256, 0);
  //equalize with round(Sk = L-1 sum(j=0 to k) nj/MN)
  //nj = single value of old histogram
  //S-k will be new histogram

  newHist[0] = (hist[0]/(nm)) *255;

  for(auto it = 1; it < L; it++)
    newHist.at(it) = (( float(hist[it]/(nm)) * L-1)) + newHist.at(it-1);

  std::cout << "Applying" << std::endl;
  for(auto i = 0; i < dst.rows; i++)
    for(auto j = 0; j < dst.cols; j++)
      dst.at<uchar>(i,j) = int(round(newHist.at( src.at<uchar>(i,j) ) ) );
}
