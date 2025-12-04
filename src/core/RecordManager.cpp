/**
 * @file RecordManager.cpp
 * @brief 上机记录管理器实现
 * @author CampusCardSystem
 * @date 2024
 */

#include "RecordManager.h"

#include <QDateTime>


namespace CampusCard {

RecordManager::RecordManager(QObject* parent) : QObject(parent) {}

void RecordManager::initialize() {
    // 加载所有卡的记录
    m_records = StorageManager::instance().loadAllRecords();
    m_activeSessions.clear();

    // 检查是否有未结束的会话
    for (auto it = m_records.begin(); it != m_records.end(); ++it) {
        for (const auto& record : it.value()) {
            if (record.isOnline()) {
                m_activeSessions[it.key()] = record.recordId();
            }
        }
    }
}

void RecordManager::loadRecordsForCard(const QString& cardId) {
    m_records[cardId] = StorageManager::instance().loadRecords(cardId);
}

void RecordManager::saveRecordsForCard(const QString& cardId) {
    if (m_records.contains(cardId)) {
        StorageManager::instance().saveRecords(cardId, m_records[cardId]);
    }
}

Record RecordManager::startSession(const QString& cardId, const QString& location) {
    // 检查是否已在上机
    if (isOnline(cardId)) {
        return Record();  // 返回空记录表示失败
    }

    // 创建新记录
    Record newRecord = Record::createNew(cardId, location);

    // 添加到记录列表
    if (!m_records.contains(cardId)) {
        m_records[cardId] = QList<Record>();
    }
    m_records[cardId].append(newRecord);

    // 设置活动会话记录ID
    m_activeSessions[cardId] = newRecord.recordId();

    // 保存并发出信号
    saveRecordsForCard(cardId);
    emit sessionStarted(cardId, location);
    emit recordsChanged(cardId);

    return newRecord;
}

double RecordManager::endSession(const QString& cardId) {
    // 检查是否在上机
    if (!isOnline(cardId)) {
        return -1.0;
    }

    // 获取当前会话记录ID
    QString recordId = m_activeSessions[cardId];
    if (recordId.isEmpty()) {
        return -1.0;
    }

    // 查找并结束会话
    double cost = -1.0;
    for (auto& record : m_records[cardId]) {
        if (record.recordId() == recordId) {
            cost = record.endSession();
            break;
        }
    }

    if (cost < 0) {
        return -1.0;
    }

    // 清除活动会话
    m_activeSessions.remove(cardId);

    // 保存并发出信号
    saveRecordsForCard(cardId);
    emit sessionEnded(cardId, cost);
    emit recordsChanged(cardId);

    return cost;
}

bool RecordManager::isOnline(const QString& cardId) const {
    return m_activeSessions.contains(cardId) && !m_activeSessions[cardId].isEmpty();
}

Record* RecordManager::getCurrentSession(const QString& cardId) {
    if (!m_activeSessions.contains(cardId)) {
        return nullptr;
    }

    QString recordId = m_activeSessions[cardId];
    if (recordId.isEmpty()) {
        return nullptr;
    }

    // 查找记录
    for (auto& record : m_records[cardId]) {
        if (record.recordId() == recordId) {
            return &record;
        }
    }
    return nullptr;
}

QList<Record> RecordManager::getRecords(const QString& cardId) const {
    if (m_records.contains(cardId)) {
        return m_records[cardId];
    }
    return QList<Record>();
}

QList<Record> RecordManager::getRecordsByDate(const QString& cardId, const QString& date) const {
    QList<Record> result;
    if (!m_records.contains(cardId)) {
        return result;
    }

    for (const auto& record : m_records[cardId]) {
        if (record.date() == date) {
            result.append(record);
        }
    }
    return result;
}

int RecordManager::getTotalDuration(const QString& cardId) const {
    int total = 0;
    if (!m_records.contains(cardId)) {
        return total;
    }

    for (const auto& record : m_records[cardId]) {
        total += record.durationMinutes();
    }
    return total;
}

double RecordManager::getTotalCost(const QString& cardId) const {
    double total = 0.0;
    if (!m_records.contains(cardId)) {
        return total;
    }

    for (const auto& record : m_records[cardId]) {
        total += record.cost();
    }
    return total;
}

double RecordManager::getDailyIncome(const QString& date) const {
    double total = 0.0;

    for (auto it = m_records.constBegin(); it != m_records.constEnd(); ++it) {
        for (const auto& record : it.value()) {
            if (record.date() == date && !record.isOnline()) {
                total += record.cost();
            }
        }
    }
    return total;
}

QList<Record> RecordManager::getAllRecordsByDate(const QString& date) const {
    QList<Record> result;

    for (auto it = m_records.constBegin(); it != m_records.constEnd(); ++it) {
        for (const auto& record : it.value()) {
            if (record.date() == date) {
                result.append(record);
            }
        }
    }
    return result;
}

QString RecordManager::getStatisticsSummary(const QString& cardId) const {
    if (!m_records.contains(cardId)) {
        return QStringLiteral("暂无上机记录");
    }

    int totalDuration = getTotalDuration(cardId);
    double totalCost = getTotalCost(cardId);
    int sessionCount = m_records[cardId].size();

    int hours = totalDuration / 60;
    int minutes = totalDuration % 60;

    return QStringLiteral("总计上机 %1 次，时长 %2 小时 %3 分钟，费用 %4 元")
        .arg(sessionCount)
        .arg(hours)
        .arg(minutes)
        .arg(totalCost, 0, 'f', 2);
}

}  // namespace CampusCard
