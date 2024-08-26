#include "processmanager.h"
#include <QDebug>

#include <Windows.h>
#include <TlHelp32.h>

ProcessManager::ProcessManager()
{
}

ProcessManager::~ProcessManager()
{
}

bool ProcessManager::getAllProcessList(PROCESSLIST *plist)
{
    // 获取系统进程快照.
    HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    // 如果句柄获取失败，错误提示.
    if (INVALID_HANDLE_VALUE == hProcessSnap) {
        throw std::exception("Process snapshot handle is invalid.");
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hProcessSnap,&pe32)) {
        CloseHandle(hProcessSnap);
        return true;
    }
    do {
        plist->emplace_back(PROCESSDATA{pe32.th32ProcessID, pe32.th32ParentProcessID, pe32.szExeFile});
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
    return true;
}

bool ProcessManager::getProcessModuleList(pid_t pid, std::vector<std::wstring> *pVec)
{
    // 获取当前进程的快照.
    HANDLE hModuleSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
    if (hModuleSnap == INVALID_HANDLE_VALUE) {
        auto str = "Process [" + std::to_string(pid) + "] get snapshot failed.";
        throw std::exception(str.c_str());
    }

    MODULEENTRY32 me32;
    me32.dwSize = sizeof(MODULEENTRY32);

    if (!Module32First(hModuleSnap, &me32)) {
        CloseHandle(hModuleSnap);
        return true;
    }
    do {
        pVec->emplace_back(me32.szExePath);

    } while(Module32Next(hModuleSnap, &me32));
    CloseHandle(hModuleSnap);
    return true;
}
