#include <iostream>
#include <opencv2/opencv.hpp>

void regionGrowing(cv::Mat src, cv::Mat &dst, int iterations, int min_threshold);

int main(int argc, char **argv)
{
  if(argc < 4)
  {
    std::cerr << "Less args" << std::endl;
    exit(-1);
  }
  cv::Mat raw_img = cv::imread(argv[1], 0);
  if(!raw_img.data)
  {
    std::cerr << "Image error" << std::endl;
    exit(-1);
  }
  //cv::Mat raw_gs_img;
  //cv::cvtColor(raw_img, raw_gs_img, cv::COLOR_BGR2GRAY);
  cv::imshow("RAW img", raw_img);
  //cv::imshow("Raw greyscale img", raw_gs_img);

  cv::Mat dst(raw_img.size(), raw_img.type());

  regionGrowing(raw_img, dst, atoi(argv[2]), atoi(argv[3]));
  cv::imshow("dst img", dst);

  cv::waitKey(0);
  return 0;
}


void regionGrowing(cv::Mat src, cv::Mat &dst, int iterations, int min_threshold)
{
  std::cout << src.rows << "x" << src.cols << std::endl;
  std::cout << dst.rows << "x" << dst.cols << std::endl;

  std::vector<cv::Point> pixelVector;

  cv::Mat regions(src.size(), CV_32F, cv::Scalar(0));

  std::cout << "Starting iterating " << iterations << std::endl;
	for(int i=0; i<iterations; i++)
  {
		int x_seed = rand()%src.rows;
		int y_seed = rand()%src.cols;
		
		if(regions.at<float>(x_seed, y_seed) == 0)
    {
			regions.at<float>(x_seed, y_seed) = 1;
			int pixel_value = src.at<uchar>(x_seed, y_seed);
			dst.at<uchar>(x_seed, y_seed) = pixel_value;

			pixelVector.clear();
			
      pixelVector.push_back(cv::Point(x_seed, y_seed));

      while(!pixelVector.empty())
      {
        cv::Point pixel = pixelVector.back();
		    int x = pixel.x;
		    int y = pixel.y;
        pixelVector.pop_back();
        
        for(int u=-1; u<=1; u++)
        {
          for(int v=-1; v<=1; v++)
          {
            if(x+u >=0 && x+u < src.rows && y+v >=0 && y+v < src.cols)
            {
              if( abs(pixel_value - src.at<uchar>(x+u, y+v)) < min_threshold \
                  && regions.at<float>(x+u, y+v) == 0)
              {
                regions.at<float>(x+u, y+v) = 1;
                dst.at<uchar>(x+u, y+v) = pixel_value;
                pixelVector.push_back(cv::Point(x+u, y+v));
              }
            }
          }
        }
      }
    }
	}
  return;
}