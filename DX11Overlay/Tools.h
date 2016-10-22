#pragma once
#include <Windows.h>
#include <stdio.h>

static HANDLE hConsole;

void CreateConsole()
{
	AllocConsole();
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
}

void XTrace(char* lpszFormat, ...)
{
	va_list args;
	va_start(args, lpszFormat);
	int nBuf;
	char szBuffer[512];
	nBuf = _vsnprintf_s(szBuffer, 511, lpszFormat, args);
	va_end(args);
	DWORD CharsWritten = 0;
	WriteConsole(hConsole, szBuffer, nBuf, &CharsWritten, NULL);
}