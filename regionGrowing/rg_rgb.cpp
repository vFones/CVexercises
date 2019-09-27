#include <iostream>
#include <time.h>
#include <opencv2/opencv.hpp>

cv::Mat raw_img;
int r_dect,
    c_dect;

void mouseDect(int event, int col, int row, int flags, void* data)
{
    if(event == cv::EVENT_LBUTTONDOWN)
    {
        r_dect = row, c_dect = col;
        cv::Vec3b rgb = raw_img.at<cv::Vec3b>(row,col);

        std::cout << "(r,c) = (" << row << ", " << col << ")\n" \
            << "BGR = [" << (int)rgb[0] << ", "  << (int)rgb[1] << ", "  << (int)rgb[2] << "]\n";
        }
}

void regionGrowingRGB(cv::Mat src, cv::Mat &dst, int min_threshold);

int main(int argc, char **argv)
{
  srand(time(NULL));
  if(argc < 3)
  {
    std::cerr << "Less args" << std::endl;
    exit(-1);
  }
  
  raw_img = cv::imread(argv[1], cv::IMREAD_ANYCOLOR);
  if(!raw_img.data)
  {
    std::cerr << "Image error" << std::endl;
    exit(-1);
  }
  

  cv::imshow("RAW img", raw_img);
  cv::Mat dst(raw_img.size(), raw_img.type());
  

  //appiattisco tutt a maronn (ngap a me preprocessing)
  //cv::medianBlur(raw_img, raw_img, 17);

  cv::namedWindow("img", cv::WINDOW_AUTOSIZE );
  cv::setMouseCallback("img", mouseDect, NULL);
  cv::imshow("img", raw_img);

  //applico RG con magagna dei colori vicini al rosso
  raw_img.copyTo(dst);
  regionGrowingRGB(raw_img, dst, atoi(argv[2]));
  cv::imshow("dst img", dst);

  cv::waitKey(0);
  return 0;
}

double euclideDist(cv::Vec3b &a, cv::Vec3b &b)
{
  double sum;
  for(int i=0; i<3; i++)
    sum += std::pow(a[i]-b[i], 2);
  sum = std::sqrt(sum);
  return sum;
}
void regionGrowingRGB(cv::Mat src, cv::Mat &dst, int min_threshold)
{
  std::vector<cv::Point> pixelVector;
  cv::Mat regions(src.size(), CV_32F, cv::Scalar(0));
  int x_seed;
  int y_seed;
  int num_pix = regions.total();
  cv::Vec3b black_pixel(0,0,0);
  
  while( num_pix )
  {

    x_seed = rand()%src.rows;
    y_seed = rand()%src.cols;
    
    if( regions.at<float>(x_seed, y_seed) == 0 )
    {
      regions.at<float>(x_seed, y_seed) = 1;
      num_pix--;
      cv::Vec3b pixelValue = src.at<cv::Vec3b>(x_seed, y_seed);

      /* MAGAGNA colore rosso
      if(pixelValue[0] < 10 && pixelValue[2] > 50 && (pixelValue[1] < 60 || pixelValue[1] == 0) )
        dst.at<cv::Vec3b>(x_seed, y_seed) = pixelValue;
      else
        dst.at<cv::Vec3b>(x_seed, y_seed) = black_pixel;
      //DIOPORCO
      */

      pixelVector.clear();
      pixelVector.push_back(cv::Point(x_seed, y_seed));

      while(!pixelVector.empty())
      {
        cv::Point pixel = pixelVector.back();
        int x = pixel.x;
        int y = pixel.y;
        pixelVector.pop_back();

        for(int u = -1; u < 2; u++)
        {
          for(int v = -1; v < 2; v++)
          {
            if( x+u >=0 && x+u < src.rows && y+v >= 0 && y+v < src.cols )
            {
              if(euclideDist(src.at<cv::Vec3b>(u+x, v+y), pixelValue) < min_threshold && regions.at<float>(u+x, v+y) == 0)
              {
                regions.at<float>(u+x, v+y) = 1;
                num_pix--;
                dst.at<cv::Vec3b>(u+x, v+y) = pixelValue;

                /* MAGAGNA colore rosso
                if(pixelValue[0] < 10 && pixelValue[2] > 50 && (pixelValue[1] < 60 || pixelValue[1] == 0) )
                  
                else
                  dst.at<cv::Vec3b>(u+x, v+y) = black_pixel;
                //MAGAGNA
                */

                pixelVector.push_back(cv::Point(u+x, y+v));
              }
            }
          }
        }
      }
    }
  }
}