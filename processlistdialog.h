#ifndef PROCESSLISTDIALOG_H
#define PROCESSLISTDIALOG_H

#include <QDialog>

struct TableViewHeadConfig {
    char *title = nullptr;
    int index = 0;
    int width = 0;
};

namespace Ui {
class ProcessListDialog;
}

class QStandardItemModel;
class QTableView;

class ProcessListDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProcessListDialog(QWidget *parent = nullptr);
    ~ProcessListDialog();

protected:

    bool initProcessList(QTableView *procView, QTableView *dllView);

private:
    Ui::ProcessListDialog *ui;

    QStandardItemModel *pProcTableModel = nullptr;

    QStandardItemModel *pDllTableModel = nullptr;
};

// 定义进程列表表头配置数据.
const TableViewHeadConfig procTableViewConfig[] = {
    {(char*)"PID", 0, 50},
    {(char*)"PPID", 1, 50},
    {(char*)"Name", 2, 150},
    {(char*)"Window", 3, 100},
    {(char*)"Path", 4, 200}
};

// 定义 dll 列表表头配置数据.
const TableViewHeadConfig dllTableViewConfig[] = {
    {(char*)"Module", 0, 530},
};

#endif // PROCESSLISTDIALOG_H
