/**
 * @file StudentPanelTest.cpp
 * @brief StudentPanel面板单元测试
 * @author CampusCardSystem
 * @date 2024
 */

#include "controller/MainController.h"
#include "model/repositories/StorageManager.h"
#include "view/panels/StudentPanel.h"

#include <QTemporaryDir>
#include <gtest/gtest.h>

using namespace CampusCard;

class StudentPanelTest : public ::testing::Test {
protected:
    QTemporaryDir tempDir;
    QString testDataPath;
    MainController* mainController;

    void SetUp() override {
        ASSERT_TRUE(tempDir.isValid());
        testDataPath = tempDir.path() + "/test_data";

        mainController = new MainController();
        mainController->initialize(testDataPath);

        // 创建测试卡
        mainController->cardService()->createCard("C001", "张三", "B17010101", 100.0);
    }

    void TearDown() override {
        delete mainController;
    }
};

// ========== 构造函数测试 ==========

TEST_F(StudentPanelTest, Constructor) {
    StudentPanel panel(mainController->cardController(), mainController->recordController());
    panel.setCurrentCard("C001");
    EXPECT_NE(panel.layout(), nullptr);
}

// ========== 数据刷新测试 ==========

TEST_F(StudentPanelTest, RefreshData) {
    StudentPanel panel(mainController->cardController(), mainController->recordController());
    panel.setCurrentCard("C001");

    // 充值
    mainController->cardService()->recharge("C001", 50.0);

    // 刷新数据
    panel.refresh();

    // 验证余额已更新
    EXPECT_DOUBLE_EQ(mainController->cardController()->getBalance("C001"), 150.0);
}

// ========== 上机状态测试 ==========

TEST_F(StudentPanelTest, InitialNotOnline) {
    StudentPanel panel(mainController->cardController(), mainController->recordController());
    panel.setCurrentCard("C001");
    EXPECT_FALSE(mainController->recordController()->isOnline("C001"));
}
