// following preprocessor defs for determining current operating system
#if defined(_WIN64) || defined(_WIN32)
#define OS 946
#include "pdh.h"
#include <windows.h>
#include <tchar.h>
#include <conio.h>
#include "psapi.h"
#elif defined(__APPLE__)
#define OS 622
#include <mach/vm_statistics.h>
#include <mach/mach_types.h>
#include <mach/mach_init.h>
#include <mach/mach_error.h>
#include <mach/mach_host.h>
#include <mach/vm_map.h>
#endif
//end operating system directives

#include <iostream>
#include "GetRam1500.h"
#include "GetCPU.h"
#pragma comment(lib, "windowscodecs.lib")
using namespace std;

//enum operatingSystem {error = 0, win32 = 32, win64 = 64, mac = 622};

int main()
{
    //initial detect-operating-system stuff
    int osType = OS;//assign preprocessor directive to variable

    string OSString = "";
    switch (osType) {
    case 946:
        OSString = "Windows";
        break;
    case 622:
        OSString = "MacOS";
        break;
    case 0:
    default:
        cout << (OSString = "Error, no operating system found. Press any key to continue...\n");
        _getch();
        return 0;//exit program if the OS is indeterminant 
    }
    std::cout << "Using Operating System: " << OSString << endl;

    GetRam1500 ram;

    cout << "Total memory: " << ram.getTotalMemory() << endl;
    cout << "Total memory used: " << ram.getTotalMemoryUsed() << endl;
    cout << "Total memory by current process: " << ram.getMemoryByCurrentProcess() << endl;
    string processInQuestion = "notepad++.exe";
    cout << "Total memory used by " << processInQuestion << ": " << ram.getMemoryByCertainProcess(processInQuestion) << endl;
    cout << "Press any key to continue..." << endl;
    _getch();
}
// Run program: Ctrl + F5