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
#include "matcher.h"


matcher::matcher()
{
}

float matcher::matching(char * file_1, char * file_2, float rate)
{
	float score;
	unsigned char TempFeature1[1202], TempFeature2[1202];
	minExtractor extractor;
	extractor.featureExtract(file_1, TempFeature1);
	extractor.featureExtract(file_2, TempFeature2);

	int len1 = (int)TempFeature1[0], len2 = (int)TempFeature2[0];
	vector<KeyPoint> keyPoints_1(len1), keyPoints_2(len2);
	extractor.cvtKeyPoint(TempFeature1, &keyPoints_1);
	extractor.cvtKeyPoint(TempFeature2, &keyPoints_2);

	//Print the keypoints
	vector<KeyPoint>::iterator KP;
	cout << "pic 1" << endl;
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
	cout << "pic 2" << endl;
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
	//进行匹配
	Mat img_1 = imread(file_1);
	Mat img_2 = imread(file_2);

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
	//convert to grey picture for orb compute
	cvtColor(img_1, d_srcL, CV_BGR2GRAY);
	cvtColor(img_2, d_srcR, CV_BGR2GRAY);

	//设置关键点间的匹配方式为NORM_L2，更建议使用 FLANNBASED = 1, BRUTEFORCE = 2, BRUTEFORCE_L1 = 3, BRUTEFORCE_HAMMING = 4, BRUTEFORCE_HAMMINGLUT = 5, BRUTEFORCE_SL2 = 6 
	OrbFeatureDetector featureDetector;
	OrbDescriptorExtractor featureExtractor;

	Mat d_descriptorsL, d_descriptorsR, d_descriptorsL_32F, d_descriptorsR_32F;

	featureExtractor.compute(d_srcL, keyPoints_1, d_descriptorsL);
	featureExtractor.compute(d_srcR, keyPoints_2, d_descriptorsR);

	Ptr<DescriptorMatcher> d_matcher = DescriptorMatcher::create("BruteForce");

	std::vector<DMatch> matches;
	std::vector<DMatch> good_matches;

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
		if (matches[i].distance < 0.7*max_dist)
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
	//imwrite("matches.png", ShowMatches);

	//画出良好匹配结果
	Mat ShowGoodMatches;
	drawMatches(img_1, keyPoints_1, img_2, keyPoints_2, good_matches, ShowGoodMatches);
	imshow("good_matches", ShowGoodMatches);
	//imwrite("good_matches.png", ShowGoodMatches);

	//画出良好匹配结果中在待测图片上的点集
	imshow("MatchPoints_in_img_2", img_2);
	//imwrite("MatchPoints_in_img_2.png", img_2);

	return good_matches.size();
}


matcher::~matcher()
{
}
