/**
 * @file CardTest.cpp
 * @brief Card实体类单元测试
 * @author CampusCardSystem
 * @date 2024
 */

#include "model/entities/Card.h"

#include <QJsonObject>
#include <gtest/gtest.h>

using namespace CampusCard;

class CardTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

// ========== 构造函数测试 ==========

TEST_F(CardTest, DefaultConstructor) {
    Card card;
    EXPECT_TRUE(card.cardId().isEmpty());
    EXPECT_TRUE(card.name().isEmpty());
    EXPECT_TRUE(card.studentId().isEmpty());
    EXPECT_DOUBLE_EQ(card.balance(), 0.0);
    EXPECT_DOUBLE_EQ(card.totalRecharge(), 0.0);
    EXPECT_EQ(card.state(), CardState::Normal);
    EXPECT_EQ(card.loginAttempts(), 0);
    EXPECT_EQ(card.password(), DEFAULT_STUDENT_PASSWORD);
}

TEST_F(CardTest, ParameterizedConstructor) {
    Card card("C001", "张三", "B17010101", 100.0);
    EXPECT_EQ(card.cardId(), "C001");
    EXPECT_EQ(card.name(), "张三");
    EXPECT_EQ(card.studentId(), "B17010101");
    EXPECT_DOUBLE_EQ(card.balance(), 100.0);
    EXPECT_DOUBLE_EQ(card.totalRecharge(), 100.0);
    EXPECT_EQ(card.state(), CardState::Normal);
    EXPECT_EQ(card.loginAttempts(), 0);
    EXPECT_EQ(card.password(), DEFAULT_STUDENT_PASSWORD);
}

TEST_F(CardTest, ParameterizedConstructorDefaultBalance) {
    Card card("C002", "李四", "B17010102");
    EXPECT_EQ(card.cardId(), "C002");
    EXPECT_EQ(card.name(), "李四");
    EXPECT_EQ(card.studentId(), "B17010102");
    EXPECT_DOUBLE_EQ(card.balance(), 0.0);
    EXPECT_DOUBLE_EQ(card.totalRecharge(), 0.0);
}

TEST_F(CardTest, ParameterizedConstructorZeroBalance) {
    Card card("C003", "王五", "B17010103", 0.0);
    EXPECT_DOUBLE_EQ(card.balance(), 0.0);
    EXPECT_DOUBLE_EQ(card.totalRecharge(), 0.0);
}

// ========== 拷贝构造和赋值测试 ==========

TEST_F(CardTest, CopyConstructor) {
    Card original("C001", "张三", "B17010101", 100.0);
    original.setState(CardState::Lost);
    original.setLoginAttempts(2);
    
    Card copy(original);
    EXPECT_EQ(copy.cardId(), original.cardId());
    EXPECT_EQ(copy.name(), original.name());
    EXPECT_EQ(copy.studentId(), original.studentId());
    EXPECT_DOUBLE_EQ(copy.balance(), original.balance());
    EXPECT_EQ(copy.state(), original.state());
    EXPECT_EQ(copy.loginAttempts(), original.loginAttempts());
}

TEST_F(CardTest, MoveConstructor) {
    Card original("C001", "张三", "B17010101", 100.0);
    Card moved(std::move(original));
    EXPECT_EQ(moved.cardId(), "C001");
    EXPECT_EQ(moved.name(), "张三");
    EXPECT_DOUBLE_EQ(moved.balance(), 100.0);
}

TEST_F(CardTest, CopyAssignment) {
    Card original("C001", "张三", "B17010101", 100.0);
    Card copy;
    copy = original;
    EXPECT_EQ(copy.cardId(), original.cardId());
    EXPECT_EQ(copy.name(), original.name());
    EXPECT_DOUBLE_EQ(copy.balance(), original.balance());
}

TEST_F(CardTest, MoveAssignment) {
    Card original("C001", "张三", "B17010101", 100.0);
    Card moved;
    moved = std::move(original);
    EXPECT_EQ(moved.cardId(), "C001");
    EXPECT_EQ(moved.name(), "张三");
}

// ========== Getter测试 ==========

TEST_F(CardTest, GetCardId) {
    Card card("C001", "张三", "B17010101", 100.0);
    EXPECT_EQ(card.cardId(), "C001");
}

TEST_F(CardTest, GetName) {
    Card card("C001", "张三", "B17010101", 100.0);
    EXPECT_EQ(card.name(), "张三");
}

TEST_F(CardTest, GetStudentId) {
    Card card("C001", "张三", "B17010101", 100.0);
    EXPECT_EQ(card.studentId(), "B17010101");
}

TEST_F(CardTest, GetTotalRecharge) {
    Card card("C001", "张三", "B17010101", 150.0);
    EXPECT_DOUBLE_EQ(card.totalRecharge(), 150.0);
}

TEST_F(CardTest, GetBalance) {
    Card card("C001", "张三", "B17010101", 200.0);
    EXPECT_DOUBLE_EQ(card.balance(), 200.0);
}

