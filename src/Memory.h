#define ROUND_UP(p, align)   (((DWORD)(p) + (align)-1) & ~((align)-1))
#define ROUND_DOWN(p, align) ((DWORD)(p) & ~((align)-1))

class Memory {
public:
	//CONSTRUCTOR
	Memory () {
		//INITIALIZE

	}
public:
	int __stdcall WriteMemory ( void* pvAddress, void* pvWriteBuffer, size_t Size )
	{
		DWORD dwAddress, dwOldProtectionFlags, dwSize;

		int iReturnCode;

		iReturnCode = 0;

		dwAddress = ROUND_DOWN ( pvAddress, 0x1000 );
		dwSize = ROUND_UP ( Size, 0x1000 );

		if ( VirtualProtect ( ( LPVOID )dwAddress, dwSize, PAGE_EXECUTE_READWRITE, &dwOldProtectionFlags ) )
		{
			memcpy ( pvAddress, pvWriteBuffer, Size );

			if ( VirtualProtect ( ( LPVOID )dwAddress, dwSize, dwOldProtectionFlags, &dwOldProtectionFlags ) )
			{
				if ( FlushInstructionCache ( GetCurrentProcess(), ( LPCVOID )dwAddress, dwSize ) )
				{
					iReturnCode = 0;
				}
				else
				{
					iReturnCode = GetLastError();
				}
			}
			else
			{
				iReturnCode = GetLastError();
			}
		}
		else
		{
			iReturnCode = GetLastError();
		}

		return iReturnCode;
	}
public:
	class Pointer {
	public:
		//CONSTRUCTOR
		Pointer () {
			//INITIALIZE

		}
	public:
		ULONG_PTR ReadPointer(DWORD Address, INT nOffset){
			ULONG_PTR* ulBase = (ULONG_PTR *)Address;
			if ( !IsBadReadPtr((VOID*)ulBase, sizeof(ULONG_PTR)) )
				if ( !IsBadReadPtr((VOID*)((*(ULONG_PTR*)ulBase)+nOffset), sizeof(ULONG_PTR)) )
					return *(ULONG_PTR*)((*(ULONG_PTR*)ulBase)+nOffset);
			return 0;
		}
		BOOL WritePointer(unsigned long ulBase, int iOffset, int iValue)
		{
			__try { *(int*)(*(unsigned long*)ulBase + iOffset) = iValue; return TRUE; }
			__except (EXCEPTION_EXECUTE_HANDLER) { return FALSE; }
		}
	};
};