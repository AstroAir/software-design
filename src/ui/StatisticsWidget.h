/**
 * @file StatisticsWidget.h
 * @brief 统计报表组件
 * @author CampusCardSystem
 * @date 2024
 */

#ifndef STATISTICSWIDGET_H
#define STATISTICSWIDGET_H

#include "core/CardManager.h"
#include "core/RecordManager.h"

#include <QWidget>


class ElaText;
class ElaTableView;
class QDateEdit;
class QStandardItemModel;

namespace CampusCard {

/**
 * @class StatisticsWidget
 * @brief 统计报表组件，显示收入和使用统计
 */
class StatisticsWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param recordManager 记录管理器
     * @param cardManager 卡管理器
     * @param parent 父窗口
     */
    explicit StatisticsWidget(RecordManager* recordManager, CardManager* cardManager,
                              QWidget* parent = nullptr);

private slots:
    /**
     * @brief 日期改变时刷新数据
     */
    void onDateChanged();

private:
    /**
     * @brief 初始化UI
     */
    void initUI();

    /**
     * @brief 刷新统计数据
     */
    void refreshStatistics();

    RecordManager* m_recordManager;  ///< 记录管理器
    CardManager* m_cardManager;      ///< 卡管理器

    QDateEdit* m_dateEdit;          ///< 日期选择
    ElaText* m_incomeLabel;         ///< 收入标签
    ElaText* m_sessionCountLabel;   ///< 上机次数标签
    ElaText* m_totalDurationLabel;  ///< 总时长标签
    ElaTableView* m_detailTable;    ///< 详细记录表格
    QStandardItemModel* m_model;    ///< 数据模型
};

}  // namespace CampusCard

#endif  // STATISTICSWIDGET_H
