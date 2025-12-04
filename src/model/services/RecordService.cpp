/**
 * @file RecordService.cpp
 * @brief 上机记录业务服务类实现
 * @author CampusCardSystem
 * @date 2024
 *
 * MVC架构 - Model层业务服务实现
 */

#include "RecordService.h"

#include <QDateTime>
#include <QSet>
#include <QUuid>


namespace CampusCard {

RecordService::RecordService(QObject* parent) : QObject(parent) {}

void RecordService::initialize() {
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

void RecordService::loadRecordsForCard(const QString& cardId) {
    m_records[cardId] = StorageManager::instance().loadRecords(cardId);
}

void RecordService::saveRecordsForCard(const QString& cardId) {
    if (m_records.contains(cardId)) {
        StorageManager::instance().saveRecords(cardId, m_records[cardId]);
    }
}

double RecordService::calculateCost(int durationMinutes) const {
    return (durationMinutes / 60.0) * COST_PER_HOUR;
}

// ========== 上下机操作 ==========

Record RecordService::startSession(const QString& cardId, const QString& location) {
    // 检查是否已在上机
    if (isOnline(cardId)) {
        return Record();  // 返回无效记录表示失败
    }

    // 创建新记录
    Record newRecord;
    newRecord.setRecordId(QUuid::createUuid().toString(QUuid::WithoutBraces));
    newRecord.setCardId(cardId);
    newRecord.setLocation(location);
    newRecord.setStartTime(QDateTime::currentDateTime());
    newRecord.setState(SessionState::Online);
    newRecord.setDurationMinutes(0);
    newRecord.setCost(0.0);

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

double RecordService::endSession(const QString& cardId) {
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
    int duration = 0;
    for (auto& record : m_records[cardId]) {
        if (record.recordId() == recordId) {
            // 计算时长和费用
            QDateTime endTime = QDateTime::currentDateTime();
            qint64 secs = record.startTime().secsTo(endTime);
            duration = static_cast<int>((secs + 59) / 60);  // 向上取整到分钟
            cost = calculateCost(duration);

            // 更新记录
            record.setEndTime(endTime);
            record.setDurationMinutes(duration);
            record.setCost(cost);
            record.setState(SessionState::Offline);
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
    emit sessionEnded(cardId, cost, duration);
    emit recordsChanged(cardId);

    return cost;
}

bool RecordService::isOnline(const QString& cardId) const {
    return m_activeSessions.contains(cardId) && !m_activeSessions[cardId].isEmpty();
}

Record RecordService::getCurrentSession(const QString& cardId) const {
    if (!m_activeSessions.contains(cardId)) {
        return Record();
    }

    QString recordId = m_activeSessions[cardId];
    if (recordId.isEmpty()) {
        return Record();
    }

    // 查找记录
    if (m_records.contains(cardId)) {
        for (const auto& record : m_records[cardId]) {
            if (record.recordId() == recordId) {
                return record;
            }
        }
    }
    return Record();
}

Record* RecordService::getCurrentSessionPtr(const QString& cardId) {
    if (!m_activeSessions.contains(cardId)) {
        return nullptr;
    }

    QString recordId = m_activeSessions[cardId];
    if (recordId.isEmpty()) {
        return nullptr;
    }

    // 查找记录
    if (m_records.contains(cardId)) {
        for (auto& record : m_records[cardId]) {
            if (record.recordId() == recordId) {
                return &record;
            }
        }
    }
    return nullptr;
}

double RecordService::calculateCurrentCost(const QString& cardId) const {
    Record session = getCurrentSession(cardId);
    if (!session.isValid() || !session.isOnline()) {
        return -1.0;
    }

    // 计算截至当前的时长和费用
    QDateTime now = QDateTime::currentDateTime();
    qint64 secs = session.startTime().secsTo(now);
    int minutes = static_cast<int>((secs + 59) / 60);
    return calculateCost(minutes);
}

// ========== 记录查询 ==========

QList<Record> RecordService::getRecords(const QString& cardId) const {
    if (m_records.contains(cardId)) {
        return m_records[cardId];
    }
    return QList<Record>();
}

QList<Record> RecordService::getRecordsByDate(const QString& cardId, const QString& date) const {
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

QList<Record> RecordService::getRecordsByDateRange(const QString& cardId, const QString& startDate,
                                                    const QString& endDate) const {
    QList<Record> result;
    if (!m_records.contains(cardId)) {
        return result;
    }

    QDate start = QDate::fromString(startDate, QStringLiteral("yyyy-MM-dd"));
    QDate end = QDate::fromString(endDate, QStringLiteral("yyyy-MM-dd"));

    for (const auto& record : m_records[cardId]) {
        QDate recordDate = QDate::fromString(record.date(), QStringLiteral("yyyy-MM-dd"));
        if (recordDate >= start && recordDate <= end) {
            result.append(record);
        }
    }
    return result;
}

QList<Record> RecordService::getRecordsByLocation(const QString& cardId,
                                                   const QString& location) const {
    QList<Record> result;
    if (!m_records.contains(cardId)) {
        return result;
    }

    for (const auto& record : m_records[cardId]) {
        if (record.location() == location) {
            result.append(record);
        }
    }
    return result;
}

QList<Record> RecordService::getAllRecordsByDate(const QString& date) const {
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

QStringList RecordService::getLocations(const QString& cardId) const {
    QSet<QString> locations;
    if (m_records.contains(cardId)) {
        for (const auto& record : m_records[cardId]) {
            if (!record.location().isEmpty()) {
                locations.insert(record.location());
            }
        }
    }
    return locations.values();
}

// ========== 统计功能 ==========

int RecordService::getTotalSessionCount(const QString& cardId) const {
    int count = 0;
    if (m_records.contains(cardId)) {
        for (const auto& record : m_records[cardId]) {
            if (record.isOffline()) {
                count++;
            }
        }
    }
    return count;
}

int RecordService::getTotalDuration(const QString& cardId) const {
    int total = 0;
    if (!m_records.contains(cardId)) {
        return total;
    }

    for (const auto& record : m_records[cardId]) {
        total += record.durationMinutes();
    }
    return total;
}

double RecordService::getTotalCost(const QString& cardId) const {
    double total = 0.0;
    if (!m_records.contains(cardId)) {
        return total;
    }

    for (const auto& record : m_records[cardId]) {
        total += record.cost();
    }
    return total;
}

double RecordService::getDailyIncome(const QString& date) const {
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

int RecordService::getDailySessionCount(const QString& date) const {
    int count = 0;

    for (auto it = m_records.constBegin(); it != m_records.constEnd(); ++it) {
        for (const auto& record : it.value()) {
            if (record.date() == date) {
                count++;
            }
        }
    }
    return count;
}

int RecordService::getDailyTotalDuration(const QString& date) const {
    int total = 0;

    for (auto it = m_records.constBegin(); it != m_records.constEnd(); ++it) {
        for (const auto& record : it.value()) {
            if (record.date() == date && !record.isOnline()) {
                total += record.durationMinutes();
            }
        }
    }
    return total;
}

QString RecordService::getStatisticsSummary(const QString& cardId) const {
    if (!m_records.contains(cardId)) {
        return QStringLiteral("暂无上机记录");
    }

    int totalDuration = getTotalDuration(cardId);
    double totalCost = getTotalCost(cardId);
    int sessionCount = getTotalSessionCount(cardId);

    int hours = totalDuration / 60;
    int minutes = totalDuration % 60;

    return QStringLiteral("总计上机 %1 次，时长 %2 小时 %3 分钟，费用 %4 元")
        .arg(sessionCount)
        .arg(hours)
        .arg(minutes)
        .arg(totalCost, 0, 'f', 2);
}

int RecordService::getOnlineCount() const {
    return m_activeSessions.size();
}

}  // namespace CampusCard
