#include <Windows.h>
#include <psapi.h>
#include "Injector.h"

// ### How to do process enumeration: http://msdn.microsoft.com/en-us/library/windows/desktop/ms682623%28v=vs.85%29.aspx
// This function searchs for process by name and open for memory read/write access, and returns its handle
HANDLE FindProcess(const char* target_proc)
{
	HANDLE hproc = NULL;
	char proc_name[MAX_PATH] = TEXT("<invalid>"); // used to store process module name
	DWORD aProcesses[1024], cbNeeded, processes_count;

    if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
    {
        return NULL;
    }

	// determine how many processes were enumerated
	processes_count = cbNeeded / sizeof(DWORD);

	for (unsigned int i = 0; i < processes_count; i++)
	{
		// now that we have all running procceses id's, attempt to find target process by looking
		// at each process' name
		if (aProcesses[i] == 0)
			continue; // invalid id, continue to next

		// valid id
		hproc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, aProcesses[i]);
		if (hproc == NULL)
			continue; // could not open process, skip

		// enumerate the current process modules
		HMODULE hMod;
		DWORD cbNeeded;

		if (EnumProcessModules(hproc, &hMod, sizeof(hMod), &cbNeeded))
		{
			// get exe name for current process, save it in proc_name
			GetModuleBaseName(hproc, hMod, proc_name, sizeof(proc_name)/sizeof(char));

			// compare name with target process name
			if (strcmpi(proc_name, target_proc) == 0)
			{
				// found process! leave it open, and return handle
				return hproc;
			}
		}

		// not found, close handle and onto the next
		CloseHandle(hproc);
	}

	return NULL;
}


bool InjectDLL(HANDLE hproc, char* dll_path)
{
	LPVOID LoadLibraryAddress = GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
	// allocate memory in the remote process for our DLL path
	LPVOID codeCave = VirtualAllocEx(hproc, NULL, strlen(dll_path), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	WriteProcessMemory(hproc, codeCave, dll_path, strlen(dll_path), NULL);
	// execute thread in remote process to load DLL
	HANDLE hthread = CreateRemoteThread(hproc, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryAddress, codeCave, 0, NULL); 
	// wait for thread to finish execution
	WaitForSingleObject(hthread, INFINITE);
	// 
	VirtualFreeEx(hproc, codeCave, 0, MEM_RELEASE | MEM_DECOMMIT);
	CloseHandle(hthread);
	return true;
}

