#include "Speed.h"
#include <Windows.h>
 
bool SpeedHackEnabled, SpeedHackInitialized;
double TickCount; //Is double so we won't lose data due to rounding, float didn't work for some odd reason
long long PerformanceCount, PerformanceFrequency;
float Acceleration = 1;
const int SleepTime = 10; //Updatate frequency for GetTickCount, timeGetTime and QueryPerformanceCounter
 
typedef MMRESULT (__stdcall *timebeginperiod) (UINT Period);
timebeginperiod TimeBeginPeriod = (timebeginperiod)GetProcAddress(LoadLibrary("winmm.dll"), "timeBeginPeriod");
 
typedef MMRESULT (__stdcall *timeendperiod) (UINT Period);
timeendperiod TimeEndPeriod = (timeendperiod)GetProcAddress(LoadLibrary("winmm.dll"), "timeEndPeriod");
 
void MakePageWritable(unsigned long Address, unsigned long ulSize)
{
    MEMORY_BASIC_INFORMATION* mbi = new MEMORY_BASIC_INFORMATION;
    VirtualQuery((void*)Address, mbi, ulSize);
    if (mbi->Protect != PAGE_EXECUTE_READWRITE)
    {
        unsigned long* ulProtect = new unsigned long;
        VirtualProtect((void*)Address, ulSize, PAGE_EXECUTE_READWRITE, ulProtect);
        delete ulProtect;
    }
    delete mbi;
}
 
bool Jump(unsigned long Address, void* Function)
{
    __try
    {
        MakePageWritable(Address, 5);
        *(unsigned char*)Address = 0xE9;
        *(unsigned long*)(Address + 1) = ((int)Function - Address) - 5;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}
 
void Tick()
{
    TimeBeginPeriod(1);
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    _onexit(ExitSpeedHack); //If you get some error saying the stack is corrupt when you exit your program then this is the fault
 
    float PerformanceSleepTime = (SleepTime * PerformanceFrequency / 1000.0f);
    //int EnableTime = GetTickCount();
 
    while (true)
    {
        /*double AcceleratedTime = (GetTickCount() - EnableTime) * (Acceleration - 1); //Speedhack inaccuracy testing, rememeber to make sure that you disable the jump at GetTickCount and don't change speed during testing
        int Inaccuracy = (GetTickCount() + AcceleratedTime) - TickCount;*/
 
        Sleep(SleepTime);
        if (SpeedHackEnabled)
        {
            TickCount += SleepTime * Acceleration;
            PerformanceCount += (long long)(PerformanceSleepTime * Acceleration);
        }
        else
        {
            TickCount += SleepTime;
            PerformanceCount += (long long)(PerformanceSleepTime);
        }
    }
}
 
int WINAPI NewTickCount()
{
    return (int)TickCount;
}
 
BOOL WINAPI NewQueryPerformanceCounter(LARGE_INTEGER* Count)
{
    Count->QuadPart = PerformanceCount;
    return TRUE;
}
 
void SpeedHack(bool Enable)
{
    SpeedHackEnabled = Enable;
    if (Enable == true)
    {
        TickCount = GetTickCount();
 
        LARGE_INTEGER Ref;
        QueryPerformanceFrequency(&Ref);
        PerformanceFrequency = Ref.QuadPart;
     
        QueryPerformanceCounter(&Ref);
        PerformanceCount = Ref.QuadPart;
 
        if (SpeedHackInitialized == false)
        {
            CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&Tick, NULL, 0, NULL);
            SpeedHackInitialized = true;
        }
     
        DWORD GetTickCount = (DWORD)GetProcAddress(LoadLibrary("kernel32.dll"), "GetTickCount");
        Jump(GetTickCount, NewTickCount);
        DWORD TimeGetTime = (DWORD)GetProcAddress(LoadLibrary("winmm.dll"), "timeGetTime");
        Jump(TimeGetTime, NewTickCount);
        DWORD QueryPerformanceCounter = (DWORD)GetProcAddress(LoadLibrary("kernel32.dll"), "QueryPerformanceCounter");
        Jump(QueryPerformanceCounter, NewQueryPerformanceCounter);
    }
}
 
int ExitSpeedHack()
{
    TimeEndPeriod(1);
    return 0;
}
