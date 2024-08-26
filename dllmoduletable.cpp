#include "dllmoduletable.h"

#include <QMenu>
#include <QAction>
#include <vector>
#include <QHeaderView>
#include <QMessageBox>
#include <QSortFilterProxyModel>
#include <QApplication>
#include <QClipboard>

DllModuleTable::DllModuleTable(QWidget *parent)
    : BaseTable(parent), m_menu(new QMenu(this)), m_actions(),
    m_model(new QStandardItemModel(this)), m_modelProxy(new QSortFilterProxyModel(this))
{
    // 设置关联数据模型
    setModel(m_model);

    // 设置右键信号支持
    setContextMenuPolicy(Qt::CustomContextMenu);

    m_actions.append(new QAction("卸载DLL", this));
    m_actions.append(new QAction("复制", this));
    m_actions.append(new QAction("刷新", this));
    m_menu->addActions(m_actions);
}

void DllModuleTable::initHeader()
{
    // 设置QTableView 可以拉伸
    setFixedWidth(580);
    // 隐藏列表头.
    verticalHeader()->setHidden(true);
    // 设置表格不可编辑.
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    // 设置表格可根据列排序
    setSortingEnabled(true);
    // 多行选中
    setSelectionBehavior(QAbstractItemView::SelectRows);
    // 设置表格列宽度自适应.
    // horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    struct _Header {
        QString title;
        int index;
        int width;
    };

    // 定义 dll 列表表头配置数据.
    const _Header headerConfig[] = {
        {"Module", 0, 530},
    };
    for (auto& config : headerConfig) {
        m_model->setHorizontalHeaderItem(config.index, new QStandardItem(config.title));
        horizontalHeader()->resizeSection(config.index, config.width);
    }
}

void DllModuleTable::initConnector()
{
    // 连接右键菜单弹出信号
    connect(this, &QTableView::customContextMenuRequested, this, [=](){
        m_menu->popup(QCursor::pos());
    });
    // 连接卸载信号
    connect(m_actions[0], &QAction::triggered, m_actions[0], [=](){
        if (!checkSelected()) {
            return;
        }
        int ret = QMessageBox::question(
            this,
            "确认消息",
            "是否卸载",
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No
            );
        if (ret != QMessageBox::Yes) {
            return;
        }
        // 发送卸载信号dll信号.
        emit uninstall(model()->data(currentIndex()).toString());
    });
    // 连接复制信息
    connect(m_actions[1], &QAction::triggered, m_actions[1], [=](){
        if (!checkSelected()) {
            return;
        }
        // 复制当前选中内容到剪切板.
        auto dllPath = model()->data(currentIndex()).toString();
        QApplication::clipboard()->setText(dllPath);
    });
    // 连接刷新信号.
    connect(m_actions[2], &QAction::triggered, m_actions[2], [=](){
        emit refresh();
        process();
    });
}

void DllModuleTable::process()
{
    if (m_data) {
        auto ptr = std::static_pointer_cast<std::vector<std::wstring>>(m_data);
        // 加载数据.
        loadData<std::vector<std::wstring>>(m_model, ptr.get());
        // 这里必须重新关联一次模型（因为模型有可能已经改成了代理模型）
        setModel(m_model);
    }
}
