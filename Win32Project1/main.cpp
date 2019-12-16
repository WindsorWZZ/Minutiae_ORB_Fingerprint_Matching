// Win32Project1.cpp : Defines the entry point for the console application.
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
#define _CRT_SECURE_NO_WARNINGS
using namespace std;
using namespace cv;

typedef int(*FCreateTemplate)(unsigned char*, int, int, unsigned char*);

int SYFp_File_LoadBMP(char* file, unsigned char* pImage, int* pX, int* pY)
{
	FILE *fp;
	int i, X, Y;
	unsigned char head[1078];

	X=*pX;
	Y=*pY;
	//Memory set 0
	memset(pImage, 0, 25600* sizeof(char));

	//When read the 'bmp' data, the order is from bottom to top and from left to right
	//The order we need is from top to bottom and from left to right
	//The data starts from the last row, while 'save' starts from the begin of the cache
	fp = fopen((const char*)file, "rb");
	if (fp == NULL)
		return -1;
	fread(head, 26, 1, fp);

	X = head[18] + (head[19] << 8) + (head[20] << 8) + (head[21] << 8);
	Y = head[22] + (head[23] << 8) + (head[24] << 8) + (head[25] << 8);
	*pX = X;
	*pY = Y;
	for (i = 0; i<Y; i++)
	{
		fseek(fp, (1078 + (Y - 1)*X) * sizeof(char) - i*X, SEEK_SET);
		fread(pImage + i*X, X * sizeof(char), 1, fp);
	}
	fclose(fp);

	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	unsigned char TempImage1[160 * 160], TempImage2[160 * 160];
	unsigned char TempFeature1[1202], TempFeature2[1202];
	int ret1, ret2;
	int x, y;
	int x1, x2;
	int y1, y2;

	HINSTANCE hIn = NULL;
	hIn = LoadLibrary(_T("SYDllTest.dll"));

	if (hIn == INVALID_HANDLE_VALUE)
	{

		cout << "Load DLL error" << endl;

		return -1;

	}

	SYFp_File_LoadBMP("1.bmp", TempImage1, &x1, &y1);
	SYFp_File_LoadBMP("2.bmp", TempImage2, &x2, &y2);
	FCreateTemplate pFun = NULL;
	pFun = (FCreateTemplate)GetProcAddress(hIn, "CreateTemplate");

	ret1 = pFun(TempImage1, 160, 160, TempFeature1);
	ret2 = pFun(TempImage2, 160, 160, TempFeature2);

	
	//Convert the feature to keypoint vector
	int i, j;
	Point2f point;
	float angle, m;
	int type;
	int len1, len2;
	len1 = (int)TempFeature1[0];
	len2 = (int)TempFeature2[0];
	vector<KeyPoint> keyPoints_1(len1), keyPoints_2(len2);
	vector<KeyPoint>::iterator KP;
	for ( i = 2, j = 0; i < 2 + 10 * len1; i += 10 )
	{
		point = Point2f((float)TempFeature1[i], (float)TempFeature1[i + 2]);
		angle = (float)TempFeature1[i + 4];
		type = (float)TempFeature1[i + 6];
		m = (float)TempFeature1[i + 8];
		keyPoints_1[j] = KeyPoint::KeyPoint(point, m, angle, 1e-5, 0, -1);
		j++;
	}
	for (i = 2, j = 0; i < 2 + 10 * len2; i += 10)
	{
		point = Point2f((float)TempFeature2[i], (float)TempFeature2[i + 2]);
		angle = (float)TempFeature2[i + 4];
		type = (float)TempFeature2[i + 6];
		m = (float)TempFeature2[i + 8];
		keyPoints_2[j] = KeyPoint::KeyPoint(point, m, angle, 1e-5, 0, -1);
		j++;
	}
	

	//进行匹配
	Mat img_1 = imread("1.bmp");
	Mat img_2 = imread("2.bmp");

	if (!img_1.data || !img_2.data)
	{
		cout << "error reading images " << endl;
		return -1;
	}

	vector<Point2f> recognized;
	vector<Point2f> scene;

	recognized.resize(500);
	scene.resize(500);

	Mat d_srcL, d_srcR;

	Mat img_matches, des_L, des_R;
	//ORB算法的目标必须是灰度图像
	cvtColor(img_1, d_srcL, CV_BGR2GRAY);//CPU版的ORB算法源码中自带对输入图像灰度化，此步可省略
	cvtColor(img_2, d_srcR, CV_BGR2GRAY);





	//设置关键点间的匹配方式为NORM_L2，更建议使用 FLANNBASED = 1, BRUTEFORCE = 2, BRUTEFORCE_L1 = 3, BRUTEFORCE_HAMMING = 4, BRUTEFORCE_HAMMINGLUT = 5, BRUTEFORCE_SL2 = 6 
	OrbFeatureDetector featureDetector;
	OrbDescriptorExtractor featureExtractor;

	Mat d_descriptorsL, d_descriptorsR, d_descriptorsL_32F, d_descriptorsR_32F;

	featureExtractor.compute(d_srcL, keyPoints_1, d_descriptorsL);
	featureExtractor.compute(d_srcR, keyPoints_2, d_descriptorsR);

	Ptr<DescriptorMatcher> d_matcher = DescriptorMatcher::create("BruteForce");

	std::vector<DMatch> matches;
	//普通匹配
	std::vector<DMatch> good_matches;
	//通过keyPoint之间距离筛选匹配度高的匹配结果

	d_matcher->match(d_descriptorsL, d_descriptorsR, matches);

	int sz = matches.size();
	double max_dist = 0; double min_dist = 100;

	for (int i = 0; i < sz; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}

	cout << "\n-- Max dist : " << max_dist << endl;
	cout << "\n-- Min dist : " << min_dist << endl;

	for (int i = 0; i < sz; i++)
	{
		if (matches[i].distance < 0.6*max_dist)
		{
			good_matches.push_back(matches[i]);
		}
	}
	//提取良好匹配结果中在待测图片上的点集，确定匹配的大概位置
	for (size_t i = 0; i < good_matches.size(); ++i)
	{
		scene.push_back(keyPoints_2[good_matches[i].trainIdx].pt);
	}

	for (unsigned int j = 0; j < scene.size(); j++)
		cv::circle(img_2, scene[j], 2, cv::Scalar(0, 255, 0), 2);
	//画出普通匹配结果
	Mat ShowMatches;
	drawMatches(img_1, keyPoints_1, img_2, keyPoints_2, matches, ShowMatches);
	imshow("matches", ShowMatches);
	imwrite("matches.png", ShowMatches);

	//画出良好匹配结果
	Mat ShowGoodMatches;
	drawMatches(img_1, keyPoints_1, img_2, keyPoints_2, good_matches, ShowGoodMatches);
	imshow("good_matches", ShowGoodMatches);
	imwrite("good_matches.png", ShowGoodMatches);

	//画出良好匹配结果中在待测图片上的点集
	imshow("MatchPoints_in_img_2", img_2);
	//imwrite("MatchPoints_in_img_2.png", img_2);
	
	/*
	for ( i = 2 ; i < 2 + 10*TempFeature1[0] ; i+=10 )
	{
		cout<< (float)TempFeature1[i]<<endl;
		cout<< (float)TempFeature1[i + 2]<<endl;
		cout<< (float)TempFeature1[i + 4]<<endl;
		cout<< (float)TempFeature1[i + 6]<<endl;
		cout<< (float)TempFeature1[i + 8] << endl;
		//keyPoints_1(j) = KeyPoint::KeyPoint(x, y, m, angle, 1, 0, -1)
	}
	*/

	//Output the converted keypoint vector
	/******************************
	for (KP = keyPoints_1.begin(); KP != keyPoints_1.end(); KP++)
	{
		cout << "PT=" << KP->pt << " ";
		cout << "SIZE=" << KP->size << " ";
		cout << "ANGLE=" << KP->angle << " ";
		cout << "RESPONSE=" << KP->response << " ";
		cout << "OCTAVE=" << KP->octave << " ";
		cout << "ID=" << KP->class_id << " ";
		cout << endl;
	}
	cout << "end of 1" << endl;
	cout << endl;
	for (KP = keyPoints_2.begin(); KP != keyPoints_2.end(); KP++)
	{
		cout << "PT=" << KP->pt << " ";
		cout << "SIZE=" << KP->size << " ";
		cout << "ANGLE=" << KP->angle << " ";
		cout << "RESPONSE=" << KP->response << " ";
		cout << "OCTAVE=" << KP->octave << " ";
		cout << "ID=" << KP->class_id << " ";
		cout << endl;
	}
	*/

	cout << "ret=" << ret1 << endl;
	cout << "ret=" << ret2 << endl;
	waitKey(1000000);
	return 0;
}


