/**
 * @file CardController.cpp
 * @brief 校园卡控制器实现
 * @author CampusCardSystem
 * @date 2024
 *
 * MVC架构 - Controller层实现
 */

#include "CardController.h"

namespace CampusCard {

CardController::CardController(CardService* cardService, QObject* parent)
    : QObject(parent), m_cardService(cardService) {

    // 连接CardService的信号，转发给View
    connect(m_cardService, &CardService::cardsChanged, this, &CardController::cardsUpdated);
    connect(m_cardService, &CardService::cardUpdated, this, &CardController::cardUpdated);
}

// ========== 查询操作 ==========

QList<Card> CardController::getAllCards() const {
    return m_cardService->getAllCards();
}

Card CardController::getCard(const QString& cardId) const {
    return m_cardService->findCard(cardId);
}

bool CardController::cardExists(const QString& cardId) const {
    return m_cardService->cardExists(cardId);
}

int CardController::getCardCount() const {
    return m_cardService->cardCount();
}

QList<Card> CardController::searchCards(const QString& keyword) const {
    QList<Card> result;
    if (keyword.isEmpty()) {
        return m_cardService->getAllCards();
    }

    QString lowerKeyword = keyword.toLower();
    for (const auto& card : m_cardService->getAllCards()) {
        if (card.cardId().toLower().contains(lowerKeyword) ||
            card.name().toLower().contains(lowerKeyword) ||
            card.studentId().toLower().contains(lowerKeyword)) {
            result.append(card);
        }
    }
    return result;
}

// ========== 创建操作 ==========

void CardController::handleCreateCard(const QString& cardId, const QString& name,
                                        const QString& studentId, double initialBalance) {
    // 验证输入
    if (cardId.isEmpty()) {
        emit cardCreateFailed(QStringLiteral("卡号不能为空"));
        return;
    }
    if (name.isEmpty()) {
        emit cardCreateFailed(QStringLiteral("姓名不能为空"));
        return;
    }
    if (studentId.isEmpty()) {
        emit cardCreateFailed(QStringLiteral("学号不能为空"));
        return;
    }

    // 检查卡号是否已存在
    if (m_cardService->cardExists(cardId)) {
        emit cardCreateFailed(QStringLiteral("该卡号已存在"));
        return;
    }

    // 创建卡
    if (m_cardService->createCard(cardId, name, studentId, initialBalance)) {
        emit cardCreated(cardId);
    } else {
        emit cardCreateFailed(QStringLiteral("创建失败"));
    }
}

void CardController::handleRegisterCard(const QString& cardId, const QString& name,
                                          const QString& studentId, const QString& password) {
    // 验证输入
    if (cardId.isEmpty()) {
        emit cardCreateFailed(QStringLiteral("卡号不能为空"));
        return;
    }
    if (name.isEmpty()) {
        emit cardCreateFailed(QStringLiteral("姓名不能为空"));
        return;
    }
    if (studentId.isEmpty()) {
        emit cardCreateFailed(QStringLiteral("学号不能为空"));
        return;
    }
    if (password.length() < 4) {
        emit cardCreateFailed(QStringLiteral("密码长度至少4位"));
        return;
    }

    // 检查卡号是否已存在
    if (m_cardService->cardExists(cardId)) {
        emit cardCreateFailed(QStringLiteral("该卡号已存在"));
        return;
    }

    // 创建卡
    Card newCard(cardId, name, studentId, 0.0);
    newCard.setPassword(password);

    if (m_cardService->createCard(newCard)) {
        emit cardCreated(cardId);
    } else {
        emit cardCreateFailed(QStringLiteral("注册失败"));
    }
}

// ========== 充值扣款操作 ==========

void CardController::handleRecharge(const QString& cardId, double amount) {
    if (amount <= 0) {
        emit rechargeFailed(QStringLiteral("充值金额必须大于0"));
        return;
    }

    if (m_cardService->recharge(cardId, amount)) {
        double newBalance = m_cardService->getBalance(cardId);
        emit rechargeSuccess(cardId, newBalance);
    } else {
        emit rechargeFailed(QStringLiteral("充值失败"));
    }
}

void CardController::handleDeduct(const QString& cardId, double amount) {
    if (amount <= 0) {
        emit deductFailed(QStringLiteral("扣款金额必须大于0"));
        return;
    }

    double currentBalance = m_cardService->getBalance(cardId);
    if (currentBalance < amount) {
        emit deductFailed(QStringLiteral("余额不足"));
        return;
    }

    if (m_cardService->deduct(cardId, amount)) {
        double newBalance = m_cardService->getBalance(cardId);
        emit deductSuccess(cardId, newBalance);
    } else {
        emit deductFailed(QStringLiteral("扣款失败"));
    }
}

double CardController::getBalance(const QString& cardId) const {
    return m_cardService->getBalance(cardId);
}

// ========== 状态管理操作 ==========

void CardController::handleReportLost(const QString& cardId) {
    if (m_cardService->reportLost(cardId)) {
        emit reportLostSuccess(cardId);
    } else {
        emit operationFailed(QStringLiteral("挂失失败"));
    }
}

void CardController::handleCancelLost(const QString& cardId) {
    if (m_cardService->cancelLost(cardId)) {
        emit cancelLostSuccess(cardId);
    } else {
        emit operationFailed(QStringLiteral("解挂失败"));
    }
}

void CardController::handleFreeze(const QString& cardId) {
    if (m_cardService->freeze(cardId)) {
        emit freezeSuccess(cardId);
    } else {
        emit operationFailed(QStringLiteral("冻结失败"));
    }
}

void CardController::handleUnfreeze(const QString& cardId) {
    if (m_cardService->unfreeze(cardId)) {
        emit unfreezeSuccess(cardId);
    } else {
        emit operationFailed(QStringLiteral("解冻失败"));
    }
}

// ========== 密码管理 ==========

void CardController::handleChangePassword(const QString& cardId, const QString& oldPassword,
                                           const QString& newPassword) {
    if (newPassword.length() < 4) {
        emit passwordChangeFailed(QStringLiteral("密码长度至少4位"));
        return;
    }

    if (m_cardService->changePassword(cardId, oldPassword, newPassword)) {
        emit passwordChanged(cardId);
    } else {
        emit passwordChangeFailed(QStringLiteral("当前密码错误"));
    }
}

void CardController::handleResetPassword(const QString& cardId, const QString& newPassword) {
    if (newPassword.isEmpty()) {
        emit passwordChangeFailed(QStringLiteral("密码不能为空"));
        return;
    }

    if (m_cardService->resetPassword(cardId, newPassword)) {
        emit passwordReset(cardId);
    } else {
        emit passwordChangeFailed(QStringLiteral("重置密码失败"));
    }
}

}  // namespace CampusCard
