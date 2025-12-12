/**
 * @file MainControllerTest.cpp
 * @brief MainController主控制器单元测试
 * @author CampusCardSystem
 * @date 2024
 */

#include "controller/MainController.h"
#include "model/repositories/StorageManager.h"

#include <QFile>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <gtest/gtest.h>

using namespace CampusCard;

class MainControllerTest : public ::testing::Test {
protected:
    QTemporaryDir tempDir;
    QString testDataPath;
    MainController* mainController;

    void SetUp() override {
        ASSERT_TRUE(tempDir.isValid());
        testDataPath = tempDir.path() + "/test_data";

        mainController = new MainController();
    }

    void TearDown() override {
        delete mainController;
    }
};

// ========== 初始化测试 ==========

TEST_F(MainControllerTest, Initialize) {
    QSignalSpy initSpy(mainController, &MainController::initialized);

    EXPECT_TRUE(mainController->initialize(testDataPath));
    EXPECT_EQ(initSpy.count(), 1);
}

TEST_F(MainControllerTest, InitializeCreatesServices) {
    mainController->initialize(testDataPath);

    EXPECT_NE(mainController->cardService(), nullptr);
    EXPECT_NE(mainController->recordService(), nullptr);
    EXPECT_NE(mainController->authService(), nullptr);
}

TEST_F(MainControllerTest, InitializeCreatesControllers) {
    mainController->initialize(testDataPath);

    EXPECT_NE(mainController->authController(), nullptr);
    EXPECT_NE(mainController->cardController(), nullptr);
    EXPECT_NE(mainController->recordController(), nullptr);
}

TEST_F(MainControllerTest, InitializeCreatesDataDirectory) {
    mainController->initialize(testDataPath);

    EXPECT_TRUE(QDir(testDataPath).exists());
    EXPECT_TRUE(QDir(testDataPath + "/records").exists());
}

// ========== 获取子控制器测试 ==========

TEST_F(MainControllerTest, AuthController) {
    mainController->initialize(testDataPath);

    AuthController* authController = mainController->authController();
    ASSERT_NE(authController, nullptr);
    EXPECT_FALSE(authController->isLoggedIn());
}

TEST_F(MainControllerTest, CardController) {
    mainController->initialize(testDataPath);

    CardController* cardController = mainController->cardController();
    ASSERT_NE(cardController, nullptr);
    EXPECT_GE(cardController->getCardCount(), 0);
}

TEST_F(MainControllerTest, RecordController) {
    mainController->initialize(testDataPath);

    RecordController* recordController = mainController->recordController();
    ASSERT_NE(recordController, nullptr);
    EXPECT_EQ(recordController->getOnlineCount(), 0);
}

// ========== 获取服务测试 ==========

TEST_F(MainControllerTest, CardService) {
    mainController->initialize(testDataPath);

    CardService* cardService = mainController->cardService();
    ASSERT_NE(cardService, nullptr);
}

TEST_F(MainControllerTest, RecordService) {
    mainController->initialize(testDataPath);

    RecordService* recordService = mainController->recordService();
    ASSERT_NE(recordService, nullptr);
}

TEST_F(MainControllerTest, AuthService) {
    mainController->initialize(testDataPath);

    AuthService* authService = mainController->authService();
    ASSERT_NE(authService, nullptr);
}

// ========== 数据管理测试 ==========

TEST_F(MainControllerTest, GenerateMockData) {
    mainController->initialize(testDataPath);

    QSignalSpy mockSpy(mainController, &MainController::mockDataGenerated);

    int initialCount = mainController->cardController()->getCardCount();
    mainController->generateMockData(5, 3);

    EXPECT_EQ(mockSpy.count(), 1);
    EXPECT_GE(mainController->cardController()->getCardCount(), initialCount + 5);
}

TEST_F(MainControllerTest, ExportData) {
    mainController->initialize(testDataPath);

    // 创建一些数据
    mainController->cardService()->createCard("C001", "张三", "B17010101", 100.0);

    QSignalSpy successSpy(mainController, &MainController::exportSuccess);

    QString exportPath = testDataPath + "/export.txt";
    EXPECT_TRUE(mainController->exportData(exportPath));
    EXPECT_EQ(successSpy.count(), 1);
    EXPECT_TRUE(QFile::exists(exportPath));
}

TEST_F(MainControllerTest, ExportDataInvalidPath) {
    mainController->initialize(testDataPath);

    QSignalSpy failedSpy(mainController, &MainController::exportFailed);

    // 尝试导出到不存在的目录
    EXPECT_FALSE(mainController->exportData("/nonexistent/directory/export.txt"));
    EXPECT_EQ(failedSpy.count(), 1);
}

