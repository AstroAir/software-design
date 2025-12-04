/**
 * @file AuthService.h
 * @brief 认证业务服务类
 * @author CampusCardSystem
 * @date 2024
 *
 * MVC架构 - Model层业务服务
 * 负责用户认证相关的业务逻辑处理
 */

#ifndef MODEL_SERVICES_AUTHSERVICE_H
#define MODEL_SERVICES_AUTHSERVICE_H

#include "CardService.h"
#include "model/entities/User.h"
#include "model/repositories/StorageManager.h"

#include <QObject>
#include <QString>

#include <optional>


namespace CampusCard {

/**
 * @class AuthService
 * @brief 认证业务服务类，处理用户登录和会话管理
 *
 * 作为Service层，负责：
 * - 学生登录验证
 * - 管理员登录验证
 * - 会话状态管理
 * - 密码错误计数和卡冻结逻辑
 * - 通过信号通知登录状态变更
 */
class AuthService : public QObject {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param cardService 卡服务引用
     * @param parent 父对象
     */
    explicit AuthService(CardService* cardService, QObject* parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~AuthService() override = default;

    // ========== 登录操作 ==========

    /**
     * @brief 学生登录
     * @param cardId 卡号
     * @param password 密码
     * @return 登录结果
     */
    LoginResult studentLogin(const QString& cardId, const QString& password);

    /**
     * @brief 管理员登录
     * @param password 管理员密码
     * @return 登录结果
     */
    LoginResult adminLogin(const QString& password);

    /**
     * @brief 登出
     */
    void logout();

    // ========== 会话状态查询 ==========

    /**
     * @brief 检查是否已登录
     * @return 是否登录
     */
    [[nodiscard]] bool isLoggedIn() const { return m_isLoggedIn; }

    /**
     * @brief 获取当前用户
     * @return 当前用户对象（未登录返回空optional）
     */
    [[nodiscard]] std::optional<User> currentUser() const;

    /**
     * @brief 获取当前用户角色
     * @return 用户角色
     */
    [[nodiscard]] UserRole currentRole() const { return m_currentRole; }

    /**
     * @brief 获取当前登录的卡号（学生专用）
     * @return 卡号
     */
    [[nodiscard]] QString currentCardId() const { return m_currentCardId; }

    /**
     * @brief 检查当前用户是否为管理员
     * @return 是否管理员
     */
    [[nodiscard]] bool isAdmin() const { return m_isLoggedIn && m_currentRole == UserRole::Admin; }

    /**
     * @brief 检查当前用户是否为学生
     * @return 是否学生
     */
    [[nodiscard]] bool isStudent() const {
        return m_isLoggedIn && m_currentRole == UserRole::Student;
    }

    // ========== 管理员密码管理 ==========

    /**
     * @brief 验证管理员密码
     * @param password 密码
     * @return 是否正确
     */
    [[nodiscard]] bool verifyAdminPassword(const QString& password) const;

    /**
     * @brief 修改管理员密码
     * @param oldPassword 旧密码
     * @param newPassword 新密码
     * @return 是否成功
     */
    bool changeAdminPassword(const QString& oldPassword, const QString& newPassword);

    /**
     * @brief 获取当前管理员密码
     * @return 管理员密码
     */
    [[nodiscard]] QString getAdminPassword() const;

    // ========== 卡状态检查（登录前预检） ==========

    /**
     * @brief 检查卡是否可以登录
     * @param cardId 卡号
     * @return 登录结果（Success表示可以登录）
     */
    [[nodiscard]] LoginResult checkCardLoginStatus(const QString& cardId) const;

    /**
     * @brief 获取卡剩余登录尝试次数
     * @param cardId 卡号
     * @return 剩余次数（-1表示卡不存在）
     */
    [[nodiscard]] int getRemainingLoginAttempts(const QString& cardId) const;

signals:
    /**
     * @brief 登录成功信号
     * @param role 用户角色
     * @param cardId 卡号（管理员为空）
     */
    void loginSucceeded(UserRole role, const QString& cardId);

    /**
     * @brief 登录失败信号
     * @param result 失败原因
     * @param cardId 卡号（管理员登录为空）
     */
    void loginFailed(LoginResult result, const QString& cardId);

    /**
     * @brief 登出信号
     */
    void loggedOut();

    /**
     * @brief 密码错误信号
     * @param cardId 卡号
     * @param remainingAttempts 剩余尝试次数
     */
    void passwordError(const QString& cardId, int remainingAttempts);

    /**
     * @brief 卡被冻结信号
     * @param cardId 卡号
     */
    void cardFrozen(const QString& cardId);

private:
    CardService* m_cardService;              ///< 卡服务指针
    bool m_isLoggedIn = false;               ///< 是否已登录
    UserRole m_currentRole = UserRole::Student;  ///< 当前角色
    QString m_currentCardId;                 ///< 当前登录的卡号
};

}  // namespace CampusCard

#endif  // MODEL_SERVICES_AUTHSERVICE_H
