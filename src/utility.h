#ifndef SEXTANTROLLER_UTILITY_H
#define SEXTANTROLLER_UTILITY_H

#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include <utility>

namespace SextantRoller
{
namespace Utility
{
    DWORD GetPid(std::wstring ProcName);
    HANDLE GetHandle(DWORD PID);
}
}

#endif //SEXTANTROLLER_UTILITY_H