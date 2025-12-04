/**
 * @file LoginDialog.h
 * @brief 登录对话框
 * @author CampusCardSystem
 * @date 2024
 */

#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include "ElaDialog.h"
#include "core/AuthManager.h"
#include "core/CardManager.h"

class ElaLineEdit;
class ElaComboBox;
class ElaPushButton;
class ElaText;

namespace CampusCard {

/**
 * @class LoginDialog
 * @brief 登录对话框，支持学生和管理员登录
 */
class LoginDialog : public ElaDialog {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param authManager 认证管理器
     * @param parent 父窗口
     */
    explicit LoginDialog(AuthManager* authManager, QWidget* parent = nullptr);

    /**
     * @brief 设置卡管理器（用于注册功能）
     * @param cardManager 卡管理器
     */
    void setCardManager(CardManager* cardManager);

private slots:
    /**
     * @brief 处理登录按钮点击
     */
    void onLoginClicked();

    /**
     * @brief 角色切换时更新UI
     * @param index 角色索引
     */
    void onRoleChanged(int index);

    /**
     * @brief 处理注册按钮点击
     */
    void onRegisterClicked();

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
     * @brief 更新提示信息显示
     * @param message 提示消息
     * @param isError 是否为错误信息
     */
    void updateHintLabel(const QString& message, bool isError = false);

    AuthManager* m_authManager;  ///< 认证管理器
    CardManager* m_cardManager;  ///< 卡管理器（用于注册）

    ElaComboBox* m_roleCombo;         ///< 角色选择下拉框
    ElaText* m_cardIdLabel;           ///< 卡号标签
    ElaLineEdit* m_cardIdEdit;        ///< 卡号输入框
    ElaLineEdit* m_passwordEdit;      ///< 密码输入框
    ElaPushButton* m_loginButton;     ///< 登录按钮
    ElaPushButton* m_registerButton;  ///< 注册按钮
    ElaText* m_hintLabel;             ///< 提示信息标签（显示剩余次数等）
};

}  // namespace CampusCard

#endif  // LOGINDIALOG_H
