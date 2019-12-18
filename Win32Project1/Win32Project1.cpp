// MinutiaeORB_Fingerprint_Matching.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <iostream>
#include<signal.h>
#include<vector>
#include<stdio.h>
#include<opencv2/opencv.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/features2d/features2d.hpp>
#include"minExtractor.h"
#include"matcher.h"


using namespace std;
using namespace cv;

int _tmain(int argc, _TCHAR* argv[])
{
	float score;
	matcher match;
	score = match.matching("1.bmp", "4.bmp", 0.7f);
	cout << "good match=" << score << endl;
	waitKey(1000000);
	return 0;
}


