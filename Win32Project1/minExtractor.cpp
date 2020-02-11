#define _CRT_SECURE_NO_WARNINGS
#include"stdafx.h"
#include"minExtractor.h"
#include<iostream>
#include<windows.h>
#include<vector>
#include<opencv2\opencv.hpp>
#include<signal.h>

using namespace cv;
using namespace std;

typedef int(*FCreateTemplate)(unsigned char*, int, int, unsigned char*);

minExtractor::minExtractor()
{
}

int minExtractor::SYFp_File_LoadBMP(char * file, unsigned char * pImage, int * pX, int * pY)
{
	//Read bmp file into intended structure
	FILE *fp;
	int i, X, Y;
	unsigned char head[1078];

	X = *pX;
	Y = *pY;
	//Memory set 0
	memset(pImage, 0, 25600 * sizeof(char));

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

int minExtractor::featureExtract(char * src_img, unsigned char* TempFeature)
{
	//This function extracts the minutiae
	//Results stored in TempFeature
	//TempFeature[0]: number of keypoints (Up to 120)
	//One keypoint info is stored in 10 bytes(starts from TempFeature[2]) 
	//x coordinate (2 bytes)
	//y coordinate (2 bytes)
	//y coordinate (2 bytes)

	//Return keypoint amount or -1 (failure)
	unsigned char TempImage[160 * 160];
	int ret;
	int a, b;
	HINSTANCE hIn = NULL;

	hIn = LoadLibrary(_T("SYDllTest.dll"));
	if (hIn == INVALID_HANDLE_VALUE)
	{
		cout << "Load DLL error" << endl;
		return -1;
	}
	ret = SYFp_File_LoadBMP(src_img, TempImage, &a, &b);
	if (ret == -1)
	{
		cout << "Load BMP Error" << endl;
		return -1;
	}
	FCreateTemplate pFun = NULL;
	pFun = (FCreateTemplate)GetProcAddress(hIn, "CreateTemplate");
	ret = pFun(TempImage, 160, 160, TempFeature);
	if (ret != 1)
	{
		cout << "Keypoints Extraction Error" << endl;
		return -1;
	}
	return 1;
}

int minExtractor::cvtKeyPoint(unsigned char* TempFeature, vector<KeyPoint>* keypoints)
{
	//Convert TempFeature to keypoint vector
	int len;
	int i, j;
	Point2f point;
	float angle, m;
	int type;

	len = (int)TempFeature[0];
	if (len == 0)
		return 0;
	for (i = 2, j = 0; i < 2 + 10 * len; i += 10)
	{
		point = Point2f((float)TempFeature[i], (float)TempFeature[i + 2]);
		angle = (float)TempFeature[i + 4];
		type = (float)TempFeature[i + 6];
		m = (float)TempFeature[i + 8];
		(*keypoints)[j] = KeyPoint::KeyPoint(point, 32, angle, m, 0, type);
		j++;
	}
	/*
	vector<KeyPoint>::iterator KP;
	for (KP = keypoints.begin(); KP != keypoints.end(); KP++)
	{
		cout << "PT=" << KP->pt << " ";
		cout << "SIZE=" << KP->size << " ";
		cout << "ANGLE=" << KP->angle << " ";
		cout << "RESPONSE=" << KP->response << " ";
		cout << "OCTAVE=" << KP->octave << " ";
		cout << "ID=" << KP->class_id << " ";
		cout << endl;
	}
	cout << "end " << endl;
	cout << endl;
	*/
	return 1;

}

int minExtractor::cvtKeyPointSwitch(unsigned char * TempFeature, vector<KeyPoint>* keypoints, float dx, float dy)
{
	//Convert TempFeature to keypoint vector
	int len;
	int i, j;
	Point2f point;
	float angle, m;
	int type;

	len = (int)TempFeature[0];
	if (len == 0)
		return 0;
	for (i = 2, j = 0; i < 2 + 10 * len; i += 10)
	{
		point = Point2f((float)TempFeature[i] + dx, (float)TempFeature[i + 2] + dy);
		angle = (float)TempFeature[i + 4];
		type = (float)TempFeature[i + 6];
		m = (float)TempFeature[i + 8];
		(*keypoints)[j] = KeyPoint::KeyPoint(point, 32, angle, m, 0, type);
		j++;
	}
	/*
	vector<KeyPoint>::iterator KP;
	for (KP = keypoints.begin(); KP != keypoints.end(); KP++)
	{
	cout << "PT=" << KP->pt << " ";
	cout << "SIZE=" << KP->size << " ";
	cout << "ANGLE=" << KP->angle << " ";
	cout << "RESPONSE=" << KP->response << " ";
	cout << "OCTAVE=" << KP->octave << " ";
	cout << "ID=" << KP->class_id << " ";
	cout << endl;
	}
	cout << "end " << endl;
	cout << endl;
	*/
	return 1;

}

int minExtractor::rmEdge(Mat src, vector<KeyPoint>* keypoints)
{
	vector<KeyPoint>::iterator KP;
	Mat patch;
	Mat bi_patch;
	Scalar intens;
	float x, y;
	/*
	int i, j;
	while (i < (*keypoints).size)
	{
		patch = src(Rect((*keypoints)[i].pt.x, (*keypoints)[i].pt.y, 32, 32)).clone();
		
	}
	*/
	for (KP = (*keypoints).begin(); KP != (*keypoints).end(); )
	{
		x = KP->pt.x;
		y = KP->pt.y;
		if (x + 16 >= 159)
			x = 143;
		if (x - 16 <= 0)
			x = 16;
		if (y + 16 >= 159)
			y = 143;
		if (y - 16 <= 0)
			y = 16;
		patch = src(Rect(x, y, 16, 16)).clone();
		cout << "pt=" << KP->pt << endl;

		if (cv::mean(patch)[0] > cv::mean(src)[0])
		{
			/*(*keypoints).swap(KP, end(*keypoints) );
			(*keypoints).pop_back();
			(*keypoints).shrink_to_fit();
			*/

			/*
			if (KP + 1 == (*keypoints).end())
			{
				(*keypoints).pop_back();
				break;
			}
			else
			*/
				KP = (*keypoints).erase(KP); 
		}
		else
			KP++;
			//cout << "intens_patch = " << mean(patch)[0] << endl;
			//cout << "intens_src = " << mean(src)[0] << endl;
			//cout << endl;


	}
	//(*keypoints).shrink_to_fit();
	return 0;
}


minExtractor::~minExtractor()
{
}
