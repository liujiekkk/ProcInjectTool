#ifndef DLG_DLLLIST_H
#define DLG_DLLLIST_H

#include <QDialog>
#include <Windows.h>

#include "processmanager.h"

namespace Ui {
    class DlgDllList;
}

class QStandardItemModel;
class QTableView;
class QSortFilterProxyModel;
class QLabel;

class DlgDllList : public QDialog
{
    Q_OBJECT

public:

    explicit DlgDllList(pid_t pid, QWidget *parent = nullptr);

    ~DlgDllList();

    void refresh();

protected:



    // 重写关闭事件.
    void closeEvent(QCloseEvent*) override; 

protected slots:

    // 初始化 dll 列表数据
    void slotInitList();

    // 卸载指定的 dll.
    void slotUninstallDll(QString dllName);

private:

    Ui::DlgDllList *ui;

    // 文件选取路径
    QString m_path;

    pid_t m_pid;

};

#endif // DLG_DLLLIST_H
