#include "LogFile.h"
#include <iostream>
#include <fstream>
using namespace std;

LogFile::LogFile()
{
	// create temp file in C:\Users\%USERNAME%\AppData\Local\Temp
	memset(logFileName, 0, MAX_PATH);
	char tmpFilePath[MAX_PATH];
	char tmpFileName[MAX_PATH];
	if (GetTempPathA(MAX_PATH, tmpFilePath) != 0)
	{
		// using prefix KL_ to find temp file easier
		if (GetTempFileNameA(tmpFilePath, "KL_", 0, tmpFileName) != 0)
		{
			// we got a temp file, save to instance var
			strncpy_s(logFileName, tmpFileName, MAX_PATH);
		}
	}
}

LogFile::~LogFile()
{
	// check if file exists before attempting to delete
	ifstream log_file(logFileName);
	if (log_file)
	{
		log_file.close();
		DeleteFileA(logFileName);
	}
}

int LogFile::GetFileSize()
{
	int fsize = 0;
	ifstream log_file(logFileName, ios::in|ios::binary|ios::ate);
	if (log_file.is_open())
	{
		fsize = log_file.tellg();
		log_file.close();
	}
	return fsize;
}

void LogFile::AppendString(char* str)
{
	ofstream log_file;
	log_file.open(logFileName, ios::app);
	log_file << str;
	log_file.close();
}

void LogFile::FlushContents()
{
	ofstream log_file;
	log_file.open(logFileName, ios::trunc);
	log_file.close();
}