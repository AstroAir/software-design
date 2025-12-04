/**
 * @file AuthManager.cpp
 * @brief 认证管理器实现
 * @author CampusCardSystem
 * @date 2024
 */

#include "AuthManager.h"

namespace CampusCard {

AuthManager::AuthManager(CardManager* cardManager, QObject* parent)
    : QObject(parent)
    , m_cardManager(cardManager)
    , m_isLoggedIn(false)
    , m_currentRole(UserRole::Student)
{
}

AuthManager::LoginResult AuthManager::studentLogin(const QString& cardId, const QString& password) {
    // 检查是否已登录
    if (m_isLoggedIn) {
        return LoginResult::AlreadyLoggedIn;
    }
    
    // 查找卡
    Card* card = m_cardManager->findCard(cardId);
    if (!card) {
        return LoginResult::CardNotFound;
    }
    
    // 检查卡状态
    if (card->state() == CardState::Lost) {
        return LoginResult::CardLost;
    }
    if (card->state() == CardState::Frozen) {
        return LoginResult::CardFrozen;
    }
    
    // 验证密码
    if (card->password() != password) {
        // 增加错误计数
        int attempts = card->incrementLoginAttempts();
        
        // 检查是否达到最大次数，如果达到则冻结卡
        if (attempts >= MAX_LOGIN_ATTEMPTS) {
            card->freeze();
            m_cardManager->saveAll();
            return LoginResult::CardFrozen;
        }
        
        m_cardManager->saveAll();
        return LoginResult::InvalidCredentials;
    }
    
    // 登录成功，重置错误计数
    card->resetLoginAttempts();
    m_cardManager->saveAll();
    
    m_isLoggedIn = true;
    m_currentRole = UserRole::Student;
    m_currentCardId = cardId;
    
    emit loginSucceeded(UserRole::Student, cardId);
    return LoginResult::Success;
}

AuthManager::LoginResult AuthManager::adminLogin(const QString& password) {
    // 检查是否已登录
    if (m_isLoggedIn) {
        return LoginResult::AlreadyLoggedIn;
    }
    
    // 获取管理员密码
    QString adminPassword = StorageManager::instance().loadAdminPassword();
    
    // 验证密码
    if (password != adminPassword) {
        return LoginResult::InvalidCredentials;
    }
    
    m_isLoggedIn = true;
    m_currentRole = UserRole::Admin;
    m_currentCardId.clear();
    
    emit loginSucceeded(UserRole::Admin, QString());
    return LoginResult::Success;
}

void AuthManager::logout() {
    if (m_isLoggedIn) {
        m_isLoggedIn = false;
        m_currentCardId.clear();
        emit loggedOut();
    }
}

std::optional<User> AuthManager::currentUser() const {
    if (!m_isLoggedIn) {
        return std::nullopt;
    }
    
    return User(
        m_currentRole == UserRole::Admin ? QStringLiteral("admin") : m_currentCardId,
        m_currentRole,
        m_currentCardId
    );
}

bool AuthManager::changeAdminPassword(const QString& oldPassword, const QString& newPassword) {
    QString currentPassword = StorageManager::instance().loadAdminPassword();
    
    // 验证旧密码
    if (oldPassword != currentPassword) {
        return false;
    }
    
    // 保存新密码
    return StorageManager::instance().saveAdminPassword(newPassword);
}

QString AuthManager::loginResultToString(LoginResult result) {
    switch (result) {
        case LoginResult::Success:
            return QStringLiteral("登录成功");
        case LoginResult::InvalidCredentials:
            return QStringLiteral("密码错误");
        case LoginResult::CardNotFound:
            return QStringLiteral("卡号不存在");
        case LoginResult::CardLost:
            return QStringLiteral("该卡已挂失，请联系管理员");
        case LoginResult::CardFrozen:
            return QStringLiteral("该卡已冻结（密码错误次数过多），请联系管理员");
        case LoginResult::AlreadyLoggedIn:
            return QStringLiteral("已经登录，请先登出");
        default:
            return QStringLiteral("未知错误");
    }
}

} // namespace CampusCard
