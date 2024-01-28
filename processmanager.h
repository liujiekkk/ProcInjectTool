#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include <QObject>

class Log;

typedef struct node{

    wchar_t *name;
    uint64_t pid;
    uint64_t parentPid;

    node():name(nullptr), pid(0), parentPid(0) {}
    node(uint64_t pid, uint64_t parentPid, wchar_t *name): pid(pid), parentPid(parentPid)
    {
        this->name = copy(name);
    }
    explicit node(const node &t)
    {
        pid = t.pid;
        parentPid = t.parentPid;
        name = copy(t.name);
    }
    node& operator=(const node &t)
    {
        if (name != nullptr) {
            delete[] name;
            name = nullptr;
        }
        name = copy(t.name);
        pid = t.pid;
        parentPid = t.parentPid;
        return *this;
    }
    ~node()
    {
        if (name != nullptr)
            delete[] name;
    }
private:

    wchar_t *copy(const wchar_t *pSrc)
    {
        if (pSrc == nullptr) {
            return nullptr;
        }
        size_t len = (wcslen(pSrc) + 1) * sizeof(wchar_t);
        wchar_t *pDst  = new wchar_t[len];
        memcpy(pDst, pSrc, len);
        return pDst;
    }
} PIDLIST_ELEM;

typedef std::vector<PIDLIST_ELEM> PIDLIST, *LPPIDLIST;

class exception;

// 定义 ProcessManager 异常对象 PMException
typedef struct {
    unsigned short code;
    const char * msg;
} PMException;

class ProcessManager : public QObject
{
    Q_OBJECT
public:
    explicit ProcessManager(QObject *parent = nullptr);

    ~ProcessManager();

    void getAllProcList(PIDLIST&);

signals:

private:

    Log *m_pLog = nullptr;
};

#endif // PROCESSMANAGER_H
