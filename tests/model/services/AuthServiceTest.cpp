/**
 * @file AuthServiceTest.cpp
 * @brief AuthService认证服务层单元测试
 * @author CampusCardSystem
 * @date 2024
 */

#include "model/repositories/StorageManager.h"
#include "model/services/AuthService.h"
#include "model/services/CardService.h"

#include <QSignalSpy>
#include <QTemporaryDir>
#include <gtest/gtest.h>

using namespace CampusCard;

class AuthServiceTest : public ::testing::Test {
protected:
    QTemporaryDir tempDir;
    QString testDataPath;
    CardService* cardService;
    AuthService* authService;

    void SetUp() override {
        ASSERT_TRUE(tempDir.isValid());
        testDataPath = tempDir.path() + "/test_data";
        StorageManager::instance().setDataPath(testDataPath);
        StorageManager::instance().initializeDataDirectory();

        // 清空卡数据
        StorageManager::instance().saveAllCards(QList<Card>());
        // 重置管理员密码
        StorageManager::instance().saveAdminPassword(DEFAULT_ADMIN_PASSWORD);

        cardService = new CardService();
        cardService->initialize();

        authService = new AuthService(cardService);
    }

    void TearDown() override {
        delete authService;
        delete cardService;
    }
};

// ========== 学生登录测试 ==========

TEST_F(AuthServiceTest, StudentLoginSuccess) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    LoginResult result = authService->studentLogin("C001", DEFAULT_STUDENT_PASSWORD);
    EXPECT_EQ(result, LoginResult::Success);
    EXPECT_TRUE(authService->isLoggedIn());
    EXPECT_TRUE(authService->isStudent());
    EXPECT_EQ(authService->currentCardId(), "C001");
}

TEST_F(AuthServiceTest, StudentLoginSuccessSignals) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    QSignalSpy successSpy(authService, &AuthService::loginSucceeded);

    authService->studentLogin("C001", DEFAULT_STUDENT_PASSWORD);

    EXPECT_EQ(successSpy.count(), 1);
    EXPECT_EQ(successSpy.takeFirst().at(0).value<UserRole>(), UserRole::Student);
}

TEST_F(AuthServiceTest, StudentLoginCardNotFound) {
    LoginResult result = authService->studentLogin("C999", "password");
    EXPECT_EQ(result, LoginResult::CardNotFound);
    EXPECT_FALSE(authService->isLoggedIn());
}

TEST_F(AuthServiceTest, StudentLoginInvalidPassword) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    LoginResult result = authService->studentLogin("C001", "wrongpassword");
    EXPECT_EQ(result, LoginResult::InvalidCredentials);
    EXPECT_FALSE(authService->isLoggedIn());
}

TEST_F(AuthServiceTest, StudentLoginPasswordErrorSignals) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    QSignalSpy errorSpy(authService, &AuthService::passwordError);

    authService->studentLogin("C001", "wrongpassword");

    EXPECT_EQ(errorSpy.count(), 1);
}

TEST_F(AuthServiceTest, StudentLoginCardLost) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    cardService->reportLost("C001");

    LoginResult result = authService->studentLogin("C001", DEFAULT_STUDENT_PASSWORD);
    EXPECT_EQ(result, LoginResult::CardLost);
    EXPECT_FALSE(authService->isLoggedIn());
}

TEST_F(AuthServiceTest, StudentLoginCardFrozen) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    cardService->freeze("C001");

    LoginResult result = authService->studentLogin("C001", DEFAULT_STUDENT_PASSWORD);
    EXPECT_EQ(result, LoginResult::CardFrozen);
    EXPECT_FALSE(authService->isLoggedIn());
}

TEST_F(AuthServiceTest, StudentLoginAlreadyLoggedIn) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    authService->studentLogin("C001", DEFAULT_STUDENT_PASSWORD);

    LoginResult result = authService->studentLogin("C001", DEFAULT_STUDENT_PASSWORD);
    EXPECT_EQ(result, LoginResult::AlreadyLoggedIn);
}

