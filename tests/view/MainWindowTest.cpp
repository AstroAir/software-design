/**
 * @file MainWindowTest.cpp
 * @brief MainWindow视图层单元测试
 * @author CampusCardSystem
 * @date 2024
 *
 * 注意：视图层测试主要验证组件创建和基本交互
 * 复杂的UI交互测试建议使用集成测试或端到端测试
 */

#include "view/MainWindow.h"

#include <gtest/gtest.h>

using namespace CampusCard;

class MainWindowTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

// ========== 构造函数测试 ==========
// 注意：MainWindow 内部会创建自己的 MainController

TEST_F(MainWindowTest, Constructor) {
    MainWindow window;
    // 验证窗口创建成功
    EXPECT_FALSE(window.windowTitle().isEmpty());
}

TEST_F(MainWindowTest, WindowTitle) {
    MainWindow window;
    // 窗口标题应包含系统名称或不为空
    EXPECT_TRUE(window.windowTitle().contains("校园卡") ||
                window.windowTitle().contains("Campus") ||
                !window.windowTitle().isEmpty());
}

// ========== 窗口属性测试 ==========

TEST_F(MainWindowTest, WindowSize) {
    MainWindow window;
    // 验证窗口有合理的尺寸
    EXPECT_GT(window.width(), 0);
    EXPECT_GT(window.height(), 0);
}
