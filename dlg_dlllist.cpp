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

#include "windowselector.h"
#include "injector.h"

// 靶子窗口指针.
QDialog* pTargetWindow = nullptr;

DlgDllList::DlgDllList(pid_t pid, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgDllList), m_path("D:/project/"), m_pid(pid)
{
    ui->setupUi(this);
    setWindowTitle("进程列表");
    setFixedWidth(600);

    // 初始化选择窗口按钮
    ui->selectWindow->setPixmap(QPixmap(":/assets/aim.png"));
    ui->selectWindow->setScaledContents(true);
    ui->selectWindow->setToolTip(tr("拖动到目标窗口"));

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

void DlgDllList::mouseReleaseEvent(QMouseEvent *)
{
    if (!pTargetWindow) {
        return;
    }
    pTargetWindow->hide();
    pTargetWindow->deleteLater();
    pTargetWindow = nullptr;
    // 获取当前所在窗口句柄.
    POINT ptCursor;
    GetCursorPos(&ptCursor);
    // 获取当前鼠标箭头所在的顶层窗口.
    HWND hWnd = WindowFromPoint(ptCursor);
    if (hWnd != 0) {
        // 获取窗口所在进程
        DWORD processId = 0;
        ::GetWindowThreadProcessId(hWnd, &processId);
        //获取窗口标题
        TCHAR szTitle[MAX_PATH];
        GetWindowText(hWnd, szTitle, MAX_PATH);
        //获取窗口类名
        TCHAR szClass[MAX_PATH];
        GetClassName(hWnd, szClass, MAX_PATH);
        //获取窗口样式
        LONG style = GetWindowLong(hWnd, GWL_STYLE);
        //获取窗口矩形
        RECT rect;
        GetWindowRect(hWnd, &rect);
        QRect window_rect(rect.left, rect.top, rect.right, rect.bottom);
        /*
        QString windowsHwnd = QString("%1").arg((quintptr)hWnd, QT_POINTER_SIZE, 16, QLatin1Char('0'));
        QString windowsTitle = QString::fromWCharArray(szTitle);
        QString windowsClass = QString::fromWCharArray(szClass);
        QString windowsStyle = QString("%1").arg(style, QT_POINTER_SIZE, 16, QChar('0'));
        QString windowsRect = QString("(%1,%2)-(%3,%4) %5×%6")
                                  .arg(window_rect.topLeft().x())
                                  .arg(window_rect.topLeft().y())
                                  .arg(window_rect.bottomRight().x())
                                  .arg(window_rect.bottomRight().y())
                                  .arg(window_rect.width())
                                  .arg(window_rect.height());
        qDebug() << "processid:" << processId;
        qDebug() << "windowHwnd:" << windowsHwnd;
        qDebug() << "windowsTitle:" << windowsTitle;
        qDebug() << "windowsClass:" << windowsClass;
        qDebug() << "windowsStyle:" << windowsStyle;
        qDebug() << "windowsRect:" << windowsRect;
        */
    }
    // qDebug() << hWnd << "x:" << ptCursor.x << "y:" << ptCursor.y;
}

void DlgDllList::mouseMoveEvent(QMouseEvent *)
{
    if (pTargetWindow) {
        // 鼠标移动时候自动移动靶子窗口.
        pTargetWindow->move(
            QCursor::pos().x() - pTargetWindow->width() / 2,
            QCursor::pos().y() - pTargetWindow->height() / 2);
    }
}

void DlgDllList::mousePressEvent(QMouseEvent *)
{
    // 点击鼠标时候判断是否在左侧的靶子范围内
    if (ui->selectWindow->underMouse()) {
        if (pTargetWindow == nullptr) {
            pTargetWindow = new WindowSelector(this);
            pTargetWindow->move(
                QCursor::pos().x() - pTargetWindow->width() / 2,
                QCursor::pos().y() - pTargetWindow->height() / 2);
            pTargetWindow->show();
        }
    }
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
