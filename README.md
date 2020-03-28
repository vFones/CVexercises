# Digital image processing
Some exercise for an exam. (Trying to not use openCV implementation)

### How to compile
Use script.sh to build cpp files

### Canny
<p align="center">
  <img src="https://i.imgur.com/1QTJtgG.png">
</p>

Canny algorithm for edge detection.
Pass Gaussian blur kernel, sigma, lower threshold, higher threshold.

### Distance transform
<p align="center">
  <img src="https://i.imgur.com/STTnxNG.png">
</p>

Used for distance mapping.

### Histogram equalizator
<p align="center">
  <img src="https://i.imgur.com/q6txhN2.png">
</p>

Equalize an image histogram.

### Harris
<p align="center">
  <img src="https://i.imgur.com/9llrdyG.png">
</p>

Algorithm for Corner detection in an image.

### Hough circles

<p align="center">
  <img src="https://i.imgur.com/zMmsnmH.png">
</p>


Detection circles in images: minimum radius, max radius and threshold for accumulator.

### Hough lines

<p align="center">
  <img src="https://i.imgur.com/VVVtdeM.png">
</p>


Detection lines in images: input is the threshold for accumulator.

### K-means

<p align="center">
  <img src="https://i.imgur.com/HhIijfS.png">
</p>

K-means algorithm find correct cluster starting from K centroids: applied to images find correct
cluster for a pixel based on RGB distance (euclidian).
Auto version stops if there isn't a variation under a certain threshold. Interactive can select with mouse centroids rather then random. Iterative use N iteration before stop.








