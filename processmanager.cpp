#include "processmanager.h"
#include <QDebug>

#include <windows.h>
#include <TlHelp32.h>

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

