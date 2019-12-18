#include "stdafx.h"
#include "analyzer.h"
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


analyzer::analyzer()
{
}

void analyzer::compareAndWrite(char* head_1, int begin_1, int end_1, char* head_2, int begin_2, int end_2, char* record_file, char* result_file)
{
	float score;
	matcher match;
	//char head[] = "data/05001-3400";
	char pic_1[] = "data/00000-0000000000", pic_2[] = "data/00000-0000000000";
	int i, j;
	char number[] = "00";
	int score_sum;
	//open the file and write at its end
	//fout_1:record every match of other pics
	//fout_2:record the score of each fingerprint
	ofstream fout_1(record_file, ios_base::out | ios_base::app);
	ofstream fout_2(result_file, ios_base::out | ios_base::app);
	for (i = begin_1; i <= end_1; i++)
	{
		score_sum = 0;
		if (i / 10 && i % 10)
		{
			number[0] = (char)('0' + i / 10);
			number[1] = (char)('0' + i % 10);
		}
		else if (!(i % 10) && (i / 10))
		{
			number[0] = (char)('0' + i / 10);
			number[1] = '0';
		}
		else if ((i % 10) && !(i / 10))
		{
			number[0] = '0';
			number[1] = (char)('0' + i % 10);
		}
		else
		{
			number[0] = '0';
			number[1] = '0';
		}
		strcpy(pic_1, head_1);
		strcat(pic_1, number);
		strcat(pic_1, ".bmp");
		for (j = begin_2; j <= end_2; j++)
		{
			if (j / 10 && j % 10)
			{
				number[0] = (char)('0' + j / 10);
				number[1] = (char)('0' + j % 10);
			}
			else if (!(j % 10) && (j / 10))
			{
				number[0] = (char)('0' + j / 10);
				number[1] = '0';
			}
			else if ((j % 10) && !(j / 10))
			{
				number[0] = '0';
				number[1] = (char)('0' + j % 10);
			}
			else
			{
				number[0] = '0';
				number[1] = '0';
			}

			strcpy(pic_2, head_2);
			strcat(pic_2, number);
			strcat(pic_2, ".bmp");
			score = match.matching(pic_1, pic_2, 0.65f);
			score_sum += score;
			//write txt
			fout_1 << pic_1 << "    vs   " << pic_2 << " = " << score << endl;
			cout << pic_1 << "    vs   " << pic_2 << " = " << score << endl;
		}
		fout_2 << pic_1 << " scores " << score_sum << " in " << head_2 << endl;
	}

	fout_1.close();
	fout_2.close();
}


analyzer::~analyzer()
{
}
