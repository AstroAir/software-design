/**
 * @file AuthControllerTest.cpp
 * @brief AuthController控制器层单元测试
 * @author CampusCardSystem
 * @date 2024
 */

#include "controller/AuthController.h"
#include "model/repositories/StorageManager.h"
#include "model/services/AuthService.h"
#include "model/services/CardService.h"

#include <QSignalSpy>
#include <QTemporaryDir>
#include <gtest/gtest.h>

using namespace CampusCard;

class AuthControllerTest : public ::testing::Test {
protected:
    QTemporaryDir tempDir;
    QString testDataPath;
    CardService* cardService;
    AuthService* authService;
    AuthController* authController;

    void SetUp() override {
        ASSERT_TRUE(tempDir.isValid());
        testDataPath = tempDir.path() + "/test_data";
        StorageManager::instance().setDataPath(testDataPath);
        StorageManager::instance().initializeDataDirectory();

        StorageManager::instance().saveAllCards(QList<Card>());
        StorageManager::instance().saveAdminPassword(DEFAULT_ADMIN_PASSWORD);

        cardService = new CardService();
        cardService->initialize();

        authService = new AuthService(cardService);
        authController = new AuthController(authService, cardService);
    }

    void TearDown() override {
        delete authController;
        delete authService;
        delete cardService;
    }
};

// ========== 学生登录测试 ==========

TEST_F(AuthControllerTest, HandleStudentLoginSuccess) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    QSignalSpy successSpy(authController, &AuthController::loginSuccess);

    authController->handleStudentLogin("C001", DEFAULT_STUDENT_PASSWORD);

    EXPECT_EQ(successSpy.count(), 1);
    EXPECT_EQ(successSpy.takeFirst().at(0).value<UserRole>(), UserRole::Student);
}

TEST_F(AuthControllerTest, HandleStudentLoginSuccessUserName) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    QSignalSpy successSpy(authController, &AuthController::loginSuccess);

    authController->handleStudentLogin("C001", DEFAULT_STUDENT_PASSWORD);

    EXPECT_EQ(successSpy.count(), 1);
    QList<QVariant> args = successSpy.takeFirst();
    EXPECT_EQ(args.at(2).toString(), "张三");
}

TEST_F(AuthControllerTest, HandleStudentLoginCardNotFound) {
    QSignalSpy failedSpy(authController, &AuthController::loginFailed);

    authController->handleStudentLogin("C999", "password");

    EXPECT_EQ(failedSpy.count(), 1);
    EXPECT_EQ(failedSpy.takeFirst().at(0).value<LoginResult>(), LoginResult::CardNotFound);
}

TEST_F(AuthControllerTest, HandleStudentLoginInvalidPassword) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    QSignalSpy failedSpy(authController, &AuthController::loginFailed);

    authController->handleStudentLogin("C001", "wrongpassword");

    EXPECT_EQ(failedSpy.count(), 1);
    EXPECT_EQ(failedSpy.takeFirst().at(0).value<LoginResult>(), LoginResult::InvalidCredentials);
}

TEST_F(AuthControllerTest, HandleStudentLoginPasswordErrorSignal) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    QSignalSpy errorSpy(authController, &AuthController::passwordError);

    authController->handleStudentLogin("C001", "wrongpassword");

    EXPECT_EQ(errorSpy.count(), 1);
}

TEST_F(AuthControllerTest, HandleStudentLoginCardFrozenSignal) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    QSignalSpy frozenSpy(authController, &AuthController::cardFrozen);

    for (int i = 0; i < MAX_LOGIN_ATTEMPTS; ++i) {
        authController->handleStudentLogin("C001", "wrongpassword");
    }

    EXPECT_EQ(frozenSpy.count(), 1);
}

// ========== 管理员登录测试 ==========

TEST_F(AuthControllerTest, HandleAdminLoginSuccess) {
    QSignalSpy successSpy(authController, &AuthController::loginSuccess);

    authController->handleAdminLogin(DEFAULT_ADMIN_PASSWORD);

    EXPECT_EQ(successSpy.count(), 1);
    QList<QVariant> args = successSpy.takeFirst();
    EXPECT_EQ(args.at(0).value<UserRole>(), UserRole::Admin);
    EXPECT_EQ(args.at(2).toString(), "管理员");
}

TEST_F(AuthControllerTest, HandleAdminLoginInvalidPassword) {
    QSignalSpy failedSpy(authController, &AuthController::loginFailed);

    authController->handleAdminLogin("wrongpassword");

    EXPECT_EQ(failedSpy.count(), 1);
    EXPECT_EQ(failedSpy.takeFirst().at(0).value<LoginResult>(), LoginResult::InvalidCredentials);
}

