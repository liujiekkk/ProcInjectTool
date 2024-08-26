#ifndef BASETABLE_H
#define BASETABLE_H

#include <QTableView>
#include <QStandardItemModel>
#include <memory>

#include "processmanager.h"

template<typename T>
struct GetElementType {
    using type = typename T::value_type; // 针对容器
};

class QSortFilterProxyModel;
class QStandardItemModel;

class BaseTable : public QTableView
{
    Q_OBJECT

private:

    // 提供一个模型代理，过滤模型内容.
    QSortFilterProxyModel *m_modelProxy;

protected:

    // 给一个万能类型的对象指针.
    std::shared_ptr<void> m_data;

public:

    BaseTable(QWidget *parent = nullptr);

    virtual ~BaseTable() {}

    // 初始化表头
    virtual void initHeader() = 0;

    // 初始化相关连接器
    virtual void initConnector() = 0;

    // 加载数据
    virtual void process() = 0;

    void setData(std::shared_ptr<void> data);

    // 检测是否选中
    virtual bool checkSelected(int num = 1);

protected:

    void showEvent(QShowEvent *event) override;

    // tableView 加载数据.
    template<typename Data>
    void loadData(QStandardItemModel *model, const Data *data);

    template<typename Data>
    void assignStandardItemData(QStandardItemModel*, int, const Data*);

    template<>
    void assignStandardItemData<std::wstring>(QStandardItemModel *pModel, int index, const std::wstring *data);

    template<>
    void assignStandardItemData<PROCESSDATA>(QStandardItemModel *pModel, int index, const PROCESSDATA *data);

signals:

    // BaseTable 加载完调用.
    void ready(bool, QString);

public slots:

    virtual void filter(const QString&);

};

template<typename Data>
void BaseTable::loadData(QStandardItemModel *model, const Data *data)
{
    // 清理表格数据.
    model->removeRows(0, model->rowCount());
    // 将进程相关信息放入tableView 中.
    for (typename Data::size_type i = 0; i < data->size(); i++) {
        using ElementType = typename GetElementType<Data>::type;
        assignStandardItemData<ElementType>(model, (int)i, &(*data)[i]);
    }
}

template<>
inline void BaseTable::assignStandardItemData<std::wstring>(QStandardItemModel *pModel, int index, const std::wstring *data)
{
    pModel->setItem(index, 0, new QStandardItem(QString::fromWCharArray(data->c_str(), data->size())));
}

template<>
inline void BaseTable::assignStandardItemData<PROCESSDATA>(QStandardItemModel *pModel, int index, const PROCESSDATA *data)
{
    // 设置进程ID到第一列
    pModel->setItem(index, 0, new QStandardItem(QString::number(data->pid)));
    // 父进程ID.
    pModel->setItem(index, 1, new QStandardItem(QString::number(data->parentId)));
    // 设置进程名称到第二列
    pModel->setItem(index, 2, new QStandardItem(QString::fromWCharArray(data->name.c_str(), data->name.size())));
}

#endif // BASETABLE_H
