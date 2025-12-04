/**
 * @file CardService.cpp
 * @brief 校园卡业务服务类实现
 * @author CampusCardSystem
 * @date 2024
 *
 * MVC架构 - Model层业务服务实现
 */

#include "CardService.h"

namespace CampusCard {

CardService::CardService(QObject* parent) : QObject(parent) {}

void CardService::initialize() {
    // 从存储加载所有卡数据
    QList<Card> cards = StorageManager::instance().loadAllCards();
    m_cards.clear();
    for (const auto& card : cards) {
        m_cards[card.cardId()] = card;
    }
}

bool CardService::saveAll() {
    QList<Card> cards = m_cards.values();
    return StorageManager::instance().saveAllCards(cards);
}

// ========== 查询操作 ==========

QList<Card> CardService::getAllCards() const {
    return m_cards.values();
}

Card CardService::findCard(const QString& cardId) const {
    auto it = m_cards.find(cardId);
    if (it != m_cards.end()) {
        return it.value();
    }
    return Card();
}

Card* CardService::getCardPtr(const QString& cardId) {
    auto it = m_cards.find(cardId);
    if (it != m_cards.end()) {
        return &it.value();
    }
    return nullptr;
}

Card CardService::findCardByStudentId(const QString& studentId) const {
    for (const auto& card : m_cards) {
        if (card.studentId() == studentId) {
            return card;
        }
    }
    return Card();
}

bool CardService::cardExists(const QString& cardId) const {
    return m_cards.contains(cardId);
}

int CardService::cardCount() const {
    return m_cards.size();
}

// ========== 创建操作 ==========

bool CardService::createCard(const QString& cardId, const QString& name, const QString& studentId,
                             double initialBalance) {
    // 检查卡号是否已存在
    if (cardExists(cardId)) {
        return false;
    }

    // 创建新卡并添加到映射
    Card newCard(cardId, name, studentId, initialBalance);
    m_cards[cardId] = newCard;

    // 保存并发出信号
    saveAll();
    emit cardCreated(cardId);
    emit cardsChanged();
    return true;
}

bool CardService::createCard(const Card& card) {
    // 检查卡号是否已存在
    if (cardExists(card.cardId())) {
        return false;
    }

    m_cards[card.cardId()] = card;

    // 保存并发出信号
    saveAll();
    emit cardCreated(card.cardId());
    emit cardsChanged();
    return true;
}

// ========== 充值扣款操作 ==========

bool CardService::recharge(const QString& cardId, double amount) {
    Card* card = getCardPtr(cardId);
    if (!card) {
        return false;
    }

    // 充值金额必须为正数
    if (amount <= 0) {
        return false;
    }

    // 执行充值
    double newBalance = card->balance() + amount;
    double newTotalRecharge = card->totalRecharge() + amount;
    card->setBalance(newBalance);
    card->setTotalRecharge(newTotalRecharge);

    // 保存并发出信号
    saveAll();
    emit cardUpdated(cardId);
    emit balanceChanged(cardId, newBalance);
    return true;
}

bool CardService::deduct(const QString& cardId, double amount) {
    Card* card = getCardPtr(cardId);
    if (!card) {
        return false;
    }

    // 检查卡是否可用
    if (!card->isUsable()) {
        return false;
    }

    // 检查金额有效性和余额充足性
    if (amount <= 0 || card->balance() < amount) {
        return false;
    }

    // 执行扣款
    double newBalance = card->balance() - amount;
    card->setBalance(newBalance);

    // 保存并发出信号
    saveAll();
    emit cardUpdated(cardId);
    emit balanceChanged(cardId, newBalance);
    return true;
}

double CardService::getBalance(const QString& cardId) const {
    auto it = m_cards.find(cardId);
    if (it != m_cards.end()) {
        return it.value().balance();
    }
    return -1.0;
}

// ========== 状态管理操作 ==========

bool CardService::reportLost(const QString& cardId) {
    Card* card = getCardPtr(cardId);
    if (!card) {
        return false;
    }

    card->setState(CardState::Lost);

    // 保存并发出信号
    saveAll();
    emit cardUpdated(cardId);
    emit cardStateChanged(cardId, CardState::Lost);
    return true;
}

bool CardService::cancelLost(const QString& cardId) {
    Card* card = getCardPtr(cardId);
    if (!card) {
        return false;
    }

    // 只有挂失状态才能解除
    if (card->state() != CardState::Lost) {
        return false;
    }

    card->setState(CardState::Normal);

    // 保存并发出信号
    saveAll();
    emit cardUpdated(cardId);
    emit cardStateChanged(cardId, CardState::Normal);
    return true;
}

bool CardService::freeze(const QString& cardId) {
    Card* card = getCardPtr(cardId);
    if (!card) {
        return false;
    }

    card->setState(CardState::Frozen);

    // 保存并发出信号
    saveAll();
    emit cardUpdated(cardId);
    emit cardStateChanged(cardId, CardState::Frozen);
    return true;
}

bool CardService::unfreeze(const QString& cardId) {
    Card* card = getCardPtr(cardId);
    if (!card) {
        return false;
    }

    card->setState(CardState::Normal);
    card->setLoginAttempts(0);  // 同时重置错误计数

    // 保存并发出信号
    saveAll();
    emit cardUpdated(cardId);
    emit cardStateChanged(cardId, CardState::Normal);
    return true;
}

// ========== 密码管理 ==========

bool CardService::verifyPassword(const QString& cardId, const QString& password) const {
    auto it = m_cards.find(cardId);
    if (it != m_cards.end()) {
        return it.value().password() == password;
    }
    return false;
}

bool CardService::changePassword(const QString& cardId, const QString& oldPassword,
                                  const QString& newPassword) {
    Card* card = getCardPtr(cardId);
    if (!card) {
        return false;
    }

    // 验证旧密码
    if (card->password() != oldPassword) {
        return false;
    }

    // 设置新密码
    card->setPassword(newPassword);

    // 保存
    saveAll();
    emit cardUpdated(cardId);
    return true;
}

bool CardService::resetPassword(const QString& cardId, const QString& newPassword) {
    Card* card = getCardPtr(cardId);
    if (!card) {
        return false;
    }

    // 设置新密码并重置登录失败计数
    card->setPassword(newPassword);
    card->setLoginAttempts(0);

    // 如果卡被冻结，自动解冻
    if (card->state() == CardState::Frozen) {
        card->setState(CardState::Normal);
        emit cardStateChanged(cardId, CardState::Normal);
    }

    // 保存
    saveAll();
    emit cardUpdated(cardId);
    return true;
}

// ========== 登录尝试管理 ==========

int CardService::incrementLoginAttempts(const QString& cardId) {
    Card* card = getCardPtr(cardId);
    if (!card) {
        return -1;
    }

    int attempts = card->loginAttempts() + 1;
    card->setLoginAttempts(attempts);

    // 达到最大次数自动冻结
    if (attempts >= MAX_LOGIN_ATTEMPTS) {
        card->setState(CardState::Frozen);
        emit cardStateChanged(cardId, CardState::Frozen);
    }

    saveAll();
    emit cardUpdated(cardId);
    return attempts;
}

bool CardService::resetLoginAttempts(const QString& cardId) {
    Card* card = getCardPtr(cardId);
    if (!card) {
        return false;
    }

    card->setLoginAttempts(0);
    saveAll();
    return true;
}

int CardService::getLoginAttempts(const QString& cardId) const {
    auto it = m_cards.find(cardId);
    if (it != m_cards.end()) {
        return it.value().loginAttempts();
    }
    return -1;
}

// ========== 卡信息更新 ==========

bool CardService::updateCard(const Card& card) {
    if (!cardExists(card.cardId())) {
        return false;
    }

    m_cards[card.cardId()] = card;
    saveAll();
    emit cardUpdated(card.cardId());
    return true;
}

}  // namespace CampusCard
