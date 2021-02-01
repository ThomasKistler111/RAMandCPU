#if defined(_WIN64) || defined(_WIN32)
#include "pdh.h"
//#include "processthreadsapi.h"
#elif defined(__APPLE__)
#include <mach/mach_init.h>
#include <mach/mach_error.h>
#include <mach/mach_host.h>
#include <mach/vm_map.h>
#endif

#include <string>
#include <windows.h>
#include <iostream>

#pragma comment(lib, "pdh.lib")
#pragma once

using namespace std;

class GetCPU
{
public: 
	GetCPU();
	//public functions, system independent
	string getTotalCPUUsed();
	string getCPUByCurrentProcess();
	string getCPUByCertainProcess(DWORD processID);
private:
	//these two for windows cpu total
	static PDH_HQUERY cpuQuery;
	static PDH_HCOUNTER cpuTotal;
	
	//these five for cpu by process
	static ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
	static int numProcessors;
	static HANDLE self;

	//windows specific
	string winCPUByCertainProcess(DWORD processID);

	//mac specific
	string macCPUByCertainProcess(DWORD processID);
	
	//util functions
	void init();
};

