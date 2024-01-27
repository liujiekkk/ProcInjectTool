#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class VersionDialog;
class ProcessListDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:

    virtual void closeEvent(QCloseEvent*);

protected slots:

    void showVersionDialog(bool);

    void showProcListWindow(bool);

private:
    Ui::MainWindow *ui;

    VersionDialog *pDialog;

    ProcessListDialog *pProcListDlg;
};
#endif // MAINWINDOW_H
