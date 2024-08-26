#include "processtable.h"

#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <memory>

#include "injector.h"

ProcessTable::ProcessTable(QWidget *parent)
    : BaseTable(parent), m_menu(new QMenu(this)), m_actions(), m_model(new QStandardItemModel(this))
{
    // 设置右键信号支持
    setContextMenuPolicy(Qt::CustomContextMenu);
    // 给表格视图设置数据模型.
    setModel(m_model);
    // 初始化右键菜单
    m_actions.append(new QAction(tr("注入")));
    m_actions.append(new QAction(tr("查看模块")));
    m_actions.append(new QAction(tr("刷新")));
    m_menu->addActions(m_actions);
}

void ProcessTable::process()
{
    if (m_data) {
        auto ptr = std::static_pointer_cast<PROCESSLIST>(m_data);
        // 加载数据.
        loadData<PROCESSLIST>(m_model, ptr.get());
        // 这里必须重新关联一次模型（因为模型有可能已经改成了代理模型）
        setModel(m_model);
    }
}

void ProcessTable::initHeader()
{
    // 设置QTableView 可以拉伸
    setFixedWidth(580);
    // 隐藏行表头.
    verticalHeader()->setHidden(true);
    // 设置表格不可编辑.
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    // 设置表格可根据列排序
    setSortingEnabled(true);
    // 单击选中行
    setSelectionBehavior(QAbstractItemView::SelectRows);
    // 设置只能选中单行.
    setSelectionMode(QAbstractItemView::SingleSelection);

    struct _Header {
        QString title;
        int index;
        int width;
    };
    // 定义进程列表表头配置数据.
    const _Header headerConfigData[] = {
        {(char*)"PID", 0, 50},
        {(char*)"PPID", 1, 50},
        {(char*)"Name", 2, 150},
        {(char*)"Window", 3, 100},
        {(char*)"Path", 4, 200}
    };

    // 设置表头的列数为配置数组的长度.
    for (auto config : headerConfigData) {
        m_model->setHorizontalHeaderItem(config.index, new QStandardItem(config.title));
        horizontalHeader()->resizeSection(config.index, config.width);
    }
    // 设置表格列宽度自适应.
    // procView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void ProcessTable::initConnector()
{
    // 关联 procview 的右键菜单信号
    connect(this, &QTableView::customContextMenuRequested, this, [=](){
        m_menu->popup(QCursor::pos());

    });
    connect(m_actions[0], &QAction::triggered, m_actions[0], [=](){
        if (!checkSelected()) {
            return;
        }
        // 选择文件
        QString selectFile = QFileDialog::getOpenFileName(this, tr("选择注入DLL文件"), QDir::currentPath(), tr("DLL (*.dll)"));

        if (selectFile.isEmpty()) {
            return;
        }
        int ret = QMessageBox::question(
            this,
            "确认消息",
            "是否注入",
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No
            );
        if (ret != QMessageBox::Yes) {
            return;
        }
        auto dwProcessId = currentIndex().siblingAtColumn(0).data().toUInt();

        try {
            // 初始化注入器.
            Injector injector(dwProcessId);
            if (injector.install(selectFile.toLatin1().data())) {
                // 注入成功
                emit inject(dwProcessId, true);
            }
        } catch (std::exception& e) {
            QMessageBox::warning(this, "警告", QString("注入失败:").append(e.what()));
        }
    });

    connect(m_actions[1], &QAction::triggered, m_actions[1], [=](){
        if (!checkSelected()) {
            return;
        }
        // 获取选中进程的ID.
        auto dwProcessId = currentIndex().siblingAtColumn(0).data().toUInt();
        emit listDll(dwProcessId);
    });
    // 连接刷新进程列表信号
    connect(m_actions[2], &QAction::triggered, m_actions[2], [=](){
        // 给父窗口发送刷新列表信号
        emit refresh();
        // 刷新完，需要重新显示
        process();
    });
}


