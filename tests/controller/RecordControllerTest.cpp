/**
 * @file RecordControllerTest.cpp
 * @brief RecordController控制器层单元测试
 * @author CampusCardSystem
 * @date 2024
 */

#include "controller/RecordController.h"
#include "model/repositories/StorageManager.h"
#include "model/services/CardService.h"
#include "model/services/RecordService.h"

#include <QDate>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QThread>
#include <gtest/gtest.h>

using namespace CampusCard;

class RecordControllerTest : public ::testing::Test {
protected:
    QTemporaryDir tempDir;
    QString testDataPath;
    CardService* cardService;
    RecordService* recordService;
    RecordController* recordController;

    void SetUp() override {
        ASSERT_TRUE(tempDir.isValid());
        testDataPath = tempDir.path() + "/test_data";
        StorageManager::instance().setDataPath(testDataPath);
        StorageManager::instance().initializeDataDirectory();

        StorageManager::instance().saveAllCards(QList<Card>());

        cardService = new CardService();
        cardService->initialize();

        recordService = new RecordService();
        recordService->initialize();

        recordController = new RecordController(recordService, cardService);
    }

    void TearDown() override {
        delete recordController;
        delete recordService;
        delete cardService;
    }
};

// ========== 上下机操作测试 ==========

TEST_F(RecordControllerTest, HandleStartSessionSuccess) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    QSignalSpy successSpy(recordController, &RecordController::sessionStarted);

    recordController->handleStartSession("C001", "机房A101");

    EXPECT_EQ(successSpy.count(), 1);
    QList<QVariant> args = successSpy.takeFirst();
    EXPECT_EQ(args.at(0).toString(), "C001");
    EXPECT_EQ(args.at(1).toString(), "机房A101");
}

TEST_F(RecordControllerTest, HandleStartSessionCardNotFound) {
    QSignalSpy failedSpy(recordController, &RecordController::sessionStartFailed);

    recordController->handleStartSession("C999", "机房A101");

    EXPECT_EQ(failedSpy.count(), 1);
}

TEST_F(RecordControllerTest, HandleStartSessionCardNotUsable) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    cardService->reportLost("C001");

    QSignalSpy failedSpy(recordController, &RecordController::sessionStartFailed);

    recordController->handleStartSession("C001", "机房A101");

    EXPECT_EQ(failedSpy.count(), 1);
}

TEST_F(RecordControllerTest, HandleStartSessionInsufficientBalance) {
    cardService->createCard("C001", "张三", "B17010101", 0.0);

    QSignalSpy failedSpy(recordController, &RecordController::sessionStartFailed);

    recordController->handleStartSession("C001", "机房A101");

    EXPECT_EQ(failedSpy.count(), 1);
}

TEST_F(RecordControllerTest, HandleStartSessionAlreadyOnline) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    recordController->handleStartSession("C001", "机房A101");

    QSignalSpy failedSpy(recordController, &RecordController::sessionStartFailed);

    recordController->handleStartSession("C001", "机房B202");

    EXPECT_EQ(failedSpy.count(), 1);
}

TEST_F(RecordControllerTest, HandleEndSessionSuccess) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    recordController->handleStartSession("C001", "机房A101");

    QThread::msleep(100);

    QSignalSpy successSpy(recordController, &RecordController::sessionEnded);

    recordController->handleEndSession("C001");

    EXPECT_EQ(successSpy.count(), 1);
}

TEST_F(RecordControllerTest, HandleEndSessionNotOnline) {
    QSignalSpy failedSpy(recordController, &RecordController::sessionEndFailed);

    recordController->handleEndSession("C001");

    EXPECT_EQ(failedSpy.count(), 1);
}

TEST_F(RecordControllerTest, HandleEndSessionDeductsBalance) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    recordController->handleStartSession("C001", "机房A101");

    QThread::msleep(100);

    double balanceBefore = cardService->getBalance("C001");
    recordController->handleEndSession("C001");
    double balanceAfter = cardService->getBalance("C001");

    EXPECT_LE(balanceAfter, balanceBefore);
}

TEST_F(RecordControllerTest, IsOnline) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    EXPECT_FALSE(recordController->isOnline("C001"));

    recordController->handleStartSession("C001", "机房A101");
    EXPECT_TRUE(recordController->isOnline("C001"));

    recordController->handleEndSession("C001");
    EXPECT_FALSE(recordController->isOnline("C001"));
}

TEST_F(RecordControllerTest, GetCurrentSession) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    recordController->handleStartSession("C001", "机房A101");

    Record session = recordController->getCurrentSession("C001");
    EXPECT_TRUE(session.isValid());
    EXPECT_EQ(session.location(), "机房A101");
}

TEST_F(RecordControllerTest, GetCurrentSessionNotOnline) {
    Record session = recordController->getCurrentSession("C001");
    EXPECT_FALSE(session.isValid());
}

TEST_F(RecordControllerTest, GetCurrentCost) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    recordController->handleStartSession("C001", "机房A101");

    double cost = recordController->getCurrentCost("C001");
    EXPECT_GE(cost, 0.0);
}

// ========== 记录查询测试 ==========

TEST_F(RecordControllerTest, GetRecords) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    recordController->handleStartSession("C001", "机房A101");
    recordController->handleEndSession("C001");
    recordController->handleStartSession("C001", "机房B202");
    recordController->handleEndSession("C001");

    QList<Record> records = recordController->getRecords("C001");
    EXPECT_EQ(records.size(), 2);
}

TEST_F(RecordControllerTest, GetRecordsByDateRange) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    recordController->handleStartSession("C001", "机房A101");
    recordController->handleEndSession("C001");

    QString today = QDate::currentDate().toString("yyyy-MM-dd");
    QString tomorrow = QDate::currentDate().addDays(1).toString("yyyy-MM-dd");

    QList<Record> records = recordController->getRecordsByDateRange("C001", today, tomorrow);
    EXPECT_GE(records.size(), 1);
}

