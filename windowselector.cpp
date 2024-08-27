#include "windowselector.h"
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>

WindowSelector::WindowSelector(QWidget* parent): QDialog::QDialog(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setFixedSize(30, 30);
    setAttribute(Qt::WA_TranslucentBackground);
}

void WindowSelector::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPixmap pix = QPixmap(":/assets/aim.png").scaled(QSize(30, 30), Qt::KeepAspectRatio);
    painter.drawPixmap(0, 0, width(), height(), pix);
}
