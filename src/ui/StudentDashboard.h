/**
 * @file StudentDashboard.h
 * @brief 学生控制面板
 * @author CampusCardSystem
 * @date 2024
 */

#ifndef STUDENTDASHBOARD_H
#define STUDENTDASHBOARD_H

#include <QWidget>
#include "core/CardManager.h"
#include "core/RecordManager.h"

class ElaText;
class ElaPushButton;
class ElaTableView;
class ElaComboBox;
class QDateEdit;

namespace CampusCard {

class RecordTableWidget;

/**
 * @class StudentDashboard
 * @brief 学生控制面板，显示余额、上机记录和统计信息
 */
class StudentDashboard : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param cardManager 卡管理器
     * @param recordManager 记录管理器
     * @param parent 父窗口
     */
    explicit StudentDashboard(CardManager* cardManager,
                              RecordManager* recordManager,
                              QWidget* parent = nullptr);

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

    CardManager* m_cardManager;         ///< 卡管理器
    RecordManager* m_recordManager;     ///< 记录管理器
    QString m_currentCardId;            ///< 当前卡号

    /**
     * @brief 处理日期筛选变化
     */
    void onDateFilterChanged();

    /**
     * @brief 处理修改密码按钮点击
     */
    void onChangePasswordClicked();

    // UI组件
    ElaText* m_nameLabel;               ///< 姓名标签
    ElaText* m_studentIdLabel;          ///< 学号标签
    ElaText* m_balanceLabel;            ///< 余额标签
    ElaText* m_statusLabel;             ///< 状态标签
    ElaText* m_sessionLabel;            ///< 上机状态标签
    ElaText* m_sessionTimeLabel;        ///< 上机时间标签
    ElaText* m_sessionLocationLabel;    ///< 上机地点标签
    
    // 统计信息标签
    ElaText* m_totalSessionsLabel;      ///< 总上机次数标签
    ElaText* m_totalDurationLabel;      ///< 总时长标签
    ElaText* m_totalCostLabel;          ///< 总费用标签
    
    // 日期筛选
    QDateEdit* m_startDateEdit;         ///< 开始日期选择
    QDateEdit* m_endDateEdit;           ///< 结束日期选择
    ElaComboBox* m_locationFilter;      ///< 地点筛选下拉框
    
    ElaPushButton* m_startSessionBtn;   ///< 开始上机按钮
    ElaPushButton* m_endSessionBtn;     ///< 结束上机按钮
    ElaPushButton* m_changePasswordBtn; ///< 修改密码按钮
    ElaPushButton* m_logoutBtn;         ///< 登出按钮
    
    RecordTableWidget* m_recordTable;   ///< 上机记录表格
};

} // namespace CampusCard

#endif // STUDENTDASHBOARD_H
