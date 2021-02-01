#if defined(_WIN64) || defined(_WIN32)
#include <windows.h>
#include <WinUser.h>
#include <tchar.h>
#include <string.h>
#include <tlhelp32.h>
#include <locale>
#include <codecvt>
#include "psapi.h"
#elif defined(__APPLE__)
#include <mach/vm_statistics.h>
#include <mach/mach_types.h>
#include <mach/mach_init.h>
#include <mach/mach_host.h>
#endif

#include <iostream>
#include <string>


#pragma once

using namespace std;

//voted best in class three years running
class GetRam1500
{
public:
	GetRam1500();
	//public methods, system independent
	string getTotalMemory();
	string getTotalMemoryUsed();
	string getMemoryByCurrentProcess();
	string getMemoryByCertainProcess(string WindowTitle);
	string getMemoryByCertainProcess(DWORD processID);
	string getMemoryByCertainWindow(string processName);
private: 
	string memUsed;
	string memAvail;
	void init();
	//windows specific
	string winMemByCertainProcess(DWORD processID);
	DWORD FindProcessId(const std::wstring& processName);
	DWORD FindProcessIdByWindow(string windowName);
	bool weirdEquals(const std::wstring& str, char const* c);

	//mac specific
	string macMemByCertainProcess(DWORD processID);
	
	//util
	string bytesToMb(string b);
	string bytesToGb(string b);
	string bytesToMbOrGb(string b);
	void Output(const string szFormat, ...);
	void Output(const char* szFormat, ...);
	void printAndOutput(string out);
	char* appendCharArr(const char* start, const char* end);
	char* addNL(const char* source);
	wstring s2ws(const std::string& str);
	wstring s2ws(const char * cstr);
};

