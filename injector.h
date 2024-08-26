#ifndef INJECTOR_H
#define INJECTOR_H

#include <Windows.h>

class Injector
{
private:

    DWORD m_pid;

    // 注入dll时分配的虚拟内存地址
    LPVOID m_lpMemoryAddrForInject;

    // 卸载dll时分配的虚拟内存地址
    LPVOID m_lpMemoryAddrForUninstall;

    // 进程句柄
    HANDLE m_processHandle;

public:

    Injector(DWORD pid);

    ~Injector();

    // 远程安装一个 dll 库
    bool install(const char *dllPath);

    // 远程卸载一个 dll 库
    bool uninstall(const char *dllPath);

private:

    // 向进程内存写入数据，并将数据的地址返回给提供的指针.
    void writeProcessMemory(HANDLE pHandle, LPBYTE pData, SIZE_T dwSize, LPVOID* lpMemoryAddr);

    FARPROC getFuncAddr(const char* moduleName, const char *funcName);

    // 获取指定进程的 module 句柄
    HMODULE getProcessModuleHandle(DWORD pid, const char *dllName);

};

#endif // INJECTOR_H
