/**
 * @file RecordTest.cpp
 * @brief Record实体类单元测试
 * @author CampusCardSystem
 * @date 2024
 */

#include "model/entities/Record.h"

#include <QDateTime>
#include <QJsonObject>
#include <gtest/gtest.h>

using namespace CampusCard;

class RecordTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

// ========== 构造函数测试 ==========

TEST_F(RecordTest, DefaultConstructor) {
    Record record;
    EXPECT_TRUE(record.recordId().isEmpty());
    EXPECT_TRUE(record.cardId().isEmpty());
    EXPECT_TRUE(record.date().isEmpty());
    EXPECT_FALSE(record.startTime().isValid());
    EXPECT_FALSE(record.endTime().isValid());
    EXPECT_EQ(record.durationMinutes(), 0);
    EXPECT_DOUBLE_EQ(record.cost(), 0.0);
    EXPECT_EQ(record.state(), SessionState::Offline);
    EXPECT_TRUE(record.location().isEmpty());
}

// ========== 拷贝构造和赋值测试 ==========

TEST_F(RecordTest, CopyConstructor) {
    Record original;
    original.setRecordId("R001");
    original.setCardId("C001");
    original.setStartTime(QDateTime::currentDateTime());
    original.setState(SessionState::Online);
    
    Record copy(original);
    EXPECT_EQ(copy.recordId(), original.recordId());
    EXPECT_EQ(copy.cardId(), original.cardId());
    EXPECT_EQ(copy.state(), original.state());
}

TEST_F(RecordTest, MoveConstructor) {
    Record original;
    original.setRecordId("R001");
    original.setCardId("C001");
    
    Record moved(std::move(original));
    EXPECT_EQ(moved.recordId(), "R001");
    EXPECT_EQ(moved.cardId(), "C001");
}

TEST_F(RecordTest, CopyAssignment) {
    Record original;
    original.setRecordId("R001");
    original.setCardId("C001");
    
    Record copy;
    copy = original;
    EXPECT_EQ(copy.recordId(), original.recordId());
    EXPECT_EQ(copy.cardId(), original.cardId());
}

TEST_F(RecordTest, MoveAssignment) {
    Record original;
    original.setRecordId("R001");
    original.setCardId("C001");
    
    Record moved;
    moved = std::move(original);
    EXPECT_EQ(moved.recordId(), "R001");
    EXPECT_EQ(moved.cardId(), "C001");
}

// ========== Getter测试 ==========

TEST_F(RecordTest, GetRecordId) {
    Record record;
    record.setRecordId("R12345");
    EXPECT_EQ(record.recordId(), "R12345");
}

TEST_F(RecordTest, GetCardId) {
    Record record;
    record.setCardId("C001");
    EXPECT_EQ(record.cardId(), "C001");
}

TEST_F(RecordTest, GetDate) {
    Record record;
    record.setDate("2024-01-15");
    EXPECT_EQ(record.date(), "2024-01-15");
}

TEST_F(RecordTest, GetStartTime) {
    Record record;
    QDateTime now = QDateTime::currentDateTime();
    record.setStartTime(now);
    EXPECT_EQ(record.startTime(), now);
}

TEST_F(RecordTest, GetEndTime) {
    Record record;
    QDateTime now = QDateTime::currentDateTime();
    record.setEndTime(now);
    EXPECT_EQ(record.endTime(), now);
}

TEST_F(RecordTest, GetDurationMinutes) {
    Record record;
    record.setDurationMinutes(60);
    EXPECT_EQ(record.durationMinutes(), 60);
}

TEST_F(RecordTest, GetCost) {
    Record record;
    record.setCost(1.5);
    EXPECT_DOUBLE_EQ(record.cost(), 1.5);
}

TEST_F(RecordTest, GetState) {
    Record record;
    EXPECT_EQ(record.state(), SessionState::Offline);
    record.setState(SessionState::Online);
    EXPECT_EQ(record.state(), SessionState::Online);
}

TEST_F(RecordTest, GetLocation) {
    Record record;
    record.setLocation("机房A101");
    EXPECT_EQ(record.location(), "机房A101");
}

// ========== Setter测试 ==========

