/**
 * @file StorageManagerTest.cpp
 * @brief StorageManager数据访问层单元测试
 * @author CampusCardSystem
 * @date 2024
 */

#include "model/entities/Card.h"
#include "model/entities/Record.h"
#include "model/repositories/StorageManager.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include <QUuid>
#include <gtest/gtest.h>

using namespace CampusCard;

class StorageManagerTest : public ::testing::Test {
protected:
    QTemporaryDir tempDir;
    QString testDataPath;

    void SetUp() override {
        ASSERT_TRUE(tempDir.isValid());
        testDataPath = tempDir.path() + "/test_data";
        StorageManager::instance().setDataPath(testDataPath);
    }

    void TearDown() override {
        // 临时目录会自动清理
    }

    Card createTestCard(const QString& cardId, const QString& name, const QString& studentId,
                        double balance = 100.0) {
        return Card(cardId, name, studentId, balance);
    }

    Record createTestRecord(const QString& cardId) {
        Record record;
        record.setRecordId(QUuid::createUuid().toString(QUuid::WithoutBraces));
        record.setCardId(cardId);
        record.setLocation("机房A101");
        record.setStartTime(QDateTime::currentDateTime().addSecs(-3600));
        record.setEndTime(QDateTime::currentDateTime());
        record.setDurationMinutes(60);
        record.setCost(1.0);
        record.setState(SessionState::Offline);
        return record;
    }
};

// ========== 单例测试 ==========

TEST_F(StorageManagerTest, SingletonInstance) {
    StorageManager& instance1 = StorageManager::instance();
    StorageManager& instance2 = StorageManager::instance();
    EXPECT_EQ(&instance1, &instance2);
}

// ========== 数据路径测试 ==========

TEST_F(StorageManagerTest, SetDataPath) {
    QString path = "/test/path";
    StorageManager::instance().setDataPath(path);
    EXPECT_EQ(StorageManager::instance().dataPath(), path);
    // 恢复测试路径
    StorageManager::instance().setDataPath(testDataPath);
}

TEST_F(StorageManagerTest, GetDataPath) {
    StorageManager::instance().setDataPath(testDataPath);
    EXPECT_EQ(StorageManager::instance().dataPath(), testDataPath);
}

// ========== 初始化测试 ==========

TEST_F(StorageManagerTest, InitializeDataDirectory) {
    EXPECT_TRUE(StorageManager::instance().initializeDataDirectory());
    EXPECT_TRUE(QDir(testDataPath).exists());
    EXPECT_TRUE(QDir(testDataPath + "/records").exists());
}

TEST_F(StorageManagerTest, InitializeCreatesDirectories) {
    // 确保目录不存在
    QDir dir(testDataPath);
    if (dir.exists()) {
        dir.removeRecursively();
    }

    EXPECT_TRUE(StorageManager::instance().initializeDataDirectory());
    EXPECT_TRUE(QDir(testDataPath).exists());
    EXPECT_TRUE(QDir(testDataPath + "/records").exists());
}

TEST_F(StorageManagerTest, CreateSampleData) {
    StorageManager::instance().initializeDataDirectory();
    
    // 验证示例数据被创建
    QList<Card> cards = StorageManager::instance().loadAllCards();
    EXPECT_FALSE(cards.isEmpty());
}

// ========== 卡数据操作测试 ==========

TEST_F(StorageManagerTest, SaveAndLoadAllCards) {
    StorageManager::instance().initializeDataDirectory();

    QList<Card> cardsToSave;
    cardsToSave.append(createTestCard("C001", "张三", "B17010101", 100.0));
    cardsToSave.append(createTestCard("C002", "李四", "B17010102", 200.0));
    cardsToSave.append(createTestCard("C003", "王五", "B17010103", 300.0));

    EXPECT_TRUE(StorageManager::instance().saveAllCards(cardsToSave));

    QList<Card> loadedCards = StorageManager::instance().loadAllCards();
    EXPECT_EQ(loadedCards.size(), 3);
}

