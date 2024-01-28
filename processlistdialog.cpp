#include "processlistdialog.h"
#include "ui_processlistdialog.h"

#include <QMessageBox>
#include <QStandardItemModel>
#include <QStandardItem>

#include "processmanager.h"

// extern TableViewHeadConfig procTableViewConfig[], dllTableViewConfig[];

ProcessListDialog::ProcessListDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ProcessListDialog)
{
    ui->setupUi(this);
    setWindowTitle("进程列表");

    // 初始化进程列表模型资源
    pProcTableModel = new QStandardItemModel(this);
    ui->procTableView->setModel(pProcTableModel);


    // 初始化dll列表模型资源
    pDllTableModel = new QStandardItemModel(this);
    ui->dllTableView->setModel(pDllTableModel);

    if (!initProcessList(ui->procTableView, ui->dllTableView)) {
        QMessageBox::warning(this, "警告", "加载进程列表异常", QMessageBox::Ok);
    }
}

ProcessListDialog::~ProcessListDialog()
{
    delete pProcTableModel;
    delete pDllTableModel;
    delete ui;
}

bool ProcessListDialog::initProcessList(QTableView *procView, QTableView *dllView)
{
    // 初始化进程列表表头
    QStandardItemModel *pProcModel = dynamic_cast<QStandardItemModel*>(procView->model());
    // 设置表头的列数为配置数组的长度.
    pProcModel->setColumnCount(sizeof(procTableViewConfig) / sizeof(procTableViewConfig[0]));
    // 隐藏行表头.
    procView->verticalHeader()->setHidden(true);
    for (auto config : procTableViewConfig) {
        pProcModel->setHeaderData(config.index, Qt::Horizontal, QString(config.title));
        procView->setColumnWidth(config.index, config.width);
    }

    // 初始化DLL列表表头
    QStandardItemModel* pDllModel = dynamic_cast<QStandardItemModel*>(dllView->model());
    pDllModel->setColumnCount(sizeof(dllTableViewConfig) / sizeof(dllTableViewConfig[0]));
    // 隐藏行表头.
    dllView->verticalHeader()->setHidden(true);
    for (auto config : dllTableViewConfig) {
        pDllModel->setHeaderData(config.index, Qt::Horizontal, QString(config.title));
        dllView->setColumnWidth(config.index, config.width);
    }


    ProcessManager procManager;
    PIDLIST pidList;
    try {
        // 获取进程PID列表.
        procManager.getAllProcList(pidList);
    } catch (const PMException& e) {
        qDebug() << e.msg;
    }
    // 将进程相关信息放入tableView 中.
    for (int i = 0; i < static_cast<int>(pidList.size()); i++) {
        // 设置进程ID到第一列
        pProcModel->setItem(i, 0, new QStandardItem(QString::number(pidList[i].pid)));
        // 父进程ID.
        pProcModel->setItem(i, 1, new QStandardItem(QString::number(pidList[i].parentPid)));
        // 设置进程名称到第二列
        pProcModel->setItem(i, 2, new QStandardItem(QString::fromWCharArray(pidList[i].name, wcslen(pidList[i].name))));

    }

    return true;
}
