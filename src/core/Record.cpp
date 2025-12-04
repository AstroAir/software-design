/**
 * @file Record.cpp
 * @brief 上机记录实体类实现
 * @author CampusCardSystem
 * @date 2024
 */

#include "Record.h"
#include <QUuid>

namespace CampusCard {

Record Record::createNew(const QString& cardId, const QString& location) {
    Record record;
    // 生成唯一ID
    record.m_recordId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    record.m_cardId = cardId;
    record.m_location = location;
    record.m_startTime = QDateTime::currentDateTime();
    record.m_date = record.m_startTime.toString(QStringLiteral("yyyy-MM-dd"));
    record.m_state = SessionState::Online;
    record.m_durationMinutes = 0;
    record.m_cost = 0.0;
    return record;
}

Record Record::fromJson(const QJsonObject& json) {
    Record record;
    record.m_recordId = json[QStringLiteral("recordId")].toString();
    record.m_cardId = json[QStringLiteral("cardId")].toString();
    record.m_date = json[QStringLiteral("date")].toString();
    record.m_startTime = QDateTime::fromString(
        json[QStringLiteral("startTime")].toString(), Qt::ISODate);
    record.m_endTime = QDateTime::fromString(
        json[QStringLiteral("endTime")].toString(), Qt::ISODate);
    record.m_durationMinutes = json[QStringLiteral("durationMinutes")].toInt();
    record.m_cost = json[QStringLiteral("cost")].toDouble();
    record.m_state = static_cast<SessionState>(json[QStringLiteral("state")].toInt());
    record.m_location = json[QStringLiteral("location")].toString();
    return record;
}

QJsonObject Record::toJson() const {
    QJsonObject json;
    json[QStringLiteral("recordId")] = m_recordId;
    json[QStringLiteral("cardId")] = m_cardId;
    json[QStringLiteral("date")] = m_date;
    json[QStringLiteral("startTime")] = m_startTime.toString(Qt::ISODate);
    json[QStringLiteral("endTime")] = m_endTime.toString(Qt::ISODate);
    json[QStringLiteral("durationMinutes")] = m_durationMinutes;
    json[QStringLiteral("cost")] = m_cost;
    json[QStringLiteral("state")] = static_cast<int>(m_state);
    json[QStringLiteral("location")] = m_location;
    return json;
}

double Record::endSession() {
    // 仅当在线状态才能结束
    if (m_state != SessionState::Online) {
        return 0.0;
    }
    
    m_endTime = QDateTime::currentDateTime();
    // 计算时长（分钟），向上取整
    qint64 secs = m_startTime.secsTo(m_endTime);
    m_durationMinutes = static_cast<int>((secs + 59) / 60);  // 向上取整到分钟
    
    // 计算费用：每小时 COST_PER_HOUR 元，按分钟计费
    m_cost = (m_durationMinutes / 60.0) * COST_PER_HOUR;
    
    m_state = SessionState::Offline;
    return m_cost;
}

double Record::calculateCurrentCost() const {
    if (m_state == SessionState::Offline) {
        return m_cost;  // 已结束，返回已计算费用
    }
    
    // 计算截至当前的时长和费用
    QDateTime now = QDateTime::currentDateTime();
    qint64 secs = m_startTime.secsTo(now);
    int minutes = static_cast<int>((secs + 59) / 60);
    return (minutes / 60.0) * COST_PER_HOUR;
}

} // namespace CampusCard
