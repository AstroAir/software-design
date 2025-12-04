/**
 * @file AdminDashboard.h
 * @brief 管理员控制面板
 * @author CampusCardSystem
 * @date 2024
 */

#ifndef ADMINDASHBOARD_H
#define ADMINDASHBOARD_H

#include "core/CardManager.h"
#include "core/RecordManager.h"

#include <QStandardItemModel>
#include <QWidget>


class ElaTableView;
class ElaPushButton;
class ElaLineEdit;
class ElaText;
class QDateEdit;

namespace CampusCard {

/**
 * @class AdminDashboard
 * @brief 管理员控制面板，提供充值、挂失、密码重置等功能
 */
class AdminDashboard : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param cardManager 卡管理器
     * @param recordManager 记录管理器
     * @param parent 父窗口
     */
    explicit AdminDashboard(CardManager* cardManager, RecordManager* recordManager,
                            QWidget* parent = nullptr);

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
     * @brief 处理充值按钮点击
     */
    void onRechargeClicked();

    /**
     * @brief 处理挂失按钮点击
     */
    void onReportLostClicked();

    /**
     * @brief 处理解挂按钮点击
     */
    void onCancelLostClicked();

    /**
     * @brief 处理解冻按钮点击
     */
    void onUnfreezeClicked();

    /**
     * @brief 处理重置密码按钮点击
     */
    void onResetPasswordClicked();

    /**
     * @brief 处理统计按钮点击
     */
    void onStatisticsClicked();

    /**
     * @brief 处理导出按钮点击
     */
    void onExportClicked();

    /**
     * @brief 处理导入按钮点击
     */
    void onImportClicked();

    /**
     * @brief 处理生成模拟数据按钮点击
     */
    void onGenerateMockDataClicked();

    /**
     * @brief 处理修改管理员密码按钮点击
     */
    void onChangeAdminPasswordClicked();

    /**
     * @brief 处理添加新卡按钮点击
     */
    void onAddCardClicked();

    /**
     * @brief 处理收入日期变化
     */
    void onIncomeDateChanged();

    /**
     * @brief 处理表格选择变化
     */
    void onSelectionChanged();

    /**
     * @brief 处理搜索
     */
    void onSearchChanged(const QString& text);

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
     * @brief 刷新卡列表
     */
    void refreshCardList();

    /**
     * @brief 获取选中的卡号
     * @return 选中的卡号，未选中返回空
     */
    QString getSelectedCardId() const;

    CardManager* m_cardManager;      ///< 卡管理器
    RecordManager* m_recordManager;  ///< 记录管理器

    ElaTableView* m_cardTable;           ///< 卡列表表格
    ElaLineEdit* m_searchEdit;           ///< 搜索框
    ElaPushButton* m_rechargeBtn;        ///< 充值按钮
    ElaPushButton* m_reportLostBtn;      ///< 挂失按钮
    ElaPushButton* m_cancelLostBtn;      ///< 解挂按钮
    ElaPushButton* m_unfreezeBtn;        ///< 解冻按钮
    ElaPushButton* m_resetPwdBtn;        ///< 重置密码按钮
    ElaPushButton* m_statisticsBtn;      ///< 统计按钮
    ElaPushButton* m_exportBtn;          ///< 导出按钮
    ElaPushButton* m_importBtn;          ///< 导入按钮
    ElaPushButton* m_mockDataBtn;        ///< 生成模拟数据按钮
    ElaPushButton* m_logoutBtn;          ///< 退出登录按钮
    ElaPushButton* m_changeAdminPwdBtn;  ///< 修改管理员密码按钮
    ElaPushButton* m_addCardBtn;         ///< 添加新卡按钮
    ElaText* m_incomeLabel;              ///< 当日收入标签
    ElaText* m_totalCardsLabel;          ///< 总卡数标签
    ElaText* m_onlineCountLabel;         ///< 当前在线人数标签
    QDateEdit* m_incomeDateEdit;         ///< 收入日期选择

    QStandardItemModel* m_cardModel;  ///< 卡数据模型
};

}  // namespace CampusCard

#endif  // ADMINDASHBOARD_H
