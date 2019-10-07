#include <stdio.h>
#include <opencv2/opencv.hpp>
#define RAD CV_PI/180

using namespace std;
using namespace cv;

void MyHoughLines(cv::Mat src, int thr, cv::Mat dst);
void FindPolar(double rho, double theta, Point& P1, Point& P2)
{  
  //In this function we translate the polar coordinate into the cartesian one.
  //From the math theory, we know that, given a RHO and a THETA, the cartesian coordinate
  //From wikipedia, those values can be found from:
  //x = rho*cos(theta)
  //y = rho*sin(theta);
  //Also, we need to draw those lines for the ENTIRE image. So, given a precise point, we could vary
  //a line lenght using a big scale factor, such as the size of the image.
  //x0 and y0 are our starting point
  int x0 = cvRound(rho * cos(theta));
  int y0 = cvRound(rho * sin(theta));
  
  //let's build the point: we must then add and subtract the big scale value to let the line vary between
  //the negative and positive scale factor.
  //We're also multiplying for -sin and cos to let them vary between the positive and negative cartesian space.
  P1.x = cvRound(x0 + 1000 * (-sin(theta)));
  P1.y = cvRound(y0 + 1000 * (cos(theta)));
  P2.x = cvRound(x0 - 1000 * (-sin(theta)));
  P2.y = cvRound(y0 - 1000 * (cos(theta)));
}

int main(int argc, char** argv )
{
  if ( argc != 3 )
  {
    printf("usage: DisplayImage.out <Image_Path>\n");
    return -1;
  }
  
  Mat image;
  image = imread( argv[1], IMREAD_GRAYSCALE );
  int thresh = atoi(argv[2]);
  
  if ( !image.data )
  {
    printf("No image data \n");
    return -1;
  }
  Mat image_blurred(image.size(), image.type());
  Mat image_canny(image.size(), image.type());
  
  GaussianBlur(image, image_blurred, Size(5, 5), 1.4);
  cout << "Gaussian" << endl;
  Canny(image_blurred, image_canny, 60, 150);
  cout << "Canny" << endl;
  
  Mat src;//(image.size(), CV_8UC3);
  //src = image.clone();
  
  cout << "Hough" << endl;
  
  cvtColor(image, src, COLOR_GRAY2BGR);
  
  MyHoughLines(image_canny, thresh, src);
  
  
  
  namedWindow("Display Image", WINDOW_AUTOSIZE );
  imshow("Display Image", image);
  
  namedWindow(" Image ", WINDOW_AUTOSIZE );
  imshow(" Image ", src);
  
  waitKey(0);
  
  return 0;
}

void MyHoughLines(cv::Mat src, int thr, cv::Mat dst)
{
  //int max_distance = hypot(src.rows, src.cols);
  int max_distance = max(src.cols, src.rows) * sqrt(2); // approssimazione del tipo di calcolo dell'ipotenusa
  Mat votes(Mat::zeros(max_distance * 2, 180, CV_32F));
  cout << "Applying hough" << endl;
  double rho;
  for(int x = 0; x < src.rows; x++)
  {
    for(int y = 0; y < src.cols; y++)
    {
      if(src.at<uchar>(x,y) > 250)
      {
        for(int theta = 0; theta < 180; theta++)
        {
          rho = cvRound(y*cos((theta-90)*RAD) + x*sin((theta-90)*RAD)) + max_distance;
          votes.at<uchar>(rho, theta)++;
        }
      }
    }
  }
  
  cout << "Made votes" << endl;
  
  for(double r = 0; r < votes.rows; r++)
  {
    for (double theta = 0; theta < 180; theta++) {
      if(votes.at<uchar>(r, theta) >= thr)
      {
        cv::Point pt1, pt2;
        FindPolar(r-max_distance, (theta-90)*RAD, pt1, pt2);
        line( dst, pt2, pt1, Scalar(120,0,250));
      }
    }
  }
}
