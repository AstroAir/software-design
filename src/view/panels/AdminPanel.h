/**
 * @file AdminPanel.h
 * @brief 管理员控制面板
 * @author CampusCardSystem
 * @date 2024
 *
 * MVC架构 - View层
 * 负责管理员界面的显示和用户交互
 */

#ifndef VIEW_PANELS_ADMINPANEL_H
#define VIEW_PANELS_ADMINPANEL_H

#include "controller/AuthController.h"
#include "controller/CardController.h"
#include "controller/MainController.h"
#include "controller/RecordController.h"

#include <QWidget>


class ElaText;
class ElaPushButton;
class ElaTableView;
class ElaLineEdit;
class QStandardItemModel;

namespace CampusCard {

/**
 * @class AdminPanel
 * @brief 管理员控制面板，提供卡管理和系统管理功能
 *
 * 作为View层，负责：
 * - 显示卡列表和状态
 * - 提供充值、挂失/解挂、冻结/解冻、重置密码等操作
 * - 提供数据导入导出、模拟数据生成、管理员密码修改等系统操作
 * - 显示统计信息
 */
class AdminPanel : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param mainController 主控制器
     * @param parent 父窗口
     */
    explicit AdminPanel(MainController* mainController, QWidget* parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~AdminPanel() override = default;

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
    // ========== 卡操作槽 ==========
    void onRechargeClicked();
    void onReportLostClicked();
    void onCancelLostClicked();
    void onUnfreezeClicked();
    void onResetPasswordClicked();
    void onAddCardClicked();

    // ========== 系统操作槽 ==========
    void onStatisticsClicked();
    void onExportClicked();
    void onImportClicked();
    void onGenerateMockDataClicked();
    void onChangeAdminPasswordClicked();

    // ========== 搜索槽 ==========
    void onSearchTextChanged(const QString& text);

    // ========== 数据更新槽 ==========
    void onCardsUpdated();
    void onCardUpdated(const QString& cardId);

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
     * @brief 刷新统计信息
     */
    void refreshStatistics();

    /**
     * @brief 获取选中的卡号
     * @return 卡号（未选中返回空）
     */
    QString getSelectedCardId() const;

    /**
     * @brief 更新按钮状态
     */
    void updateButtonStates();

    // ========== 控制器 ==========
    MainController* m_mainController;
    CardController* m_cardController;
    RecordController* m_recordController;
    AuthController* m_authController;

    // ========== UI组件 ==========
    ElaTableView* m_cardTable;       ///< 卡列表表格
    QStandardItemModel* m_cardModel; ///< 卡表格数据模型
    ElaLineEdit* m_searchEdit;       ///< 搜索输入框

    // 卡操作按钮
    ElaPushButton* m_rechargeBtn;
    ElaPushButton* m_reportLostBtn;
    ElaPushButton* m_cancelLostBtn;
    ElaPushButton* m_unfreezeBtn;
    ElaPushButton* m_resetPasswordBtn;
    ElaPushButton* m_addCardBtn;

    // 系统操作按钮
    ElaPushButton* m_statisticsBtn;
    ElaPushButton* m_exportBtn;
    ElaPushButton* m_importBtn;
    ElaPushButton* m_mockDataBtn;
    ElaPushButton* m_changeAdminPwdBtn;
    ElaPushButton* m_logoutBtn;

    // 统计信息标签
    ElaText* m_totalCardsLabel;
    ElaText* m_todayIncomeLabel;
    ElaText* m_onlineCountLabel;
};

}  // namespace CampusCard

#endif  // VIEW_PANELS_ADMINPANEL_H
