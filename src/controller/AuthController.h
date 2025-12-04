/**
 * @file AuthController.h
 * @brief 认证控制器
 * @author CampusCardSystem
 * @date 2024
 *
 * MVC架构 - Controller层
 * 负责协调认证相关的View和Model交互
 */

#ifndef CONTROLLER_AUTHCONTROLLER_H
#define CONTROLLER_AUTHCONTROLLER_H

#include "model/services/AuthService.h"
#include "model/services/CardService.h"

#include <QObject>


namespace CampusCard {

/**
 * @class AuthController
 * @brief 认证控制器，处理登录/登出相关的用户交互
 *
 * 作为Controller层，负责：
 * - 接收View的登录/登出请求
 * - 调用AuthService处理业务逻辑
 * - 将结果反馈给View
 */
class AuthController : public QObject {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param authService 认证服务
     * @param cardService 卡服务
     * @param parent 父对象
     */
    explicit AuthController(AuthService* authService, CardService* cardService,
                            QObject* parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~AuthController() override = default;

    // ========== 登录操作 ==========

    /**
     * @brief 处理学生登录请求
     * @param cardId 卡号
     * @param password 密码
     */
    void handleStudentLogin(const QString& cardId, const QString& password);

    /**
     * @brief 处理管理员登录请求
     * @param password 密码
     */
    void handleAdminLogin(const QString& password);

    /**
     * @brief 处理登出请求
     */
    void handleLogout();

    // ========== 状态查询 ==========

    /**
     * @brief 检查是否已登录
     * @return 是否登录
     */
    [[nodiscard]] bool isLoggedIn() const;

    /**
     * @brief 获取当前用户角色
     * @return 用户角色
     */
    [[nodiscard]] UserRole currentRole() const;

    /**
     * @brief 获取当前卡号
     * @return 卡号
     */
    [[nodiscard]] QString currentCardId() const;

    /**
     * @brief 获取当前用户名称
     * @return 用户名称（学生返回姓名，管理员返回"管理员"）
     */
    [[nodiscard]] QString currentUserName() const;

    // ========== 登录预检 ==========

    /**
     * @brief 检查卡是否可以登录
     * @param cardId 卡号
     * @return 登录结果
     */
    [[nodiscard]] LoginResult checkCardStatus(const QString& cardId) const;

    /**
     * @brief 获取剩余登录尝试次数
     * @param cardId 卡号
     * @return 剩余次数
     */
    [[nodiscard]] int getRemainingAttempts(const QString& cardId) const;

    // ========== 密码管理 ==========

    /**
     * @brief 处理修改管理员密码请求
     * @param oldPassword 旧密码
     * @param newPassword 新密码
     */
    void handleChangeAdminPassword(const QString& oldPassword, const QString& newPassword);

signals:
    // ========== 登录结果信号 ==========

    /**
     * @brief 登录成功信号
     * @param role 用户角色
     * @param cardId 卡号
     * @param userName 用户名称
     */
    void loginSuccess(UserRole role, const QString& cardId, const QString& userName);

    /**
     * @brief 登录失败信号
     * @param result 失败原因
     * @param message 错误消息
     */
    void loginFailed(LoginResult result, const QString& message);

    /**
     * @brief 登出成功信号
     */
    void logoutSuccess();

    // ========== 密码相关信号 ==========

    /**
     * @brief 密码错误信号
     * @param remainingAttempts 剩余尝试次数
     */
    void passwordError(int remainingAttempts);

    /**
     * @brief 卡被冻结信号
     * @param cardId 卡号
     */
    void cardFrozen(const QString& cardId);

    /**
     * @brief 管理员密码修改成功信号
     */
    void adminPasswordChanged();

    /**
     * @brief 管理员密码修改失败信号
     * @param message 错误消息
     */
    void adminPasswordChangeFailed(const QString& message);

private:
    AuthService* m_authService;  ///< 认证服务
    CardService* m_cardService;  ///< 卡服务
};

}  // namespace CampusCard

#endif  // CONTROLLER_AUTHCONTROLLER_H
