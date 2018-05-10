#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include <stdarg.h>
#include <time.h>
#include <mutex>

using namespace std;

class Out
{
private:
	static mutex mtx;
public:
	static void print(char* format, ...)
	{
		mtx.lock();
		va_list ap;
		va_start(ap, format);
		time_t t = time(0);
		tm* timeStruct = localtime(&t);
		printf("[%02i:%02i:%02i]: ", timeStruct->tm_hour, timeStruct->tm_min, timeStruct->tm_sec);
		vprintf(format, ap);
		va_end(ap);
		mtx.unlock();
	}

	static void println(char* format, ...)
	{
		mtx.lock();
		va_list ap;
		va_start(ap, format);
		time_t t = time(0);
		tm* timeStruct = localtime(&t);
		printf("[%02i:%02i:%02i]: ", timeStruct->tm_hour, timeStruct->tm_min, timeStruct->tm_sec);
		vprintf(format, ap);
		va_end(ap);
		cout << endl;
		mtx.unlock();
	}
};