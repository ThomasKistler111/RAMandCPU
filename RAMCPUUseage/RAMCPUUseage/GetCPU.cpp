#include "GetCPU.h"

#if defined(_WIN64) || defined(_WIN32)
#include "pdh.h"
//#include "processthreadsapi.h"
#elif defined(__APPLE__)
#include <mach/mach_init.h>
#include <mach/mach_error.h>
#include <mach/mach_host.h>
#include <mach/vm_map.h>
#endif

#include <windows.h>
#include <string>
#include <iostream>

#pragma comment(lib, "pdh.lib")
#pragma once 

using namespace std;

GetCPU::GetCPU() {
	init();//call init to load stuff for cpuQuery
}

void GetCPU::init() {

#if defined(_WIN64) || defined(_WIN32)
	//following is for total CPU windows
	PdhOpenQuery(NULL, NULL, &cpuQuery);
	// You can also use L"\\Processor(*)\\% Processor Time" and get individual CPU values with PdhGetFormattedCounterArray()
	PdhAddEnglishCounter(cpuQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
	PdhCollectQueryData(cpuQuery);

	//following is for current process CPU windows
	SYSTEM_INFO sysInfo;
	FILETIME ftime, fsys, fuser;

	GetSystemInfo(&sysInfo);
	numProcessors = sysInfo.dwNumberOfProcessors;

	GetSystemTimeAsFileTime(&ftime);
	memcpy(&lastCPU, &ftime, sizeof(FILETIME));

	self = GetCurrentProcess();
	GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
	memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
	memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));
#elif defined(__APPLE__)

#endif
	
}

string GetCPU::getTotalCPUUsed() {
#if defined(_WIN64) || defined(_WIN32)
	PDH_FMT_COUNTERVALUE counterVal;

	PdhCollectQueryData(cpuQuery);
	PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
	return to_string(counterVal.doubleValue);
#elif defined(__APPLE__)
	return "stub";
#endif
}

string GetCPU::getCPUByCurrentProcess() {
#if defined(_WIN64) || defined(_WIN32)
	FILETIME ftime, fsys, fuser;
	ULARGE_INTEGER now, sys, user;
	double percent;

	GetSystemTimeAsFileTime(&ftime);
	memcpy(&now, &ftime, sizeof(FILETIME));

	GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);//self is the current process
	memcpy(&sys, &fsys, sizeof(FILETIME));
	memcpy(&user, &fuser, sizeof(FILETIME));
	percent = (sys.QuadPart - lastSysCPU.QuadPart) +
		(user.QuadPart - lastUserCPU.QuadPart);
	percent /= (now.QuadPart - lastCPU.QuadPart);
	percent /= numProcessors;
	lastCPU = now;
	lastUserCPU = user;
	lastSysCPU = sys;

	return  to_string(percent * 100);
#elif defined(__APPLE__)
	return "stub";
#endif
}

string GetCPU::getCPUByCertainProcess(DWORD processID) {
#if defined(_WIN64) || defined(_WIN32)
	return winCPUByCertainProcess(processID);
#elif defined(__APPLE__)
	return macCPUByCertainProcess();
#endif
}



string GetCPU::winCPUByCertainProcess(DWORD processID) {
#if defined(_WIN64) || defined(_WIN32)
	HANDLE hProcess;

	cout << "looking for windows process ID (CPU): " << processID << endl;

	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, processID);

	if (NULL == hProcess) {
		string o = "Process";
		o += to_string(processID);
		o += " not found";
		return o;//could throw exception here instead
	}
	FILETIME ftime, fsys, fuser;
	ULARGE_INTEGER now, sys, user;
	double percent;

	GetSystemTimeAsFileTime(&ftime);
	memcpy(&now, &ftime, sizeof(FILETIME));

	GetProcessTimes(hProcess, &ftime, &ftime, &fsys, &fuser);
	memcpy(&sys, &fsys, sizeof(FILETIME));
	memcpy(&user, &fuser, sizeof(FILETIME));
	percent = (sys.QuadPart - lastSysCPU.QuadPart) +
		(user.QuadPart - lastUserCPU.QuadPart);
	percent /= (now.QuadPart - lastCPU.QuadPart);
	percent /= numProcessors;
	lastCPU = now;
	lastUserCPU = user;
	lastSysCPU = sys;

	CloseHandle(hProcess);
	return  to_string(percent * 100);
#elif defined(__APPLE__)
	return "this function not meant for MAC";
#endif
	
}


string GetCPU::macCPUByCertainProcess(DWORD processID) {
#if defined(_WIN64) || defined(_WIN32)
	return "this function not meant for Windows";
#elif defined(__APPLE__)
	return "stub";
#endif
}