TEST_F(AuthServiceTest, StudentLoginMaxAttemptsFreeze) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    QSignalSpy frozenSpy(authService, &AuthService::cardFrozen);

    for (int i = 0; i < MAX_LOGIN_ATTEMPTS; ++i) {
        authService->studentLogin("C001", "wrongpassword");
    }

    EXPECT_EQ(frozenSpy.count(), 1);

    // 验证卡被冻结
    Card card = cardService->findCard("C001");
    EXPECT_EQ(card.state(), CardState::Frozen);
}

TEST_F(AuthServiceTest, StudentLoginResetsAttemptsOnSuccess) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    // 先失败两次
    authService->studentLogin("C001", "wrong1");
    authService->studentLogin("C001", "wrong2");

    EXPECT_EQ(cardService->getLoginAttempts("C001"), 2);

    // 成功登录
    authService->studentLogin("C001", DEFAULT_STUDENT_PASSWORD);

    // 验证计数被重置
    EXPECT_EQ(cardService->getLoginAttempts("C001"), 0);
}

// ========== 管理员登录测试 ==========

TEST_F(AuthServiceTest, AdminLoginSuccess) {
    LoginResult result = authService->adminLogin(DEFAULT_ADMIN_PASSWORD);
    EXPECT_EQ(result, LoginResult::Success);
    EXPECT_TRUE(authService->isLoggedIn());
    EXPECT_TRUE(authService->isAdmin());
    EXPECT_TRUE(authService->currentCardId().isEmpty());
}

TEST_F(AuthServiceTest, AdminLoginSuccessSignals) {
    QSignalSpy successSpy(authService, &AuthService::loginSucceeded);

    authService->adminLogin(DEFAULT_ADMIN_PASSWORD);

    EXPECT_EQ(successSpy.count(), 1);
    EXPECT_EQ(successSpy.takeFirst().at(0).value<UserRole>(), UserRole::Admin);
}

TEST_F(AuthServiceTest, AdminLoginInvalidPassword) {
    LoginResult result = authService->adminLogin("wrongpassword");
    EXPECT_EQ(result, LoginResult::InvalidCredentials);
    EXPECT_FALSE(authService->isLoggedIn());
}

TEST_F(AuthServiceTest, AdminLoginAlreadyLoggedIn) {
    authService->adminLogin(DEFAULT_ADMIN_PASSWORD);

    LoginResult result = authService->adminLogin(DEFAULT_ADMIN_PASSWORD);
    EXPECT_EQ(result, LoginResult::AlreadyLoggedIn);
}

// ========== 登出测试 ==========

TEST_F(AuthServiceTest, Logout) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    authService->studentLogin("C001", DEFAULT_STUDENT_PASSWORD);

    authService->logout();

    EXPECT_FALSE(authService->isLoggedIn());
    EXPECT_TRUE(authService->currentCardId().isEmpty());
}

TEST_F(AuthServiceTest, LogoutSignals) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    authService->studentLogin("C001", DEFAULT_STUDENT_PASSWORD);

    QSignalSpy logoutSpy(authService, &AuthService::loggedOut);

    authService->logout();

    EXPECT_EQ(logoutSpy.count(), 1);
}

TEST_F(AuthServiceTest, LogoutWhenNotLoggedIn) {
    QSignalSpy logoutSpy(authService, &AuthService::loggedOut);

    authService->logout();

    EXPECT_EQ(logoutSpy.count(), 0);
}

// ========== 会话状态查询测试 ==========

TEST_F(AuthServiceTest, IsLoggedIn) {
    EXPECT_FALSE(authService->isLoggedIn());

    cardService->createCard("C001", "张三", "B17010101", 100.0);
    authService->studentLogin("C001", DEFAULT_STUDENT_PASSWORD);

    EXPECT_TRUE(authService->isLoggedIn());
}

TEST_F(AuthServiceTest, CurrentUser) {
    EXPECT_FALSE(authService->currentUser().has_value());

    cardService->createCard("C001", "张三", "B17010101", 100.0);
    authService->studentLogin("C001", DEFAULT_STUDENT_PASSWORD);

    auto user = authService->currentUser();
    EXPECT_TRUE(user.has_value());
    EXPECT_EQ(user->role(), UserRole::Student);
    EXPECT_EQ(user->cardId(), "C001");
}

