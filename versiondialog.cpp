#include "versiondialog.h"
#include "ui_versiondialog.h"

VersionDialog::VersionDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::VersionDialog)
{
    ui->setupUi(this);
    setFixedWidth(200);
    setFixedHeight(100);
    setWindowTitle("版本号");
}

VersionDialog::~VersionDialog()
{
    delete ui;
}
