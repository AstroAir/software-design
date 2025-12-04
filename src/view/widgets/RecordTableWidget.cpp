/**
 * @file RecordTableWidget.cpp
 * @brief 上机记录表格组件实现
 * @author CampusCardSystem
 * @date 2024
 *
 * MVC架构 - View层实现
 */

#include "RecordTableWidget.h"

#include "ElaTableView.h"
#include "model/Types.h"

#include <QHeaderView>
#include <QStandardItemModel>
#include <QVBoxLayout>

#include <algorithm>


namespace CampusCard {

RecordTableWidget::RecordTableWidget(QWidget* parent) : QWidget(parent) {
    initUI();
}

void RecordTableWidget::initUI() {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_tableView = new ElaTableView(this);
    m_model = new QStandardItemModel(this);

    // 设置表头
    m_model->setHorizontalHeaderLabels({QStringLiteral("日期"), QStringLiteral("开始时间"),
                                        QStringLiteral("结束时间"), QStringLiteral("时长(分钟)"),
                                        QStringLiteral("费用(元)"), QStringLiteral("地点"),
                                        QStringLiteral("状态")});

    m_tableView->setModel(m_model);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    m_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    layout->addWidget(m_tableView);
}

void RecordTableWidget::setRecords(const QList<Record>& records) {
    clear();

    // 按时间倒序排列（最新的在前）
    QList<Record> sortedRecords = records;
    std::sort(sortedRecords.begin(), sortedRecords.end(),
              [](const Record& a, const Record& b) { return a.startTime() > b.startTime(); });

    for (const auto& record : sortedRecords) {
        QList<QStandardItem*> row;

        row << new QStandardItem(record.date());
        row << new QStandardItem(record.startTime().toString(QStringLiteral("HH:mm:ss")));

        if (record.isOnline()) {
            row << new QStandardItem(QStringLiteral("--"));
            row << new QStandardItem(QStringLiteral("--"));
            row << new QStandardItem(QStringLiteral("--"));
        } else {
            row << new QStandardItem(record.endTime().toString(QStringLiteral("HH:mm:ss")));
            row << new QStandardItem(QString::number(record.durationMinutes()));
            row << new QStandardItem(QString::number(record.cost(), 'f', 2));
        }

        row << new QStandardItem(record.location());
        row << new QStandardItem(sessionStateToString(record.state()));

        // 存储记录ID
        row[0]->setData(record.recordId(), Qt::UserRole);

        m_model->appendRow(row);
    }
}

void RecordTableWidget::clear() {
    m_model->removeRows(0, m_model->rowCount());
}

QStringList RecordTableWidget::getSelectedRecordIds() const {
    QStringList ids;
    QModelIndexList selection = m_tableView->selectionModel()->selectedRows();
    for (const auto& index : selection) {
        QString recordId = m_model->item(index.row(), 0)->data(Qt::UserRole).toString();
        if (!recordId.isEmpty()) {
            ids.append(recordId);
        }
    }
    return ids;
}

int RecordTableWidget::recordCount() const {
    return m_model->rowCount();
}

}  // namespace CampusCard