// ========== 登出测试 ==========

TEST_F(AuthControllerTest, HandleLogout) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    authController->handleStudentLogin("C001", DEFAULT_STUDENT_PASSWORD);

    QSignalSpy logoutSpy(authController, &AuthController::logoutSuccess);

    authController->handleLogout();

    EXPECT_EQ(logoutSpy.count(), 1);
    EXPECT_FALSE(authController->isLoggedIn());
}

// ========== 状态查询测试 ==========

TEST_F(AuthControllerTest, IsLoggedIn) {
    EXPECT_FALSE(authController->isLoggedIn());

    cardService->createCard("C001", "张三", "B17010101", 100.0);
    authController->handleStudentLogin("C001", DEFAULT_STUDENT_PASSWORD);

    EXPECT_TRUE(authController->isLoggedIn());
}

TEST_F(AuthControllerTest, CurrentRole) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    authController->handleStudentLogin("C001", DEFAULT_STUDENT_PASSWORD);
    EXPECT_EQ(authController->currentRole(), UserRole::Student);

    authController->handleLogout();
    authController->handleAdminLogin(DEFAULT_ADMIN_PASSWORD);
    EXPECT_EQ(authController->currentRole(), UserRole::Admin);
}

TEST_F(AuthControllerTest, CurrentCardId) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    authController->handleStudentLogin("C001", DEFAULT_STUDENT_PASSWORD);
    EXPECT_EQ(authController->currentCardId(), "C001");

    authController->handleLogout();
    authController->handleAdminLogin(DEFAULT_ADMIN_PASSWORD);
    EXPECT_TRUE(authController->currentCardId().isEmpty());
}

TEST_F(AuthControllerTest, CurrentUserName) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    authController->handleStudentLogin("C001", DEFAULT_STUDENT_PASSWORD);
    EXPECT_EQ(authController->currentUserName(), "张三");

    authController->handleLogout();
    authController->handleAdminLogin(DEFAULT_ADMIN_PASSWORD);
    EXPECT_EQ(authController->currentUserName(), "管理员");
}

TEST_F(AuthControllerTest, CurrentUserNameNotLoggedIn) {
    EXPECT_TRUE(authController->currentUserName().isEmpty());
}

// ========== 登录预检测试 ==========

TEST_F(AuthControllerTest, CheckCardStatus) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    EXPECT_EQ(authController->checkCardStatus("C001"), LoginResult::Success);
}

TEST_F(AuthControllerTest, CheckCardStatusNotFound) {
    EXPECT_EQ(authController->checkCardStatus("C999"), LoginResult::CardNotFound);
}

TEST_F(AuthControllerTest, CheckCardStatusLost) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    cardService->reportLost("C001");

    EXPECT_EQ(authController->checkCardStatus("C001"), LoginResult::CardLost);
}

TEST_F(AuthControllerTest, CheckCardStatusFrozen) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    cardService->freeze("C001");

    EXPECT_EQ(authController->checkCardStatus("C001"), LoginResult::CardFrozen);
}

TEST_F(AuthControllerTest, GetRemainingAttempts) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    EXPECT_EQ(authController->getRemainingAttempts("C001"), MAX_LOGIN_ATTEMPTS);

    authController->handleStudentLogin("C001", "wrong");
    EXPECT_EQ(authController->getRemainingAttempts("C001"), MAX_LOGIN_ATTEMPTS - 1);
}

// ========== 密码管理测试 ==========

TEST_F(AuthControllerTest, HandleChangeAdminPasswordSuccess) {
    QSignalSpy successSpy(authController, &AuthController::adminPasswordChanged);

    authController->handleChangeAdminPassword(DEFAULT_ADMIN_PASSWORD, "newpassword");

    EXPECT_EQ(successSpy.count(), 1);
}

TEST_F(AuthControllerTest, HandleChangeAdminPasswordWrongOld) {
    QSignalSpy failedSpy(authController, &AuthController::adminPasswordChangeFailed);

    authController->handleChangeAdminPassword("wrongold", "newpassword");

    EXPECT_EQ(failedSpy.count(), 1);
}

TEST_F(AuthControllerTest, HandleChangeAdminPasswordTooShort) {
    QSignalSpy failedSpy(authController, &AuthController::adminPasswordChangeFailed);

    authController->handleChangeAdminPassword(DEFAULT_ADMIN_PASSWORD, "abc");

    EXPECT_EQ(failedSpy.count(), 1);
}
