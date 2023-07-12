#include <Windows.h>
#include <TlHelp32.h> 
#include <iostream>

DWORD getProcId(const char* procName) {
    DWORD procId = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnap == INVALID_HANDLE_VALUE) {
        std::cout << "Cannot create snapshot. Error: " << GetLastError() << "\n";
        return 0;
    }

    PROCESSENTRY32 procEntry;
    procEntry.dwSize = sizeof(procEntry);

    if (Process32First(hSnap, &procEntry)) {
        do {
            if (!_stricmp(procEntry.szExeFile, procName)) {
                procId = procEntry.th32ProcessID;
                break;
            }
        } while (Process32Next(hSnap, &procEntry));

        if (procId == 0) {
            std::cout << "Cannot find process with given name.\n";
        }
    }
    else {
        std::cout << "Cannot read first process. Error: " << GetLastError() << "\n";
    }

    CloseHandle(hSnap);
    return procId;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <dll_path> <process_name>\n";
        return 1;
    }

    const char* dllPath = argv[1];
    const char* procName = argv[2];

    DWORD procId = getProcId(procName);
    if (procId == 0) {
        std::cout << "Cannot obtain process ID.\n";
        system("PAUSE");
        return 1;
    }

    std::cout << "Process ID is: " << procId << "\n";

    HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procId);
    if (handle == INVALID_HANDLE_VALUE) {
        std::cout << "Cannot open process. Error: " << GetLastError() << "\n";
        system("PAUSE");
        return 1;
    }

    void* loc = VirtualAllocEx(handle, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (loc == NULL) {
        std::cout << "Cannot allocate memory. Error: " << GetLastError() << "\n";
        system("PAUSE");
        CloseHandle(handle);
        return 1;
    }

    if (!WriteProcessMemory(handle, loc, dllPath, (strlen(dllPath) + 1) * sizeof(char), 0)) {
        std::cout << "Cannot write to process memory. Error: " << GetLastError() << "\n";
        system("PAUSE");
        VirtualFreeEx(handle, loc, 0, MEM_RELEASE);
        CloseHandle(handle);
        return 1;
    }

    FARPROC hLoadLibrary = GetProcAddress(GetModuleHandle("Kernel32.dll"), "LoadLibraryA");

    if (!hLoadLibrary) {
        std::cout << "Cannot get LoadLibrary address. Error: " << GetLastError() << "\n";
        system("PAUSE");
        VirtualFreeEx(handle, loc, 0, MEM_RELEASE);
        CloseHandle(handle);
        return 1;
    }

    HANDLE hThread = CreateRemoteThread(handle, 0, 0, (LPTHREAD_START_ROUTINE)hLoadLibrary, loc, 0, 0);
    if (hThread == NULL) {
        std::cout << "Cannot create remote thread. Error: " << GetLastError() << "\n";
        system("PAUSE");
        VirtualFreeEx(handle, loc, 0, MEM_RELEASE);
        CloseHandle(handle);
        return 1;
    }

    std::cout << "DLL injected successfully.\n";
    return 0;
}
