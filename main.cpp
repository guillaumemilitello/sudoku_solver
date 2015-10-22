/*
 * main.cpp
 *
 *  Created on: 18 Oct 2015
 *      Author: guillaume
 */

#include <opencv2/opencv.hpp>

#include "OCR.h"
#include "Sudoku.h"
#include "Image.h"

using namespace std;
using namespace cv;
using namespace cv::ml;

int main(int argc, char *argv[])
{
	// input image
	Mat img_src;

	// Read stored sample and label for training
	Mat sample, response;

	/*******************************/
	/* parse the different options */
	/*******************************/
	try
	{
		if (argc == 4 && !strcmp(argv[1], "-s"))
		{
			// get input image to solve the sudoku
			img_src = imread(argv[2], CV_LOAD_IMAGE_UNCHANGED);

			//check the image source
			if (img_src.empty()) throw String("the image " + string(argv[2]) + " cannot be loaded");

			loadTrainedOCR(argv[3], sample, response);
		}
		else if (argc >= 4 && !strcmp(argv[1], "-t"))
		{
			// parse options
			int i = 2;

			// train data
			while (++i < argc)
			{
				// train data of every file
				trainOCR(argv[i], sample, response);

				// save trained samples and responses to the last filename
				if (i == argc - 1)	saveTrainedOCR(argv[2], sample, response);
			}
			return 0;
		}
		else if (argc >= 4 && !strcmp(argv[1], "-tl"))
		{
			// parse options
			int i = 2;

			// train data
			while (++i < argc)
			{
				// read trained data
				if (i == 2)	loadTrainedOCR(argv[2], sample, response);

				// train data of every file
				else
				{
					trainOCR(argv[i], sample, response);

					// save trained samples and responses to the last filename
					if (i == argc - 1) saveTrainedOCR(argv[2], sample, response);
				}
			}
			return 0;
		}
		else
		{
			cout << "input error" << endl;
			cout << "-s  <sudoku image file> <trained OCR data file>to solve a sudoku" << endl;
			cout << "-t  <trained OCR data file> <trainning image files> to train a new OCR" << endl;
			cout << "-tl <trained OCR data file> <trainning image files> to enhance the OCR" << endl;
			return 0;
		}
	}
	catch (cv::Exception& e)
	{
		cerr << "OpenCV error while running : "  << e.what() << endl;
		return 0;
	}
	catch (String& s)
	{
		cerr << "error while running : " << s << endl;
		return 0;
	}

	// images transformed, numbers
	Mat img_tr, img_num;

	// sudoku numbers data
	int grid[9][9], grid_solved[9][9];

	// knearest pointer creation
	Ptr<KNearest> knn(KNearest::create());

	/****************************************/
	/* train with the samples and responses */
	/****************************************/
	knn->train(sample, ROW_SAMPLE, response);
	cout << "OCR initialization complete" << endl;

	// rectangle delimiting the sudoku grid
	Rect sudoku_rect;

	// create window
	namedWindow("Sudoku", WINDOW_AUTOSIZE);

	// show source image
	displayImage("Sudoku", img_src);

	// get a clean transformed sudoku grid
	transformSudoku(img_src, img_tr, sudoku_rect);

	// draw and show the sudoku grid on the input image
	rectangle(img_src, sudoku_rect.tl(), sudoku_rect.br(), Scalar(0, 255, 0), 2);
	displayImage("Sudoku", img_src);

	// pre-process the transformed input
	Mat img_p = processInput(img_tr);

	/************************************************/
	/* recognize the number for each of the segment */
	/************************************************/
	for (int y = 0; y < 9; ++y)
	{
		for (int x = 0; x < 9; ++x)
		{
			// get only the grid according to the coordinates
			segmentSudoku(img_p, img_num, x, y);

			// use the OCR data to recognize the number and fill the grid
			grid[x][y] = getNumberOCR(img_num, knn);
		}
	}

	Sudoku su(grid);

	// check the viability of the input grid
	if (!su.checkGrid())
	{
		cerr << "issues into the input grid : " << endl << su << endl;
		waitKey(0);
		return -1;
	}

	// display original sudoku
	cout << "input sudoku : " << endl << su << endl;

	// solve the sudoku
	if (sudokuSolve(su))
	{
		// get the solved grid
		su.getGrid(grid_solved);

		// display the solution on the console
		cout << "solution :" << endl << su << endl;
	}

	// remove original number from the solved grid for not drawing it
	for (int y = 0; y < 9; ++y)
	{
		for (int x = 0; x < 9; ++x)
		{
			// draw number
			if (grid[x][y] != 0)
			{
				grid_solved[x][y] = 0;
			}
		}
	}

	/****************************************/
	/* draw the solution on the input image */
	/****************************************/
	drawSudoku(img_src, sudoku_rect, grid_solved);

	// show result image
	displayImage("Sudoku", img_src);

	// destroy window
	destroyWindow("Sudoku");

	return 0;
}

