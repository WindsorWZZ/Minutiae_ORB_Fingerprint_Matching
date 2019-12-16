#pragma once
#include "stdafx.h"
#include <windows.h>
#include <iostream>
#include<signal.h>
#include<vector>
#include<stdio.h>
#include<opencv2/opencv.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/features2d/features2d.hpp>

using namespace std;
using namespace cv;

class minExtractor
{
public:
	minExtractor();
	int SYFp_File_LoadBMP(char* file, unsigned char* pImage, int* pX, int* pY);
	int featureExtract(char * src_img, unsigned char* TempFeature);
	int cvtKeyPoint(unsigned char* TempFeature, vector<KeyPoint>* keypoints);
	~minExtractor();
};

