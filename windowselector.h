#ifndef WINDOWSELECTOR_H
#define WINDOWSELECTOR_H

#include <QDialog>
#include <QObject>

class WindowSelector : public QDialog
{
    Q_OBJECT

public:

    WindowSelector(QWidget* parent);

protected:

    void paintEvent(QPaintEvent*) override;
};

#endif // WINDOWSELECTOR_H