TEST_F(RecordControllerTest, GetRecordsByLocation) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    recordController->handleStartSession("C001", "机房A101");
    recordController->handleEndSession("C001");
    recordController->handleStartSession("C001", "机房B202");
    recordController->handleEndSession("C001");

    QList<Record> records = recordController->getRecordsByLocation("C001", "机房A101");
    EXPECT_EQ(records.size(), 1);
}

TEST_F(RecordControllerTest, GetFilteredRecords) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    recordController->handleStartSession("C001", "机房A101");
    recordController->handleEndSession("C001");

    QString today = QDate::currentDate().toString("yyyy-MM-dd");
    QString tomorrow = QDate::currentDate().addDays(1).toString("yyyy-MM-dd");

    QList<Record> records = recordController->getFilteredRecords("C001", today, tomorrow, "机房A101");
    EXPECT_EQ(records.size(), 1);
}

TEST_F(RecordControllerTest, GetFilteredRecordsEmptyLocation) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    recordController->handleStartSession("C001", "机房A101");
    recordController->handleEndSession("C001");
    recordController->handleStartSession("C001", "机房B202");
    recordController->handleEndSession("C001");

    QString today = QDate::currentDate().toString("yyyy-MM-dd");
    QString tomorrow = QDate::currentDate().addDays(1).toString("yyyy-MM-dd");

    QList<Record> records = recordController->getFilteredRecords("C001", today, tomorrow, "");
    EXPECT_EQ(records.size(), 2);
}

TEST_F(RecordControllerTest, GetLocations) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    recordController->handleStartSession("C001", "机房A101");
    recordController->handleEndSession("C001");
    recordController->handleStartSession("C001", "机房B202");
    recordController->handleEndSession("C001");

    QStringList locations = recordController->getLocations("C001");
    EXPECT_EQ(locations.size(), 2);
}

TEST_F(RecordControllerTest, GetAllRecordsByDate) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    cardService->createCard("C002", "李四", "B17010102", 100.0);
    recordController->handleStartSession("C001", "机房A101");
    recordController->handleEndSession("C001");
    recordController->handleStartSession("C002", "机房B202");
    recordController->handleEndSession("C002");

    QString today = QDate::currentDate().toString("yyyy-MM-dd");
    QList<Record> records = recordController->getAllRecordsByDate(today);
    EXPECT_GE(records.size(), 2);
}

// ========== 统计查询测试 ==========

TEST_F(RecordControllerTest, GetTotalSessionCount) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    recordController->handleStartSession("C001", "机房A101");
    recordController->handleEndSession("C001");
    recordController->handleStartSession("C001", "机房B202");
    recordController->handleEndSession("C001");

    EXPECT_EQ(recordController->getTotalSessionCount("C001"), 2);
}

TEST_F(RecordControllerTest, GetTotalDuration) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    recordController->handleStartSession("C001", "机房A101");
    QThread::msleep(100);
    recordController->handleEndSession("C001");

    int duration = recordController->getTotalDuration("C001");
    EXPECT_GE(duration, 0);
}

TEST_F(RecordControllerTest, GetTotalCost) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    recordController->handleStartSession("C001", "机房A101");
    QThread::msleep(100);
    recordController->handleEndSession("C001");

    double cost = recordController->getTotalCost("C001");
    EXPECT_GE(cost, 0.0);
}

TEST_F(RecordControllerTest, GetDailyIncome) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    recordController->handleStartSession("C001", "机房A101");
    recordController->handleEndSession("C001");

    QString today = QDate::currentDate().toString("yyyy-MM-dd");
    double income = recordController->getDailyIncome(today);
    EXPECT_GE(income, 0.0);
}

TEST_F(RecordControllerTest, GetDailySessionCount) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    recordController->handleStartSession("C001", "机房A101");
    recordController->handleEndSession("C001");

    QString today = QDate::currentDate().toString("yyyy-MM-dd");
    int count = recordController->getDailySessionCount(today);
    EXPECT_GE(count, 1);
}

TEST_F(RecordControllerTest, GetDailyTotalDuration) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    recordController->handleStartSession("C001", "机房A101");
    recordController->handleEndSession("C001");

    QString today = QDate::currentDate().toString("yyyy-MM-dd");
    int duration = recordController->getDailyTotalDuration(today);
    EXPECT_GE(duration, 0);
}

TEST_F(RecordControllerTest, GetStatisticsSummary) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    recordController->handleStartSession("C001", "机房A101");
    recordController->handleEndSession("C001");

    QString summary = recordController->getStatisticsSummary("C001");
    EXPECT_FALSE(summary.isEmpty());
}

TEST_F(RecordControllerTest, GetOnlineCount) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    cardService->createCard("C002", "李四", "B17010102", 100.0);

    EXPECT_EQ(recordController->getOnlineCount(), 0);

    recordController->handleStartSession("C001", "机房A101");
    EXPECT_EQ(recordController->getOnlineCount(), 1);

    recordController->handleStartSession("C002", "机房B202");
    EXPECT_EQ(recordController->getOnlineCount(), 2);

    recordController->handleEndSession("C001");
    EXPECT_EQ(recordController->getOnlineCount(), 1);
}

// ========== 信号转发测试 ==========

TEST_F(RecordControllerTest, RecordsUpdatedSignal) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    QSignalSpy updatedSpy(recordController, &RecordController::recordsUpdated);

    recordController->handleStartSession("C001", "机房A101");

    EXPECT_EQ(updatedSpy.count(), 1);
}
