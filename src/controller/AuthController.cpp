/**
 * @file AuthController.cpp
 * @brief 认证控制器实现
 * @author CampusCardSystem
 * @date 2024
 *
 * MVC架构 - Controller层实现
 */

#include "AuthController.h"

namespace CampusCard {

AuthController::AuthController(AuthService* authService, CardService* cardService,
                               QObject* parent)
    : QObject(parent), m_authService(authService), m_cardService(cardService) {

    // 连接AuthService的信号
    connect(m_authService, &AuthService::passwordError, this,
            [this](const QString& /*cardId*/, int remainingAttempts) {
                emit passwordError(remainingAttempts);
            });

    connect(m_authService, &AuthService::cardFrozen, this,
            [this](const QString& cardId) {
                emit cardFrozen(cardId);
            });
}

// ========== 登录操作 ==========

void AuthController::handleStudentLogin(const QString& cardId, const QString& password) {
    LoginResult result = m_authService->studentLogin(cardId, password);

    if (result == LoginResult::Success) {
        QString userName = currentUserName();
        emit loginSuccess(UserRole::Student, cardId, userName);
    } else {
        QString message = loginResultToString(result);
        emit loginFailed(result, message);
    }
}

void AuthController::handleAdminLogin(const QString& password) {
    LoginResult result = m_authService->adminLogin(password);

    if (result == LoginResult::Success) {
        emit loginSuccess(UserRole::Admin, QString(), QStringLiteral("管理员"));
    } else {
        QString message = loginResultToString(result);
        emit loginFailed(result, message);
    }
}

void AuthController::handleLogout() {
    m_authService->logout();
    emit logoutSuccess();
}

// ========== 状态查询 ==========

bool AuthController::isLoggedIn() const {
    return m_authService->isLoggedIn();
}

UserRole AuthController::currentRole() const {
    return m_authService->currentRole();
}

QString AuthController::currentCardId() const {
    return m_authService->currentCardId();
}

QString AuthController::currentUserName() const {
    if (!m_authService->isLoggedIn()) {
        return QString();
    }

    if (m_authService->isAdmin()) {
        return QStringLiteral("管理员");
    }

    // 获取学生姓名
    QString cardId = m_authService->currentCardId();
    Card card = m_cardService->findCard(cardId);
    if (!card.cardId().isEmpty()) {
        return card.name();
    }
    return cardId;
}

// ========== 登录预检 ==========

LoginResult AuthController::checkCardStatus(const QString& cardId) const {
    return m_authService->checkCardLoginStatus(cardId);
}

int AuthController::getRemainingAttempts(const QString& cardId) const {
    return m_authService->getRemainingLoginAttempts(cardId);
}

// ========== 密码管理 ==========

void AuthController::handleChangeAdminPassword(const QString& oldPassword,
                                                const QString& newPassword) {
    if (newPassword.length() < 4) {
        emit adminPasswordChangeFailed(QStringLiteral("密码长度至少4位"));
        return;
    }

    if (m_authService->changeAdminPassword(oldPassword, newPassword)) {
        emit adminPasswordChanged();
    } else {
        emit adminPasswordChangeFailed(QStringLiteral("当前密码错误"));
    }
}

}  // namespace CampusCard
