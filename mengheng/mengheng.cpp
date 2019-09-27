#include <iostream>
#include <opencv2/opencv.hpp>

int main(int argc, char **argv)
{
  if(argc < 2)
  {
    std::cerr << "Less args" << std::endl;
    exit(EXIT_FAILURE);
  }
  cv::Mat src = cv::imread(argv[1], cv::IMREAD_ANYCOLOR);
  if(!src.data)
  {
    std::cout << "Image error" << std::endl;
    exit(EXIT_FAILURE);
  }
  cv::imshow("Source image", src);

  //mengheng();

  cv::waitKey(0);
  return EXIT_SUCCESS;
}