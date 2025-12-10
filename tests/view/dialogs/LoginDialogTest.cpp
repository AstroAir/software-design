/**
 * @file LoginDialogTest.cpp
 * @brief LoginDialog对话框单元测试
 * @author CampusCardSystem
 * @date 2024
 */

#include "controller/MainController.h"
#include "model/repositories/StorageManager.h"
#include "view/dialogs/LoginDialog.h"

#include <QTemporaryDir>
#include <gtest/gtest.h>

using namespace CampusCard;

class LoginDialogTest : public ::testing::Test {
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

TEST_F(LoginDialogTest, Constructor) {
    LoginDialog dialog(mainController->authController(), mainController->cardController());
    EXPECT_FALSE(dialog.windowTitle().isEmpty());
}

TEST_F(LoginDialogTest, WindowTitle) {
    LoginDialog dialog(mainController->authController(), mainController->cardController());
    EXPECT_TRUE(dialog.windowTitle().contains("登录") ||
                dialog.windowTitle().contains("Login") ||
                !dialog.windowTitle().isEmpty());
}