TEST_F(StorageManagerTest, LoadAllCardsEmpty) {
    StorageManager::instance().initializeDataDirectory();
    
    // 删除cards.txt文件
    QFile::remove(testDataPath + "/cards.txt");
    
    QList<Card> cards = StorageManager::instance().loadAllCards();
    EXPECT_TRUE(cards.isEmpty());
}

TEST_F(StorageManagerTest, LoadCard) {
    StorageManager::instance().initializeDataDirectory();

    QList<Card> cardsToSave;
    cardsToSave.append(createTestCard("C001", "张三", "B17010101", 100.0));
    cardsToSave.append(createTestCard("C002", "李四", "B17010102", 200.0));
    StorageManager::instance().saveAllCards(cardsToSave);

    Card card = StorageManager::instance().loadCard("C001");
    EXPECT_EQ(card.cardId(), "C001");
    EXPECT_EQ(card.name(), "张三");
}

TEST_F(StorageManagerTest, LoadCardNotFound) {
    StorageManager::instance().initializeDataDirectory();

    QList<Card> cardsToSave;
    cardsToSave.append(createTestCard("C001", "张三", "B17010101", 100.0));
    StorageManager::instance().saveAllCards(cardsToSave);

    Card card = StorageManager::instance().loadCard("C999");
    EXPECT_TRUE(card.cardId().isEmpty());
}

TEST_F(StorageManagerTest, SaveAllCardsOverwrite) {
    StorageManager::instance().initializeDataDirectory();

    // 第一次保存
    QList<Card> cards1;
    cards1.append(createTestCard("C001", "张三", "B17010101", 100.0));
    StorageManager::instance().saveAllCards(cards1);

    // 第二次保存（覆盖）
    QList<Card> cards2;
    cards2.append(createTestCard("C002", "李四", "B17010102", 200.0));
    StorageManager::instance().saveAllCards(cards2);

    QList<Card> loadedCards = StorageManager::instance().loadAllCards();
    EXPECT_EQ(loadedCards.size(), 1);
    EXPECT_EQ(loadedCards[0].cardId(), "C002");
}

// ========== 记录数据操作测试 ==========
// 根据文档要求，记录文件以学号命名（如 B17010101.txt）

TEST_F(StorageManagerTest, SaveAndLoadRecords) {
    StorageManager::instance().initializeDataDirectory();

    QString studentId = "B17010101";  // 使用学号作为文件名
    QString cardId = "C001";
    QList<Record> recordsToSave;
    recordsToSave.append(createTestRecord(cardId));
    recordsToSave.append(createTestRecord(cardId));

    EXPECT_TRUE(StorageManager::instance().saveRecords(studentId, recordsToSave));

    QList<Record> loadedRecords = StorageManager::instance().loadRecords(studentId);
    EXPECT_EQ(loadedRecords.size(), 2);
}

TEST_F(StorageManagerTest, LoadRecordsEmpty) {
    StorageManager::instance().initializeDataDirectory();

    QList<Record> records = StorageManager::instance().loadRecords("B99999999");
    EXPECT_TRUE(records.isEmpty());
}

TEST_F(StorageManagerTest, AppendRecord) {
    StorageManager::instance().initializeDataDirectory();

    QString studentId = "B17010101";  // 使用学号作为文件名
    QString cardId = "C001";
    
    // 先保存一条记录
    QList<Record> initialRecords;
    initialRecords.append(createTestRecord(cardId));
    StorageManager::instance().saveRecords(studentId, initialRecords);

    // 追加一条记录
    Record newRecord = createTestRecord(cardId);
    EXPECT_TRUE(StorageManager::instance().appendRecord(studentId, newRecord));

    QList<Record> loadedRecords = StorageManager::instance().loadRecords(studentId);
    EXPECT_EQ(loadedRecords.size(), 2);
}

