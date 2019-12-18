#pragma once
#include <windows.h>
#include <iostream>
#include<signal.h>
#include<vector>
#include<stdio.h>
#include<opencv2/opencv.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/features2d/features2d.hpp>
#include"minExtractor.h"
class matcher
{
public:
	matcher();
	float matching(char* img_1, char* img_2, float rate);
	~matcher();
};

