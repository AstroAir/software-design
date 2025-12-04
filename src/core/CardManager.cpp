/**
 * @file CardManager.cpp
 * @brief 校园卡管理器实现
 * @author CampusCardSystem
 * @date 2024
 */

#include "CardManager.h"

namespace CampusCard {

CardManager::CardManager(QObject* parent)
    : QObject(parent)
{
}

void CardManager::initialize() {
    // 从存储加载所有卡数据
    QList<Card> cards = StorageManager::instance().loadAllCards();
    m_cards.clear();
    for (const auto& card : cards) {
        m_cards[card.cardId()] = card;
    }
}

bool CardManager::saveAll() {
    QList<Card> cards = m_cards.values();
    return StorageManager::instance().saveAllCards(cards);
}

QList<Card> CardManager::getAllCards() const {
    return m_cards.values();
}

Card* CardManager::findCard(const QString& cardId) {
    auto it = m_cards.find(cardId);
    if (it != m_cards.end()) {
        return &it.value();
    }
    return nullptr;
}

Card* CardManager::findCardByStudentId(const QString& studentId) {
    for (auto& card : m_cards) {
        if (card.studentId() == studentId) {
            return &card;
        }
    }
    return nullptr;
}

bool CardManager::cardExists(const QString& cardId) const {
    return m_cards.contains(cardId);
}

bool CardManager::createCard(const QString& cardId, const QString& name,
                             const QString& studentId, double initialBalance) {
    // 检查卡号是否已存在
    if (cardExists(cardId)) {
        return false;
    }
    
    // 创建新卡并添加到映射
    Card newCard(cardId, name, studentId, initialBalance);
    m_cards[cardId] = newCard;
    
    // 保存并发出信号
    saveAll();
    emit cardsChanged();
    return true;
}

bool CardManager::rechargeCard(const QString& cardId, double amount) {
    Card* card = findCard(cardId);
    if (!card) {
        return false;
    }
    
    // 执行充值
    if (card->recharge(amount)) {
        saveAll();
        emit cardUpdated(cardId);
        return true;
    }
    return false;
}

bool CardManager::deductFromCard(const QString& cardId, double amount) {
    Card* card = findCard(cardId);
    if (!card) {
        return false;
    }
    
    // 检查卡是否可用
    if (!card->isUsable()) {
        return false;
    }
    
    // 执行扣款
    if (card->deduct(amount)) {
        saveAll();
        emit cardUpdated(cardId);
        return true;
    }
    return false;
}

bool CardManager::reportCardLost(const QString& cardId) {
    Card* card = findCard(cardId);
    if (!card) {
        return false;
    }
    
    card->reportLost();
    saveAll();
    emit cardUpdated(cardId);
    return true;
}

bool CardManager::cancelCardLost(const QString& cardId) {
    Card* card = findCard(cardId);
    if (!card) {
        return false;
    }
    
    card->cancelLost();
    saveAll();
    emit cardUpdated(cardId);
    return true;
}

bool CardManager::freezeCard(const QString& cardId) {
    Card* card = findCard(cardId);
    if (!card) {
        return false;
    }
    
    card->freeze();
    saveAll();
    emit cardUpdated(cardId);
    return true;
}

bool CardManager::unfreezeCard(const QString& cardId) {
    Card* card = findCard(cardId);
    if (!card) {
        return false;
    }
    
    card->unfreeze();
    saveAll();
    emit cardUpdated(cardId);
    return true;
}

bool CardManager::resetPassword(const QString& cardId, const QString& newPassword) {
    Card* card = findCard(cardId);
    if (!card) {
        return false;
    }
    
    // 设置新密码并重置登录失败计数
    card->setPassword(newPassword);
    card->resetLoginAttempts();
    
    // 如果卡被冻结，自动解冻
    if (card->state() == CardState::Frozen) {
        card->unfreeze();
    }
    
    saveAll();
    emit cardUpdated(cardId);
    return true;
}

bool CardManager::updateCard(const Card& card) {
    if (!cardExists(card.cardId())) {
        return false;
    }
    
    m_cards[card.cardId()] = card;
    saveAll();
    emit cardUpdated(card.cardId());
    return true;
}

} // namespace CampusCard
