#ifndef DLGWINTOOL_H
#define DLGWINTOOL_H

#include <QDialog>

namespace Ui {
class DlgWinTool;
}

class QLabel;

class DlgWinTool : public QDialog
{
    Q_OBJECT

public:
    explicit DlgWinTool(QWidget *parent = nullptr);

    ~DlgWinTool();

protected:

    void displayIcon(bool = true);

    void mouseReleaseEvent(QMouseEvent*) override;

    void mouseMoveEvent(QMouseEvent*) override;

    void mousePressEvent(QMouseEvent*) override;

private:

    Ui::DlgWinTool *ui;

    // 靶子窗口指针.
    QDialog* m_targetWindow;

};

#endif // DLGWINTOOL_H
