#include <iostream>
#include <random>
#include <opencv2/opencv.hpp>

double distance(cv::Vec3b &a, cv::Vec3b &b);
int whichCluster(std::vector<cv::Vec3b> &centroids, cv::Vec3b &pixel);
void updateCentroids(cv::Mat src, cv::Mat &clusters, std::vector<cv::Vec3b> &centroids);

cv::Mat kmeans(cv::Mat src, int clusterSize, int maxIterations);

int main(int argc, char **argv)
{
  if(argc < 3)
  {
    std::cerr << "exe, path, k, min pixel variation" << std::endl;
    exit(EXIT_FAILURE);
  }
  cv::Mat src = cv::imread(argv[1], cv::IMREAD_ANYCOLOR);
  
  if(src.empty())
  {
    std::cerr << "Image not valid" << std::endl;
    exit(EXIT_FAILURE);
  }
  int k = atoi(argv[2]);
  int maxIt = atoi(argv[3]);

  cv::Mat dst = kmeans(src, k, maxIt);

  cv::imshow("SRC", src);
  cv::imshow("Clusters", dst);

  cv::waitKey(0);
  return 0;
}


cv::Mat kmeans(cv::Mat src, int clusterSize, int maxIterations)
{
  
  cv::Mat clusters(src.size(), CV_32SC1, cv::Scalar(0));
  std::vector<cv::Vec3b> centroids(clusterSize);

  //randomize
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<> dist(0, src.rows);
  
  //getting random pixels and using them as centroids for my clusters 
  for(int i = 0; i < clusterSize; i++)
  {
    int x = dist(mt);
    int y = dist(mt);
    centroids.at(i) = src.at<cv::Vec3b>(x, y);
    clusters.at<int>(x, y) = i;
    std::cout << centroids.at(i) << " " << std::endl;
  }

  updateCentroids(src, clusters, centroids);
  int it = 0;
  int pixelchanges = 0;

  do
  {
    pixelchanges = 0;
    for(auto i = 0; i < src.rows; i++)
    {
      for(auto j = 0; j < src.cols; j++)
      {
        //look up for a suitable cluster for that pixel and add that
        int val = whichCluster(centroids, src.at<cv::Vec3b>(i, j));

        if(val != clusters.at<int>(i, j))
        {
          clusters.at<int>(i, j) = val;
          pixelchanges++;
        }
      }
    }
    it++;
    //update centroids
    updateCentroids(src, clusters, centroids);
  }while(pixelchanges > maxIterations);

  std::cout << "Fin: " << it << std::endl;

  cv::Mat dst(src.size(), src.type());

  for(int i = 0; i < clusters.rows; i++)
    for(int j = 0; j < clusters.cols; j++)
      dst.at<cv::Vec3b>(i, j) = centroids.at(clusters.at<int>(i, j));
      //coloring image with clusters position info using centroids color

  return dst;
}


double distance(cv::Vec3b &a, cv::Vec3b &b)
{
  double sum = 0.0;
  //calculate Euclidian distance for two RGB pixels
  for(auto i = 0; i < 3; i++)
    sum += std::pow(a[i] - b[i], 2);
  return std::sqrt(sum);
}

int whichCluster(std::vector<cv::Vec3b> &centroids, cv::Vec3b &pixel)
{
  //search for a suitable cluster for a pixel
  double min = distance(centroids.at(0), pixel);
  int k = 0;

  for(int i = 1; i < (int)centroids.size(); i++)
  {
    double dist = distance(centroids.at(i), pixel);
    if( dist < min )
    {
      min = dist;
      k = i;
    }
  }
  //minimum distantiated one will be returned as index
  return k;
}


void updateCentroids(cv::Mat src, cv::Mat &clusters, std::vector<cv::Vec3b> &centroids)
{
  std::vector<cv::Vec3d> sum(centroids.size(), cv::Vec3b(0,0,0));
  std::vector<int> pixels(centroids.size(), 0);

  //sum every pixels
  for(auto i = 0; i < clusters.rows; i++)
  {
    for(auto j = 0; j < clusters.cols; j++)
    {
      int k = clusters.at<int>(i, j);
      sum.at(k) += src.at<cv::Vec3b>(i,j);
      pixels.at(k)++;
    }
  }

  //if pixels sum is different from 0 start dividing and update centroids
  for(auto k = 0; k < (int) centroids.size(); k++)
  {
    for(auto i = 0; i < 3; i++)
    {
      double value = pixels.at(k) == 0 ? 0 : (1.0*sum.at(k)[i])/(1.0*pixels.at(k));
      centroids.at(k)[i] = cv::saturate_cast<uchar>(value);
      //using new mean for centroid info
    }
  }
}
