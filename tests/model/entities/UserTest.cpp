/**
 * @file UserTest.cpp
 * @brief User实体类单元测试
 * @author CampusCardSystem
 * @date 2024
 */

#include "model/entities/User.h"

#include <QJsonObject>
#include <gtest/gtest.h>

using namespace CampusCard;

class UserTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

// ========== 构造函数测试 ==========

TEST_F(UserTest, DefaultConstructor) {
    User user;
    EXPECT_TRUE(user.userId().isEmpty());
    EXPECT_EQ(user.role(), UserRole::Student);
    EXPECT_TRUE(user.cardId().isEmpty());
}

TEST_F(UserTest, ParameterizedConstructorStudent) {
    User user("U001", UserRole::Student, "C001");
    EXPECT_EQ(user.userId(), "U001");
    EXPECT_EQ(user.role(), UserRole::Student);
    EXPECT_EQ(user.cardId(), "C001");
}

TEST_F(UserTest, ParameterizedConstructorAdmin) {
    User user("admin", UserRole::Admin);
    EXPECT_EQ(user.userId(), "admin");
    EXPECT_EQ(user.role(), UserRole::Admin);
    EXPECT_TRUE(user.cardId().isEmpty());
}

TEST_F(UserTest, ParameterizedConstructorDefaultCardId) {
    User user("U002", UserRole::Student);
    EXPECT_EQ(user.userId(), "U002");
    EXPECT_EQ(user.role(), UserRole::Student);
    EXPECT_TRUE(user.cardId().isEmpty());
}

// ========== 拷贝构造和赋值测试 ==========

TEST_F(UserTest, CopyConstructor) {
    User original("U001", UserRole::Student, "C001");
    User copy(original);
    EXPECT_EQ(copy.userId(), original.userId());
    EXPECT_EQ(copy.role(), original.role());
    EXPECT_EQ(copy.cardId(), original.cardId());
}

TEST_F(UserTest, MoveConstructor) {
    User original("U001", UserRole::Student, "C001");
    User moved(std::move(original));
    EXPECT_EQ(moved.userId(), "U001");
    EXPECT_EQ(moved.role(), UserRole::Student);
    EXPECT_EQ(moved.cardId(), "C001");
}

TEST_F(UserTest, CopyAssignment) {
    User original("U001", UserRole::Student, "C001");
    User copy;
    copy = original;
    EXPECT_EQ(copy.userId(), original.userId());
    EXPECT_EQ(copy.role(), original.role());
    EXPECT_EQ(copy.cardId(), original.cardId());
}

TEST_F(UserTest, MoveAssignment) {
    User original("U001", UserRole::Student, "C001");
    User moved;
    moved = std::move(original);
    EXPECT_EQ(moved.userId(), "U001");
    EXPECT_EQ(moved.role(), UserRole::Student);
    EXPECT_EQ(moved.cardId(), "C001");
}

// ========== Getter测试 ==========

TEST_F(UserTest, GetUserId) {
    User user("TestUser", UserRole::Student, "C001");
    EXPECT_EQ(user.userId(), "TestUser");
}

TEST_F(UserTest, GetRole) {
    User studentUser("S001", UserRole::Student, "C001");
    User adminUser("A001", UserRole::Admin);
    EXPECT_EQ(studentUser.role(), UserRole::Student);
    EXPECT_EQ(adminUser.role(), UserRole::Admin);
}

TEST_F(UserTest, GetCardId) {
    User user("U001", UserRole::Student, "C123");
    EXPECT_EQ(user.cardId(), "C123");
}

// ========== Setter测试 ==========

TEST_F(UserTest, SetUserId) {
    User user;
    user.setUserId("NewUserId");
    EXPECT_EQ(user.userId(), "NewUserId");
}

TEST_F(UserTest, SetRole) {
    User user;
    user.setRole(UserRole::Admin);
    EXPECT_EQ(user.role(), UserRole::Admin);
    user.setRole(UserRole::Student);
    EXPECT_EQ(user.role(), UserRole::Student);
}

