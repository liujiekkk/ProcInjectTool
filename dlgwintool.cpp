#include "dlgwintool.h"
#include "ui_dlgwintool.h"
#include "windowselector.h"

#include <QLabel>

#include <Windows.h>

DlgWinTool::DlgWinTool(QWidget *parent)
    : QDialog(parent) , ui(new Ui::DlgWinTool), m_targetWindow(new WindowSelector(this))
{
    ui->setupUi(this);
}

void DlgWinTool::mouseReleaseEvent(QMouseEvent *event)
{
    // 只有左键释放，且靶子窗口可见的情况下获取窗口信息，其他情况均无操作.
    if (!(event->button() == Qt::LeftButton && !m_targetWindow->isHidden())) {
        return;
    }
    // 显示图标.
    displayIcon();
    m_targetWindow->hide();
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
        LONG style = GetWindowLongPtr(hWnd, GWL_STYLE);
        //获取窗口矩形
        RECT rect;
        GetWindowRect(hWnd, &rect);
        QRect window_rect(rect.left, rect.top, rect.right, rect.bottom);

        QString processIdLine = "进程ID：" + QString::number(processId);
        QString windowHwndLine = "窗口句柄：0x" + QString::number((uintptr_t)hWnd, 16).rightJustified(QT_POINTER_SIZE, '0').toUpper();
        QString windowTitleLine = "窗口标题：" + QString::fromWCharArray(szTitle);
        QString windowClassLine = "窗口类：" + QString::fromWCharArray(szClass);
        QString windowsStyleLine = "窗口样式：";
        if (style < 0) {
            windowsStyleLine += "-0x" + QString::number(-style, 16);
        } else {
            windowsStyleLine += "0x" + QString::number(style, 16).rightJustified(QT_POINTER_SIZE, '0').toUpper();
        }
        QString windowsRectLine = "窗口位置：" + QString("(%1,%2) - (%3,%4) %5×%6")
                                  .arg(window_rect.topLeft().x())
                                  .arg(window_rect.topLeft().y())
                                  .arg(window_rect.bottomRight().x())
                                  .arg(window_rect.bottomRight().y())
                                  .arg(window_rect.width())
                                  .arg(window_rect.height());

        // 显示到信息获取的地方.
        ui->plainTextEdit->clear();
        ui->plainTextEdit->appendPlainText(processIdLine);
        ui->plainTextEdit->appendPlainText(windowHwndLine);
        ui->plainTextEdit->appendPlainText(windowClassLine);
        ui->plainTextEdit->appendPlainText(windowTitleLine);
        ui->plainTextEdit->appendPlainText(windowsStyleLine);
        ui->plainTextEdit->appendPlainText(windowsRectLine);

    }
    // qDebug() << hWnd << "x:" << ptCursor.x << "y:" << ptCursor.y;
}

void DlgWinTool::mouseMoveEvent(QMouseEvent *event)
{
    // 只有左键处于按住状态才能产生拖动.
    if (event->button() == Qt::NoButton && event->buttons() & Qt::LeftButton) {
        if (!m_targetWindow->isHidden()) {
            // 鼠标移动时候自动移动靶子窗口.
            m_targetWindow->move(
                QCursor::pos().x() - m_targetWindow->width() / 2,
                QCursor::pos().y() - m_targetWindow->height() / 2);
        }
    }
}

void DlgWinTool::mousePressEvent(QMouseEvent *event)
{
    // 点击鼠标时候判断是否在靶子标签范围内，只有左键触发，且在图标范围才会产生拖动效果.
    if (event->button() == Qt::LeftButton && ui->selectWindow->underMouse()) {
        // 将标签内的图标隐藏
        displayIcon(false);
        if (m_targetWindow->isHidden()) {
            m_targetWindow->move(
                QCursor::pos().x() - m_targetWindow->width() / 2,
                QCursor::pos().y() - m_targetWindow->height() / 2);
            m_targetWindow->show();
        }
    }
}

DlgWinTool::~DlgWinTool()
{
    delete ui;
}

void DlgWinTool::displayIcon(bool isShow)
{
    if (isShow) {
        // 初始化选择窗口按钮
        ui->selectWindow->setPixmap(QPixmap(":/assets/aim.png"));
    } else {
        ui->selectWindow->clear();
    }
}
