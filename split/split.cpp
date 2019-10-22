#include <iostream>
#include <opencv2/opencv.hpp>

cv::Mat src, dst;
cv::Vec3d mean(int ri, int rf, int ci, int cf);
double variance(int ri, int rf, int ci, int cf);
void colorImg(cv::Mat immagine,int ri, int rf, int ci, int cf, cv::Vec3b colore);
void split_color(int ri, int rf, int ci, int cf);
double thres, size;

int main(int argc, char **argv)
{
  
  src = imread(argv[1], cv::IMREAD_COLOR);
  
  if (src.data == nullptr){
    std::cerr << "error" << std::endl;
    exit(EXIT_FAILURE);
  }
  
  cv::imshow("Source img", src);
 
  dst = cv::Mat::zeros(src.size(), src.type());

  thres = atof(argv[2]);
  size = atoi(argv[3]);

  split_color(0, src.rows, 0, src.cols);
  
  cv::imshow("Output img", dst);
  cv::waitKey(0);
  return 0;
}


void split_color(int ri, int rf, int ci, int cf)
{
  cv::Vec3d media = mean(ri, rf, ci, cf);

  double vars = variance(ri, rf ,ci ,cf);
  
  int h = rf - ri;
  int w = cf - ci;

  if( vars > thres  || (h < size && w < size))
    colorimg(dst, ri, rf, ci, cf, media);
  else
  {
    std::cout  << vars << std::endl;
    int newr = (rf - ri) / 2;
    int newc = (cf - ci) / 2;
    split_color(ri,        ri + newr, ci,        ci + newc);
    split_color(ri,        ri + newr, ci + newc, cf);
    split_color(ri + newr, rf,        ci,        ci + newc);
    split_color(ri + newr, rf,        ci + newc, cf);
  }
}


cv::Vec3d mean(int ri, int rf, int ci, int cf){
  cv::Vec3d media;
  int pix=0;
  for (int i=ri; i<rf ; i++)
    for(int j=ci; j<cf; j++)
    {
      for(auto k = 0; k < 3; k++)
        media[k] += src.at<cv::Vec3b>(i,j).val[k];
      pix++;
    }
  for(int i=0; i<3; i++)
     media[i] /= pix;

  return media;
}


double variance(int ri, int rf, int ci, int cf)
{
  cv::Vec3d var;
  cv::Vec3d meanofrg = mean(ri, rf, ci, cf);
  int pix=0;
  
  for (int i = ri; i < rf ; i++)
     for(int j = ci; j < cf; j++)
     {
       for(auto k = 0; k < 3; k++)
         var[k] += pow(src.at<cv::Vec3b>(i,j).val[k] - meanofrg[k], 2);
      pix++;
     }

  double avg=0;

  for(int i=0; i<3; i++)
    avg += (sqrt(var[i]) / pix);

  return (avg/3);
}


void colorimg(cv::Mat tocolor, int ri, int rf, int ci, int cf, cv::Vec3d color)
{
  for(int i = ri; i < rf; i++)
    for(int j = ci; j < cf; j++)
      for(int k = 0; k < 3; k++)
        tocolor.at<cv::Vec3b>(i, j).val[k] = cv::saturate_cast<uchar>(color[k]);
}
