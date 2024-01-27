#include "processmanager.h"
#include "common.h"
#include "log.h"

#include <windows.h>
#include <TlHelp32.h>
#include <QMap>
#include <QDebug>

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

    // 释放进程ID列表资源
    delete m_pidList;
    m_pidList = nullptr;
}

PIDLIST &ProcessManager::getAllProcList()
{
    // 如果已经存在进程ID数据，释放重新获取.
    if (m_pidList) {
        delete m_pidList;
    }
    // 实例化 PID list.
    m_pidList = new PIDLIST;
    // 获取系统进程快照.
    HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    // 如果句柄获取失败，错误提示.
    if (INVALID_HANDLE_VALUE == hProcessSnap) {

    }
    return *m_pidList;
}