TEST_F(CardTest, GetState) {
    Card card("C001", "张三", "B17010101", 100.0);
    EXPECT_EQ(card.state(), CardState::Normal);
}

TEST_F(CardTest, GetLoginAttempts) {
    Card card("C001", "张三", "B17010101", 100.0);
    EXPECT_EQ(card.loginAttempts(), 0);
}

TEST_F(CardTest, GetPassword) {
    Card card("C001", "张三", "B17010101", 100.0);
    EXPECT_EQ(card.password(), DEFAULT_STUDENT_PASSWORD);
}

// ========== Setter测试 ==========

TEST_F(CardTest, SetCardId) {
    Card card;
    card.setCardId("C999");
    EXPECT_EQ(card.cardId(), "C999");
}

TEST_F(CardTest, SetName) {
    Card card;
    card.setName("新用户");
    EXPECT_EQ(card.name(), "新用户");
}

TEST_F(CardTest, SetStudentId) {
    Card card;
    card.setStudentId("B20010101");
    EXPECT_EQ(card.studentId(), "B20010101");
}

TEST_F(CardTest, SetTotalRecharge) {
    Card card;
    card.setTotalRecharge(500.0);
    EXPECT_DOUBLE_EQ(card.totalRecharge(), 500.0);
}

TEST_F(CardTest, SetBalance) {
    Card card;
    card.setBalance(300.0);
    EXPECT_DOUBLE_EQ(card.balance(), 300.0);
}

TEST_F(CardTest, SetPassword) {
    Card card;
    card.setPassword("newpassword");
    EXPECT_EQ(card.password(), "newpassword");
}

TEST_F(CardTest, SetState) {
    Card card;
    card.setState(CardState::Lost);
    EXPECT_EQ(card.state(), CardState::Lost);
    card.setState(CardState::Frozen);
    EXPECT_EQ(card.state(), CardState::Frozen);
    card.setState(CardState::Normal);
    EXPECT_EQ(card.state(), CardState::Normal);
}

TEST_F(CardTest, SetLoginAttempts) {
    Card card;
    card.setLoginAttempts(2);
    EXPECT_EQ(card.loginAttempts(), 2);
}

// ========== 状态检查方法测试 ==========

TEST_F(CardTest, IsUsableNormal) {
    Card card("C001", "张三", "B17010101", 100.0);
    EXPECT_TRUE(card.isUsable());
}

TEST_F(CardTest, IsUsableLost) {
    Card card("C001", "张三", "B17010101", 100.0);
    card.setState(CardState::Lost);
    EXPECT_FALSE(card.isUsable());
}

TEST_F(CardTest, IsUsableFrozen) {
    Card card("C001", "张三", "B17010101", 100.0);
    card.setState(CardState::Frozen);
    EXPECT_FALSE(card.isUsable());
}

TEST_F(CardTest, IsNormal) {
    Card card("C001", "张三", "B17010101", 100.0);
    EXPECT_TRUE(card.isNormal());
    card.setState(CardState::Lost);
    EXPECT_FALSE(card.isNormal());
}

TEST_F(CardTest, IsLost) {
    Card card("C001", "张三", "B17010101", 100.0);
    EXPECT_FALSE(card.isLost());
    card.setState(CardState::Lost);
    EXPECT_TRUE(card.isLost());
}

TEST_F(CardTest, IsFrozen) {
    Card card("C001", "张三", "B17010101", 100.0);
    EXPECT_FALSE(card.isFrozen());
    card.setState(CardState::Frozen);
    EXPECT_TRUE(card.isFrozen());
}

TEST_F(CardTest, HasReachedMaxLoginAttempts) {
    Card card("C001", "张三", "B17010101", 100.0);
    EXPECT_FALSE(card.hasReachedMaxLoginAttempts());
    
    card.setLoginAttempts(1);
    EXPECT_FALSE(card.hasReachedMaxLoginAttempts());
    
    card.setLoginAttempts(2);
    EXPECT_FALSE(card.hasReachedMaxLoginAttempts());
    
    card.setLoginAttempts(MAX_LOGIN_ATTEMPTS);
    EXPECT_TRUE(card.hasReachedMaxLoginAttempts());
    
    card.setLoginAttempts(MAX_LOGIN_ATTEMPTS + 1);
    EXPECT_TRUE(card.hasReachedMaxLoginAttempts());
}

// ========== JSON序列化测试 ==========

