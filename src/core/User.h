/**
 * @file User.h
 * @brief 用户实体类定义
 * @author CampusCardSystem
 * @date 2024
 */

#ifndef USER_H
#define USER_H

#include "Types.h"
#include <QString>
#include <QJsonObject>

namespace CampusCard {

/**
 * @class User
 * @brief 用户实体类，表示系统中的用户（学生或管理员）
 */
class User {
public:
    /**
     * @brief 默认构造函数
     */
    User() = default;

    /**
     * @brief 构造函数
     * @param userId 用户ID
     * @param role 用户角色
     * @param cardId 关联的卡号（学生专用，管理员为空）
     */
    User(const QString& userId, UserRole role, const QString& cardId = QString());

    /**
     * @brief 从JSON反序列化
     * @param json JSON对象
     * @return User对象
     */
    static User fromJson(const QJsonObject& json);

    /**
     * @brief 序列化为JSON
     * @return JSON对象
     */
    [[nodiscard]] QJsonObject toJson() const;

    // ========== Getters ==========

    /**
     * @brief 获取用户ID
     * @return 用户ID
     */
    [[nodiscard]] QString userId() const { return m_userId; }

    /**
     * @brief 获取用户角色
     * @return 角色枚举
     */
    [[nodiscard]] UserRole role() const { return m_role; }

    /**
     * @brief 获取关联卡号
     * @return 卡号（管理员返回空）
     */
    [[nodiscard]] QString cardId() const { return m_cardId; }

    /**
     * @brief 判断是否为管理员
     * @return 是否管理员
     */
    [[nodiscard]] bool isAdmin() const { return m_role == UserRole::Admin; }

    /**
     * @brief 判断是否为学生
     * @return 是否学生
     */
    [[nodiscard]] bool isStudent() const { return m_role == UserRole::Student; }

private:
    QString m_userId;           ///< 用户ID
    UserRole m_role = UserRole::Student;  ///< 用户角色
    QString m_cardId;           ///< 关联的校园卡号（学生专用）
};

} // namespace CampusCard

#endif // USER_H
