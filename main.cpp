#include "stdafx.h"
#include<Windows.h>
#include<iostream>
#include<stdlib.h>
#include "opencv2/opencv.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include "opencv2/core/core.hpp"
#include<vector>
#include<utility>
#include<math.h>
#include<algorithm>
using namespace cv;
using namespace std;

#define PATCH 6
#define WINDOW 15
int t()
{
	//return 0;
	Mat subject = imread("C:\\Users\\User\\Downloads\\Prior_study\\Prior_study\\subject.png", CV_LOAD_IMAGE_GRAYSCALE);
	Mat other = imread("C:\\Users\\User\\Downloads\\Prior_study\\Prior_study\\99518.png", CV_LOAD_IMAGE_GRAYSCALE);
	if (!subject.data)
	{
		cout << "Subject Image not found\nTerminaion process\n";
		return 0;

	}

	if (!other.data)
	{
		cout << "Other Image not found\nTerminaion process\n";
		return 0;

	}
	
/*	namedWindow("subject", CV_WINDOW_FREERATIO);
	imshow("subject", subject);
	namedWindow("other", CV_WINDOW_FREERATIO);
	imshow("other", other);
	*/

	vector<pair<double, pair<int, int> > > *sol;
	sol = new vector < pair <double, pair<int, int> > >[260000]();
	
	for (int i = 3; i < subject.rows - 3; i++)
	{
		for (int j = 3; j < subject.cols - 3; j++)
		{
			for (int k = i - 4; k <= i + 4 && k < other.rows - 3 && k >= 3; k++)
			{
				for (int l = j - 4; l <= j + 4 && l < other.cols - 3 && l >= 3; l++)
				{
					double sum = 0;
					for (int m = - PATCH / 2; m <= PATCH / 2; m++)
					{
						for (int n = - PATCH / 2; n <= PATCH / 2; n++)
						{
							sum += pow((int)subject.at<uchar>(i + m, j + n) - (int)other.at<uchar>(k + m, l + n), 2);
						}
					}
					pair<int, int> p;
					p = make_pair(k, l);
						sol[(i - 3)*(subject.cols-6) + j-3].push_back(make_pair(sum,p));
				}
			}
		}
	
	}

	for (int i = 0; i < 260000; i++)
		sort(sol[i].begin(), sol[i].end());
	waitKey(0);
	return 0;
}