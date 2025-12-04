/**
 * @file AuthManager.h
 * @brief 认证管理器，处理登录验证
 * @author CampusCardSystem
 * @date 2024
 */

#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include "CardManager.h"
#include "StorageManager.h"
#include "Types.h"
#include "User.h"

#include <QObject>
#include <QString>

#include <optional>


namespace CampusCard {

/**
 * @class AuthManager
 * @brief 认证管理器，负责用户登录和会话管理
 */
class AuthManager : public QObject {
    Q_OBJECT

public:
    /**
     * @brief 登录结果枚举
     */
    enum class LoginResult {
        Success,             ///< 登录成功
        InvalidCredentials,  ///< 密码错误
        CardNotFound,        ///< 卡号不存在
        CardLost,            ///< 卡已挂失
        CardFrozen,          ///< 卡已冻结
        AlreadyLoggedIn      ///< 已经登录
    };

    /**
     * @brief 构造函数
     * @param cardManager 卡管理器引用
     * @param parent 父对象
     */
    explicit AuthManager(CardManager* cardManager, QObject* parent = nullptr);

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
     * @brief 修改管理员密码
     * @param oldPassword 旧密码
     * @param newPassword 新密码
     * @return 是否成功
     */
    bool changeAdminPassword(const QString& oldPassword, const QString& newPassword);

    /**
     * @brief 将登录结果转换为中文描述
     * @param result 登录结果
     * @return 描述字符串
     */
    static QString loginResultToString(LoginResult result);

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
     */
    void loginFailed(LoginResult result);

    /**
     * @brief 登出信号
     */
    void loggedOut();

private:
    CardManager* m_cardManager;                  ///< 卡管理器指针
    bool m_isLoggedIn = false;                   ///< 是否已登录
    UserRole m_currentRole = UserRole::Student;  ///< 当前角色
    QString m_currentCardId;                     ///< 当前登录的卡号
};

}  // namespace CampusCard

#endif  // AUTHMANAGER_H
