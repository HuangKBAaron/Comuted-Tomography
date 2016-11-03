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
#include<fstream>
using namespace cv;
using namespace std;

#define PATCH 5
#define WINDOW 10
int main()
{

	LARGE_INTEGER frequency; // ticks per second
	LARGE_INTEGER t1, t2; // ticks
	double elapsedTime;

	//return 0;
	Mat subject = imread("C:\\Users\\User\\Downloads\\Prior_study\\Prior_study\\subject.png", CV_LOAD_IMAGE_GRAYSCALE);
	Mat other[20];
	other[0]= imread("C:\\Users\\User\\Downloads\\Prior_study\\Prior_study\\99518.png", CV_LOAD_IMAGE_GRAYSCALE);
	other[1] = imread("C:\\Users\\User\\Downloads\\Prior_study\\Prior_study\\99519.png", CV_LOAD_IMAGE_GRAYSCALE);
	other[2] = imread("C:\\Users\\User\\Downloads\\Prior_study\\Prior_study\\99520.png", CV_LOAD_IMAGE_GRAYSCALE);
	other[3] = imread("C:\\Users\\User\\Downloads\\Prior_study\\Prior_study\\99521.png", CV_LOAD_IMAGE_GRAYSCALE);
	other[4] = imread("C:\\Users\\User\\Downloads\\Prior_study\\Prior_study\\99522.png", CV_LOAD_IMAGE_GRAYSCALE);
	other[5] = imread("C:\\Users\\User\\Downloads\\Prior_study\\Prior_study\\99523.png", CV_LOAD_IMAGE_GRAYSCALE);
	other[6] = imread("C:\\Users\\User\\Downloads\\Prior_study\\Prior_study\\1.png", CV_LOAD_IMAGE_GRAYSCALE);
	other[7] = imread("C:\\Users\\User\\Downloads\\Prior_study\\Prior_study\\2.png", CV_LOAD_IMAGE_GRAYSCALE);
	other[8] = imread("C:\\Users\\User\\Downloads\\Prior_study\\Prior_study\\3.png", CV_LOAD_IMAGE_GRAYSCALE);
	other[9] = imread("C:\\Users\\User\\Downloads\\Prior_study\\Prior_study\\4.png", CV_LOAD_IMAGE_GRAYSCALE);
	other[10] = imread("C:\\Users\\User\\Downloads\\Prior_study\\Prior_study\\5.png", CV_LOAD_IMAGE_GRAYSCALE);
	other[11] = imread("C:\\Users\\User\\Downloads\\Prior_study\\Prior_study\\6.png", CV_LOAD_IMAGE_GRAYSCALE);

	if (!subject.data)
	{
		cout << "Subject Image not found\nTerminaion process\n";
		return 0;

	}

	for (int i = 0; i < 12; i++)
		if (!other[i].data)
			cout << "Image cant be loaded " << i << "\n";
	
//	namedWindow("subject", CV_WINDOW_FREERATIO);
//	imshow("subject", subject);

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&t1);

	vector< pair< pair < double,int>, pair<int, int> > > *sol;
		sol = new vector < pair < pair<double,int>, pair<int, int> > > [200000]();
	//	sol[19][2000].push_back(make_pair(0, make_pair(0,0)));
	int q=0;

	
		for (int i = 0; i <=subject.rows - PATCH ; i+=PATCH)
			for (int j = 0; j <= subject.cols - PATCH; j += PATCH)
			{
				for (int w = 0; w < 12; w++)
				{
					for (int k = i - WINDOW; k <= i + WINDOW  && k <= other[w].rows - PATCH ; k += PATCH)
					{
						for (int l = j - WINDOW; l <= j + WINDOW  && l <= other[w].cols - PATCH ; l += PATCH)
						{
							if (k >= 0 && l >= 0){
								double sum = 0;
								for (int m = 0; m < PATCH; m++)
									for (int n = 0; n < PATCH; n++)
										sum += pow((int)subject.at<uchar>(i + m, j + n) - (int)other[w].at<uchar>(k + m, l + n), 2);

								sol[q].push_back(make_pair(make_pair(sum, w), make_pair(k, l)));
							}
						}
					}
					
				}
				q++;
			}
		QueryPerformanceCounter(&t2);
		// compute and print the elapsed time in millisec
		elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 /
			frequency.QuadPart;

		for (int i = 0; i < 200000; i++)
			sort(sol[i].begin(), sol[i].end());
	
	ofstream myfile;
	myfile.open("C:\\Users\\User\\Downloads\\Prior_study\\game.txt");



	for (int i = 0; i < q; i++)
	{
		for (int j = 0; j < sol[i].size() && j<5 ; j++)
			myfile <<(int) sol[i][j].first.first << " "<<(int)sol[i][j].first.second<<" ";
		myfile <<"\n\n\n";
	}

	cout << elapsedTime << " ms.\n";

		waitKey(0);
	return 0;
}