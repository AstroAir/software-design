/**
 * @file StudentPanel.h
 * @brief 学生控制面板
 * @author CampusCardSystem
 * @date 2024
 *
 * MVC架构 - View层
 * 负责学生界面的显示和用户交互
 */

#ifndef VIEW_PANELS_STUDENTPANEL_H
#define VIEW_PANELS_STUDENTPANEL_H

#include "controller/CardController.h"
#include "controller/RecordController.h"
#include "view/widgets/RecordTableWidget.h"

#include <QWidget>


class ElaText;
class ElaPushButton;
class ElaComboBox;
class QDateEdit;

namespace CampusCard {

/**
 * @class StudentPanel
 * @brief 学生控制面板，显示余额、上机记录和统计信息
 *
 * 作为View层，负责：
 * - 显示卡片信息（姓名、学号、余额、状态）
 * - 显示和管理上机状态
 * - 显示上机记录和统计
 * - 提供上机/下机、修改密码等操作
 */
class StudentPanel : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param cardController 卡控制器
     * @param recordController 记录控制器
     * @param parent 父窗口
     */
    explicit StudentPanel(CardController* cardController, RecordController* recordController,
                          QWidget* parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~StudentPanel() override = default;

    /**
     * @brief 设置当前卡号
     * @param cardId 卡号
     */
    void setCurrentCard(const QString& cardId);

    /**
     * @brief 刷新数据
     */
    void refresh();

signals:
    /**
     * @brief 登出请求信号
     */
    void logoutRequested();

private slots:
    /**
     * @brief 处理开始上机按钮
     */
    void onStartSessionClicked();

    /**
     * @brief 处理结束上机按钮
     */
    void onEndSessionClicked();

    /**
     * @brief 处理修改密码按钮点击
     */
    void onChangePasswordClicked();

    /**
     * @brief 处理日期筛选变化
     */
    void onFilterChanged();

    /**
     * @brief 处理上机成功
     * @param cardId 卡号
     * @param location 地点
     */
    void onSessionStarted(const QString& cardId, const QString& location);

    /**
     * @brief 处理下机成功
     * @param cardId 卡号
     * @param cost 费用
     * @param duration 时长
     */
    void onSessionEnded(const QString& cardId, double cost, int duration);

    /**
     * @brief 处理记录更新
     * @param cardId 卡号
     */
    void onRecordsUpdated(const QString& cardId);

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
     * @brief 更新卡信息显示
     */
    void updateCardInfo();

    /**
     * @brief 更新上机状态
     */
    void updateSessionStatus();

    /**
     * @brief 更新统计信息
     */
    void updateStatistics();

    /**
     * @brief 更新地点筛选下拉框
     */
    void updateLocationFilter();

    /**
     * @brief 更新记录表格
     */
    void updateRecordTable();

    CardController* m_cardController;      ///< 卡控制器
    RecordController* m_recordController;  ///< 记录控制器
    QString m_currentCardId;               ///< 当前卡号

    // UI组件 - 卡信息
    ElaText* m_nameLabel;
    ElaText* m_studentIdLabel;
    ElaText* m_balanceLabel;
    ElaText* m_statusLabel;

    // UI组件 - 上机状态
    ElaText* m_sessionLabel;
    ElaText* m_sessionTimeLabel;
    ElaText* m_sessionLocationLabel;

    // UI组件 - 统计信息
    ElaText* m_totalSessionsLabel;
    ElaText* m_totalDurationLabel;
    ElaText* m_totalCostLabel;

    // UI组件 - 日期筛选
    QDateEdit* m_startDateEdit;
    QDateEdit* m_endDateEdit;
    ElaComboBox* m_locationFilter;

    // UI组件 - 按钮
    ElaPushButton* m_startSessionBtn;
    ElaPushButton* m_endSessionBtn;
    ElaPushButton* m_changePasswordBtn;
    ElaPushButton* m_logoutBtn;

    // UI组件 - 记录表格
    RecordTableWidget* m_recordTable;
};

}  // namespace CampusCard

#endif  // VIEW_PANELS_STUDENTPANEL_H
