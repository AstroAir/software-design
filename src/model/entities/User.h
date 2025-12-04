/**
 * @file User.h
 * @brief 用户实体类定义
 * @author CampusCardSystem
 * @date 2024
 *
 * MVC架构 - Model层实体类
 * 用户实体是纯数据对象，只包含数据和序列化方法
 */

#ifndef MODEL_ENTITIES_USER_H
#define MODEL_ENTITIES_USER_H

#include "model/Types.h"

#include <QJsonObject>
#include <QString>


namespace CampusCard {

/**
 * @class User
 * @brief 用户实体类，表示系统中的用户（学生或管理员）
 *
 * 这是一个纯数据对象(Entity)，只包含：
 * - 数据成员
 * - Getter/Setter方法
 * - 序列化/反序列化方法
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
     * @brief 拷贝构造函数
     */
    User(const User& other) = default;

    /**
     * @brief 移动构造函数
     */
    User(User&& other) noexcept = default;

    /**
     * @brief 拷贝赋值运算符
     */
    User& operator=(const User& other) = default;

    /**
     * @brief 移动赋值运算符
     */
    User& operator=(User&& other) noexcept = default;

    /**
     * @brief 析构函数
     */
    ~User() = default;

    // ========== 序列化方法 ==========

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

    // ========== Setters ==========

    /**
     * @brief 设置用户ID
     * @param userId 用户ID
     */
    void setUserId(const QString& userId) { m_userId = userId; }

    /**
     * @brief 设置用户角色
     * @param role 角色
     */
    void setRole(UserRole role) { m_role = role; }

    /**
     * @brief 设置关联卡号
     * @param cardId 卡号
     */
    void setCardId(const QString& cardId) { m_cardId = cardId; }

private:
    QString m_userId;                     ///< 用户ID
    UserRole m_role = UserRole::Student;  ///< 用户角色
    QString m_cardId;                     ///< 关联的校园卡号（学生专用）
};

}  // namespace CampusCard

#endif  // MODEL_ENTITIES_USER_H
