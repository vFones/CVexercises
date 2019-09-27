#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;
int main(int argc, char** argv )
{
    if ( argc != 2 )
    {
        printf("usage: opencv <Image_Path>\n");
        return -1;
    }
    Mat image, dst, image_gray;

    image = imread( argv[1], COLOR_BGR2GRAY);
    
    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }

    imshow("Image", image);

    cvtColor(image, image_gray, COLOR_BGR2GRAY);

    image_gray.copyTo(dst);

    for(int i = 0; i < image.rows; i++)
    {
      for(int j = 0; j < image.cols; j++)
      {
        if(dst.at<uchar>(i,j) > 55)
          dst.at<uchar>(i,j) = 255;
        else
          dst.at<uchar>(i,j) = 0;
      }
    }

    imshow("Threshold", dst);

    waitKey(0);
    return 0;
}
