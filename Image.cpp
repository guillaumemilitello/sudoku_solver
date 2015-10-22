/*
 * Image.cpp
 *
 *  Created on: 21 Oct 2015
 *      Author: guillaume
 */

#include "Image.h"

int displayImage(const String display_name, Mat img)
{
	Mat display;

	// keep the ratio, resize and display
	resize(img, display, Size(400, (int) ((double) img.rows / img.cols * 400)));
	imshow(display_name, display);

	// wait for the user to respond
	return waitKey(0);
}

Mat processInput(Mat img)
{
	Mat img_gray, img_adpth;

	// convert to gray
	if(img.channels() == 3 || img.channels() == 4)
	{
		cvtColor(img, img_gray, CV_BGR2GRAY);
	}
	else
	{
		img_gray = img.clone();
	}

	// adaptive threshold
	adaptiveThreshold(img_gray, img_adpth, 255, 1, 1, 15, 10);

	return img_adpth;
}

void transformSudoku(Mat img, Mat& img_tr, Rect& big_rect)
{
	// pre-process the input image
	Mat img_contours = processInput(img);

	 // find all the contours
	 vector<vector<Point> > contours;
	 vector<Vec4i> hierarchy;

	 findContours(img_contours, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

	 // biggest rectangle
	 int rect_size_max = 0;

	 for(auto i: contours)
	 {
		 vector<Point> contours_poly;

		 // approximate contours to polygons
		 approxPolyDP(Mat(i), contours_poly, 4, true);

		 // only process rectangle polygons
		 if(contours_poly.size() == 4 && isContourConvex(contours_poly))
		 {
			 int rect_size = contourArea(contours_poly, false);

			 // area of the polygon
			 if(rect_size > rect_size_max)
			 {
				 // save the biggest rectangle
				 rect_size_max = rect_size;
				 big_rect = boundingRect(Mat(contours_poly));
			 }
		 }
	 }

	 // size of the output image fitting the OCR matching size
	 Size img_tr_size;
	 img_tr_size.width = case_size * 9;
	 img_tr_size.height = case_size * 9;

	 // grid coordinates
	 Point2f in_c[4], out_c[4];

	 // fitting the size of the biggest rectangle
	 in_c[0] = Point2f(big_rect.x,                  big_rect.y);
	 in_c[1] = Point2f(big_rect.x + big_rect.width, big_rect.y);
	 in_c[2] = Point2f(big_rect.x + big_rect.width, big_rect.y + big_rect.height);
	 in_c[3] = Point2f(big_rect.x,                  big_rect.y + big_rect.height);

	 // fitting the size of the OCR matching size
	 out_c[0] = Point2f(0,                 0);
	 out_c[1] = Point2f(img_tr_size.width, 0);
	 out_c[2] = Point2f(img_tr_size.width, img_tr_size.height);
	 out_c[3] = Point2f(0,                 img_tr_size.height);

	// perspective transform from the input image
	warpPerspective(img, img_tr, getPerspectiveTransform(in_c, out_c), img_tr_size);
}

void segmentSudoku(Mat img, Mat& img_num, int x, int y)
{
	int x_offset = max(0, x * case_size - crop_size);
	int y_offset = max(0, y * case_size - crop_size);
	int width = case_size + crop_size;
	int height =  case_size + crop_size;

	img(Rect(x_offset, y_offset, width, height)).copyTo(img_num);

}

void drawSudoku(Mat img, Rect rect, int grid[9][9])
{
	// draw missing numbers
	for (int y = 0; y < 9; ++y)
	{
		for (int x = 0; x < 9; ++x)
		{
			// draw number
			if(grid[x][y] != 0)
			{
				// adjust on the input image
				float sudoku_rect_x_offset = rect.x + (rect.width / 9) * (x + 0.25);
				float sudoku_rect_y_offset = rect.y + (rect.height / 9) * (y + 0.75);
				float font_scale = rect.height * 0.0035 - 0.2;

				// draw solved number
				putText(img, to_string(grid[x][y]), Point2f(sudoku_rect_x_offset, sudoku_rect_y_offset), FONT_HERSHEY_SIMPLEX, font_scale, Scalar(0,255,0,255));
			}
		}
	}
}

