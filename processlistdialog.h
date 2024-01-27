#ifndef PROCESSLISTDIALOG_H
#define PROCESSLISTDIALOG_H

#include <QDialog>

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

#endif // PROCESSLISTDIALOG_H
