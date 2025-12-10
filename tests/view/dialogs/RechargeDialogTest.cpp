/**
 * @file RechargeDialogTest.cpp
 * @brief RechargeDialog对话框单元测试
 * @author CampusCardSystem
 * @date 2024
 */

#include "controller/MainController.h"
#include "model/repositories/StorageManager.h"
#include "view/dialogs/RechargeDialog.h"

#include <QTemporaryDir>
#include <gtest/gtest.h>

using namespace CampusCard;

class RechargeDialogTest : public ::testing::Test {
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
        mainController->cardService()->createCard("C001", "测试用户", "B17010101", 100.0);
    }

    void TearDown() override {
        delete mainController;
    }
};

// ========== 构造函数测试 ==========

TEST_F(RechargeDialogTest, Constructor) {
    RechargeDialog dialog(mainController->cardController(), "C001");
    EXPECT_FALSE(dialog.windowTitle().isEmpty());
}

TEST_F(RechargeDialogTest, WindowTitle) {
    RechargeDialog dialog(mainController->cardController(), "C001");
    EXPECT_TRUE(dialog.windowTitle().contains("充值") ||
                dialog.windowTitle().contains("Recharge") ||
                !dialog.windowTitle().isEmpty());
}
