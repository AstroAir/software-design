/**
 * @file RecordTableWidget.h
 * @brief 上机记录表格组件
 * @author CampusCardSystem
 * @date 2024
 *
 * MVC架构 - View层
 * 负责上机记录表格的显示
 */

#ifndef VIEW_WIDGETS_RECORDTABLEWIDGET_H
#define VIEW_WIDGETS_RECORDTABLEWIDGET_H

#include "model/entities/Record.h"

#include <QWidget>


class ElaTableView;
class QStandardItemModel;

namespace CampusCard {

/**
 * @class RecordTableWidget
 * @brief 显示上机记录的表格组件
 *
 * 作为View层的可复用组件，负责：
 * - 显示上机记录列表
 * - 支持排序和选择
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
     * @brief 析构函数
     */
    ~RecordTableWidget() override = default;

    /**
     * @brief 设置记录数据
     * @param records 记录列表
     */
    void setRecords(const QList<Record>& records);

    /**
     * @brief 清空记录
     */
    void clear();

    /**
     * @brief 获取选中的记录ID
     * @return 记录ID列表
     */
    [[nodiscard]] QStringList getSelectedRecordIds() const;

    /**
     * @brief 获取记录数量
     * @return 记录数量
     */
    [[nodiscard]] int recordCount() const;

private:
    /**
     * @brief 初始化UI
     */
    void initUI();

    ElaTableView* m_tableView;    ///< 表格视图
    QStandardItemModel* m_model;  ///< 数据模型
};

}  // namespace CampusCard

#endif  // VIEW_WIDGETS_RECORDTABLEWIDGET_H
