#pragma once
class analyzer
{
public:
	analyzer();
	void compareAndWrite(char* head_1, int begin_1, int end_1, char* head_2, int begin_2, int end_2, char* record_file, char* result_file);
	~analyzer();
};

