#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include <QObject>

class Log;

typedef QMap<QString, qint64> PIDLIST;

class ProcessManager : public QObject
{
    Q_OBJECT
public:
    explicit ProcessManager(QObject *parent = nullptr);

    ~ProcessManager();

    PIDLIST &getAllProcList();

signals:

private:

    PIDLIST *m_pidList = nullptr;

    Log *m_pLog = nullptr;
};

#endif // PROCESSMANAGER_H
