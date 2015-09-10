#include <string>
#include "Headers.h"
#include "Memory.h"
#include "Speed.h"
#include "Scanner.h"
#include "detours.h"
using namespace std;

static Scanner *scan = new Scanner();
static Memory *mem = new Memory();
static Memory::Pointer *ptr = new Memory::Pointer();

#pragma region DETOURS:
	DWORD eJmp = 0;
	VOID setJMP(DWORD jNew) {
		eJmp = jNew;
	}
	__declspec(naked) void aJmp()
	{
		__asm
		{
			Jmp [eJmp]
		}
	}
	VOID doJMP(DWORD jFrom, DWORD jNew) {
		setJMP(jNew);
		::DetourFunction((PBYTE)jFrom, (PBYTE)&aJmp);
	}
#pragma endregion

#define HSCallBack1	    0x908F0
#define HSCallBack2	    0xAC28

#define BASE		0x76CB00
#define CHANNEL		0x545C
#define CARAT		0x5D98
#define LEVEL		0x5D5C

#define dwBegin		0x00400000
#define dwEnd		0x0FFFFFFF

class Library {
private:
	DWORD CRASH_1, CRASH_2, EXP_1, EXP_2, caratAddy, mcAddy;
public:
	DWORD vladAddy;
private:

	VOID setAddys(string module) {
		
		if (module == "$PyE_C" || module == "$PyE_C_G") {

			CRASH_1 = scan->findPattern(dwBegin, dwEnd, (PBYTE)"\x89\x4E\x38\x8B\x4D\xE8\x0F\xBF\xC2\x99\x89\x56\x34\x89\x46\x30", "xxxxxxxxxxxxxxxx");
			caratAddy = CRASH_1 + 0x7B;
		}
		if (module == "$PyE_E" || module == "$PyE_E_G") {

			CRASH_1 = scan->findPattern(dwBegin, dwEnd, (PBYTE)"\x89\x4E\x38\x8B\x4D\xE8\x0F\xBF\xC2\x99\x89\x56\x34\x89\x46\x30", "xxxxxxxxxxxxxxxx");
			CRASH_2 = CRASH_1 + 0x15;
			EXP_1 = CRASH_1 + 0xD;
			EXP_2 = scan->findPattern(dwBegin, dwEnd, (PBYTE)"\x89\x46\x23\x66\x89\x46\x27", "xxxxxxx");
		}
	}
public:

	/*
	 *	INITIALIZE_MAIN
	 *	@function		Performs address scan/set for function calls.
	 *	@return			void
	 */
		VOID Initialize(string module) {

			setAddys(module);
		}
	//CARAT_HACK
		VOID doCARAT() {

			mem->WriteMemory((VOID*)(caratAddy),(PBYTE)"\xC7\x46\x3C\x57\x02\x00\x00\x90\x90", 9);
		}
	//EXP_HACK
		VOID doEXP() {

			mem->WriteMemory((VOID*)(CRASH_1),(PBYTE)"\xC7\x46\x38\x57\x02\x00\x00\x90\x90", 9);
			mem->WriteMemory((VOID*)(CRASH_2),(PBYTE)"\x90\x90\x90\x90", 4);

			mem->WriteMemory((VOID*)(EXP_1),(PBYTE)"\xC7\x46\x30\x57\x02\x00\x00\x90\x90", 8);
			mem->WriteMemory((VOID*)(EXP_2),(PBYTE)"\xC7\x46\x2F\x57\x02\x00\x00", 7);
		}
	//CHANNEL_HACK
		VOID doCHANNEL(int chan) {

			ptr->WritePointer(BASE, CHANNEL, chan);
		}
public:

	INT getChannel() {
		int chan = 0;
		try {
			chan = ptr->ReadPointer(BASE, CHANNEL);
		}
		catch (...) {}
		return chan;
	}
	INT getCarat() {
		int carat;
		try {
			carat = ptr->ReadPointer(BASE, CARAT);
		} catch(...) { carat = NULL; }
		return carat;
	}
	INT getLevel() {
		int level;
		try {
			level = ptr->ReadPointer(BASE, LEVEL);
		} catch(...) { level = NULL; }
		return level;
	}
};

class Speed {

public:
	VOID setSPEED( int newSpeed ) {

		Acceleration = newSpeed;
	}
	VOID doSPEED() {

		SpeedHack(true);
	}
};

enum bTypes {

	hackShield = 0,
	CRC = 1
};
class Bypass {

private:
	int bType;
public:
	/*
	 *	CONSTRUCTOR
	 *	@function		Specify bypass type for execution, 'bType'.
	 *	@return			void
	 */
		Bypass(int Type) {

			bType = Type;
		}
	/*
	 *	INITIALIZE_MAIN
	 *	@function		Execute bypass dependent on type 'bType'.
	 *	@return			void
	 */
		VOID Initialize() {
			switch (bType) {
			case bTypes::hackShield:
				hBypass hshield;
				hshield.Initialize();
				break;
			}
		}
private:
	class hBypass {
	private:

		//HB_JUMP
			DWORD bgnJmp, endJmp;
	private:
		VOID getJMP() {

			bgnJmp = scan->findPattern(dwBegin, dwEnd, (PBYTE)"\x74\x6A\x8B\x35\x68\xC3\x75\x00", "xxxx???x");
			endJmp = scan->findPattern(dwBegin, dwEnd, (PBYTE)"\x8B\x8F\x00\x03\x00\x00\xE8", "xx?xxxx");
		}
	public:
		VOID Initialize() {

			DWORD dwEhSvc;
			do{
				dwEhSvc = (DWORD)GetModuleHandleA("EhSvc.dll");
				Sleep(100);
			}while(!dwEhSvc);
	
			//HACKSHIELD_CRIPPLE
				mem->WriteMemory((VOID*)(dwEhSvc + HSCallBack1),(PBYTE)"\xC3", 1); //CALLBACK_1 | ERRORS
				mem->WriteMemory((VOID*)(dwEhSvc + HSCallBack2),(PBYTE)"\x74",1); //CALLBACK_2 | CRC SUB
			//DETOUR_HB_THREAD
				getJMP();
				::doJMP( bgnJmp, endJmp );
		}
	};
};
