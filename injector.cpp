#include "injector.h"

#include "processthreadsapi.h"
#include "TlHelp32.h"
#include <QString>
#include <QDebug>

#include <string>
#include <stdexcept>

Injector::Injector(DWORD pid)
    : m_pid(pid), m_lpMemoryAddrForInject(nullptr), m_processHandle()
{
    // 打开进程句柄
    m_processHandle = ::OpenProcess(PROCESS_ALL_ACCESS, false, pid);
    if (m_processHandle == INVALID_HANDLE_VALUE) {
        throw std::exception("Injector open process failed.");
    }
}

Injector::~Injector()
{
    if (m_lpMemoryAddrForInject) {
        // 释放所有已经分配的内存
        ::VirtualFreeEx(m_processHandle, m_lpMemoryAddrForInject, 0, MEM_FREE);
    }
    if (m_lpMemoryAddrForUninstall) {
        ::VirtualFreeEx(m_processHandle, m_lpMemoryAddrForUninstall, 0, MEM_FREE);
    }
    if (m_processHandle) {
        // 释放进程句柄.
        ::CloseHandle(m_processHandle);
    }
}

bool Injector::install(const char *dllPath)
{
    // dll 文件名写入到进程内存.
    writeProcessMemory(m_processHandle, (PBYTE)dllPath, std::strlen(dllPath) + 1, &m_lpMemoryAddrForInject);
    // 创建远程线程.
    SECURITY_ATTRIBUTES securityAttributes;
    securityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
    securityAttributes.bInheritHandle = false;
    securityAttributes.lpSecurityDescriptor = NULL;

    LPTHREAD_START_ROUTINE lpRoutineAddr = (LPTHREAD_START_ROUTINE)getFuncAddr("kernel32.dll", "LoadLibraryA");
    DWORD threadId;
    // 创建远程线程.
    HANDLE threadHandle = ::CreateRemoteThread(m_processHandle, &securityAttributes, 0, lpRoutineAddr, m_lpMemoryAddrForInject, 0, &threadId);
    if (threadHandle == NULL) {
        return false;
    }
    // 一直等待线程执行完毕.
    WaitForSingleObject(threadHandle, -1);
    // 释放线程句柄.
    CloseHandle(threadHandle);
    return true;
}

bool Injector::uninstall(const char *dllPath)
{
    // 获取目标进程句柄
    HMODULE hModule = getProcessModuleHandle(m_pid, dllPath);
    if (hModule == NULL) {
        throw std::runtime_error("获取dll模块句柄失败.");
    }
    // 将模块句柄写入到远程线程.
    // writeProcessMemory(m_processHandle, (LPBYTE)&hModule, sizeof(HMODULE), &m_lpMemoryAddrForUninstall);

    // 创建远程线程.
    SECURITY_ATTRIBUTES securityAttributes;
    securityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
    securityAttributes.bInheritHandle = false;
    securityAttributes.lpSecurityDescriptor = NULL;

    LPTHREAD_START_ROUTINE lpRoutineAddr = (LPTHREAD_START_ROUTINE)getFuncAddr("kernel32.dll", "FreeLibrary");

    // 创建远程线程, 注入参数地址必须是远程进程中的地址（需要通过开辟新内存写入进去，不能用本进程中的变量或者内存地址）
    HANDLE threadHandle = ::CreateRemoteThread(m_processHandle, &securityAttributes, 0, lpRoutineAddr, hModule, 0, NULL);
    if (threadHandle == NULL) {
        DWORD lastError = GetLastError();
        throw std::runtime_error("CreateRemoteThread failed with error code " + std::to_string(lastError));
    }

    // 一直等待线程执行完毕.
    DWORD waitResult = WaitForSingleObject(threadHandle, INFINITE);
    if (waitResult != WAIT_OBJECT_0) {
        DWORD lastError = GetLastError();
        throw std::runtime_error("WaitForSingleObject failed with error code " + std::to_string(lastError));
    }

    // 释放线程句柄.
    if (!CloseHandle(threadHandle)) {
        DWORD lastError = GetLastError();
        throw std::runtime_error("CloseHandle failed with error code " + std::to_string(lastError));
    }

    // 检查 DLL 是否已经被卸载
    HMODULE checkModule = getProcessModuleHandle(m_pid, dllPath);
    if (checkModule != NULL) {
        return false;
    }
    return true;
}

void Injector::writeProcessMemory(HANDLE pHandle, LPBYTE pData, SIZE_T dwSize, LPVOID* lpMemoryAddress)
{
    // 给进程分配内存
    *lpMemoryAddress = ::VirtualAllocEx(pHandle, NULL, dwSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (*lpMemoryAddress == NULL) {
        throw std::exception("Injector alloc memory failed");
    }
    // 每次写入内存的大小.
    SIZE_T writeLen = 0;
    // 已经写入的大小.
    SIZE_T total = 0;
    do {
        // 如果写入失败.
        if (!WriteProcessMemory(pHandle, *lpMemoryAddress, pData + total, dwSize - total, &writeLen)) {
            throw std::exception("Injector write memory file name failed.");
        }
        total += writeLen;
    } while(total < dwSize);
}

FARPROC Injector::getFuncAddr(const char *moduleName, const char *funcName)
{
    HMODULE hModule = ::LoadLibraryA(moduleName);
    if (hModule == NULL) {
        throw std::exception("Injector get func address failed");
    }
    return ::GetProcAddress(hModule, funcName);
}

HMODULE Injector::getProcessModuleHandle(DWORD pid, const char *dllName)
{
    // 获取当前进程的快照.
    HANDLE hModuleSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
    if (hModuleSnap == INVALID_HANDLE_VALUE) {
        throw std::exception(std::string("Process [" + std::to_string(pid) + "] get snapshot failed.").c_str());
    }

    MODULEENTRY32 me32;
    me32.dwSize = sizeof(MODULEENTRY32);

    if (!Module32First(hModuleSnap, &me32)) {
        CloseHandle(hModuleSnap);
        return NULL;
    }
    QString name(dllName);
    do {
        if (std::wcscmp(me32.szExePath, name.toStdWString().c_str()) == 0) {
            CloseHandle(hModuleSnap);
            return me32.hModule;
        }

    } while(Module32Next(hModuleSnap, &me32));
    CloseHandle(hModuleSnap);
    return NULL;
}
