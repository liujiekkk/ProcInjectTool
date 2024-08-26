#ifndef PROCESSTABLE_H
#define PROCESSTABLE_H

#include "basetable.h"

#include <QList>

class QMenu;
class QAction;
class QStandardItemModel;

class ProcessTable : public BaseTable
{
    Q_OBJECT

private:

    QMenu *m_menu;

    QList<QAction*> m_actions;

    QStandardItemModel *m_model;

public:

    ProcessTable(QWidget *parent);

    // 初始化进程列表
    void process() override;

    // 初始化列表表头
    void initHeader() override;

    // 关联相关信号槽
    void initConnector() override;

signals:

    // 发送注入结果.
    void inject(pid_t, bool);

    // 发送查看模块列表信号.
    void listDll(pid_t);

    // 刷新
    void refresh();

};

#endif // PROCESSTABLE_H
