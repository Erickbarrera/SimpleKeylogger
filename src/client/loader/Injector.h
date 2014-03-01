#pragma once
HANDLE FindProcess(const char* target_proc);
bool InjectDLL(HANDLE hproc, char* dll_path);