TEST_F(StorageManagerTest, UpdateRecord) {
    StorageManager::instance().initializeDataDirectory();

    QString studentId = "B17010101";  // 使用学号作为文件名
    QString cardId = "C001";
    Record record = createTestRecord(cardId);
    QString recordId = record.recordId();

    QList<Record> records;
    records.append(record);
    StorageManager::instance().saveRecords(studentId, records);

    // 更新记录
    record.setCost(5.0);
    record.setDurationMinutes(300);
    EXPECT_TRUE(StorageManager::instance().updateRecord(studentId, record));

    QList<Record> loadedRecords = StorageManager::instance().loadRecords(studentId);
    EXPECT_EQ(loadedRecords.size(), 1);
    EXPECT_DOUBLE_EQ(loadedRecords[0].cost(), 5.0);
    EXPECT_EQ(loadedRecords[0].durationMinutes(), 300);
}

TEST_F(StorageManagerTest, UpdateRecordNotFound) {
    StorageManager::instance().initializeDataDirectory();

    QString studentId = "B17010101";  // 使用学号作为文件名
    QString cardId = "C001";
    Record record = createTestRecord(cardId);

    QList<Record> records;
    records.append(record);
    StorageManager::instance().saveRecords(studentId, records);

    // 尝试更新不存在的记录
    Record nonExistentRecord = createTestRecord(cardId);
    nonExistentRecord.setRecordId("non-existent-id");
    EXPECT_FALSE(StorageManager::instance().updateRecord(studentId, nonExistentRecord));
}

TEST_F(StorageManagerTest, LoadAllRecords) {
    StorageManager::instance().initializeDataDirectory();

    // 为多个学生保存记录（使用学号作为文件名）
    QList<Record> records1;
    records1.append(createTestRecord("C001"));
    StorageManager::instance().saveRecords("B17010101", records1);

    QList<Record> records2;
    records2.append(createTestRecord("C002"));
    records2.append(createTestRecord("C002"));
    StorageManager::instance().saveRecords("B17010102", records2);

    QMap<QString, QList<Record>> allRecords = StorageManager::instance().loadAllRecords();
    EXPECT_EQ(allRecords.size(), 2);
    EXPECT_EQ(allRecords["B17010101"].size(), 1);
    EXPECT_EQ(allRecords["B17010102"].size(), 2);
}

// ========== 管理员密码测试 ==========

TEST_F(StorageManagerTest, SaveAndLoadAdminPassword) {
    StorageManager::instance().initializeDataDirectory();

    QString newPassword = "newAdminPass123";
    EXPECT_TRUE(StorageManager::instance().saveAdminPassword(newPassword));

    QString loadedPassword = StorageManager::instance().loadAdminPassword();
    EXPECT_EQ(loadedPassword, newPassword);
}

TEST_F(StorageManagerTest, LoadAdminPasswordDefault) {
    StorageManager::instance().initializeDataDirectory();

    // 删除admin.txt文件
    QFile::remove(testDataPath + "/admin.txt");

    QString password = StorageManager::instance().loadAdminPassword();
    EXPECT_EQ(password, DEFAULT_ADMIN_PASSWORD);
}

// ========== 导入导出测试 ==========

TEST_F(StorageManagerTest, ExportAllData) {
    StorageManager::instance().initializeDataDirectory();

    // 准备测试数据
    QList<Card> cards;
    cards.append(createTestCard("C001", "张三", "B17010101", 100.0));
    StorageManager::instance().saveAllCards(cards);

    // 使用学号作为记录文件名
    QList<Record> records;
    records.append(createTestRecord("C001"));
    StorageManager::instance().saveRecords("B17010101", records);

    StorageManager::instance().saveAdminPassword("testpass");

    // 导出
    QString exportPath = testDataPath + "/export.txt";
    EXPECT_TRUE(StorageManager::instance().exportAllData(exportPath));
    EXPECT_TRUE(QFile::exists(exportPath));
}

