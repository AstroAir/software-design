/**
 * @file Card.cpp
 * @brief 校园卡实体类实现
 * @author CampusCardSystem
 * @date 2024
 */

#include "Card.h"
#include <QJsonObject>

namespace CampusCard {

Card::Card(const QString& cardId, const QString& name, 
           const QString& studentId, double balance)
    : m_cardId(cardId)
    , m_name(name)
    , m_studentId(studentId)
    , m_balance(balance)
    , m_totalRecharge(balance)  // 初始余额视为首次充值
    , m_state(CardState::Normal)
    , m_loginAttempts(0)
    , m_password(QStringLiteral("123456"))  // 默认密码
{
}

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
    card.m_password = json[QStringLiteral("password")].toString(QStringLiteral("123456"));
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

bool Card::recharge(double amount) {
    // 充值金额必须为正数
    if (amount <= 0) {
        return false;
    }
    m_balance += amount;
    m_totalRecharge += amount;
    return true;
}

bool Card::deduct(double amount) {
    // 检查余额是否充足
    if (amount <= 0 || m_balance < amount) {
        return false;
    }
    m_balance -= amount;
    return true;
}

void Card::reportLost() {
    m_state = CardState::Lost;
}

void Card::cancelLost() {
    // 只有挂失状态才能解除
    if (m_state == CardState::Lost) {
        m_state = CardState::Normal;
    }
}

void Card::freeze() {
    m_state = CardState::Frozen;
}

void Card::unfreeze() {
    m_state = CardState::Normal;
    m_loginAttempts = 0;  // 同时重置错误计数
}

int Card::incrementLoginAttempts() {
    ++m_loginAttempts;
    // 达到最大次数自动冻结
    if (m_loginAttempts >= MAX_LOGIN_ATTEMPTS) {
        freeze();
    }
    return m_loginAttempts;
}

void Card::resetLoginAttempts() {
    m_loginAttempts = 0;
}

bool Card::isUsable() const {
    return m_state == CardState::Normal;
}

} // namespace CampusCard