TEST_F(AuthServiceTest, CurrentRole) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    authService->studentLogin("C001", DEFAULT_STUDENT_PASSWORD);
    EXPECT_EQ(authService->currentRole(), UserRole::Student);

    authService->logout();
    authService->adminLogin(DEFAULT_ADMIN_PASSWORD);
    EXPECT_EQ(authService->currentRole(), UserRole::Admin);
}

TEST_F(AuthServiceTest, CurrentCardId) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    authService->studentLogin("C001", DEFAULT_STUDENT_PASSWORD);
    EXPECT_EQ(authService->currentCardId(), "C001");

    authService->logout();
    authService->adminLogin(DEFAULT_ADMIN_PASSWORD);
    EXPECT_TRUE(authService->currentCardId().isEmpty());
}

TEST_F(AuthServiceTest, IsAdmin) {
    EXPECT_FALSE(authService->isAdmin());

    authService->adminLogin(DEFAULT_ADMIN_PASSWORD);
    EXPECT_TRUE(authService->isAdmin());
}

TEST_F(AuthServiceTest, IsStudent) {
    EXPECT_FALSE(authService->isStudent());

    cardService->createCard("C001", "张三", "B17010101", 100.0);
    authService->studentLogin("C001", DEFAULT_STUDENT_PASSWORD);
    EXPECT_TRUE(authService->isStudent());
}

// ========== 管理员密码管理测试 ==========

TEST_F(AuthServiceTest, VerifyAdminPassword) {
    EXPECT_TRUE(authService->verifyAdminPassword(DEFAULT_ADMIN_PASSWORD));
    EXPECT_FALSE(authService->verifyAdminPassword("wrongpassword"));
}

TEST_F(AuthServiceTest, ChangeAdminPassword) {
    EXPECT_TRUE(authService->changeAdminPassword(DEFAULT_ADMIN_PASSWORD, "newpassword"));

    EXPECT_TRUE(authService->verifyAdminPassword("newpassword"));
    EXPECT_FALSE(authService->verifyAdminPassword(DEFAULT_ADMIN_PASSWORD));
}

TEST_F(AuthServiceTest, ChangeAdminPasswordWrongOld) {
    EXPECT_FALSE(authService->changeAdminPassword("wrongold", "newpassword"));

    // 密码应该没有改变
    EXPECT_TRUE(authService->verifyAdminPassword(DEFAULT_ADMIN_PASSWORD));
}

TEST_F(AuthServiceTest, GetAdminPassword) {
    QString password = authService->getAdminPassword();
    EXPECT_EQ(password, DEFAULT_ADMIN_PASSWORD);
}

// ========== 卡状态检查测试 ==========

TEST_F(AuthServiceTest, CheckCardLoginStatusSuccess) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    LoginResult result = authService->checkCardLoginStatus("C001");
    EXPECT_EQ(result, LoginResult::Success);
}

TEST_F(AuthServiceTest, CheckCardLoginStatusNotFound) {
    LoginResult result = authService->checkCardLoginStatus("C999");
    EXPECT_EQ(result, LoginResult::CardNotFound);
}

TEST_F(AuthServiceTest, CheckCardLoginStatusLost) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    cardService->reportLost("C001");

    LoginResult result = authService->checkCardLoginStatus("C001");
    EXPECT_EQ(result, LoginResult::CardLost);
}

TEST_F(AuthServiceTest, CheckCardLoginStatusFrozen) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    cardService->freeze("C001");

    LoginResult result = authService->checkCardLoginStatus("C001");
    EXPECT_EQ(result, LoginResult::CardFrozen);
}

TEST_F(AuthServiceTest, GetRemainingLoginAttempts) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    EXPECT_EQ(authService->getRemainingLoginAttempts("C001"), MAX_LOGIN_ATTEMPTS);

    authService->studentLogin("C001", "wrong1");
    EXPECT_EQ(authService->getRemainingLoginAttempts("C001"), MAX_LOGIN_ATTEMPTS - 1);
}

TEST_F(AuthServiceTest, GetRemainingLoginAttemptsCardNotFound) {
    EXPECT_EQ(authService->getRemainingLoginAttempts("C999"), -1);
}
