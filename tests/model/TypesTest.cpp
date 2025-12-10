/**
 * @file TypesTest.cpp
 * @brief Types.h类型定义和工具函数单元测试
 * @author CampusCardSystem
 * @date 2024
 */

#include "model/Types.h"

#include <gtest/gtest.h>

using namespace CampusCard;

class TypesTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

// ========== 枚举值测试 ==========

TEST_F(TypesTest, UserRoleEnumValues) {
    EXPECT_NE(static_cast<int>(UserRole::Student), static_cast<int>(UserRole::Admin));
}

TEST_F(TypesTest, CardStateEnumValues) {
    EXPECT_EQ(static_cast<int>(CardState::Normal), 0);
    EXPECT_EQ(static_cast<int>(CardState::Lost), 1);
    EXPECT_EQ(static_cast<int>(CardState::Frozen), 2);
}

TEST_F(TypesTest, SessionStateEnumValues) {
    EXPECT_EQ(static_cast<int>(SessionState::Offline), 0);
    EXPECT_EQ(static_cast<int>(SessionState::Online), 1);
}

TEST_F(TypesTest, LoginResultEnumValues) {
    // 确保所有LoginResult值都是不同的
    EXPECT_NE(static_cast<int>(LoginResult::Success), static_cast<int>(LoginResult::InvalidCredentials));
    EXPECT_NE(static_cast<int>(LoginResult::Success), static_cast<int>(LoginResult::CardNotFound));
    EXPECT_NE(static_cast<int>(LoginResult::Success), static_cast<int>(LoginResult::CardLost));
    EXPECT_NE(static_cast<int>(LoginResult::Success), static_cast<int>(LoginResult::CardFrozen));
    EXPECT_NE(static_cast<int>(LoginResult::Success), static_cast<int>(LoginResult::AlreadyLoggedIn));
}

// ========== cardStateToString测试 ==========

TEST_F(TypesTest, CardStateToStringNormal) {
    QString result = cardStateToString(CardState::Normal);
    EXPECT_EQ(result, QString::fromUtf8("正常"));
}

TEST_F(TypesTest, CardStateToStringLost) {
    QString result = cardStateToString(CardState::Lost);
    EXPECT_EQ(result, QString::fromUtf8("挂失"));
}

TEST_F(TypesTest, CardStateToStringFrozen) {
    QString result = cardStateToString(CardState::Frozen);
    EXPECT_EQ(result, QString::fromUtf8("冻结"));
}

TEST_F(TypesTest, CardStateToStringUnknown) {
    // 测试未知值的处理
    QString result = cardStateToString(static_cast<CardState>(99));
    EXPECT_EQ(result, QString::fromUtf8("未知"));
}

// ========== sessionStateToString测试 ==========

TEST_F(TypesTest, SessionStateToStringOffline) {
    QString result = sessionStateToString(SessionState::Offline);
    EXPECT_EQ(result, QString::fromUtf8("离线"));
}

TEST_F(TypesTest, SessionStateToStringOnline) {
    QString result = sessionStateToString(SessionState::Online);
    EXPECT_EQ(result, QString::fromUtf8("上机中"));
}

TEST_F(TypesTest, SessionStateToStringUnknown) {
    QString result = sessionStateToString(static_cast<SessionState>(99));
    EXPECT_EQ(result, QString::fromUtf8("未知"));
}

// ========== loginResultToString测试 ==========

TEST_F(TypesTest, LoginResultToStringSuccess) {
    QString result = loginResultToString(LoginResult::Success);
    EXPECT_EQ(result, QString::fromUtf8("登录成功"));
}

TEST_F(TypesTest, LoginResultToStringInvalidCredentials) {
    QString result = loginResultToString(LoginResult::InvalidCredentials);
    EXPECT_EQ(result, QString::fromUtf8("密码错误"));
}

TEST_F(TypesTest, LoginResultToStringCardNotFound) {
    QString result = loginResultToString(LoginResult::CardNotFound);
    EXPECT_EQ(result, QString::fromUtf8("卡号不存在"));
}

TEST_F(TypesTest, LoginResultToStringCardLost) {
    QString result = loginResultToString(LoginResult::CardLost);
    EXPECT_EQ(result, QString::fromUtf8("该卡已挂失，请联系管理员"));
}

TEST_F(TypesTest, LoginResultToStringCardFrozen) {
    QString result = loginResultToString(LoginResult::CardFrozen);
    EXPECT_EQ(result, QString::fromUtf8("该卡已冻结（密码错误次数过多），请联系管理员"));
}

TEST_F(TypesTest, LoginResultToStringAlreadyLoggedIn) {
    QString result = loginResultToString(LoginResult::AlreadyLoggedIn);
    EXPECT_EQ(result, QString::fromUtf8("已经登录，请先登出"));
}

TEST_F(TypesTest, LoginResultToStringUnknown) {
    QString result = loginResultToString(static_cast<LoginResult>(99));
    EXPECT_EQ(result, QString::fromUtf8("未知错误"));
}

// ========== 常量测试 ==========

TEST_F(TypesTest, CostPerHour) {
    EXPECT_DOUBLE_EQ(COST_PER_HOUR, 1.0);
}

TEST_F(TypesTest, DefaultAdminPassword) {
    EXPECT_EQ(DEFAULT_ADMIN_PASSWORD, QString::fromUtf8("admin123"));
}

TEST_F(TypesTest, MaxLoginAttempts) {
    EXPECT_EQ(MAX_LOGIN_ATTEMPTS, 3);
}

TEST_F(TypesTest, DefaultStudentPassword) {
    EXPECT_EQ(DEFAULT_STUDENT_PASSWORD, QString::fromUtf8("123456"));
}

// ========== 常量合理性测试 ==========

TEST_F(TypesTest, CostPerHourIsPositive) {
    EXPECT_GT(COST_PER_HOUR, 0.0);
}

TEST_F(TypesTest, MaxLoginAttemptsIsPositive) {
    EXPECT_GT(MAX_LOGIN_ATTEMPTS, 0);
}

TEST_F(TypesTest, DefaultPasswordsNotEmpty) {
    EXPECT_FALSE(DEFAULT_ADMIN_PASSWORD.isEmpty());
    EXPECT_FALSE(DEFAULT_STUDENT_PASSWORD.isEmpty());
}

// ========== 枚举完整性测试 ==========

TEST_F(TypesTest, AllCardStatesHaveStrings) {
    // 确保所有已知的CardState都有对应的字符串
    EXPECT_FALSE(cardStateToString(CardState::Normal).isEmpty());
    EXPECT_FALSE(cardStateToString(CardState::Lost).isEmpty());
    EXPECT_FALSE(cardStateToString(CardState::Frozen).isEmpty());
}

TEST_F(TypesTest, AllSessionStatesHaveStrings) {
    EXPECT_FALSE(sessionStateToString(SessionState::Offline).isEmpty());
    EXPECT_FALSE(sessionStateToString(SessionState::Online).isEmpty());
}

TEST_F(TypesTest, AllLoginResultsHaveStrings) {
    EXPECT_FALSE(loginResultToString(LoginResult::Success).isEmpty());
    EXPECT_FALSE(loginResultToString(LoginResult::InvalidCredentials).isEmpty());
    EXPECT_FALSE(loginResultToString(LoginResult::CardNotFound).isEmpty());
    EXPECT_FALSE(loginResultToString(LoginResult::CardLost).isEmpty());
    EXPECT_FALSE(loginResultToString(LoginResult::CardFrozen).isEmpty());
    EXPECT_FALSE(loginResultToString(LoginResult::AlreadyLoggedIn).isEmpty());
}
