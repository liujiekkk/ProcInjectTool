#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include <string>
#include <vector>

typedef unsigned long pid_t;

typedef struct _ProcessData {
    pid_t pid;
    unsigned parentId;
    std::wstring name;
} PROCESSDATA, *PPROCESSDATA;

typedef std::vector<PROCESSDATA> PROCESSLIST;

class ProcessManager
{

public:

    explicit ProcessManager();

    ~ProcessManager();

    // 获取进程信息列表.
    static bool getAllProcessList(PROCESSLIST *plist);

    // 获取指定进程的dll打开列表
    static bool getProcessModuleList(pid_t pid, std::vector<std::wstring> *pVec);

private:

};

#endif // PROCESSMANAGER_H
