/**
 * @file AuthService.cpp
 * @brief 认证业务服务类实现
 * @author CampusCardSystem
 * @date 2024
 *
 * MVC架构 - Model层业务服务实现
 */

#include "AuthService.h"

namespace CampusCard {

AuthService::AuthService(CardService* cardService, QObject* parent)
    : QObject(parent), m_cardService(cardService), m_isLoggedIn(false),
      m_currentRole(UserRole::Student) {}

// ========== 登录操作 ==========

LoginResult AuthService::studentLogin(const QString& cardId, const QString& password) {
    // 检查是否已登录
    if (m_isLoggedIn) {
        return LoginResult::AlreadyLoggedIn;
    }

    // 查找卡
    Card card = m_cardService->findCard(cardId);
    if (card.cardId().isEmpty()) {
        emit loginFailed(LoginResult::CardNotFound, cardId);
        return LoginResult::CardNotFound;
    }

    // 检查卡状态
    if (card.isLost()) {
        emit loginFailed(LoginResult::CardLost, cardId);
        return LoginResult::CardLost;
    }
    if (card.isFrozen()) {
        emit loginFailed(LoginResult::CardFrozen, cardId);
        return LoginResult::CardFrozen;
    }

    // 验证密码
    if (!m_cardService->verifyPassword(cardId, password)) {
        // 增加错误计数
        int attempts = m_cardService->incrementLoginAttempts(cardId);
        int remaining = MAX_LOGIN_ATTEMPTS - attempts;

        // 检查是否达到最大次数
        if (remaining <= 0) {
            emit cardFrozen(cardId);
            emit loginFailed(LoginResult::CardFrozen, cardId);
            return LoginResult::CardFrozen;
        }

        emit passwordError(cardId, remaining);
        emit loginFailed(LoginResult::InvalidCredentials, cardId);
        return LoginResult::InvalidCredentials;
    }

    // 登录成功，重置错误计数
    m_cardService->resetLoginAttempts(cardId);

    m_isLoggedIn = true;
    m_currentRole = UserRole::Student;
    m_currentCardId = cardId;

    emit loginSucceeded(UserRole::Student, cardId);
    return LoginResult::Success;
}

LoginResult AuthService::adminLogin(const QString& password) {
    // 检查是否已登录
    if (m_isLoggedIn) {
        return LoginResult::AlreadyLoggedIn;
    }

    // 获取管理员密码并验证
    QString adminPassword = StorageManager::instance().loadAdminPassword();
    if (password != adminPassword) {
        emit loginFailed(LoginResult::InvalidCredentials, QString());
        return LoginResult::InvalidCredentials;
    }

    m_isLoggedIn = true;
    m_currentRole = UserRole::Admin;
    m_currentCardId.clear();

    emit loginSucceeded(UserRole::Admin, QString());
    return LoginResult::Success;
}

void AuthService::logout() {
    if (m_isLoggedIn) {
        m_isLoggedIn = false;
        m_currentCardId.clear();
        emit loggedOut();
    }
}

// ========== 会话状态查询 ==========

std::optional<User> AuthService::currentUser() const {
    if (!m_isLoggedIn) {
        return std::nullopt;
    }

    return User(m_currentRole == UserRole::Admin ? QStringLiteral("admin") : m_currentCardId,
                m_currentRole, m_currentCardId);
}

// ========== 管理员密码管理 ==========

bool AuthService::verifyAdminPassword(const QString& password) const {
    QString adminPassword = StorageManager::instance().loadAdminPassword();
    return password == adminPassword;
}

bool AuthService::changeAdminPassword(const QString& oldPassword, const QString& newPassword) {
    // 验证旧密码
    if (!verifyAdminPassword(oldPassword)) {
        return false;
    }

    // 保存新密码
    return StorageManager::instance().saveAdminPassword(newPassword);
}

QString AuthService::getAdminPassword() const {
    return StorageManager::instance().loadAdminPassword();
}

// ========== 卡状态检查 ==========

LoginResult AuthService::checkCardLoginStatus(const QString& cardId) const {
    Card card = m_cardService->findCard(cardId);
    if (card.cardId().isEmpty()) {
        return LoginResult::CardNotFound;
    }

    if (card.isLost()) {
        return LoginResult::CardLost;
    }

    if (card.isFrozen()) {
        return LoginResult::CardFrozen;
    }

    return LoginResult::Success;
}

int AuthService::getRemainingLoginAttempts(const QString& cardId) const {
    int attempts = m_cardService->getLoginAttempts(cardId);
    if (attempts < 0) {
        return -1;  // 卡不存在
    }
    return MAX_LOGIN_ATTEMPTS - attempts;
}

}  // namespace CampusCard
