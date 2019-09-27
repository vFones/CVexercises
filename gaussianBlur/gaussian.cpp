#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, char** argv )
{
    if ( argc < 4 )
    {
        printf("usage: opencv <Image_Path>\n");
        return -1;
    }
    Mat image, dst;
    image = imread( argv[1], 0);
    // opening as greyscale image

    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }
    int size = atoi(argv[2]);
    int sigma = atof(argv[3]);

    imshow("Start", image);

    image.copyTo(dst);

    GaussianBlur(image, dst, Size(size, size), sigma);
    //process
    
    imshow("Result", dst);

    waitKey(0);
    return 0;
}