TEST_F(CardTest, ToJson) {
    Card card("C001", "张三", "B17010101", 100.0);
    card.setTotalRecharge(150.0);
    card.setState(CardState::Normal);
    card.setLoginAttempts(1);
    card.setPassword("testpass");
    
    QJsonObject json = card.toJson();
    
    EXPECT_EQ(json["cardId"].toString(), "C001");
    EXPECT_EQ(json["name"].toString(), "张三");
    EXPECT_EQ(json["studentId"].toString(), "B17010101");
    EXPECT_DOUBLE_EQ(json["balance"].toDouble(), 100.0);
    EXPECT_DOUBLE_EQ(json["totalRecharge"].toDouble(), 150.0);
    EXPECT_EQ(json["state"].toInt(), static_cast<int>(CardState::Normal));
    EXPECT_EQ(json["loginAttempts"].toInt(), 1);
    EXPECT_EQ(json["password"].toString(), "testpass");
}

TEST_F(CardTest, FromJson) {
    QJsonObject json;
    json["cardId"] = "C002";
    json["name"] = "李四";
    json["studentId"] = "B17010102";
    json["balance"] = 200.0;
    json["totalRecharge"] = 250.0;
    json["state"] = static_cast<int>(CardState::Lost);
    json["loginAttempts"] = 2;
    json["password"] = "pass123";
    
    Card card = Card::fromJson(json);
    
    EXPECT_EQ(card.cardId(), "C002");
    EXPECT_EQ(card.name(), "李四");
    EXPECT_EQ(card.studentId(), "B17010102");
    EXPECT_DOUBLE_EQ(card.balance(), 200.0);
    EXPECT_DOUBLE_EQ(card.totalRecharge(), 250.0);
    EXPECT_EQ(card.state(), CardState::Lost);
    EXPECT_EQ(card.loginAttempts(), 2);
    EXPECT_EQ(card.password(), "pass123");
}

TEST_F(CardTest, FromJsonDefaultPassword) {
    QJsonObject json;
    json["cardId"] = "C003";
    json["name"] = "王五";
    json["studentId"] = "B17010103";
    json["balance"] = 50.0;
    json["totalRecharge"] = 50.0;
    json["state"] = static_cast<int>(CardState::Normal);
    json["loginAttempts"] = 0;
    // 不设置password
    
    Card card = Card::fromJson(json);
    EXPECT_EQ(card.password(), DEFAULT_STUDENT_PASSWORD);
}

TEST_F(CardTest, JsonRoundTrip) {
    Card original("C001", "张三", "B17010101", 100.0);
    original.setState(CardState::Frozen);
    original.setLoginAttempts(2);
    original.setPassword("mypassword");
    original.setTotalRecharge(200.0);
    
    QJsonObject json = original.toJson();
    Card restored = Card::fromJson(json);
    
    EXPECT_EQ(restored.cardId(), original.cardId());
    EXPECT_EQ(restored.name(), original.name());
    EXPECT_EQ(restored.studentId(), original.studentId());
    EXPECT_DOUBLE_EQ(restored.balance(), original.balance());
    EXPECT_DOUBLE_EQ(restored.totalRecharge(), original.totalRecharge());
    EXPECT_EQ(restored.state(), original.state());
    EXPECT_EQ(restored.loginAttempts(), original.loginAttempts());
    EXPECT_EQ(restored.password(), original.password());
}

TEST_F(CardTest, FromJsonEmptyObject) {
    QJsonObject json;
    Card card = Card::fromJson(json);
    
    EXPECT_TRUE(card.cardId().isEmpty());
    EXPECT_TRUE(card.name().isEmpty());
    EXPECT_TRUE(card.studentId().isEmpty());
    EXPECT_DOUBLE_EQ(card.balance(), 0.0);
    EXPECT_DOUBLE_EQ(card.totalRecharge(), 0.0);
    EXPECT_EQ(card.state(), CardState::Normal);
    EXPECT_EQ(card.loginAttempts(), 0);
}

// ========== 边界条件测试 ==========

TEST_F(CardTest, NegativeBalance) {
    Card card;
    card.setBalance(-100.0);
    EXPECT_DOUBLE_EQ(card.balance(), -100.0);
}

TEST_F(CardTest, VeryLargeBalance) {
    Card card;
    card.setBalance(1e12);
    EXPECT_DOUBLE_EQ(card.balance(), 1e12);
}

TEST_F(CardTest, SpecialCharactersInName) {
    Card card("C001", "张@三#$", "B17010101", 100.0);
    EXPECT_EQ(card.name(), "张@三#$");
}

TEST_F(CardTest, UnicodeInName) {
    Card card("C001", "日本語テスト", "B17010101", 100.0);
    EXPECT_EQ(card.name(), "日本語テスト");
}

TEST_F(CardTest, EmptyCardId) {
    Card card("", "张三", "B17010101", 100.0);
    EXPECT_TRUE(card.cardId().isEmpty());
}

TEST_F(CardTest, ZeroLoginAttempts) {
    Card card;
    card.setLoginAttempts(0);
    EXPECT_EQ(card.loginAttempts(), 0);
    EXPECT_FALSE(card.hasReachedMaxLoginAttempts());
}

TEST_F(CardTest, NegativeLoginAttempts) {
    Card card;
    card.setLoginAttempts(-1);
    EXPECT_EQ(card.loginAttempts(), -1);
}
