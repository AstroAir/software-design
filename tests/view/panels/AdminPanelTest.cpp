/**
 * @file AdminPanelTest.cpp
 * @brief AdminPanel面板单元测试
 * @author CampusCardSystem
 * @date 2024
 */

#include "controller/MainController.h"
#include "model/repositories/StorageManager.h"
#include "view/panels/AdminPanel.h"

#include <QTemporaryDir>
#include <gtest/gtest.h>

using namespace CampusCard;

class AdminPanelTest : public ::testing::Test {
protected:
    QTemporaryDir tempDir;
    QString testDataPath;
    MainController* mainController;

    void SetUp() override {
        ASSERT_TRUE(tempDir.isValid());
        testDataPath = tempDir.path() + "/test_data";

        mainController = new MainController();
        mainController->initialize(testDataPath);
    }

    void TearDown() override {
        delete mainController;
    }
};

// ========== 构造函数测试 ==========

TEST_F(AdminPanelTest, Constructor) {
    AdminPanel panel(mainController);
    EXPECT_NE(panel.layout(), nullptr);
}

// ========== 数据刷新测试 ==========

TEST_F(AdminPanelTest, RefreshData) {
    AdminPanel panel(mainController);

    // 添加一张卡
    mainController->cardService()->createCard("C001", "张三", "B17010101", 100.0);

    // 调用刷新
    panel.refresh();

    // 验证卡已添加
    EXPECT_TRUE(mainController->cardController()->cardExists("C001"));
}