TEST_F(StorageManagerTest, ImportDataOverwrite) {
    StorageManager::instance().initializeDataDirectory();

    // 先创建一些数据
    QList<Card> originalCards;
    originalCards.append(createTestCard("C001", "张三", "B17010101", 100.0));
    StorageManager::instance().saveAllCards(originalCards);

    // 导出
    QString exportPath = testDataPath + "/export.txt";
    StorageManager::instance().exportAllData(exportPath);

    // 修改数据
    QList<Card> newCards;
    newCards.append(createTestCard("C002", "李四", "B17010102", 200.0));
    StorageManager::instance().saveAllCards(newCards);

    // 导入（覆盖模式）
    EXPECT_TRUE(StorageManager::instance().importData(exportPath, false));

    QList<Card> loadedCards = StorageManager::instance().loadAllCards();
    EXPECT_EQ(loadedCards.size(), 1);
    EXPECT_EQ(loadedCards[0].cardId(), "C001");
}

TEST_F(StorageManagerTest, ImportDataMerge) {
    StorageManager::instance().initializeDataDirectory();

    // 先创建一些数据
    QList<Card> originalCards;
    originalCards.append(createTestCard("C001", "张三", "B17010101", 100.0));
    StorageManager::instance().saveAllCards(originalCards);

    // 导出
    QString exportPath = testDataPath + "/export.txt";
    StorageManager::instance().exportAllData(exportPath);

    // 添加新数据
    QList<Card> newCards;
    newCards.append(createTestCard("C002", "李四", "B17010102", 200.0));
    StorageManager::instance().saveAllCards(newCards);

    // 导入（合并模式）
    EXPECT_TRUE(StorageManager::instance().importData(exportPath, true));

    QList<Card> loadedCards = StorageManager::instance().loadAllCards();
    EXPECT_EQ(loadedCards.size(), 2);
}

TEST_F(StorageManagerTest, ImportDataFileNotFound) {
    StorageManager::instance().initializeDataDirectory();
    EXPECT_FALSE(StorageManager::instance().importData("/nonexistent/path.txt", false));
}

// ========== 模拟数据生成测试 ==========

TEST_F(StorageManagerTest, GenerateMockData) {
    StorageManager::instance().initializeDataDirectory();

    int cardCount = 5;
    int recordsPerCard = 3;
    StorageManager::instance().generateMockData(cardCount, recordsPerCard);

    QList<Card> cards = StorageManager::instance().loadAllCards();
    // 可能已经有示例数据，所以至少有cardCount张卡
    EXPECT_GE(cards.size(), cardCount);
}

// ========== 边界条件测试 ==========

TEST_F(StorageManagerTest, SaveEmptyCardList) {
    StorageManager::instance().initializeDataDirectory();

    QList<Card> emptyCards;
    EXPECT_TRUE(StorageManager::instance().saveAllCards(emptyCards));

    QList<Card> loadedCards = StorageManager::instance().loadAllCards();
    EXPECT_TRUE(loadedCards.isEmpty());
}

TEST_F(StorageManagerTest, SaveEmptyRecordList) {
    StorageManager::instance().initializeDataDirectory();

    // 使用学号作为记录文件名
    QList<Record> emptyRecords;
    EXPECT_TRUE(StorageManager::instance().saveRecords("B17010101", emptyRecords));

    QList<Record> loadedRecords = StorageManager::instance().loadRecords("B17010101");
    EXPECT_TRUE(loadedRecords.isEmpty());
}

TEST_F(StorageManagerTest, CardWithSpecialCharactersInId) {
    StorageManager::instance().initializeDataDirectory();

    // 注意：某些特殊字符可能不适合作为文件名的一部分
    QList<Card> cards;
    cards.append(createTestCard("C-001", "张三", "B17010101", 100.0));
    EXPECT_TRUE(StorageManager::instance().saveAllCards(cards));

    QList<Card> loadedCards = StorageManager::instance().loadAllCards();
    EXPECT_EQ(loadedCards.size(), 1);
    EXPECT_EQ(loadedCards[0].cardId(), "C-001");
}
