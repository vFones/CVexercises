#!/bin/sh

g++ -std=c++11 -Wall -Wextra -pedantic $1 \
	-I /usr/local/include/opencv4 \
	-L /usr/local/lib/ \
	-l opencv_core \
	-l opencv_imgproc \
	-l opencv_imgcodecs \
	-l opencv_highgui

