/**
 * @file RechargeDialog.h
 * @brief 充值对话框
 * @author CampusCardSystem
 * @date 2024
 */

#ifndef RECHARGEDIALOG_H
#define RECHARGEDIALOG_H

#include "ElaContentDialog.h"
#include "core/CardManager.h"

class ElaLineEdit;
class ElaText;
class ElaPushButton;

namespace CampusCard {

/**
 * @class RechargeDialog
 * @brief 充值对话框，用于管理员给学生卡充值
 */
class RechargeDialog : public ElaContentDialog {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param cardManager 卡管理器
     * @param cardId 要充值的卡号
     * @param parent 父窗口
     */
    explicit RechargeDialog(CardManager* cardManager,
                           const QString& cardId,
                           QWidget* parent = nullptr);

private slots:
    /**
     * @brief 处理充值按钮点击
     */
    void onRechargeClicked();

private:
    /**
     * @brief 初始化UI
     */
    void initUI();

    CardManager* m_cardManager;     ///< 卡管理器
    QString m_cardId;               ///< 卡号
    
    ElaText* m_cardInfoLabel;       ///< 卡信息标签
    ElaText* m_balanceLabel;        ///< 余额标签
    ElaLineEdit* m_amountEdit;      ///< 金额输入框
    ElaPushButton* m_rechargeBtn;   ///< 充值按钮
};

} // namespace CampusCard

#endif // RECHARGEDIALOG_H
