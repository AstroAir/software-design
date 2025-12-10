/**
 * @file RecordServiceTest.cpp
 * @brief RecordService业务服务层单元测试
 * @author CampusCardSystem
 * @date 2024
 */

#include "model/entities/Record.h"
#include "model/repositories/StorageManager.h"
#include "model/services/RecordService.h"

#include <QDate>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QThread>
#include <gtest/gtest.h>

using namespace CampusCard;

class RecordServiceTest : public ::testing::Test {
protected:
    QTemporaryDir tempDir;
    QString testDataPath;
    RecordService* recordService;

    void SetUp() override {
        ASSERT_TRUE(tempDir.isValid());
        testDataPath = tempDir.path() + "/test_data";
        StorageManager::instance().setDataPath(testDataPath);
        StorageManager::instance().initializeDataDirectory();

        recordService = new RecordService();
        recordService->initialize();
    }

    void TearDown() override {
        delete recordService;
    }
};

// ========== 初始化测试 ==========

TEST_F(RecordServiceTest, Initialize) {
    RecordService service;
    service.initialize();
    // 初始化应该成功，不抛出异常
    EXPECT_EQ(service.getOnlineCount(), 0);
}

// ========== 上下机操作测试 ==========

TEST_F(RecordServiceTest, StartSession) {
    Record record = recordService->startSession("C001", "机房A101");

    EXPECT_TRUE(record.isValid());
    EXPECT_EQ(record.cardId(), "C001");
    EXPECT_EQ(record.location(), "机房A101");
    EXPECT_EQ(record.state(), SessionState::Online);
    EXPECT_TRUE(record.startTime().isValid());
}

TEST_F(RecordServiceTest, StartSessionSignals) {
    QSignalSpy startedSpy(recordService, &RecordService::sessionStarted);
    QSignalSpy changedSpy(recordService, &RecordService::recordsChanged);

    recordService->startSession("C001", "机房A101");

    EXPECT_EQ(startedSpy.count(), 1);
    EXPECT_EQ(changedSpy.count(), 1);
    EXPECT_EQ(startedSpy.takeFirst().at(0).toString(), "C001");
}

TEST_F(RecordServiceTest, StartSessionAlreadyOnline) {
    recordService->startSession("C001", "机房A101");
    
    Record secondRecord = recordService->startSession("C001", "机房B202");
    EXPECT_FALSE(secondRecord.isValid());
}

TEST_F(RecordServiceTest, EndSession) {
    recordService->startSession("C001", "机房A101");
    
    // 等待一小段时间以确保有时长
    QThread::msleep(100);

    double cost = recordService->endSession("C001");

    EXPECT_GE(cost, 0.0);
    EXPECT_FALSE(recordService->isOnline("C001"));
}

TEST_F(RecordServiceTest, EndSessionSignals) {
    recordService->startSession("C001", "机房A101");

    QSignalSpy endedSpy(recordService, &RecordService::sessionEnded);
    QSignalSpy changedSpy(recordService, &RecordService::recordsChanged);

    recordService->endSession("C001");

    EXPECT_EQ(endedSpy.count(), 1);
    EXPECT_GE(changedSpy.count(), 1);
}

TEST_F(RecordServiceTest, EndSessionNotOnline) {
    double cost = recordService->endSession("C001");
    EXPECT_DOUBLE_EQ(cost, -1.0);
}

TEST_F(RecordServiceTest, IsOnline) {
    EXPECT_FALSE(recordService->isOnline("C001"));

    recordService->startSession("C001", "机房A101");
    EXPECT_TRUE(recordService->isOnline("C001"));

    recordService->endSession("C001");
    EXPECT_FALSE(recordService->isOnline("C001"));
}

TEST_F(RecordServiceTest, GetCurrentSession) {
    recordService->startSession("C001", "机房A101");

    Record session = recordService->getCurrentSession("C001");
    EXPECT_TRUE(session.isValid());
    EXPECT_EQ(session.cardId(), "C001");
    EXPECT_EQ(session.state(), SessionState::Online);
}

TEST_F(RecordServiceTest, GetCurrentSessionNotOnline) {
    Record session = recordService->getCurrentSession("C001");
    EXPECT_FALSE(session.isValid());
}

