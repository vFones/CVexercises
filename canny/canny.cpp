#include <iostream>
#include <opencv2/opencv.hpp>

void Canny(cv::Mat &magnitude, cv::Mat &phase, cv::Mat &dst, int t1, int t2);

int main(int argc, char **argv)
{
  if(argc < 5)
  {
    std::cerr << "usage: opencv <Image_Path> <blur mask size> <sigma> <high> <low>" << std::endl;
    return -1;
  }
  cv::Mat src = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);

  if(src.empty())
  {
    std::cerr << "No image data" << std::endl;
    return -1;
  }
  cv::imshow("SRC", src);

  //applying a Gaussian Blur to smoooothing everything
  cv::GaussianBlur(src, src, cv::Size(atoi(argv[2]), atoi(argv[2])), atof(argv[3]) );

  //calculating derivates with sobel
  cv::Mat gx(CV_32F, src.rows, src.cols);
  cv::Mat gy(CV_32F, src.rows, src.cols);
  cv::Mat magnitude;
  cv::Mat magn_dir;
  cv::Mat dst = src.clone();

  cv::Sobel(src, gx, CV_32F, 1, 0);
  cv::Sobel(src, gy, CV_32F, 0, 1);

  //calculating intensity of an edge and the direction (mag and phase)
  cv::magnitude(gx, gy, magnitude);
  cv::phase(gx, gy, magn_dir, true); //true so result in RADIANS

  //applying Canny edge detector
  Canny(magnitude, magn_dir, dst, atoi(argv[4]), atoi(argv[5]));

  cv::imshow("DST", dst);
  cv::waitKey(0);
  return 0;
}


void Canny(cv::Mat &magnitude, cv::Mat &phase, cv::Mat &dst, int t1, int t2)
{
  std::cout << "Suppressing non maximum edges" << std::endl;
  magnitude.convertTo(dst, CV_8U);

  //finding edge direction
  for(auto i = 0; i < dst.rows; i++)
  {
    for(auto j = 0; j < dst.cols; j++)
    {
      int edge = cv::saturate_cast<int>(phase.at<float>(i, j)/45.0)%4;
      int prev = 0, next = 0, curr = 0;

      switch(edge)
      {
        case 0:
        //case edge is horizontal so left and right check
          if(j-1 >= 0)
            prev = dst.at<uchar>(i, j-1);
          if(j+1 < dst.cols)
            next = dst.at<uchar>(i, j+1);
        break;

        case 1:
        //edge is diagonal
          if(i-1 >= 0 && j-1 >= 0)
            prev = dst.at<uchar>(i-1, j-1);
          if(i+1 < dst.rows && j+1 < dst.cols)
            next = dst.at<uchar>(i+1, j+1);
        break;

        case 2:
        // edge is vertical
          if(i-1 >= 0)
            prev = dst.at<uchar>(i-1, j);
          if(i+1 < dst.rows)
            next = dst.at<uchar>(i+1, j);
        break;

        case 3:
        //edge is reverse diagonal
          if(i-1 >= 0 && j+1 < dst.cols)
            prev = dst.at<uchar>(i-1, j+1);
          if(i+1 < dst.rows && j-1 <= 0 )
            next = dst.at<uchar>(i+1, j-1);
        break;        
      }
      //if current is less than his prev and next pixel then remove it
      curr = dst.at<uchar>(i,j);
      if(prev > curr || next > curr)
        dst.at<uchar>(i,j) = 0;
    }
  }
  
  std::cout << "Histeresis" << std::endl;
  // using thresholds for a first binarization
  // if magnitude are higher than higher threshold then they are strong edges
  auto strong_edges = std::list<cv::Point2i>();

  for(auto i = 0; i < dst.rows; i++)
  {
    for(auto j = 0; j < dst.cols; j++)
    {
      if(dst.at<uchar>(i,j) > t2)
      {
        dst.at<uchar>(i,j) = 255;
        strong_edges.push_back(cv::Point(i,j));
      }
      else if(dst.at<uchar>(i,j) < t1)
        dst.at<uchar>(i,j) = 0;
    }
  }

  std::cout << "Second threshold" << std::endl;
  while(strong_edges.size())
  { //check near strong edges if there are value not strong edges make them
    cv::Point2i edge = strong_edges.front();
    strong_edges.pop_front();
    for(auto i = -1; i <= 1; i++)
      for(auto j = -1; j <= 1; j++)
      {
        cv::Point2i point(i+edge.x, j+edge.y);
        if(point.x >= 0 && point.y >= 0 && point.x < dst.rows && point.y < dst.cols)
        {
          auto value = dst.at<uchar>(point.x, point.y);
          if(value != 255 && value != 0)
          {
            dst.at<uchar>(point.x, point.y) = 255;
            strong_edges.push_back(point);
          }
        }
      }
  }

  //since there are no more strong edges normalize every others
  for(auto i = 0; i < dst.rows; i++)
    for(auto j = 0; j < dst.cols; j++)
      if(dst.at<uchar>(i,j) != 0 && dst.at<uchar>(i,j) != 255 )
        dst.at<uchar>(i,j) = 0;

  std::cout << "Canny applyied" << std::endl;
}