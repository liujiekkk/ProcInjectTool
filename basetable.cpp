#include "basetable.h"

#include <QSortFilterProxyModel>
#include <QMessageBox>

BaseTable::BaseTable(QWidget *parent)
    : QTableView::QTableView(parent), m_modelProxy(new QSortFilterProxyModel(this)), m_data(nullptr)
{
}

void BaseTable::setData(std::shared_ptr<void> data)
{
    m_data = data;
}

bool BaseTable::checkSelected(int num)
{
    if (this->selectionModel()->selectedIndexes().size() < num) {
        QMessageBox::warning(this, tr("警告"), tr("至少选择一项"));
        return false;
    }
    return true;
}

void BaseTable::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    try {
        initHeader();
        initConnector();
        process();
        emit ready(true, tr("加载完成"));
    } catch(const std::exception& e) {
        emit ready(false, e.what());
    } catch(...) {
        emit ready(false, tr("加载过程中出现未知错误"));
    }
}

void BaseTable::filter(const QString &text)
{
    if (model() != m_modelProxy) {
        m_modelProxy->setSourceModel(model());
    }
    // 对表格的所有列数据进行过滤.
    m_modelProxy->setFilterKeyColumn(-1);
    // 忽略输入数据大小写.
    // m_modelProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    // 固定字符串筛选
    // m_modelProxy->setFilterFixedString(text);

    // 正则表达式筛选内容，支持 unicode 字符, 不区分大小写.
    QRegularExpression reg(text, QRegularExpression::UseUnicodePropertiesOption | QRegularExpression::CaseInsensitiveOption);
    // 正则有效才进行过滤.
    if (reg.isValid()) {
        m_modelProxy->setFilterRegularExpression(reg);
    }
    setModel(m_modelProxy);
}
