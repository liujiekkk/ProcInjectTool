#include "dlg_dlllist.h"
#include "ui_dlg_dlllist.h"

#include <QStandardItemModel>
#include <QStandardItem>
#include <QSortFilterProxyModel>
#include <QRegularExpression>
#include <QMouseEvent>
#include <QMessageBox>
#include <QMenu>
#include <QFileDialog>
#include <QDir>
#include <memory>
#include <vector>

#include "injector.h"

DlgDllList::DlgDllList(pid_t pid, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgDllList), m_path("D:/project/"), m_pid(pid)
{
    ui->setupUi(this);
    setWindowTitle("进程列表");
    setFixedWidth(600);

    // 连接进程名称过滤框信号.
    connect(
        ui->lineEditorFilterByProcName,
        SIGNAL(textChanged(QString)),
        ui->dllTableView,
        SLOT(filter(QString))
    );

    // 连接刷新列表信号
    connect(ui->dllTableView, &DllModuleTable::refresh, this, &DlgDllList::slotInitList, Qt::DirectConnection);
    // 连接刷新按钮
    connect(ui->btnRefresh, &QPushButton::clicked, this, &DlgDllList::refresh);
    // 卸载选中
    connect(ui->btnUninstall, &QPushButton::clicked, this, [=](){
        if (!ui->dllTableView->checkSelected()) {
            return;
        }
        int ret = QMessageBox::question(
            this,
            "确认消息",
            "是否卸载",
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No
            );
        if (ret != QMessageBox::Yes) {
            return;
        }
        auto modelIndex = ui->dllTableView->currentIndex();
        // 卸载选中dll
        slotUninstallDll(ui->dllTableView->model()->data(modelIndex).toString());
    });
    // 连接右键卸载dll菜单
    connect(ui->dllTableView, SIGNAL(uninstall(QString)), this, SLOT(slotUninstallDll(QString)));
    // 默认加载一次
    slotInitList();
}

DlgDllList::~DlgDllList()
{
}

void DlgDllList::refresh()
{
    // 给列表设置数据
    slotInitList();
    // 刷新列表
    ui->dllTableView->process();
}

void DlgDllList::closeEvent(QCloseEvent *)
{
}

void DlgDllList::slotInitList()
{
    std::shared_ptr<std::vector<std::wstring>> plist = std::make_shared<std::vector<std::wstring>>();
    // 将模块列表放入容器.
    ProcessManager::getProcessModuleList(m_pid, plist.get());
    // 将数据放到dllview.
    ui->dllTableView->setData(plist);
}

void DlgDllList::slotUninstallDll(QString dllName)
{
    try {
        Injector injector(m_pid);
        if (injector.uninstall(dllName.toStdString().c_str())) {
            // 刷新dll列表.
            refresh();
            QMessageBox::information(this, tr("信息"), QString(dllName) + " 卸载成功.");
        } else {
            QMessageBox::warning(this, tr("警告"), QString(dllName) + " 卸载失败.");
        }
    } catch (std::exception& e) {
        QMessageBox::critical(this, tr("错误"), QString(dllName) + " 卸载失败." + e.what());
    }
}
