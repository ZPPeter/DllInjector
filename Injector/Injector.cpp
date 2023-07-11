#include <Windows.h>
#include <TlHelp32.h> 
#include <iostream>

DWORD getProcId(const wchar_t* procName) {
    DWORD procId = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnap == INVALID_HANDLE_VALUE) {
        std::wcout << L"Cannot create snapshot. Error: " << GetLastError() << L"\n";
        return 0;
    }

    PROCESSENTRY32 procEntry;
    procEntry.dwSize = sizeof(procEntry);

    if (Process32First(hSnap, &procEntry)) {
        do {
            if (!_wcsicmp(procEntry.szExeFile, procName)) {
                procId = procEntry.th32ProcessID;
                break;
            }
        } while (Process32Next(hSnap, &procEntry));

        if (procId == 0) {
            std::wcout << L"Cannot find process with given name.\n";
        }
    }
    else {
        std::wcout << L"Cannot read first process. Error: " << GetLastError() << L"\n";
    }

    CloseHandle(hSnap);
    return procId;
}

int wmain(int argc, wchar_t** argv) {
    if (argc != 3) {
        std::wcout << L"Usage: " << argv[0] << L" <dll_path> <process_name>\n";
        return 1;
    }

    const wchar_t* dllPath = argv[1];
    const wchar_t* procName = argv[2];

    DWORD procId = getProcId(procName);
    if (procId == 0) {
        std::wcout << L"Cannot obtain process ID.\n";
        system("PAUSE");
        return 1;
    }

    std::wcout << L"Process ID is: " << procId << L"\n";

    HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procId);
    if (handle == INVALID_HANDLE_VALUE) {
        std::wcout << L"Cannot open process. Error: " << GetLastError() << L"\n";
        system("PAUSE");
        return 1;
    }

    void* loc = VirtualAllocEx(handle, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (loc == NULL) {
        std::wcout << L"Cannot allocate memory. Error: " << GetLastError() << L"\n";
        system("PAUSE");
        CloseHandle(handle);
        return 1;
    }

    if (!WriteProcessMemory(handle, loc, dllPath, (wcslen(dllPath) + 1) * sizeof(wchar_t), 0)) {
        std::wcout << L"Cannot write to process memory. Error: " << GetLastError() << L"\n";
        system("PAUSE");
        VirtualFreeEx(handle, loc, 0, MEM_RELEASE);
        CloseHandle(handle);
        return 1;
    }

#ifdef _WIN64
    //64位系统中的代码
    FARPROC hLoadLibrary = GetProcAddress(GetModuleHandleA("Kernel32.dll"), "LoadLibraryW");
#else
    //32位系统中的代码
    FARPROC hLoadLibrary = GetProcAddress(GetModuleHandleA("Kernel32.dll"), "LoadLibraryA");
#endif

    if (!hLoadLibrary) {
        std::wcout << L"Cannot get LoadLibrary address. Error: " << GetLastError() << L"\n";
        system("PAUSE");
        VirtualFreeEx(handle, loc, 0, MEM_RELEASE);
        CloseHandle(handle);
        return 1;
    }

    HANDLE hThread = CreateRemoteThread(handle, 0, 0, (LPTHREAD_START_ROUTINE)hLoadLibrary, loc, 0, 0);
    if (hThread == NULL) {
        std::wcout << L"Cannot create remote thread. Error: " << GetLastError() << L"\n";
        system("PAUSE");
        VirtualFreeEx(handle, loc, 0, MEM_RELEASE);
        CloseHandle(handle);
        return 1;
    }

    std::wcout << L"DLL injected successfully.\n";
    return 0;
}
