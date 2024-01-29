#include "processlistdialog.h"
#include "ui_processlistdialog.h"

#include <QMessageBox>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QSortFilterProxyModel>
#include <QRegularExpression>

#include "processmanager.h"

// extern TableViewHeadConfig procTableViewConfig[], dllTableViewConfig[];

ProcessListDialog::ProcessListDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ProcessListDialog)
{
    ui->setupUi(this);
    setWindowTitle("进程列表");

    // 进程名称过滤框添加信号.
    connect(
        ui->lineEditorFilterByProcName,
        SIGNAL(textChanged(QString)),
        this,
        SLOT(onFilterByProceName(QString))
    );
    // 点击进程触发信号，加载进程模块.
    connect(
        ui->procTableView,
        SIGNAL(activated(QModelIndex)),
        this,
        SLOT(onActivated(QModelIndex))
    );

    // 初始化表格过滤代理模型.
    pSortFilterProxyModel = new QSortFilterProxyModel(this);

    // 初始化进程列表模型资源
    pProcTableModel = new QStandardItemModel(this);
    ui->procTableView->setModel(pProcTableModel);


    // 初始化dll列表模型资源
    pDllTableModel = new QStandardItemModel(this);
    ui->dllTableView->setModel(pDllTableModel);

    initProcessList(ui->procTableView, ui->dllTableView);
}

ProcessListDialog::~ProcessListDialog()
{
    delete pProcTableModel;
    pProcTableModel = nullptr;
    delete pDllTableModel;
    pDllTableModel = nullptr;
    delete pSortFilterProxyModel;
    pSortFilterProxyModel = nullptr;
    delete ui;
}

void ProcessListDialog::initProcessList(QTableView *procView, QTableView *dllView)
{
    // 加载进程列表.
    loadProcModelData(procView);
    // 加载 dll 模块列表.
    loadDllModelData(dllView, 0);
}

void ProcessListDialog::initProcTableViewHead(QTableView *procView)
{
    // 初始化进程列表表头
    QStandardItemModel *pProcModel = dynamic_cast<QStandardItemModel*>(procView->model());
    // 设置表头的列数为配置数组的长度.
    pProcModel->setColumnCount(sizeof(procTableViewConfig) / sizeof(procTableViewConfig[0]));
    // 隐藏行表头.
    procView->verticalHeader()->setHidden(true);
    // 设置表格不可编辑.
    procView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // 设置表格可根据列排序
    procView->setSortingEnabled(true);
    // 单击选中行
    procView->setSelectionBehavior(QAbstractItemView::SelectRows);
    // 设置只能选中单行.
    procView->setSelectionMode(QAbstractItemView::SingleSelection);
    for (auto config : procTableViewConfig) {
        pProcModel->setHeaderData(config.index, Qt::Horizontal, QString(config.title));
        procView->setColumnWidth(config.index, config.width);
    }
    // 设置表格列宽度自适应.
    // procView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void ProcessListDialog::loadProcModelData(QTableView *view)
{
    auto pProcModel = dynamic_cast<QStandardItemModel*>(view->model());
    // 清理表格数据.
    pProcModel->clear();

    initProcTableViewHead(view);

    ProcessManager procManager;
    PIDLIST pidList;
    try {
        // 获取进程PID列表.
        procManager.getAllProcList(pidList);
    } catch (const PMException& e) {
        qDebug() << e.msg;
    }
    // 将进程相关信息放入tableView 中.
    for (int i = 0; i < static_cast<int>(pidList.size()); i++) {
        // 设置进程ID到第一列
        pProcModel->setItem(i, 0, new QStandardItem(QString::number(pidList[i].pid)));
        // 父进程ID.
        pProcModel->setItem(i, 1, new QStandardItem(QString::number(pidList[i].parentPid)));
        // 设置进程名称到第二列
        pProcModel->setItem(i, 2, new QStandardItem(QString::fromWCharArray(pidList[i].name, wcslen(pidList[i].name))));

    }
}

void ProcessListDialog::initDllTableViewHead(QTableView *dllView)
{
    // 初始化DLL列表表头
    QStandardItemModel* pDllModel = dynamic_cast<QStandardItemModel*>(dllView->model());
    pDllModel->setColumnCount(sizeof(dllTableViewConfig) / sizeof(dllTableViewConfig[0]));
    // 隐藏行表头.
    dllView->verticalHeader()->setHidden(true);
    // 设置表格不可编辑.
    dllView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // 单击选中行
    dllView->setSelectionBehavior(QAbstractItemView::SelectRows);
    // 设置只能选中单行模式.
    dllView->setSelectionMode(QAbstractItemView::SingleSelection);
    for (auto config : dllTableViewConfig) {
        pDllModel->setHeaderData(config.index, Qt::Horizontal, QString(config.title));
        dllView->setColumnWidth(config.index, config.width);
    }
}

void ProcessListDialog::loadDllModelData(QTableView *view, uint64_t pid)
{
    QStandardItemModel *pModel = dynamic_cast<QStandardItemModel*>(view->model());
    // 清理 model 中数据.
    pModel->clear();
    // 重新初始化表头.
    initDllTableViewHead(view);
    if (pid <= 0) {
        return;
    }
    ProcessManager pmg;
    ModuleEntryList list;
    try {
        list = pmg.getModuleList(pid);
    } catch (const PMException &e) {
        qDebug() << e.msg;
        return;
    }
    // 将进程相关信息放入tableView 中.
    for (int i = 0; i < static_cast<int>(list.size()); i++) {
        // 设置 dll 模块第一列
        pModel->setItem(i, 0, new QStandardItem(QString::fromStdWString(list[i].szExePath)));
        // 设置进程名称到第二列
        // pDllTableModel->setItem(i, 1, new QStandardItem(QString::fromStdWString(list[i].szModule)));
    }
}

void ProcessListDialog::onFilterByProceName(const QString &text)
{
    pSortFilterProxyModel->setSourceModel(pProcTableModel);
    // 对表格的所有列数据进行过滤.
    pSortFilterProxyModel->setFilterKeyColumn(-1);
    // 忽略输入数据大小写.
    // pSortFilterProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    // 固定字符串筛选
    // pSortFilterProxyModel->setFilterFixedString(text);

    // 正则表达式筛选内容，支持 unicode 字符, 不区分大小写.
    QRegularExpression reg(text, QRegularExpression::UseUnicodePropertiesOption | QRegularExpression::CaseInsensitiveOption);
    // 正则有效才进行过滤.
    if (reg.isValid()) {
        pSortFilterProxyModel->setFilterRegularExpression(reg);
    }
    ui->procTableView->setModel(pSortFilterProxyModel);
}

void ProcessListDialog::onActivated(const QModelIndex &index)
{
    // 获取当前选中的行的进程ID.注意，model 在筛选进程名的时候会换成 QSortFilterProxyModel.
    QAbstractItemModel *model = pProcTableModel;
    if (pSortFilterProxyModel->sourceModel()) {
        model = pSortFilterProxyModel;
    }
    QModelIndex newIndex = model->index(index.row(), 0);
    // 获取当前选中的进程ID.
    uint64_t pid = newIndex.data().toULongLong();
    loadDllModelData(ui->dllTableView, pid);
}
