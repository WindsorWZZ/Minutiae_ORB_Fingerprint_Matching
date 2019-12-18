// MinutiaeORB_Fingerprint_Matching.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <windows.h>
#include <iostream>
#include<fstream>
#include<signal.h>
#include<vector>
#include<stdio.h>
#include <string>
#include<opencv2/opencv.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/features2d/features2d.hpp>
#include"minExtractor.h"
#include"matcher.h"
#include"analyzer.h"


using namespace std;
using namespace cv;

int _tmain(int argc, _TCHAR* argv[])
{
	analyzer Ana;
	char head_1[] = "data/05001-3400";
	char head_2[] = "data/05001-5200";
	char head_3[] = "data/05009-1400";
	char head_4[] = "data/05046-4900";
	char head_5[] = "data/05048-1800";
	char head_6[] = "data/05048-1900";
	char head_7[] = "data/05052-2400";
	char head_8[] = "data/05052-2600";
	char head_9[] = "data/05055-1200";
	char* head[9];
	head[0] = head_1;
	head[1] = head_2;
	head[2] = head_3;
	head[3] = head_4;
	head[4] = head_5;
	head[5] = head_6;
	head[6] = head_7;
	head[7] = head_8;
	head[8] = head_9;

	for (int num = 0; num < 99; num++)
	{
		for (int i = 0; i < 9; i++)
			Ana.compareAndWrite(head[0], num, num, head[i], 0, 99, "results/rejFake.txt", "results/results.txt");

		ofstream fout("results/results.txt", ios_base::out | ios_base::app);
		fout << endl;
		fout.close();
	}
	
	return 0;
}


