#include <Windows.h>
#include <string>
#include <vector>
#include <string.h>
#include <locale.h>
#include <TCHAR.H>
#include <tlhelp32.h>
#include <shlwapi.h>

using namespace std;

#define _WIN32_WINNT 0x0501 // WinXP only.
#define VC_EXTRALEAN


struct FindWindowData {
    FindWindowData( TCHAR const * windowTitle )
        : WindowTitle( windowTitle )
        , ResultHandle( 0 )
    {}

    std::basic_string<TCHAR> WindowTitle;
    HWND ResultHandle;
};
BOOL CALLBACK FindWindowImpl( HWND hWnd, LPARAM lParam ) {
    FindWindowData * p = reinterpret_cast<FindWindowData*>( LongToPtr( lParam ) );
    if( !p ) {
        // Finish enumerating we received an invalid parameter
        return FALSE;
    }

    int length = GetWindowTextLength( hWnd ) + 1;
    if( length > 0 ) {
        std::vector<TCHAR> buffer( std::size_t( length ), 0 );      
        if( GetWindowText( hWnd, &buffer[0], length ) ) {
                    // Comparing the string - If you want to add some features you can do it here
            if( _tcsnicmp( &buffer[0], p->WindowTitle.c_str(), min( buffer.size(), p->WindowTitle.size() )  ) == 0 ) {
                p->ResultHandle = hWnd;
                // Finish enumerating we found what we need
                return FALSE;
            }
        }
    }
    // Continue enumerating
    return TRUE;
}
// Returns the window handle when found if it returns 0 GetLastError() will return more information
HWND FindWindowStart( TCHAR const * windowTitle ) {

    if( !windowTitle ) {
        SetLastError( ERROR_INVALID_PARAMETER );
        return 0;
    }

    FindWindowData data( windowTitle );
    if( !EnumWindows( FindWindowImpl, PtrToLong(&data) ) && data.ResultHandle != 0 ) {
        SetLastError( ERROR_SUCCESS );
        return data.ResultHandle;
    }

    // Return ERROR_FILE_NOT_FOUND in GetLastError
    SetLastError( ERROR_FILE_NOT_FOUND );
    return 0;
}
DWORD CountProcesses(CHAR *pProcessName) 
{
    DWORD dwCount = 0;
    HANDLE hSnap = NULL;
    PROCESSENTRY32 proc32;
 
    if((hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)) == INVALID_HANDLE_VALUE)
        return -1;
    proc32.dwSize=sizeof(PROCESSENTRY32);
    while((Process32Next(hSnap, &proc32)) == TRUE)
        if(stricmp(proc32.szExeFile,pProcessName) == 0)
            ++dwCount;
    CloseHandle(hSnap); 
    return dwCount;
}

class Secure {
private:
	int seconds;
	bool isRUNNING;
	DWORD sHandle;
public:
	Secure(int seconds) {
		this->seconds = seconds;
		isRUNNING = true;
		sHandle = NULL;
	}
private:
#pragma region Scanners
bool IsCERunning() {
	if (CountProcesses(TEXT("cheatengine-x86_64.exe")))
		return true;
	if (CountProcesses(TEXT("cheatengine-i386.exe")))
		return true;
	else
		return false;
}
bool IsMHSRunning() {
	if (CountProcesses(TEXT("MHS.exe")))
		return true;
	else
		return false;
}
bool IsCheatEngineActive()
{
	if(FindWindowStart(TEXT("Cheat Engine")))
	return true;
	else 
	return false;
}
bool IsMoonEngineActive()
{
	if(FindWindowStart(TEXT("MoonLight Engine")))
	return true;
	else 
	return false;
}
bool IsPEActive()
{
	if(FindWindowStart(TEXT("PE Tools")))
	return true;
	else 
	return false;
}
bool IsOllyActive()
{
	if(FindWindowStart(TEXT("OllyDbg")))
	return true;
	else 
	return false;
}
bool IsIDA_Active()
{
	if(FindWindowStart(TEXT("IDA")))
	return true;
	else 
	return false;
}
bool IsImmunityActive()
{
	if(FindWindowStart(TEXT("Immunity Debugger")))
	return true;
	else 
	return false;
}
bool IsKernelDetectiveActive()
{
	if(FindWindowStart(TEXT("Kernel Detective")))
	return true;
	else 
	return false;
}
bool IsInjectorActive()
{
	if(FindWindowStart(TEXT("Injector")))
	return true;
	else 
	return false;
}
bool pScan() {
	if(IsCERunning())
		return false;
	if(IsMHSRunning())
		return false;
	if(IsCheatEngineActive())
		return false;
	if(IsMoonEngineActive())
		return false;
	if(IsPEActive())
		return false;
	if(IsOllyActive())
		return false;
	if(IsKernelDetectiveActive())
		return false;
	if(IsIDA_Active())
		return false;
	if(IsImmunityActive())
		return false;
	if(IsKernelDetectiveActive())
		return false;
	if(IsInjectorActive())
		return false;

	return true;
}
#pragma endregion
	void doSCAN() {
		while (isRUNNING) {
			if (!pScan()) {
				isRUNNING = false;
				::ExitProcess(0);
			}
			else {
				Sleep((seconds*1000));
			}
		}
	}
public:
	void _SCAN() {
		if (!pScan()) {
			isRUNNING = false;
			::ExitProcess(0);
		}
	}
	void InitScan() {
		doSCAN();
	}
};