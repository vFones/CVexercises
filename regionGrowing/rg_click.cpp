#include <iostream>
#include <opencv2/opencv.hpp>

cv::Mat regionGrowing(cv::Mat &src, int x_seed, int y_seed, int threshold);
cv::Mat src;
int x_seed, y_seed;
int pixel_rg;

void callback(int event, int col, int row, int, void*)
{
  if(event == cv::EVENT_LBUTTONDOWN)
  {
    x_seed = row; y_seed = col;
    std::cout << src.at<cv::Vec3b>(row, col) << std::endl;
  }
}


int main(int argc, char **argv)
{
  if(argc != 3)
    exit(-1);

  src = imread(argv[1], cv::IMREAD_COLOR);
  if(src.empty())
    exit(-1);

  int col, row;
  pixel_rg = 0;
  
  col = row = -1;

  cv::Mat dst(src.size(), src.type(), cv::Scalar(0));
  
  int threshold = atoi(argv[2]);

  cv::namedWindow("Src");
  cv::setMouseCallback("Src", callback, 0);
  cv::imshow("Src", src);
  cv::waitKey(0);
  
  //select for a seed and press a key to start expansion
  while(row != x_seed && col != y_seed && pixel_rg < src.total())
  {
    row = x_seed;
    col = y_seed;
    
    dst += regionGrowing(src, x_seed, y_seed, threshold);
    cv::imshow("Dst", dst);
    cv::waitKey(0);
  }
  cv::imwrite("regione.png", dst);
  return 0;
}

double euclideDist(cv::Vec3b &a, cv::Vec3b &b)
{
  double sum = 0.0;
  for(int i=0; i<3; i++)
    sum += std::pow(a[i]-b[i], 2);
  return std::sqrt(sum);
}


cv::Mat regionGrowing(cv::Mat &src, int x_seed, int y_seed, int threshold)
{
  cv::Mat dst(src.size(), src.type(), cv::Scalar(0));
  std::vector<cv::Point> pixels;
  cv::Vec3b seedValue = src.at<cv::Vec3b>(x_seed, y_seed);
  cv::Mat regionValue = cv::Mat::zeros(src.size(), CV_8U);
 
  pixels.push_back(cv::Point(x_seed, y_seed));

  std::cout << "Starting growing" << std::endl;

  while(!pixels.empty())
  {
    cv::Point pixel = pixels.back();
    pixels.pop_back();
    int x = pixel.x;
    int y = pixel.y;
    for(int u = -1; u < 2; u++)
    {
      for(int v = -1; v < 2; v++)
      {
        if(x+u >= 0 && y+v >= 0 && x+u < src.rows && y+v < src.cols)
        {
          if(regionValue.at<int>(x+u, y+v) == 0 && euclideDist(seedValue, src.at<cv::Vec3b>(x+u, y+v)) < threshold)
          {
            regionValue.at<int>(x+u, y+v) = 1;
            dst.at<cv::Vec3b>(x+u, y+v) = seedValue;
            pixels.push_back(cv::Point(x+u, y+v)) ;
            pixel_rg++;
          }
        }
      }
    }
  }
  std::cout << "Fin growing." << std::endl;
  return dst; 
}