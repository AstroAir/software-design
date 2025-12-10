/**
 * @file RegisterDialogTest.cpp
 * @brief RegisterDialog对话框单元测试
 * @author CampusCardSystem
 * @date 2024
 */

#include "controller/MainController.h"
#include "model/repositories/StorageManager.h"
#include "view/dialogs/RegisterDialog.h"

#include <QTemporaryDir>
#include <gtest/gtest.h>

using namespace CampusCard;

class RegisterDialogTest : public ::testing::Test {
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

TEST_F(RegisterDialogTest, Constructor) {
    RegisterDialog dialog(mainController->cardController());
    EXPECT_FALSE(dialog.windowTitle().isEmpty());
}

TEST_F(RegisterDialogTest, WindowTitle) {
    RegisterDialog dialog(mainController->cardController());
    EXPECT_TRUE(dialog.windowTitle().contains("注册") ||
                dialog.windowTitle().contains("Register") ||
                !dialog.windowTitle().isEmpty());
}
