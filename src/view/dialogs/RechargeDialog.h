/**
 * @file RechargeDialog.h
 * @brief 充值对话框
 * @author CampusCardSystem
 * @date 2024
 *
 * MVC架构 - View层
 * 负责充值界面的显示和用户交互
 */

#ifndef VIEW_DIALOGS_RECHARGEDIALOG_H
#define VIEW_DIALOGS_RECHARGEDIALOG_H

#include "ElaContentDialog.h"
#include "controller/CardController.h"

class ElaLineEdit;
class ElaText;
class ElaPushButton;

namespace CampusCard {

/**
 * @class RechargeDialog
 * @brief 充值对话框，用于管理员给学生卡充值
 *
 * 作为View层，负责：
 * - 显示充值界面
 * - 收集用户输入
 * - 通过Controller处理充值请求
 * - 显示充值结果
 */
class RechargeDialog : public ElaContentDialog {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param cardController 卡控制器
     * @param cardId 要充值的卡号
     * @param parent 父窗口
     */
    explicit RechargeDialog(CardController* cardController, const QString& cardId,
                            QWidget* parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~RechargeDialog() override = default;

private slots:
    /**
     * @brief 处理充值按钮点击
     */
    void onRechargeClicked();

    /**
     * @brief 处理充值成功
     * @param cardId 卡号
     * @param newBalance 新余额
     */
    void onRechargeSuccess(const QString& cardId, double newBalance);

    /**
     * @brief 处理充值失败
     * @param message 错误消息
     */
    void onRechargeFailed(const QString& message);

private:
    /**
     * @brief 初始化UI
     */
    void initUI();

    /**
     * @brief 初始化信号槽
     */
    void initConnections();

    CardController* m_cardController;  ///< 卡控制器
    QString m_cardId;                  ///< 卡号

    ElaText* m_cardInfoLabel;      ///< 卡信息标签
    ElaText* m_balanceLabel;       ///< 余额标签
    ElaLineEdit* m_amountEdit;     ///< 金额输入框
    ElaPushButton* m_rechargeBtn;  ///< 充值按钮
};

}  // namespace CampusCard

#endif  // VIEW_DIALOGS_RECHARGEDIALOG_H
