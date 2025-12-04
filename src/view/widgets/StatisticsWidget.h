/**
 * @file StatisticsWidget.h
 * @brief 统计报表组件
 * @author CampusCardSystem
 * @date 2024
 *
 * MVC架构 - View层
 * 负责统计报表的显示
 */

#ifndef VIEW_WIDGETS_STATISTICSWIDGET_H
#define VIEW_WIDGETS_STATISTICSWIDGET_H

#include "controller/CardController.h"
#include "controller/RecordController.h"

#include <QWidget>


class ElaText;
class ElaTableView;
class QDateEdit;
class QStandardItemModel;

namespace CampusCard {

/**
 * @class StatisticsWidget
 * @brief 统计报表组件，显示收入和使用统计
 *
 * 作为View层的可复用组件，负责：
 * - 显示日期选择器
 * - 显示统计摘要（收入、次数、时长）
 * - 显示详细记录表格
 */
class StatisticsWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param recordController 记录控制器
     * @param cardController 卡控制器
     * @param parent 父窗口
     */
    explicit StatisticsWidget(RecordController* recordController, CardController* cardController,
                              QWidget* parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~StatisticsWidget() override = default;

    /**
     * @brief 刷新统计数据
     */
    void refresh();

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
     * @brief 初始化信号槽
     */
    void initConnections();

    /**
     * @brief 刷新统计数据
     */
    void refreshStatistics();

    RecordController* m_recordController;  ///< 记录控制器
    CardController* m_cardController;      ///< 卡控制器

    QDateEdit* m_dateEdit;          ///< 日期选择
    ElaText* m_incomeLabel;         ///< 收入标签
    ElaText* m_sessionCountLabel;   ///< 上机次数标签
    ElaText* m_totalDurationLabel;  ///< 总时长标签
    ElaTableView* m_detailTable;    ///< 详细记录表格
    QStandardItemModel* m_model;    ///< 数据模型
};

}  // namespace CampusCard

#endif  // VIEW_WIDGETS_STATISTICSWIDGET_H
