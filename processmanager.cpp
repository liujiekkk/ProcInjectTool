#include "processmanager.h"
#include <QDebug>

#include <Windows.h>
#include <TlHelp32.h>
#include <sstream>

#include "common.h"
#include "log.h"


ProcessManager::ProcessManager(QObject *parent)
    : QObject{parent}
{
    m_pLog = new Log();

    // 测试记录日志.
    // m_pLog->info("haha this is a test % %", {"@", "mm"});
    // m_pLog->warning("haha this is a test % %", {"@", "mm"});
    // m_pLog->debug("haha this is a test % %", {"@", "mm"});
    // m_pLog->error("haha this is a test % %", {"@", "mm"});
}

ProcessManager::~ProcessManager()
{
    // 释放日志资源.
    delete m_pLog;
    m_pLog = nullptr;
}

void ProcessManager::getAllProcList(PIDLIST &plist)
{
    // 获取系统进程快照.
    HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    // 如果句柄获取失败，错误提示.
    if (INVALID_HANDLE_VALUE == hProcessSnap) {
        m_pLog->error("Process snapshot handle is invalid.");
        throw PMException {1, "Process snapshot handle is invalid."};
    }

    PROCESSENTRY32 pe;

    pe.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hProcessSnap,&pe)) {
        CloseHandle(hProcessSnap);
        return;
    }
    do {
        // 将获取到的 pid 封装.
        plist.push_back(PIDLIST_ELEM {pe.th32ProcessID, pe.th32ParentProcessID, pe.szExeFile});

    } while (Process32Next(hProcessSnap, &pe));

    CloseHandle(hProcessSnap);
}

ModuleEntryList ProcessManager::getModuleList(uint64_t pid)
{
    // 获取当前进程的快照.
    HANDLE hModuleSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
    if (hModuleSnap == INVALID_HANDLE_VALUE) {
        m_pLog->error("Process [%] get snapshot failed.", {std::to_string(pid)});
        std::stringstream ss;
        ss << "Process [" << pid << "] get snapshot failed.";
        throw PMException {2, ss.str().c_str()};
    }

    MODULEENTRY32 me32;
    me32.dwSize = sizeof(MODULEENTRY32);

    ModuleEntryList list;

    if (!Module32First(hModuleSnap, &me32)) {
        CloseHandle(hModuleSnap);
    }
    do {
        list.push_back(ModuleEntryElement{me32.th32ProcessID, me32.szModule, me32.szExePath});

    } while(Module32Next(hModuleSnap, &me32));
    CloseHandle(hModuleSnap);
    return list;
}

