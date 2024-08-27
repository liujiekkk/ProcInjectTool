#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSortFilterProxyModel>

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class VersionDialog;
class DlgDllList;
class DlgWinTool;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

protected slots:

    void showVersionDialog(bool);

    void showDlgDllList(unsigned);

    void slotInitProcList();

protected:

    void closeEvent(QCloseEvent*);

private slots:
    void on_actWinTool_triggered();

private:

    Ui::MainWindow *ui;

    VersionDialog *m_pDialog;

    DlgDllList *m_pProcListDlg;

    DlgWinTool *m_dlgWinTool;

};

#endif // MAINWINDOW_H
