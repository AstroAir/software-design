/**
 * @file StatisticsWidget.cpp
 * @brief 统计报表组件实现
 * @author CampusCardSystem
 * @date 2024
 */

#include "StatisticsWidget.h"
#include "ElaText.h"
#include "ElaTableView.h"
#include <QDateEdit>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QDate>

namespace CampusCard {

StatisticsWidget::StatisticsWidget(RecordManager* recordManager,
                                   CardManager* cardManager,
                                   QWidget* parent)
    : QWidget(parent)
    , m_recordManager(recordManager)
    , m_cardManager(cardManager)
{
    initUI();
    refreshStatistics();
}

void StatisticsWidget::initUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // 标题
    auto* titleLabel = new ElaText(QStringLiteral("统计报表"), this);
    titleLabel->setTextPixelSize(24);
    mainLayout->addWidget(titleLabel);

    // 日期选择
    QHBoxLayout* dateLayout = new QHBoxLayout();
    auto* dateLabel = new ElaText(QStringLiteral("选择日期："), this);
    m_dateEdit = new QDateEdit(this);
    m_dateEdit->setDate(QDate::currentDate());
    m_dateEdit->setCalendarPopup(true);
    connect(m_dateEdit, &QDateEdit::dateChanged, this, &StatisticsWidget::onDateChanged);
    dateLayout->addWidget(dateLabel);
    dateLayout->addWidget(m_dateEdit);
    dateLayout->addStretch();
    mainLayout->addLayout(dateLayout);

    // 统计摘要
    QGroupBox* summaryGroup = new QGroupBox(QStringLiteral("当日统计"), this);
    QVBoxLayout* summaryLayout = new QVBoxLayout(summaryGroup);
    
    m_incomeLabel = new ElaText(QStringLiteral("当日收入：0.00 元"), summaryGroup);
    m_incomeLabel->setTextPixelSize(18);
    m_sessionCountLabel = new ElaText(QStringLiteral("上机次数：0 次"), summaryGroup);
    m_totalDurationLabel = new ElaText(QStringLiteral("总时长：0 分钟"), summaryGroup);
    
    summaryLayout->addWidget(m_incomeLabel);
    summaryLayout->addWidget(m_sessionCountLabel);
    summaryLayout->addWidget(m_totalDurationLabel);
    mainLayout->addWidget(summaryGroup);

    // 详细记录表格
    QGroupBox* detailGroup = new QGroupBox(QStringLiteral("当日明细"), this);
    QVBoxLayout* detailLayout = new QVBoxLayout(detailGroup);
    
    m_detailTable = new ElaTableView(detailGroup);
    m_model = new QStandardItemModel(this);
    m_model->setHorizontalHeaderLabels({
        QStringLiteral("卡号"),
        QStringLiteral("姓名"),
        QStringLiteral("开始时间"),
        QStringLiteral("结束时间"),
        QStringLiteral("时长(分钟)"),
        QStringLiteral("费用(元)"),
        QStringLiteral("地点")
    });
    
    m_detailTable->setModel(m_model);
    m_detailTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_detailTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_detailTable->horizontalHeader()->setStretchLastSection(true);
    m_detailTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    detailLayout->addWidget(m_detailTable);
    mainLayout->addWidget(detailGroup, 1);
}

void StatisticsWidget::onDateChanged() {
    refreshStatistics();
}

void StatisticsWidget::refreshStatistics() {
    QString date = m_dateEdit->date().toString(QStringLiteral("yyyy-MM-dd"));
    
    // 获取当日记录
    QList<Record> records = m_recordManager->getAllRecordsByDate(date);
    
    // 计算统计数据
    double totalIncome = 0.0;
    int sessionCount = 0;
    int totalDuration = 0;
    
    for (const auto& record : records) {
        if (!record.isOnline()) {
            totalIncome += record.cost();
            totalDuration += record.durationMinutes();
        }
        sessionCount++;
    }
    
    // 更新统计标签
    m_incomeLabel->setText(QStringLiteral("当日收入：") + 
                           QString::number(totalIncome, 'f', 2) + 
                           QStringLiteral(" 元"));
    m_sessionCountLabel->setText(QStringLiteral("上机次数：") + 
                                 QString::number(sessionCount) + 
                                 QStringLiteral(" 次"));
    m_totalDurationLabel->setText(QStringLiteral("总时长：") + 
                                  QString::number(totalDuration) + 
                                  QStringLiteral(" 分钟"));
    
    // 更新详细表格
    m_model->removeRows(0, m_model->rowCount());
    
    for (const auto& record : records) {
        // 获取卡信息
        Card* card = m_cardManager->findCard(record.cardId());
        QString name = card ? card->name() : QStringLiteral("未知");
        
        QList<QStandardItem*> row;
        row << new QStandardItem(record.cardId());
        row << new QStandardItem(name);
        row << new QStandardItem(record.startTime().toString(QStringLiteral("HH:mm:ss")));
        
        if (record.isOnline()) {
            row << new QStandardItem(QStringLiteral("上机中"));
            row << new QStandardItem(QStringLiteral("--"));
            row << new QStandardItem(QStringLiteral("--"));
        } else {
            row << new QStandardItem(record.endTime().toString(QStringLiteral("HH:mm:ss")));
            row << new QStandardItem(QString::number(record.durationMinutes()));
            row << new QStandardItem(QString::number(record.cost(), 'f', 2));
        }
        
        row << new QStandardItem(record.location());
        
        m_model->appendRow(row);
    }
}

} // namespace CampusCard
