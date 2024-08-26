#ifndef DLLMODULETABLE_H
#define DLLMODULETABLE_H

#include "basetable.h"

#include <QList>

class QMenu;
class QAction;
class QStandardItemModel;
class QSortFilterProxyModel;

class DllModuleTable : public BaseTable
{
    Q_OBJECT

private:

    QMenu *m_menu;

    QList<QAction*> m_actions;

    QStandardItemModel *m_model;

    QSortFilterProxyModel *m_modelProxy;

public:

    DllModuleTable(QWidget *parent = nullptr);

    void initHeader() override;

    void initConnector() override;

    void process() override;

signals:

    void refresh();

    void uninstall(QString dllName);

};

#endif // DLLMODULETABLE_H
