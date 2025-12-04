/**
 * @file User.cpp
 * @brief 用户实体类实现
 * @author CampusCardSystem
 * @date 2024
 */

#include "User.h"

namespace CampusCard {

User::User(const QString& userId, UserRole role, const QString& cardId)
    : m_userId(userId)
    , m_role(role)
    , m_cardId(cardId)
{
}

User User::fromJson(const QJsonObject& json) {
    User user;
    user.m_userId = json[QStringLiteral("userId")].toString();
    user.m_role = static_cast<UserRole>(json[QStringLiteral("role")].toInt());
    user.m_cardId = json[QStringLiteral("cardId")].toString();
    return user;
}

QJsonObject User::toJson() const {
    QJsonObject json;
    json[QStringLiteral("userId")] = m_userId;
    json[QStringLiteral("role")] = static_cast<int>(m_role);
    json[QStringLiteral("cardId")] = m_cardId;
    return json;
}

} // namespace CampusCard