TEST_F(RecordTest, SetRecordId) {
    Record record;
    record.setRecordId("NewRecordId");
    EXPECT_EQ(record.recordId(), "NewRecordId");
}

TEST_F(RecordTest, SetCardId) {
    Record record;
    record.setCardId("C999");
    EXPECT_EQ(record.cardId(), "C999");
}

TEST_F(RecordTest, SetDate) {
    Record record;
    record.setDate("2024-12-31");
    EXPECT_EQ(record.date(), "2024-12-31");
}

TEST_F(RecordTest, SetStartTimeUpdatesDate) {
    Record record;
    QDateTime dateTime = QDateTime::fromString("2024-06-15T10:30:00", Qt::ISODate);
    record.setStartTime(dateTime);
    EXPECT_EQ(record.startTime(), dateTime);
    EXPECT_EQ(record.date(), "2024-06-15");
}

TEST_F(RecordTest, SetEndTime) {
    Record record;
    QDateTime endTime = QDateTime::currentDateTime().addSecs(3600);
    record.setEndTime(endTime);
    EXPECT_EQ(record.endTime(), endTime);
}

TEST_F(RecordTest, SetDurationMinutes) {
    Record record;
    record.setDurationMinutes(120);
    EXPECT_EQ(record.durationMinutes(), 120);
}

TEST_F(RecordTest, SetCost) {
    Record record;
    record.setCost(2.5);
    EXPECT_DOUBLE_EQ(record.cost(), 2.5);
}

TEST_F(RecordTest, SetState) {
    Record record;
    record.setState(SessionState::Online);
    EXPECT_EQ(record.state(), SessionState::Online);
    record.setState(SessionState::Offline);
    EXPECT_EQ(record.state(), SessionState::Offline);
}

TEST_F(RecordTest, SetLocation) {
    Record record;
    record.setLocation("图书馆电子阅览室");
    EXPECT_EQ(record.location(), "图书馆电子阅览室");
}

// ========== 状态检查方法测试 ==========

TEST_F(RecordTest, IsOnline) {
    Record record;
    EXPECT_FALSE(record.isOnline());
    record.setState(SessionState::Online);
    EXPECT_TRUE(record.isOnline());
}

TEST_F(RecordTest, IsOffline) {
    Record record;
    EXPECT_TRUE(record.isOffline());
    record.setState(SessionState::Online);
    EXPECT_FALSE(record.isOffline());
}

TEST_F(RecordTest, IsValidWithRecordId) {
    Record record;
    EXPECT_FALSE(record.isValid());
    record.setRecordId("R001");
    EXPECT_TRUE(record.isValid());
}

TEST_F(RecordTest, IsValidEmptyRecordId) {
    Record record;
    record.setRecordId("");
    EXPECT_FALSE(record.isValid());
}

// ========== JSON序列化测试 ==========

TEST_F(RecordTest, ToJson) {
    Record record;
    record.setRecordId("R001");
    record.setCardId("C001");
    record.setDate("2024-01-15");
    QDateTime startTime = QDateTime::fromString("2024-01-15T10:00:00", Qt::ISODate);
    QDateTime endTime = QDateTime::fromString("2024-01-15T11:00:00", Qt::ISODate);
    record.setStartTime(startTime);
    record.setEndTime(endTime);
    record.setDurationMinutes(60);
    record.setCost(1.0);
    record.setState(SessionState::Offline);
    record.setLocation("机房A101");
    
    QJsonObject json = record.toJson();
    
    EXPECT_EQ(json["recordId"].toString(), "R001");
    EXPECT_EQ(json["cardId"].toString(), "C001");
    EXPECT_EQ(json["date"].toString(), "2024-01-15");
    EXPECT_EQ(json["durationMinutes"].toInt(), 60);
    EXPECT_DOUBLE_EQ(json["cost"].toDouble(), 1.0);
    EXPECT_EQ(json["state"].toInt(), static_cast<int>(SessionState::Offline));
    EXPECT_EQ(json["location"].toString(), "机房A101");
}

