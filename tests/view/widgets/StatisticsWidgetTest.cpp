/**
 * @file StatisticsWidgetTest.cpp
 * @brief StatisticsWidget组件单元测试
 * @author CampusCardSystem
 * @date 2024
 */

#include "controller/MainController.h"
#include "model/repositories/StorageManager.h"
#include "view/widgets/StatisticsWidget.h"

#include <QTemporaryDir>
#include <gtest/gtest.h>

using namespace CampusCard;

class StatisticsWidgetTest : public ::testing::Test {
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

TEST_F(StatisticsWidgetTest, Constructor) {
    StatisticsWidget widget(mainController->recordController(), mainController->cardController());
    EXPECT_NE(widget.layout(), nullptr);
}

// ========== 刷新测试 ==========

TEST_F(StatisticsWidgetTest, RefreshStatistics) {
    StatisticsWidget widget(mainController->recordController(), mainController->cardController());
    // 刷新不应抛出异常
    widget.refresh();
    EXPECT_NE(widget.layout(), nullptr);
}
