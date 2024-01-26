#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "versiondialog.h"
#include <QDebug>
#include <QCloseEvent>
#include <QMessageBox>

#define MAINWINDOW_WIDTH 600
#define MAINWINDOW_HEIGHT 400

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 设置主窗口固定大小
    setFixedWidth(MAINWINDOW_WIDTH);
    setFixedHeight(MAINWINDOW_HEIGHT);
    // 设置主窗口标题栏
    setWindowTitle("ProcInjectTool");
    // 设置窗口不能改变大小.
    setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);

    // connect(ui->actVersion, SIGNAL(triggered(bool)), this, SLOT(showVersionDialog(bool)));
    connect(ui->actVersion, &QAction::triggered, this, &MainWindow::showVersionDialog);
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

void MainWindow::showVersionDialog(bool checked)
{
    Q_UNUSED(checked);
    pDialog = new VersionDialog(this);
    pDialog->exec();
}
