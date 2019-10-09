#include <iostream>
#include <opencv2/opencv.hpp>

cv::Mat regionGrowing(cv::Mat &src, int x_seed, int y_seed, int threshold);
cv::Mat src;
cv::Mat hsv;
int x_seed, y_seed;
int pixel_rg;
int hsvtint0, hsvtint1;

void callback(int event, int col, int row, int, void*)
{
  if(event == cv::EVENT_LBUTTONDOWN)
  {
    x_seed = row; y_seed = col;
    std::cout << "BGR: " << src.at<cv::Vec3b>(row, col) << std::endl;
    std::cout << "HSV: " << hsv.at<cv::Vec3b>(row, col) << std::endl;

  }
}

int main(int argc, char **argv)
{
  if(argc < 4)
  {
    std::cerr << "exe, image, distance, hsv tint" << std::endl;
    exit(EXIT_FAILURE);
  }

  cv::Mat raw = imread(argv[1], cv::IMREAD_COLOR);
  if(raw.empty())
  {
    std::cerr << "image not valid" << std::endl;
    exit(EXIT_FAILURE);
  }
  src = raw.clone();

  int col, row;
  pixel_rg = 0;
  
  col = row = -1;

  cv::Mat dst(raw.size(), raw.type(), cv::Scalar(0));
  
  int threshold = atoi(argv[2]);
  hsvtint0 = atoi(argv[3]);
  hsvtint1 = atoi(argv[4]);
  
  cv::namedWindow("Src");
  cv::setMouseCallback("Src", callback);
  cv::imshow("Src", raw);
  
  cv::Mat closed;
  cv::medianBlur(raw, closed, 11);
  cv::medianBlur(closed, closed, 11);

  cv::imshow("Closed", closed);

  cv::cvtColor(closed, hsv, cv::COLOR_BGR2HSV);
  
  cv::waitKey(0);
  
  //select for a seed and press a key to start expansion
  while(row != x_seed && col != y_seed && pixel_rg < src.total())
  {
    row = x_seed;
    col = y_seed;
    
    dst += regionGrowing(closed, x_seed, y_seed, threshold);
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
  cv::Vec3b seedValue_hsv = hsv.at<cv::Vec3b>(x_seed, y_seed);
  cv::Vec3b seedValue_bgr = src.at<cv::Vec3b>(x_seed, y_seed);

  cv::Mat regionValue = cv::Mat::zeros(src.size(), CV_8U);

  pixel_rg = 0;
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
          cv::Vec3b currPixel_hsv = hsv.at<cv::Vec3b>(x+u,y+v);
          cv::Vec3b currPixel_rgb = src.at<cv::Vec3b>(x+u,y+v);

          if(regionValue.at<int>(x+u, y+v) == 0 && euclideDist(seedValue_bgr, currPixel_rgb) < threshold && currPixel_hsv.val[0] >= hsvtint0 && currPixel_hsv.val[0] < hsvtint1)
          {
            regionValue.at<int>(x+u, y+v) = 1;
            dst.at<cv::Vec3b>(x+u, y+v) = seedValue_bgr;
            pixels.push_back(cv::Point(x+u, y+v));
            pixel_rg++;
          }
        }
      }
    }
  }
  std::cout << "Fin growing -> " << pixel_rg << std::endl;
  return dst; 
}