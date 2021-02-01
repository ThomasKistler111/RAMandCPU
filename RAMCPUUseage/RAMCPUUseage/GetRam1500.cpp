#include "GetRam1500.h"

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

#include <string>
#include <iostream>

using namespace std;

//voted best in class three years running
GetRam1500::GetRam1500() {
	init();
}

/*Used to initialize values like total memory and total used memory, which don't require the same update frequency*/
void GetRam1500::init() {
#if defined(_WIN64) || defined(_WIN32)
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	DWORDLONG totalPhysMem = memInfo.ullTotalPhys;
	DWORDLONG memUsedInit = memInfo.ullTotalPhys - memInfo.ullAvailPhys;
	memAvail = to_string(totalPhysMem);
	memUsed = to_string(memUsedInit);
#elif defined(__APPLE__)
	cout<<  "Mac not currently supported for init()" << endl;//reminder
#endif
}

/*Gets total memory of the system in Gb*/
string GetRam1500::getTotalMemory() {
#if defined(_WIN64) || defined(_WIN32)
	//no need to refresh unless you somehow installed ram while your computer was running
	return bytesToMbOrGb(memAvail);
#elif defined(__APPLE__)
	return "Stub";
#endif
}

/*Gets the total memory in use (in Mb).  This is OS dependent.  */
string GetRam1500::getTotalMemoryUsed() {
#if defined(_WIN64) || defined(_WIN32)\
	init();//refresh value of memory used
	return bytesToMbOrGb(memUsed);
#elif defined(__APPLE__)
	return "Stub";
#endif
}

/*Gets the memory used in MB by the current process.  This is OS dependent.  */
string GetRam1500::getMemoryByCurrentProcess() {
#if defined(_WIN64) || defined(_WIN32)
	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;
	return bytesToMbOrGb(to_string(virtualMemUsedByMe));
#elif defined(__APPLE__)
	return "Stub";
#endif
}

/*Helper function for getting memory by process.  Unless you know the litteral processID, it is not wise to call this directly. */
string GetRam1500::getMemoryByCertainProcess(DWORD processID) {
#if defined(_WIN64) || defined(_WIN32)
	return winMemByCertainProcess(processID);
#elif defined(__APPLE__)
	return macMemByCertainProcess();
#endif
}

/*Gets the memory used in MB by a certain window.  This is less exact than getMemoryByCertainProcess(string)*/
string GetRam1500::getMemoryByCertainWindow(string processName) {
#if defined(_WIN64) || defined(_WIN32)
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	//utf8
	//std::string narrow = converter.to_bytes(processName);
	//utf16
	std::wstring wide = converter.from_bytes(processName);

	DWORD processID = FindProcessId(wide);

	if (processID == 0)
		std::wcout << "Could not find " << processName.c_str() << std::endl;
	else
		std::wcout << "Process ID is " << processID << std::endl;

	return winMemByCertainProcess(processID);
#elif defined(__APPLE__)
	return macMemByCertainProcess();
#endif
}

string GetRam1500::getMemoryByCertainProcess(string processName) {
#if defined(_WIN64) || defined(_WIN32)
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	//utf8
	//std::string narrow = converter.to_bytes(processName);
	//utf16
	std::wstring wide = converter.from_bytes(processName);

	DWORD processID = FindProcessId(wide);

	if (processID == 0)
		std::wcout << "Could not find " << processName.c_str() << std::endl;
	else
		std::wcout << "Process ID is " << processID << std::endl;

	return winMemByCertainProcess(processID);
#elif defined(__APPLE__)
	return macMemByCertainProcess();
#endif
}

//windows specific
string GetRam1500::winMemByCertainProcess(DWORD processID) {
#if defined(_WIN64) || defined(_WIN32)
	HANDLE hProcess;
	//PROCESS_MEMORY_COUNTERS pmc;
	PROCESS_MEMORY_COUNTERS_EX pmc;

	cout << "looking at windows process ID (RAM): " << processID << endl;
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, processID);

	if (NULL == hProcess) {
		string o = "Process";
		o += to_string(processID);
		o += " not found";
		return o;//could throw exception here
	}

	GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;
	CloseHandle(hProcess);
	return bytesToMbOrGb(to_string(virtualMemUsedByMe));
#elif defined(__APPLE__)
	return "function not meant for MacOS";
#endif	
}