TEST_F(RecordTest, FromJson) {
    QJsonObject json;
    json["recordId"] = "R002";
    json["cardId"] = "C002";
    json["date"] = "2024-02-20";
    json["startTime"] = "2024-02-20T14:00:00";
    json["endTime"] = "2024-02-20T16:30:00";
    json["durationMinutes"] = 150;
    json["cost"] = 2.5;
    json["state"] = static_cast<int>(SessionState::Offline);
    json["location"] = "机房B202";
    
    Record record = Record::fromJson(json);
    
    EXPECT_EQ(record.recordId(), "R002");
    EXPECT_EQ(record.cardId(), "C002");
    EXPECT_EQ(record.date(), "2024-02-20");
    EXPECT_EQ(record.durationMinutes(), 150);
    EXPECT_DOUBLE_EQ(record.cost(), 2.5);
    EXPECT_EQ(record.state(), SessionState::Offline);
    EXPECT_EQ(record.location(), "机房B202");
}

TEST_F(RecordTest, JsonRoundTrip) {
    Record original;
    original.setRecordId("R003");
    original.setCardId("C003");
    QDateTime startTime = QDateTime::fromString("2024-03-10T09:00:00", Qt::ISODate);
    QDateTime endTime = QDateTime::fromString("2024-03-10T10:30:00", Qt::ISODate);
    original.setStartTime(startTime);
    original.setEndTime(endTime);
    original.setDurationMinutes(90);
    original.setCost(1.5);
    original.setState(SessionState::Offline);
    original.setLocation("实验楼C301");
    
    QJsonObject json = original.toJson();
    Record restored = Record::fromJson(json);
    
    EXPECT_EQ(restored.recordId(), original.recordId());
    EXPECT_EQ(restored.cardId(), original.cardId());
    EXPECT_EQ(restored.date(), original.date());
    EXPECT_EQ(restored.durationMinutes(), original.durationMinutes());
    EXPECT_DOUBLE_EQ(restored.cost(), original.cost());
    EXPECT_EQ(restored.state(), original.state());
    EXPECT_EQ(restored.location(), original.location());
}

TEST_F(RecordTest, FromJsonEmptyObject) {
    QJsonObject json;
    Record record = Record::fromJson(json);
    
    EXPECT_TRUE(record.recordId().isEmpty());
    EXPECT_TRUE(record.cardId().isEmpty());
    EXPECT_TRUE(record.date().isEmpty());
    EXPECT_EQ(record.durationMinutes(), 0);
    EXPECT_DOUBLE_EQ(record.cost(), 0.0);
    EXPECT_EQ(record.state(), SessionState::Offline);
    EXPECT_TRUE(record.location().isEmpty());
}

// ========== 边界条件测试 ==========

TEST_F(RecordTest, ZeroDuration) {
    Record record;
    record.setDurationMinutes(0);
    EXPECT_EQ(record.durationMinutes(), 0);
}

TEST_F(RecordTest, NegativeDuration) {
    Record record;
    record.setDurationMinutes(-10);
    EXPECT_EQ(record.durationMinutes(), -10);
}

TEST_F(RecordTest, VeryLargeDuration) {
    Record record;
    record.setDurationMinutes(100000);
    EXPECT_EQ(record.durationMinutes(), 100000);
}

TEST_F(RecordTest, ZeroCost) {
    Record record;
    record.setCost(0.0);
    EXPECT_DOUBLE_EQ(record.cost(), 0.0);
}

TEST_F(RecordTest, NegativeCost) {
    Record record;
    record.setCost(-5.0);
    EXPECT_DOUBLE_EQ(record.cost(), -5.0);
}

TEST_F(RecordTest, SpecialCharactersInLocation) {
    Record record;
    record.setLocation("机房#A-101@楼");
    EXPECT_EQ(record.location(), "机房#A-101@楼");
}

TEST_F(RecordTest, EmptyLocation) {
    Record record;
    record.setLocation("");
    EXPECT_TRUE(record.location().isEmpty());
}

TEST_F(RecordTest, InvalidDateTimeFromJson) {
    QJsonObject json;
    json["recordId"] = "R004";
    json["startTime"] = "invalid-date";
    json["endTime"] = "also-invalid";
    
    Record record = Record::fromJson(json);
    EXPECT_FALSE(record.startTime().isValid());
    EXPECT_FALSE(record.endTime().isValid());
}
