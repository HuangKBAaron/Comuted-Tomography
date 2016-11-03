#include<Windows.h>
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include<cuda.h>
#include <stdio.h>
#include<time.h>
#include <iostream>
#include<device_atomic_functions.h>
#include "opencv2/opencv.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <cuda_runtime_api.h>
#include<stdlib.h>
#include<device_functions.h>
#include "opencv2\core\core.hpp"

#include<fstream>
#include<algorithm>
#define PATCH 5
#define WINDOW 10
#define IMG_CNT 5
#define usg unsigned char 

using namespace cv;
using namespace std;

typedef struct point{
	int sum, image, x, y;
}point;

boolean comparator(const point &p1, const point &p2)
{
	if (p1.sum < p2.sum)
		return true;
	else if (p1.sum>p2.sum)
		return false;
	else return p1.image < p2.image;
}
__global__ void calc_diff(usg *cuda_s, usg *cuda_o, point sol[][102][25*IMG_CNT], int rows, int cols) {
	sol[blockIdx.y][blockIdx.x][threadIdx.x].sum = 99999999;
	sol[blockIdx.y][blockIdx.x][threadIdx.x].image = -1;
	sol[blockIdx.y][blockIdx.x][threadIdx.x].x = -1;
	sol[blockIdx.y][blockIdx.x][threadIdx.x].y = -1;
	int x = blockIdx.y * 5;
	int y = blockIdx.x * 5;
	int image_index = threadIdx.x % 25;
	int x_test = x - 10 + (image_index / 5) * 5;
	int y_test = y - 10 + (image_index % 5) * 5;

	if (x_test >= 0 && x_test < rows - 5 && y_test >= 0 && y_test < cols - 5 && x >= 0 && x < rows - 5 && y >= 0 && y < cols - 5) {
		int sum = 0;
		for (int m = 0; m < PATCH; m++)
			for (int n = 0; n < PATCH; n++)
				sum += ((int)cuda_s[(x + m)*cols + y + n] - (int)cuda_o[(threadIdx.x / 25) * 512 * 512 + (x_test + m)*cols + y_test + n])*((int)cuda_s[(x + m)*cols + y + n] - (int)cuda_o[(threadIdx.x / 25) * 512 * 512 + (x_test + m)*cols + y_test + n]);
		sol[blockIdx.y][blockIdx.x][threadIdx.x].sum = sum;
		sol[blockIdx.y][blockIdx.x][threadIdx.x].image = threadIdx.x  / 25;
		sol[blockIdx.y][blockIdx.x][threadIdx.x].x = x_test;
		sol[blockIdx.y][blockIdx.x][threadIdx.x].y = y_test;
	}
	__syncthreads();
}

int main()
{
	LARGE_INTEGER frequency; // ticks per second
	LARGE_INTEGER t1, t2; // ticks
	double elapsedTime;

	Mat subject = imread("C:\\Users\\User\\Downloads\\subject.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	Mat other[5];
	other[0] = imread("C:\\Users\\User\\Downloads\\sample images\\1.png", CV_LOAD_IMAGE_GRAYSCALE);
	other[1] = imread("C:\\Users\\User\\Downloads\\sample images\\2.png", CV_LOAD_IMAGE_GRAYSCALE);
	other[2] = imread("C:\\Users\\User\\Downloads\\sample images\\3.png", CV_LOAD_IMAGE_GRAYSCALE);
	other[3] = imread("C:\\Users\\User\\Downloads\\sample images\\4.png", CV_LOAD_IMAGE_GRAYSCALE);
	other[4] = imread("C:\\Users\\User\\Downloads\\sample images\\5.png", CV_LOAD_IMAGE_GRAYSCALE);
	

	if (!subject.data)
	{
		cout << "Subject Image not found\nTerminaion process\n";
		return 0;

	}

	for (int i = 0; i < IMG_CNT; i++)
		if (!other[i].data)
			cout << "Image cant be loaded " << i << "\n";

	usg *cuda_s, *cuda_o, *s, *o;

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&t1);
	typedef point  narray[102][25*IMG_CNT];
	narray  *cuda_sol, *sol;
	sol = (narray *)malloc(102 * 102 * 25 * IMG_CNT * sizeof(point));

	cudaMalloc((void **)&cuda_sol, (102 * 102 * 25 * IMG_CNT)*sizeof(point));

	s = (usg *)malloc(sizeof(char)*subject.rows*subject.cols);
	o = (usg *)malloc(sizeof(char) * 512 * 512 * IMG_CNT);

	cudaMalloc((void **)&cuda_s, sizeof(char)*subject.rows*subject.cols);


		cudaMalloc((void **)&cuda_o, sizeof(char)*512*512*IMG_CNT);

	for (int i = 0; i < subject.rows*subject.cols; i++)
		s[i] = subject.at<uchar>(i / subject.cols, i%subject.cols);
	for (int k = 0; k < IMG_CNT; k++)
		for (int i = 0; i < other[k].rows*other[k].cols; i++)
			o[k * 512 * 512 + i] = other[k].at<uchar>(i / other[k].cols, i%other[k].cols);

	cudaMemcpy(cuda_s, s, sizeof(char)*subject.rows*subject.cols, cudaMemcpyHostToDevice);
	cudaMemcpy(cuda_o, o, sizeof(char)*512*512*IMG_CNT, cudaMemcpyHostToDevice);

	ofstream myfile;
	myfile.open("C:\\Users\\User\\Downloads\\Prior_study\\game_cuda.txt");

	dim3 blockDim(25 * IMG_CNT);
	dim3 gridDim(102,102);
	
	
	calc_diff << <gridDim, blockDim >> > (cuda_s, cuda_o, cuda_sol, subject.rows, subject.cols);
	cudaMemcpy(sol, cuda_sol, 102 * 102 * 25 * IMG_CNT * sizeof(point), cudaMemcpyDeviceToHost);


	QueryPerformanceCounter(&t2);
	// compute and print the elapsed time in millisec
	elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 /
		frequency.QuadPart;

	for (int i = 0; i < 102; i++)
		for (int j = 0; j < 102; j++){
			sort(sol[i][j], sol[i][j] + 25*IMG_CNT,&comparator);
			
				myfile <<"subject image row = "<<i*5<<" "<<"subject image column = "<<5*j<<" "<< "Difference = "<<(int)sol[i][j][0].sum << " " << "otherimage number = "<<(int)sol[i][j][0].image << " " << "other image row = "<<(int)sol[i][j][0].x << " " <<"other image column = "<< (int)sol[i][j][0].y << "\t";
			
			myfile << "\n\n\n";
		}

	cout << elapsedTime << " ms.\n";

	cudaFree(cuda_s);
	cudaFree(cuda_o);
	cudaFree(cuda_sol);
	free(sol);
	free(s);
	free(o);
	waitKey(0);
	return 0;
}