DWORD GetRam1500::FindProcessIdByWindow(string windowName)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	//utf-8
	//std::string narrow = converter.to_bytes(processName);
	//UTF-16
	std::wstring wwindowName = converter.from_bytes(windowName);

	HWND windowHandle = FindWindowW(NULL, wwindowName.c_str());
	DWORD* processID = new DWORD;
	GetWindowThreadProcessId(windowHandle, processID);

	std::wcout << L"Process ID of " << wwindowName.c_str() << L" is: " << *processID << std::endl;
	return *processID;
}

//windows specific
DWORD GetRam1500::FindProcessId(const std::wstring& processName)
{
	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(processInfo);

	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (processesSnapshot == INVALID_HANDLE_VALUE)
		return 0;

	Process32First(processesSnapshot, &processInfo);
	
	if (!processName.compare(s2ws(processInfo.szExeFile)))
	{
		CloseHandle(processesSnapshot);
		return processInfo.th32ProcessID;
	}

	while (Process32Next(processesSnapshot, &processInfo))
	{
		if (!processName.compare(s2ws(processInfo.szExeFile)))
		{
			CloseHandle(processesSnapshot);
			return processInfo.th32ProcessID;
		}
	}

	CloseHandle(processesSnapshot);
	return 0;
}

bool GetRam1500::weirdEquals(const std::wstring& str, char const* c)
{
	std::string c_str(c);
	if (str.size() < c_str.size())
	{
		return false;
	}
	return std::equal(c_str.begin(), c_str.end(), str.begin());
}

//mac specific
string GetRam1500::macMemByCertainProcess(DWORD processID) {
#if defined(_WIN64) || defined(_WIN32)
	return "function not meant for windows";
#elif defined(__APPLE__)
	return "Stub";
#endif
}

//util functions

/*Decides if Mb or GB is more appropriate*/
string GetRam1500::bytesToMbOrGb(string b) {
	float ans;
	try {
		long long bytes = stoll(b);
		if (bytes > 1000000000) {//if greater than one Gb
			return bytesToGb(b);
		}
		else {
			return bytesToMb(b);
		}
	}
	catch (exception& err) {
		Output("Ran into an error converting " + b + " to Mb or Gb");
		cout << "Ran into an exception converting " << b << " to Mb or Gb" << endl;
		return 0;
	}
}


string GetRam1500::bytesToMb(string b) {
	float ans;
	try {
		long long bytes = stoll(b);
		float div = 1000000;
		ans = (float)bytes / div;
		return to_string(ans) + " Mb";
	}
	catch (exception& err) {
		Output("Ran into an error converting " + b + " to Mb");
		cout << "Ran into an exception converting " << b << " to Mb" << endl;
		return 0;
	}
	
}

string GetRam1500::bytesToGb(string b) {
	try {
		long long bytes = stoll(b);
		float div = 1000000000;
		return to_string(((float)bytes / (float)div)) + " Gb";
	}
	catch (exception& err) {
		Output("Ran into an error converting " + b + " to Gb");
		cout << "Ran into an exception converting " << b << " to Gb" << endl;
	}
}

void GetRam1500::Output(const string szFormat, ...)
{
#if defined(_WIN64) || defined(_WIN32)
	char szBuff[1024];
	va_list arg;
	va_start(arg, szFormat);
	_vsnprintf_s(szBuff, sizeof(szBuff), (szFormat + "\n").c_str(), arg);
	va_end(arg);
	OutputDebugString(szBuff);
#elif defined(__APPLE__)
	return "Stub";
#endif
}

void GetRam1500::Output(const char* szFormat, ...)
{
#if defined(_WIN64) || defined(_WIN32)
	char szBuff[1024];
	va_list arg;
	va_start(arg, szFormat);
	_vsnprintf_s(szBuff, sizeof(szBuff), addNL(szFormat), arg);
	va_end(arg);
	OutputDebugString(szBuff);
#elif defined(__APPLE__)
	return "Stub";
#endif
	
}

char* GetRam1500::appendCharArr(const char* start, const char* end)
{
	char* newArray = new char[std::strlen(start) + std::strlen(end) + 2];
	strcpy_s(newArray, sizeof newArray, start);
	strcat_s(newArray, sizeof newArray, end);
	return newArray;
}

char* GetRam1500::addNL(const char* source) {
	const char nl[2] = "\n";
	return appendCharArr(source, nl);
}

void GetRam1500::printAndOutput(string out) {
	cout << out << endl;
	Output(out);
}

wstring GetRam1500::s2ws(const char* cstr) 
{
	string str(cstr);
	return s2ws(str);
}

std::wstring GetRam1500::s2ws(const std::string& str)
{
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}