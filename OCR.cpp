/*
 * OCR.cpp
 *
 *  Created on: 21 Oct 2015
 *      Author: guillaume
 */

#include "OCR.h"
#include "Image.h"

void trainOCR(String filename, Mat& sample, Mat& response)
{

	//Process image to extract contour
	Mat img_src, img_gray, img_p, img_contours, mat_tmp;

	// load the file to train numbers
	img_src = imread(filename, CV_LOAD_IMAGE_UNCHANGED);

	//check the image source
	if (img_src.empty()) throw String("the image " + filename + " cannot be loaded");

	// show the original train data image
	namedWindow("Train Data", WINDOW_AUTOSIZE);

	cout << "input red squared number [0-9] or ESC directly from the Train Data window" << endl;

	// pre-process before getting the contours
	img_p = processInput(img_src);
	img_contours = img_p.clone();

	// find all the contours
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	findContours(img_contours, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

	// iterate through first hierarchy level contours
	for (int i = 0; i < contours.size(); i = hierarchy[i][0])
	{
		// find bounding rectangle
		Rect rect = boundingRect(contours.at(i));

		// reject too small rectangles in height
		if(rect.height <= train_height_min)	continue;

		// draw a red rectangle to point data to train
		rectangle(img_src, Point(rect.x, rect.y), Point(rect.x + rect.width, rect.y + rect.height), Scalar(0, 0, 255));

		// crop the image with the rectangle contours and resize it to the matching size
		resize(img_p(rect), mat_tmp, Size(matching_size, matching_size), 0, 0, INTER_LINEAR);

		// convert to float
		mat_tmp.convertTo(img_contours, CV_32FC1);

		// read corresponding label for contour from keyboard
		int key = displayImage("Train Data", img_src);

		// convert number into integer
		if (key >= '0' && key <= '9')
		{
			// convert ASCII to integer value
			key -= '0';
		}
		// otherwise skip input (numbers only)
		else continue;

		cout << "number " << key << " trained" << endl;

		// store label
		response.push_back(key);

		// store data
		sample.push_back(img_contours.reshape(1, 1));

		// draw a green rectangle the rectangle
		rectangle(img_src, Point(rect.x, rect.y), Point(rect.x + rect.width, rect.y + rect.height), Scalar(0, 255, 0));
	}

	destroyWindow("Train Data");

	cout << "file trained" << endl;
}

void loadTrainedOCR(String filename, Mat& sample, Mat& response)
{
	Mat mat_tmp;

	// load data from filename
	FileStorage OCR(filename, FileStorage::READ);

	// get back the data
	OCR["sample"] >> sample;
	OCR["response"] >> response;
	OCR.release();

	// reshape a continuous matrix
	mat_tmp = response.reshape(1, response.cols);

	// convert it to signed data
	mat_tmp.convertTo(response, CV_32S);

	cout << "samples and responses loaded from " << filename << endl;
}

void saveTrainedOCR(String filename, Mat& sample, Mat& response)
{
	Mat mat_tmp;

	// reshape matrix for training
	mat_tmp = response.reshape(1, 1);

	// convert it to float
	mat_tmp.convertTo(response, CV_32F);

	// store data to filename
	FileStorage OCR(filename, FileStorage::WRITE);
	OCR << "sample" << sample << "response" << response;
	OCR.release();

	cout << "samples and responses saved to " << filename << endl;
}

int getNumberOCR(Mat img, Ptr<KNearest> knn)
{
	// clone before finding the contours
	Mat img_contours = img.clone();

	// to display the contours
	Mat img_color;
	cvtColor(img, img_color, CV_GRAY2BGR);

	// Apply the dilation operation
	//dilate(processInput(img), img_p, getStructuringElement(MORPH_CROSS, Size(3, 3), Point(1, 1)));

	// find the contours
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	// show big rectangle image
//	namedWindow("Number", CV_WINDOW_KEEPRATIO);
//	imshow("Number", img);

	// wait infinite time for a key press
//	waitKey(0);
//	destroyWindow("Number");

	// finding all the contours
	findContours(img_contours, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

	int best_number = 0, best_distance = INT32_MAX;

	// iterate through first hierarchy level contours
	for (auto i : contours)
	{
		Mat img_tmp, img_num;

		// create vectors to get the response and the distance
		vector<float> response, distance;

		// find bounding rectangle
		Rect rect = boundingRect(i);

		// only process rectangle with height correspond to the size of a number
		if(rect.height > matching_size * 0.8 && rect.height <= matching_size * 1.5)
		{
			// resize the image at the shape of the rectangle
			resize(img(boundingRect(i)), img_tmp, Size(matching_size, matching_size), 0, 0, INTER_LINEAR);
			// convert to float
			img_tmp.convertTo(img_num, CV_32FC1);

			// find nearest
			knn->findNearest(img_num.reshape(1, 1), 1, noArray(), response, distance);

			// show big rectangle image
			//cout << "rectangle height :" << rect.height << endl;
//			rectangle(img_color, rect.tl(), rect.br(), Scalar(0, 255, 0), 2);
//			namedWindow("Number to solve", CV_WINDOW_KEEPRATIO);
//			imshow("Number to solve", img_color);
//			waitKey(0);
//			destroyWindow("Number to solve");

			// display result
//			cout << (int) response.at(0) << " distance : " << (int) distance.at(0) << endl;

			// save the best result below a distance threshold
			if(distance.at(0) < minimum_matching_dist && distance.at(0) < best_distance)
			{
				best_distance = (int) distance.at(0);
				best_number = (int) response.at(0);
			}
		}
	}
	if(best_number != 0)
	{
		cout << "number solved : " << best_number << " @" << best_distance << endl;
	}
	return best_number;
}



