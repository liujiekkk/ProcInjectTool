#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "versiondialog.h"
#include "dlg_dlllist.h"
#include "dlgwintool.h"

#include <QCloseEvent>
#include <QMessageBox>

#define MAINWINDOW_WIDTH 600
#define MAINWINDOW_HEIGHT 400

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), m_pDialog(nullptr), m_pProcListDlg(nullptr), m_dlgWinTool(nullptr)
{
    ui->setupUi(this);
    // 设置窗口图标.
    setWindowIcon(QIcon(":/assets/main.png"));
    // 设置主窗口固定大小
    setFixedWidth(MAINWINDOW_WIDTH);
    setFixedHeight(MAINWINDOW_HEIGHT);
    // 设置主窗口标题栏
    setWindowTitle("ProcInjectTool");

    connect(ui->actVersion, &QAction::triggered, this, &MainWindow::showVersionDialog);

    // 关联查看dll列表右键菜单
    connect(ui->processTableView, &ProcessTable::listDll, this, &MainWindow::showDlgDllList);

    // 连接筛选进程输入框内容改变信号
    connect(ui->lineEdit, SIGNAL(textChanged(QString)), ui->processTableView, SLOT(filter(QString)));

    // 连接刷新进程列表信号
    connect(ui->processTableView, SIGNAL(refresh()), this, SLOT(slotInitProcList()), Qt::DirectConnection);

    // 设置进程数据，为第一次显示做准备.
    slotInitProcList();
    // 状态栏
    ui->statusbar->showMessage("进程加载完成", 2000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    int ret = QMessageBox::question(
        this,
        "退出程序",
        "是否退出程序！！！",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
    );
    if (ret == QMessageBox::Yes) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::showVersionDialog(bool)
{
    if (!m_pDialog) {
        m_pDialog = new VersionDialog(this);
    }
    m_pDialog->exec();
}

void MainWindow::showDlgDllList(unsigned pid)
{
    try {
        // 捕获获取进程信息时候产生的异常.
        if (!m_pProcListDlg) {
            m_pProcListDlg = new DlgDllList(pid, this);
        } else {
            m_pProcListDlg->refresh();
        }
        m_pProcListDlg->exec();
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "警告", e.what());
    } catch (...) {
        QMessageBox::critical(this, tr("未知错误"), tr("未知错误"));
    }
}

void MainWindow::slotInitProcList()
{
    // 获取所有进程列表
    std::shared_ptr<PROCESSLIST> plist = std::make_shared<PROCESSLIST>();
    ProcessManager::getAllProcessList(plist.get());
    ui->processTableView->setData(plist);
}

void MainWindow::on_actWinTool_triggered()
{
    if (!m_dlgWinTool) {
        m_dlgWinTool = new DlgWinTool(this);
    }
    m_dlgWinTool->exec();
}

