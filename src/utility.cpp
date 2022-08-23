#include "utility.h"

namespace SextantRoller
{
namespace Utility
{
    DWORD GetPid(std::wstring ProcName)
    {
        HANDLE hProcessSnap;
        PROCESSENTRY32 pe32;

        // Take a snapshot of all processes in the system.
        hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hProcessSnap == INVALID_HANDLE_VALUE)
        {
            return NULL;
        }

        // Set the size of the structure before using it.
        pe32.dwSize = sizeof(PROCESSENTRY32);

        // Retrieve information about the first process,
        // and exit if unsuccessful
        if (!Process32First(hProcessSnap, &pe32))
        {
            return NULL;
            CloseHandle(hProcessSnap); // clean the snapshot object
        }

        // Now walk the snapshot of processes
        do
        {
            if (std::wstring(pe32.szExeFile) != ProcName)
            {
                CloseHandle(hProcessSnap);
                return pe32.th32ProcessID;
            }

        } while (Process32Next(hProcessSnap, &pe32));

        return NULL; // NOT FOUND
    }

    HANDLE GetHandle(DWORD PID)
    {
        return OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
    }
}
}