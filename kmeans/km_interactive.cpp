#include <iostream>
#include <opencv2/opencv.hpp>

int r, c;
cv::Mat src;
cv::Mat clusters;
std::vector<cv::Vec3b> centroids;
cv::Mat kmeans(cv::Mat &src, int maxIter);
double distanceBGR(cv::Vec3b &a, cv::Vec3b &b);
int whichCluster(std::vector<cv::Vec3b> &centroids, cv::Vec3b pixel);
void updateCentrois(cv::Mat src, cv::Mat clusters, std::vector<cv::Vec3b> &centroids);

void onMouse(int event, int row, int col, int, void*)
{
  if(event == cv::EVENT_LBUTTONDOWN)
  {
    r = row; c = col;
    std::cout << "Pushing: " << r << " " << c << std::endl;
    centroids.push_back(src.at<cv::Vec3b>(row, col));
    clusters.at<int>(r, c) = centroids.size();
  }
}

int main(int argc, char **argv)
{
  if(argc < 3)
    exit(-1);
  src = cv::imread(argv[1], cv::IMREAD_ANYCOLOR);
  if(src.empty())
    exit(-1);
  
  int r_tmp, c_tmp;
  r_tmp = c_tmp = -1;
  int maxIter = atoi(argv[2]);
  clusters = cv::Mat(src.size(), CV_32FC1, cv::Scalar(0));

  std::cout << "Press any key after selected when finished" << std::endl;

  cv::namedWindow("Src");
  cv::imshow("Src", src);
  cv::setMouseCallback("Src", onMouse);

  while(r_tmp != r && c_tmp != c)
  {
    r_tmp = r;
    c_tmp = c;
    cv::waitKey(0);
  }

  std::cout << "Starting iterating " << maxIter <<" times for " << centroids.size() << " clusters, please wait..." << std::endl;

  cv::Mat dst = kmeans(src, maxIter);
  cv::imshow("Dst", dst);

  cv::waitKey(0);
  return 0;
}

cv::Mat kmeans(cv::Mat &src, int maxIter)
{
  cv::Mat dst(src.size(), src.type());
  for(int it = 0; it < maxIter; it++)
  {
    for(auto i = 0; i < src.rows; i++)
    {
      for(auto j = 0; j < src.cols; j++)
      {
        int k = whichCluster(centroids, src.at<cv::Vec3b>(i, j));
        clusters.at<int>(i,j) = k;
      }
    }
    updateCentrois(src, clusters, centroids);
  }

  for(auto i=0; i<src.rows; i++)
    for(auto j=0; j<src.cols; j++)
      dst.at<cv::Vec3b>(i, j) = centroids.at(clusters.at<int>(i, j));
  
  return dst;
}

double distanceBGR(cv::Vec3b &a, cv::Vec3b &b)
{
  double sum = 0.0;
  for(auto i=0; i<3; i++)
    sum += pow(a[i] - b[i], 2);
  return sqrt(sum);
}

int whichCluster(std::vector<cv::Vec3b> &centroids, cv::Vec3b pixel)
{
  double min = distanceBGR(centroids.at(0), pixel);
  int index = 0;
  for(auto k = 1; k < (int) centroids.size(); k++)
  {
    double value = distanceBGR(centroids.at(k), pixel);
    if(value < min)
    {
      min = value;
      index = k;
    }
  }
  return index;
}

void updateCentrois(cv::Mat src, cv::Mat clusters, std::vector<cv::Vec3b> &centroids)
{
  std::vector<cv::Vec3d> sum(centroids.size());
  std::vector<int> pixels(centroids.size());
  
  for(auto i = 0; i < src.rows; i++)
  {
    for(auto j = 0; j < src.cols; j++)
    {
      int k = clusters.at<int>(i, j);
      sum.at(k) += src.at<cv::Vec3b>(i, j);
      pixels.at(k)++;
    }
  }
  
  for(auto k = 0; k < (int)centroids.size(); k++)
  {
    for(int i=0; i<3; i++)
    {
      double value = pixels.at(k) == 0 ? 0 : (1.0*sum.at(k)[i])/(pixels.at(k)*1.0);
      centroids.at(k)[i] = cv::saturate_cast<uchar>(value);
    }
  }
}