TEST_F(RecordServiceTest, GetCurrentSessionPtr) {
    recordService->startSession("C001", "机房A101");

    Record* sessionPtr = recordService->getCurrentSessionPtr("C001");
    ASSERT_NE(sessionPtr, nullptr);
    EXPECT_EQ(sessionPtr->cardId(), "C001");
}

TEST_F(RecordServiceTest, GetCurrentSessionPtrNotOnline) {
    Record* sessionPtr = recordService->getCurrentSessionPtr("C001");
    EXPECT_EQ(sessionPtr, nullptr);
}

TEST_F(RecordServiceTest, CalculateCurrentCost) {
    recordService->startSession("C001", "机房A101");

    double cost = recordService->calculateCurrentCost("C001");
    EXPECT_GE(cost, 0.0);
}

TEST_F(RecordServiceTest, CalculateCurrentCostNotOnline) {
    double cost = recordService->calculateCurrentCost("C001");
    EXPECT_DOUBLE_EQ(cost, -1.0);
}

// ========== 记录查询测试 ==========

TEST_F(RecordServiceTest, GetRecords) {
    recordService->startSession("C001", "机房A101");
    recordService->endSession("C001");
    recordService->startSession("C001", "机房B202");
    recordService->endSession("C001");

    QList<Record> records = recordService->getRecords("C001");
    EXPECT_EQ(records.size(), 2);
}

TEST_F(RecordServiceTest, GetRecordsEmpty) {
    QList<Record> records = recordService->getRecords("C999");
    EXPECT_TRUE(records.isEmpty());
}

TEST_F(RecordServiceTest, GetRecordsByDate) {
    recordService->startSession("C001", "机房A101");
    recordService->endSession("C001");

    QString today = QDate::currentDate().toString("yyyy-MM-dd");
    QList<Record> records = recordService->getRecordsByDate("C001", today);
    EXPECT_GE(records.size(), 1);
}

TEST_F(RecordServiceTest, GetRecordsByDateNoMatch) {
    recordService->startSession("C001", "机房A101");
    recordService->endSession("C001");

    QList<Record> records = recordService->getRecordsByDate("C001", "2000-01-01");
    EXPECT_TRUE(records.isEmpty());
}

TEST_F(RecordServiceTest, GetRecordsByDateRange) {
    recordService->startSession("C001", "机房A101");
    recordService->endSession("C001");

    QString today = QDate::currentDate().toString("yyyy-MM-dd");
    QString tomorrow = QDate::currentDate().addDays(1).toString("yyyy-MM-dd");

    QList<Record> records = recordService->getRecordsByDateRange("C001", today, tomorrow);
    EXPECT_GE(records.size(), 1);
}

TEST_F(RecordServiceTest, GetRecordsByLocation) {
    recordService->startSession("C001", "机房A101");
    recordService->endSession("C001");
    recordService->startSession("C001", "机房B202");
    recordService->endSession("C001");

    QList<Record> records = recordService->getRecordsByLocation("C001", "机房A101");
    EXPECT_EQ(records.size(), 1);
}

TEST_F(RecordServiceTest, GetAllRecordsByDate) {
    recordService->startSession("C001", "机房A101");
    recordService->endSession("C001");
    recordService->startSession("C002", "机房B202");
    recordService->endSession("C002");

    QString today = QDate::currentDate().toString("yyyy-MM-dd");
    QList<Record> records = recordService->getAllRecordsByDate(today);
    EXPECT_GE(records.size(), 2);
}

TEST_F(RecordServiceTest, GetLocations) {
    recordService->startSession("C001", "机房A101");
    recordService->endSession("C001");
    recordService->startSession("C001", "机房B202");
    recordService->endSession("C001");
    recordService->startSession("C001", "机房A101");
    recordService->endSession("C001");

    QStringList locations = recordService->getLocations("C001");
    EXPECT_EQ(locations.size(), 2);
    EXPECT_TRUE(locations.contains("机房A101"));
    EXPECT_TRUE(locations.contains("机房B202"));
}

// ========== 统计功能测试 ==========