TEST_F(MainControllerTest, ImportDataOverwrite) {
    mainController->initialize(testDataPath);

    // 创建初始数据
    mainController->cardService()->createCard("C001", "张三", "B17010101", 100.0);

    // 导出
    QString exportPath = testDataPath + "/export.txt";
    mainController->exportData(exportPath);

    // 添加新数据
    mainController->cardService()->createCard("C002", "李四", "B17010102", 200.0);
    EXPECT_EQ(mainController->cardController()->getCardCount(), 2);

    // 导入（覆盖）
    QSignalSpy successSpy(mainController, &MainController::importSuccess);

    EXPECT_TRUE(mainController->importData(exportPath, false));
    EXPECT_EQ(successSpy.count(), 1);
    EXPECT_EQ(mainController->cardController()->getCardCount(), 1);
}

TEST_F(MainControllerTest, ImportDataMerge) {
    mainController->initialize(testDataPath);

    // 创建初始数据
    mainController->cardService()->createCard("C001", "张三", "B17010101", 100.0);

    // 导出
    QString exportPath = testDataPath + "/export.txt";
    mainController->exportData(exportPath);

    // 清空数据并创建新数据
    StorageManager::instance().saveAllCards(QList<Card>());
    mainController->reloadData();
    mainController->cardService()->createCard("C002", "李四", "B17010102", 200.0);

    // 导入（合并）
    QSignalSpy successSpy(mainController, &MainController::importSuccess);

    EXPECT_TRUE(mainController->importData(exportPath, true));
    EXPECT_EQ(successSpy.count(), 1);
    EXPECT_EQ(mainController->cardController()->getCardCount(), 2);
}

TEST_F(MainControllerTest, ImportDataInvalidFile) {
    mainController->initialize(testDataPath);

    QSignalSpy failedSpy(mainController, &MainController::importFailed);

    EXPECT_FALSE(mainController->importData("/nonexistent/file.txt", false));
    EXPECT_EQ(failedSpy.count(), 1);
}

TEST_F(MainControllerTest, ReloadData) {
    mainController->initialize(testDataPath);

    // 创建数据
    mainController->cardService()->createCard("C001", "张三", "B17010101", 100.0);

    QSignalSpy reloadSpy(mainController, &MainController::dataReloaded);

    mainController->reloadData();

    EXPECT_EQ(reloadSpy.count(), 1);
    // 数据应该仍然存在
    EXPECT_EQ(mainController->cardController()->getCardCount(), 1);
}

// ========== 集成测试 ==========

TEST_F(MainControllerTest, FullWorkflow) {
    mainController->initialize(testDataPath);

    // 创建卡
    mainController->cardController()->handleCreateCard("C001", "张三", "B17010101", 100.0);
    EXPECT_EQ(mainController->cardController()->getCardCount(), 1);

    // 学生登录
    mainController->authController()->handleStudentLogin("C001", DEFAULT_STUDENT_PASSWORD);
    EXPECT_TRUE(mainController->authController()->isLoggedIn());

    // 开始上机
    mainController->recordController()->handleStartSession("C001", "机房A101");
    EXPECT_TRUE(mainController->recordController()->isOnline("C001"));

    // 结束上机
    mainController->recordController()->handleEndSession("C001");
    EXPECT_FALSE(mainController->recordController()->isOnline("C001"));

    // 登出
    mainController->authController()->handleLogout();
    EXPECT_FALSE(mainController->authController()->isLoggedIn());
}

TEST_F(MainControllerTest, AdminWorkflow) {
    mainController->initialize(testDataPath);

    // 管理员登录
    mainController->authController()->handleAdminLogin(DEFAULT_ADMIN_PASSWORD);
    EXPECT_TRUE(mainController->authController()->isLoggedIn());
    EXPECT_EQ(mainController->authController()->currentRole(), UserRole::Admin);

    // 创建卡
    mainController->cardController()->handleCreateCard("C001", "张三", "B17010101", 100.0);
    EXPECT_EQ(mainController->cardController()->getCardCount(), 1);

    // 充值
    mainController->cardController()->handleRecharge("C001", 50.0);
    EXPECT_DOUBLE_EQ(mainController->cardController()->getBalance("C001"), 150.0);

    // 冻结卡
    mainController->cardController()->handleFreeze("C001");
    Card card = mainController->cardController()->getCard("C001");
    EXPECT_EQ(card.state(), CardState::Frozen);

    // 解冻卡
    mainController->cardController()->handleUnfreeze("C001");
    card = mainController->cardController()->getCard("C001");
    EXPECT_EQ(card.state(), CardState::Normal);

    // 登出
    mainController->authController()->handleLogout();
    EXPECT_FALSE(mainController->authController()->isLoggedIn());
}
