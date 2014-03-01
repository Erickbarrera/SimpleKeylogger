#include "HookUtils.h"

// Helper to detour function call (HOOK)
LPVOID DetourCode(PBYTE src, PBYTE dst, int len)
{
	unsigned long long int_math = 0; // for 64bit (pointers are 8 bytes)

	PBYTE jmp = (PBYTE)VirtualAlloc(NULL, len+5, MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	memset(jmp, 0x90, len+5);

	DWORD dwback;
	VirtualProtect(src, len, PAGE_READWRITE, &dwback);

	CopyMemory(jmp, src, len);
	jmp += len;
	
	jmp[0] = 0xE9;
	int_math = (src+len-jmp) - 5;
	*(DWORD*)(jmp+1) = (DWORD)(int_math);
	
	src[0] = 0xE9;
	int_math = (dst-src) - 5;
	*(DWORD*)(src+1) = (DWORD)int_math;

	VirtualProtect(src, len, dwback, &dwback);
	return (jmp-len);
}