TEST_F(RecordServiceTest, GetTotalSessionCount) {
    recordService->startSession("C001", "机房A101");
    recordService->endSession("C001");
    recordService->startSession("C001", "机房B202");
    recordService->endSession("C001");

    EXPECT_EQ(recordService->getTotalSessionCount("C001"), 2);
}

TEST_F(RecordServiceTest, GetTotalSessionCountOnlineNotCounted) {
    recordService->startSession("C001", "机房A101");
    recordService->endSession("C001");
    recordService->startSession("C001", "机房B202");
    // 不结束第二个会话

    EXPECT_EQ(recordService->getTotalSessionCount("C001"), 1);
}

TEST_F(RecordServiceTest, GetTotalDuration) {
    recordService->startSession("C001", "机房A101");
    QThread::msleep(100);
    recordService->endSession("C001");

    int duration = recordService->getTotalDuration("C001");
    EXPECT_GE(duration, 0);
}

TEST_F(RecordServiceTest, GetTotalCost) {
    recordService->startSession("C001", "机房A101");
    QThread::msleep(100);
    recordService->endSession("C001");

    double cost = recordService->getTotalCost("C001");
    EXPECT_GE(cost, 0.0);
}

TEST_F(RecordServiceTest, GetDailyIncome) {
    recordService->startSession("C001", "机房A101");
    recordService->endSession("C001");
    recordService->startSession("C002", "机房B202");
    recordService->endSession("C002");

    QString today = QDate::currentDate().toString("yyyy-MM-dd");
    double income = recordService->getDailyIncome(today);
    EXPECT_GE(income, 0.0);
}

TEST_F(RecordServiceTest, GetDailySessionCount) {
    recordService->startSession("C001", "机房A101");
    recordService->endSession("C001");
    recordService->startSession("C002", "机房B202");
    recordService->endSession("C002");

    QString today = QDate::currentDate().toString("yyyy-MM-dd");
    int count = recordService->getDailySessionCount(today);
    EXPECT_GE(count, 2);
}

TEST_F(RecordServiceTest, GetDailyTotalDuration) {
    recordService->startSession("C001", "机房A101");
    recordService->endSession("C001");

    QString today = QDate::currentDate().toString("yyyy-MM-dd");
    int duration = recordService->getDailyTotalDuration(today);
    EXPECT_GE(duration, 0);
}

TEST_F(RecordServiceTest, GetStatisticsSummary) {
    recordService->startSession("C001", "机房A101");
    recordService->endSession("C001");

    QString summary = recordService->getStatisticsSummary("C001");
    EXPECT_FALSE(summary.isEmpty());
}

TEST_F(RecordServiceTest, GetStatisticsSummaryNoRecords) {
    QString summary = recordService->getStatisticsSummary("C999");
    EXPECT_FALSE(summary.isEmpty());
}

TEST_F(RecordServiceTest, GetOnlineCount) {
    EXPECT_EQ(recordService->getOnlineCount(), 0);

    recordService->startSession("C001", "机房A101");
    EXPECT_EQ(recordService->getOnlineCount(), 1);

    recordService->startSession("C002", "机房B202");
    EXPECT_EQ(recordService->getOnlineCount(), 2);

    recordService->endSession("C001");
    EXPECT_EQ(recordService->getOnlineCount(), 1);
}

// ========== 边界条件测试 ==========

TEST_F(RecordServiceTest, MultipleCardsIndependent) {
    recordService->startSession("C001", "机房A101");
    recordService->startSession("C002", "机房B202");

    EXPECT_TRUE(recordService->isOnline("C001"));
    EXPECT_TRUE(recordService->isOnline("C002"));

    recordService->endSession("C001");
    EXPECT_FALSE(recordService->isOnline("C001"));
    EXPECT_TRUE(recordService->isOnline("C002"));
}

TEST_F(RecordServiceTest, EmptyLocation) {
    Record record = recordService->startSession("C001", "");
    EXPECT_TRUE(record.isValid());
    EXPECT_TRUE(record.location().isEmpty());
}

TEST_F(RecordServiceTest, SpecialCharactersInLocation) {
    Record record = recordService->startSession("C001", "机房#A-101@楼");
    EXPECT_TRUE(record.isValid());
    EXPECT_EQ(record.location(), "机房#A-101@楼");
}
