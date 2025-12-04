/**
 * @file Card.cpp
 * @brief 校园卡实体类实现
 * @author CampusCardSystem
 * @date 2024
 *
 * MVC架构 - Model层实体类实现
 */

#include "Card.h"

#include <QJsonObject>


namespace CampusCard {

Card::Card(const QString& cardId, const QString& name, const QString& studentId, double balance)
    : m_cardId(cardId), m_name(name), m_studentId(studentId), m_balance(balance),
      m_totalRecharge(balance),  // 初始余额视为首次充值
      m_state(CardState::Normal), m_loginAttempts(0),
      m_password(DEFAULT_STUDENT_PASSWORD)  // 默认密码
{}

Card Card::fromJson(const QJsonObject& json) {
    Card card;
    // 从JSON读取各字段
    card.m_cardId = json[QStringLiteral("cardId")].toString();
    card.m_name = json[QStringLiteral("name")].toString();
    card.m_studentId = json[QStringLiteral("studentId")].toString();
    card.m_totalRecharge = json[QStringLiteral("totalRecharge")].toDouble();
    card.m_balance = json[QStringLiteral("balance")].toDouble();
    card.m_state = static_cast<CardState>(json[QStringLiteral("state")].toInt());
    card.m_loginAttempts = json[QStringLiteral("loginAttempts")].toInt();
    card.m_password = json[QStringLiteral("password")].toString(DEFAULT_STUDENT_PASSWORD);
    return card;
}

QJsonObject Card::toJson() const {
    QJsonObject json;
    // 序列化各字段到JSON
    json[QStringLiteral("cardId")] = m_cardId;
    json[QStringLiteral("name")] = m_name;
    json[QStringLiteral("studentId")] = m_studentId;
    json[QStringLiteral("totalRecharge")] = m_totalRecharge;
    json[QStringLiteral("balance")] = m_balance;
    json[QStringLiteral("state")] = static_cast<int>(m_state);
    json[QStringLiteral("loginAttempts")] = m_loginAttempts;
    json[QStringLiteral("password")] = m_password;
    return json;
}

}  // namespace CampusCard
