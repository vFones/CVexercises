#include <iostream>
#include <opencv2/opencv.hpp>


void kmeans(cv::Mat &src, cv::Mat &cluster, std::vector<cv::Vec3b> &centroid, int k, int max_iter);

int main(int argc, char **argv)
{
  std::srand(std::time(nullptr));
  if(argc < 4)
  {
    std::cerr << "Less args" << std::endl;
    exit(EXIT_FAILURE);
  }
  cv::Mat src = cv::imread(argv[1], cv::IMREAD_ANYCOLOR);
  if(!src.data)
  {
    std::cerr << "Image error" << std::endl;
    exit(EXIT_FAILURE);
  }
  cv::imshow("Input image", src);

  int k = atoi(argv[2]);
  int iterations = atoi(argv[3]);
  
  //cv::resize(src,src,src.size()/3);
  

  cv::Mat dst(src.size(), src.type());
  cv::Mat clusters(src.size(), CV_8U, cv::Scalar(0));
  std::vector<cv::Vec3b> centroid(k);

  kmeans(src, clusters, centroid, k, iterations);
  std::cout << "Fin." << std::endl;

  for (int i=0; i<clusters.rows; i++) {
        for (int j=0; j<clusters.cols; j++) {
            int h = clusters.at<uchar>(i,j);
            dst.at<cv::Vec3b>(i,j) = centroid[h];
        }
    }
  

  cv::imshow("Clustered", dst);



  cv::waitKey(0);
  return 0;
}


float distBGR(cv::Vec3b &a, cv::Vec3b &b)
{
  float sum = 0.0;
  for(int i = 0; i < 3; i++)
    sum += std::pow(a[i]-b[i], 2);
  sum = std::sqrt(sum);
  return sum;
}

int PixelInWhichCluster(std::vector<cv::Vec3b> &cluster, cv::Vec3b &pixel)
{
  float min = distBGR(cluster.at(0), pixel);
  int clusterindex = 0;
  
  for(int h = 1; h < cluster.size(); h++)
  {
    float distance = distBGR(cluster.at(h), pixel);
    if( distance < min )
    {
      min = distance;
      clusterindex = h;
    }
  }

  return clusterindex;
}


void updateCentroid(cv::Mat &src, cv::Mat &clusters, std::vector<cv::Vec3b> &centroid)
{
  std::vector<cv::Vec3d> sums(centroid.size(), cv::Vec3d(0,0,0));
  std::vector<int> counts(centroid.size(), 0);

  for (int i=0; i<clusters.rows; i++)
  {
    for (int j=0; j<clusters.cols; j++)
    {
      int h = clusters.at<uchar>(i,j);
      cv::Vec3b pixel = src.at<cv::Vec3b>(i,j);
      sums[h] += pixel;
      counts[h]++;
    }
  }

  for (int k=0; k<centroid.size(); k++)
  {
    for (int i=0; i<3; i++)
    {
      double value = counts[i] == 0 ? 0 : (1.0*sums[k][i])/(1.0*counts[k]);
      centroid[k][i] = cv::saturate_cast<uchar>(value);
    }
  }
  //std::cout << "updated centroid" << std::endl;
}



void kmeans(cv::Mat &src, cv::Mat &cluster, std::vector<cv::Vec3b> &centroid, int k, int max_iter)
{
  std::cerr << "starting kmeans" << std::endl;

/*
  for (int i=0; i<cluster.rows; i++)
  {
    for (int j=0; j<cluster.cols; j++)
    {
      int c = std::rand() % k;
      cluster.at<uchar>(i,j) = cv::saturate_cast<uchar>(c);
    }
  }
*/
  
  for( auto i = 0; i < k; i++)
  {
    int x = rand()%src.rows;
    int y = rand()%src.cols;
    centroid.at(i) = src.at<cv::Vec3b>(x, y);

    cluster.at<uchar>(x, y) = i;
  }


  std::cerr << " first touch" << std::endl;

  //updateCentroid(src, cluster, centroid);

  for(int h = 0; h < max_iter; h++)
  {

    for(int i = 0; i < src.rows; i++)
    {
      for(int j = 0; j < src.cols; j++)
      {
        cv::Vec3b pixel = src.at<cv::Vec3b>(i, j);
        int index = PixelInWhichCluster(centroid, pixel);

        //dst.at<cv::Vec3b>(i,j) = centroid.at(index);
        
        cluster.at<uchar>(i, j) = index;
      }
    }

    updateCentroid(src, cluster, centroid);
  }
  return;
}