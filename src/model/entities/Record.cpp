/**
 * @file Record.cpp
 * @brief 上机记录实体类实现
 * @author CampusCardSystem
 * @date 2024
 *
 * MVC架构 - Model层实体类实现
 */

#include "Record.h"


namespace CampusCard {

Record Record::fromJson(const QJsonObject& json) {
    Record record;
    record.m_recordId = json[QStringLiteral("recordId")].toString();
    record.m_cardId = json[QStringLiteral("cardId")].toString();
    record.m_date = json[QStringLiteral("date")].toString();
    record.m_startTime =
        QDateTime::fromString(json[QStringLiteral("startTime")].toString(), Qt::ISODate);
    record.m_endTime =
        QDateTime::fromString(json[QStringLiteral("endTime")].toString(), Qt::ISODate);
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

}  // namespace CampusCard
