#include "processlistdialog.h"
#include "ui_processlistdialog.h"
#include "processmanager.h"

#include <QMessageBox>
#include <QStandardItemModel>

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
    pProcModel->setColumnCount(4);
    pProcModel->setHeaderData(0, Qt::Horizontal, "PID");
    pProcModel->setHeaderData(1, Qt::Horizontal, "Name");
    pProcModel->setHeaderData(2, Qt::Horizontal, "Window");
    pProcModel->setHeaderData(3, Qt::Horizontal, "Path");
    procView->setShowGrid(true); // 设置显示表格
    procView->setColumnWidth(0, 40);
    procView->setColumnWidth(1, 160);
    procView->setColumnWidth(2, 100);
    procView->setColumnWidth(3, 280);


    // 初始化DLL列表表头
    QStandardItemModel* pDllModel = dynamic_cast<QStandardItemModel*>(dllView->model());
    pDllModel->setColumnCount(3);
    pDllModel->setHeaderData(0, Qt::Horizontal, "Module");
    pDllModel->setHeaderData(1, Qt::Horizontal, "AddrBase");
    pDllModel->setHeaderData(2, Qt::Horizontal, "Size");
    dllView->setShowGrid(true); // 设置显示表格
    dllView->setColumnWidth(0, 450);
    dllView->setColumnWidth(1, 80);
    dllView->setColumnWidth(2, 50);

    ProcessManager procManager;

    return true;
}
