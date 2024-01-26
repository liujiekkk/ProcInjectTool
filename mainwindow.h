#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class VersionDialog;
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

private:
    Ui::MainWindow *ui;

    VersionDialog *pDialog;
};
#endif // MAINWINDOW_H