TEST_F(UserTest, SetCardId) {
    User user;
    user.setCardId("C999");
    EXPECT_EQ(user.cardId(), "C999");
}

// ========== 角色判断方法测试 ==========

TEST_F(UserTest, IsAdmin) {
    User adminUser("admin", UserRole::Admin);
    User studentUser("student", UserRole::Student, "C001");
    EXPECT_TRUE(adminUser.isAdmin());
    EXPECT_FALSE(studentUser.isAdmin());
}

TEST_F(UserTest, IsStudent) {
    User adminUser("admin", UserRole::Admin);
    User studentUser("student", UserRole::Student, "C001");
    EXPECT_FALSE(adminUser.isStudent());
    EXPECT_TRUE(studentUser.isStudent());
}

// ========== JSON序列化测试 ==========

TEST_F(UserTest, ToJsonStudent) {
    User user("U001", UserRole::Student, "C001");
    QJsonObject json = user.toJson();
    
    EXPECT_EQ(json["userId"].toString(), "U001");
    EXPECT_EQ(json["role"].toInt(), static_cast<int>(UserRole::Student));
    EXPECT_EQ(json["cardId"].toString(), "C001");
}

TEST_F(UserTest, ToJsonAdmin) {
    User user("admin", UserRole::Admin, "");
    QJsonObject json = user.toJson();
    
    EXPECT_EQ(json["userId"].toString(), "admin");
    EXPECT_EQ(json["role"].toInt(), static_cast<int>(UserRole::Admin));
    EXPECT_TRUE(json["cardId"].toString().isEmpty());
}

TEST_F(UserTest, FromJsonStudent) {
    QJsonObject json;
    json["userId"] = "U002";
    json["role"] = static_cast<int>(UserRole::Student);
    json["cardId"] = "C002";
    
    User user = User::fromJson(json);
    EXPECT_EQ(user.userId(), "U002");
    EXPECT_EQ(user.role(), UserRole::Student);
    EXPECT_EQ(user.cardId(), "C002");
}

TEST_F(UserTest, FromJsonAdmin) {
    QJsonObject json;
    json["userId"] = "admin";
    json["role"] = static_cast<int>(UserRole::Admin);
    json["cardId"] = "";
    
    User user = User::fromJson(json);
    EXPECT_EQ(user.userId(), "admin");
    EXPECT_EQ(user.role(), UserRole::Admin);
    EXPECT_TRUE(user.cardId().isEmpty());
}

TEST_F(UserTest, JsonRoundTrip) {
    User original("U003", UserRole::Student, "C003");
    QJsonObject json = original.toJson();
    User restored = User::fromJson(json);
    
    EXPECT_EQ(restored.userId(), original.userId());
    EXPECT_EQ(restored.role(), original.role());
    EXPECT_EQ(restored.cardId(), original.cardId());
}

TEST_F(UserTest, FromJsonEmptyObject) {
    QJsonObject json;
    User user = User::fromJson(json);
    
    EXPECT_TRUE(user.userId().isEmpty());
    EXPECT_EQ(user.role(), UserRole::Student);  // 默认值
    EXPECT_TRUE(user.cardId().isEmpty());
}

// ========== 边界条件测试 ==========

TEST_F(UserTest, EmptyUserId) {
    User user("", UserRole::Student, "C001");
    EXPECT_TRUE(user.userId().isEmpty());
}

TEST_F(UserTest, SpecialCharactersInUserId) {
    User user("用户@#$%", UserRole::Student, "C001");
    EXPECT_EQ(user.userId(), "用户@#$%");
}

TEST_F(UserTest, LongUserId) {
    QString longId(1000, 'A');
    User user(longId, UserRole::Student, "C001");
    EXPECT_EQ(user.userId(), longId);
}
