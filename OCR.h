/*
 * OCR.h
 *
 *  Created on: 21 Oct 2015
 *      Author: guillaume
 */

#ifndef OCR_H_
#define OCR_H_

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
using namespace cv::ml;

// minimum height in pix for input of train data
const int train_height_min = 9;

// matching OCR pixel size (square)
const int matching_size = 35;

// minimum matching distance to solve a number
const int minimum_matching_dist = 10000000;

void trainOCR(String filename, Mat& sample, Mat& response);
void loadTrainedOCR(String filename, Mat& sample, Mat& response);
void saveTrainedOCR(String filename, Mat& sample, Mat& response);
int getNumberOCR(Mat img, Ptr<KNearest> knn);


#endif /* OCR_H_ */
