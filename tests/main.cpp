/**
 * @file main.cpp
 * @brief 单元测试入口
 * @author CampusCardSystem
 * @date 2024
 */

#include <QCoreApplication>
#include <gtest/gtest.h>

int main(int argc, char** argv) {
    // 初始化Qt应用程序（某些测试可能需要Qt事件循环）
    QCoreApplication app(argc, argv);

    // 初始化GTest
    ::testing::InitGoogleTest(&argc, argv);

    // 运行所有测试
    return RUN_ALL_TESTS();
}
