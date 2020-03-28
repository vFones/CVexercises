#include <iostream>
#include <opencv2/opencv.hpp>


//using a pair of Point and float type for storing corners
typedef std::pair<cv::Point, float> corner;

//harris function return a vector of corners, using that to draw circler in main
std::vector<corner> harris(cv::Mat &src, float thresh, int corner_dist);

int main(int argc, char **argv)
{
  if ( argc != 4 )
  {
    std::cerr << "usage: opencv <Image_Path> <threshold>" << std::endl;
    return -1;
  }
  
  cv::Mat src = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);
  if(src.empty())
   {
    std::cerr << "No image data" << std::endl;
    return -1;
  }

  cv::imshow("SRC", src);
  
  //thresholg used for corner response (100 suitable for chessboard)
  float thresh = atof(argv[2]);

  //corner distance in pixel (less = more nearby corners)
  int corner_dist = atoi(argv[3]);

  cv::Mat dst = src.clone();
  //passing image and threshold and corner distance to harris function
  std::vector<corner> corners = harris(src, thresh, corner_dist);

  //coloring destination for drawing colored circles
  cv::cvtColor(dst, dst, cv::COLOR_GRAY2BGR);


  for(auto i = 0; i < corners.size(); i++)
    cv::circle(dst, cv::Point(corners[i].first.y, corners[i].first.x), 7, cv::Scalar(0,0,255), 1);

  cv::imshow("DST", dst);
  cv::waitKey(0);
  exit(EXIT_SUCCESS);
}


std::vector<corner> harris(cv::Mat &src, float thresh, int corner_dist)
{
  cv::Mat gx = cv::Mat(src.rows, src.cols, CV_32F);
  cv::Mat gy = cv::Mat(src.rows, src.cols, CV_32F);
  cv::Mat gxy, gx2, gy2;
  //first apply a gaussian blur to image
  cv::GaussianBlur(src, src, cv::Size(5,5), 2.4);
  //calculate derivates with Sobel operators
  cv::Sobel(src, gx, CV_32F, 1, 0);
  cv::Sobel(src, gy, CV_32F, 0, 1);
  gxy = gx.mul(gy);
  gx2 = gx.mul(gx);
  gy2 = gy.mul(gy);

  //using gaussian to apply a funzione peso to consider only suitable corner pixel
  cv::GaussianBlur(gxy, gxy, cv::Size(5,5), 2.4);
  cv::GaussianBlur(gx2, gx2, cv::Size(5,5), 2.4);
  cv::GaussianBlur(gy2, gy2, cv::Size(5,5), 2.4);

  std::vector <corner> corners;
  cv::Mat R = cv::Mat(src.size(), CV_32F);

  std::cout << "founding corner" << std::endl;

  for(int i = 0; i < src.rows; i++)
    for(int j = 0; j < src.cols; j++)
    {
      //calculating harris Response matrix with det(M) - K * trace(M)^2
      //where M is coovariance matrix and K is a very low number (usually 0.04 <= k <= 0.06)
      float det = gx2.at<float>(i,j) * gy2.at<float>(i,j) - pow(gxy.at<float>(i,j),2);
      float trace = gx2.at<float>(i,j) + gy2.at<float>(i,j);
      R.at<float>(i,j) = det - 0.04*(trace*trace);
    }

  //pushing values only if corner is higher than value
  for(int i = 0; i < src.rows; i++)
    for(int j = 0 ; j < src.cols; j++)
      if(R.at<float>(i,j) > thresh)
        corners.push_back(std::make_pair(cv::Point(i, j), R.at<float>(i, j)));

  std::cout << "Sorting corners" << std::endl;

  //sorting using values as meter
  std::sort(
    corners.begin(),
    corners.end(),
    [](const corner &l, const corner &r) {
      return l.second > r.second; });


  //suppressing corners that are in a suitable distance later in vector since
  //sorted reversed we are looking for a "lesser corner" and then erasing from vector
  std::cout << "Suppressing " << corners.size() << std::endl;
  for(auto i = 0; i < corners.size(); i++)
    for(auto j = i+1; j < corners.size(); j++)
    {
      cv::Point I(corners[i].first.x, corners[i].first.y);
      cv::Point J(corners[j].first.x, corners[j].first.y);
      
      if(abs(I.x - J.x) < corner_dist && abs(I.y - J.y) < corner_dist)
      {
        corners.erase(corners.begin()+j);
        j--;
      }
    }
  
  std::cout << "Fin harris" << std::endl;
  return corners;
}