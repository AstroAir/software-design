/**
 * @file RecordTableWidget.h
 * @brief 上机记录表格组件
 * @author CampusCardSystem
 * @date 2024
 */

#ifndef RECORDTABLEWIDGET_H
#define RECORDTABLEWIDGET_H

#include <QWidget>
#include "core/Record.h"

class ElaTableView;
class QStandardItemModel;

namespace CampusCard {

/**
 * @class RecordTableWidget
 * @brief 显示上机记录的表格组件
 */
class RecordTableWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口
     */
    explicit RecordTableWidget(QWidget* parent = nullptr);

    /**
     * @brief 设置记录数据
     * @param records 记录列表
     */
    void setRecords(const QList<Record>& records);

    /**
     * @brief 清空记录
     */
    void clear();

private:
    /**
     * @brief 初始化UI
     */
    void initUI();

    ElaTableView* m_tableView;          ///< 表格视图
    QStandardItemModel* m_model;        ///< 数据模型
};

} // namespace CampusCard

#endif // RECORDTABLEWIDGET_H
