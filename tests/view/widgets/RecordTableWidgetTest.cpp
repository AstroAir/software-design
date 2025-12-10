/**
 * @file RecordTableWidgetTest.cpp
 * @brief RecordTableWidget组件单元测试
 * @author CampusCardSystem
 * @date 2024
 */

#include "model/entities/Record.h"
#include "view/widgets/RecordTableWidget.h"

#include <QDateTime>
#include <gtest/gtest.h>

using namespace CampusCard;

class RecordTableWidgetTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}

    Record createTestRecord(const QString& cardId, const QString& location) {
        Record record;
        record.setRecordId("R001");
        record.setCardId(cardId);
        record.setLocation(location);
        record.setStartTime(QDateTime::currentDateTime().addSecs(-3600));
        record.setEndTime(QDateTime::currentDateTime());
        record.setDurationMinutes(60);
        record.setCost(1.0);
        record.setState(SessionState::Offline);
        return record;
    }
};

// ========== 构造函数测试 ==========

TEST_F(RecordTableWidgetTest, Constructor) {
    RecordTableWidget widget;
    EXPECT_EQ(widget.recordCount(), 0);
}

// ========== 数据设置测试 ==========

TEST_F(RecordTableWidgetTest, SetRecordsEmpty) {
    RecordTableWidget widget;
    widget.setRecords(QList<Record>());
    EXPECT_EQ(widget.recordCount(), 0);
}

TEST_F(RecordTableWidgetTest, SetRecordsSingle) {
    RecordTableWidget widget;

    QList<Record> records;
    records.append(createTestRecord("C001", "机房A101"));

    widget.setRecords(records);
    EXPECT_EQ(widget.recordCount(), 1);
}

TEST_F(RecordTableWidgetTest, SetRecordsMultiple) {
    RecordTableWidget widget;

    QList<Record> records;
    records.append(createTestRecord("C001", "机房A101"));
    records.append(createTestRecord("C001", "机房B202"));
    records.append(createTestRecord("C001", "机房C303"));

    widget.setRecords(records);
    EXPECT_EQ(widget.recordCount(), 3);
}

// ========== 清除测试 ==========

TEST_F(RecordTableWidgetTest, ClearRecords) {
    RecordTableWidget widget;

    QList<Record> records;
    records.append(createTestRecord("C001", "机房A101"));
    widget.setRecords(records);

    EXPECT_EQ(widget.recordCount(), 1);

    widget.clear();
    EXPECT_EQ(widget.recordCount(), 0);
}
