/**
 * @file LoginDialog.h
 * @brief 登录对话框
 * @author CampusCardSystem
 * @date 2024
 *
 * MVC架构 - View层
 * 负责登录界面的显示和用户交互
 */

#ifndef VIEW_DIALOGS_LOGINDIALOG_H
#define VIEW_DIALOGS_LOGINDIALOG_H

#include "ElaDialog.h"
#include "controller/AuthController.h"
#include "controller/CardController.h"

class ElaLineEdit;
class ElaComboBox;
class ElaPushButton;
class ElaText;

namespace CampusCard {

/**
 * @class LoginDialog
 * @brief 登录对话框，支持学生和管理员登录
 *
 * 作为View层，负责：
 * - 显示登录界面
 * - 收集用户输入
 * - 通过Controller处理登录请求
 * - 显示登录结果
 */
class LoginDialog : public ElaDialog {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param authController 认证控制器
     * @param cardController 卡控制器（用于注册）
     * @param parent 父窗口
     */
    explicit LoginDialog(AuthController* authController, CardController* cardController,
                         QWidget* parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~LoginDialog() override = default;

signals:
    /**
     * @brief 注册请求信号
     */
    void registerRequested();

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

    /**
     * @brief 处理登录成功
     * @param role 用户角色
     * @param cardId 卡号
     * @param userName 用户名称
     */
    void onLoginSuccess(UserRole role, const QString& cardId, const QString& userName);

    /**
     * @brief 处理登录失败
     * @param result 失败原因
     * @param message 错误消息
     */
    void onLoginFailed(LoginResult result, const QString& message);

    /**
     * @brief 处理密码错误
     * @param remainingAttempts 剩余尝试次数
     */
    void onPasswordError(int remainingAttempts);

    /**
     * @brief 处理卡被冻结
     * @param cardId 卡号
     */
    void onCardFrozen(const QString& cardId);

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

    /**
     * @brief 重置输入框
     */
    void resetInputs();

    AuthController* m_authController;  ///< 认证控制器
    CardController* m_cardController;  ///< 卡控制器

    ElaComboBox* m_roleCombo;         ///< 角色选择下拉框
    ElaText* m_cardIdLabel;           ///< 卡号标签
    ElaLineEdit* m_cardIdEdit;        ///< 卡号输入框
    ElaLineEdit* m_passwordEdit;      ///< 密码输入框
    ElaPushButton* m_loginButton;     ///< 登录按钮
    ElaPushButton* m_registerButton;  ///< 注册按钮
    ElaText* m_hintLabel;             ///< 提示信息标签
};

}  // namespace CampusCard

#endif  // VIEW_DIALOGS_LOGINDIALOG_H
