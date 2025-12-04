/**
 * @file RecordController.cpp
 * @brief 上机记录控制器实现
 * @author CampusCardSystem
 * @date 2024
 *
 * MVC架构 - Controller层实现
 */

#include "RecordController.h"

#include <QDate>

namespace CampusCard {

RecordController::RecordController(RecordService* recordService, CardService* cardService,
                                   QObject* parent)
    : QObject(parent), m_recordService(recordService), m_cardService(cardService) {

    // 连接RecordService的信号，转发给View
    connect(m_recordService, &RecordService::recordsChanged, this,
            &RecordController::recordsUpdated);
}

// ========== 上下机操作 ==========

void RecordController::handleStartSession(const QString& cardId, const QString& location) {
    // 检查卡是否存在
    Card card = m_cardService->findCard(cardId);
    if (card.cardId().isEmpty()) {
        emit sessionStartFailed(QStringLiteral("卡不存在"));
        return;
    }

    // 检查卡是否可用
    if (!card.isUsable()) {
        emit sessionStartFailed(QStringLiteral("卡片状态异常，无法上机"));
        return;
    }

    // 检查余额
    if (card.balance() <= 0) {
        emit sessionStartFailed(QStringLiteral("余额不足，请先充值"));
        return;
    }

    // 检查是否已在上机
    if (m_recordService->isOnline(cardId)) {
        emit sessionStartFailed(QStringLiteral("当前已在上机中"));
        return;
    }

    // 开始上机
    Record record = m_recordService->startSession(cardId, location);
    if (record.isValid()) {
        emit sessionStarted(cardId, location);
    } else {
        emit sessionStartFailed(QStringLiteral("开始上机失败"));
    }
}

void RecordController::handleEndSession(const QString& cardId) {
    // 检查是否在上机
    if (!m_recordService->isOnline(cardId)) {
        emit sessionEndFailed(QStringLiteral("当前未在上机中"));
        return;
    }

    // 结束上机
    double cost = m_recordService->endSession(cardId);
    if (cost < 0) {
        emit sessionEndFailed(QStringLiteral("结束上机失败"));
        return;
    }

    // 获取时长
    int duration = 0;
    QList<Record> records = m_recordService->getRecords(cardId);
    if (!records.isEmpty()) {
        // 最后一条记录就是刚结束的
        for (int i = records.size() - 1; i >= 0; --i) {
            if (records[i].isOffline()) {
                duration = records[i].durationMinutes();
                break;
            }
        }
    }

    // 扣款
    if (cost > 0) {
        m_cardService->deduct(cardId, cost);
    }

    emit sessionEnded(cardId, cost, duration);
}

bool RecordController::isOnline(const QString& cardId) const {
    return m_recordService->isOnline(cardId);
}

Record RecordController::getCurrentSession(const QString& cardId) const {
    return m_recordService->getCurrentSession(cardId);
}

double RecordController::getCurrentCost(const QString& cardId) const {
    return m_recordService->calculateCurrentCost(cardId);
}

// ========== 记录查询 ==========

QList<Record> RecordController::getRecords(const QString& cardId) const {
    return m_recordService->getRecords(cardId);
}

QList<Record> RecordController::getRecordsByDateRange(const QString& cardId,
                                                       const QString& startDate,
                                                       const QString& endDate) const {
    return m_recordService->getRecordsByDateRange(cardId, startDate, endDate);
}

QList<Record> RecordController::getRecordsByLocation(const QString& cardId,
                                                      const QString& location) const {
    return m_recordService->getRecordsByLocation(cardId, location);
}

QList<Record> RecordController::getFilteredRecords(const QString& cardId, const QString& startDate,
                                                    const QString& endDate,
                                                    const QString& location) const {
    QList<Record> records = m_recordService->getRecords(cardId);
    QList<Record> result;

    QDate start = QDate::fromString(startDate, QStringLiteral("yyyy-MM-dd"));
    QDate end = QDate::fromString(endDate, QStringLiteral("yyyy-MM-dd"));

    for (const auto& record : records) {
        // 日期筛选
        QDate recordDate = QDate::fromString(record.date(), QStringLiteral("yyyy-MM-dd"));
        if (recordDate < start || recordDate > end) {
            continue;
        }

        // 地点筛选
        if (!location.isEmpty() && record.location() != location) {
            continue;
        }

        result.append(record);
    }

    return result;
}

QStringList RecordController::getLocations(const QString& cardId) const {
    return m_recordService->getLocations(cardId);
}

QList<Record> RecordController::getAllRecordsByDate(const QString& date) const {
    return m_recordService->getAllRecordsByDate(date);
}

// ========== 统计查询 ==========

int RecordController::getTotalSessionCount(const QString& cardId) const {
    return m_recordService->getTotalSessionCount(cardId);
}

int RecordController::getTotalDuration(const QString& cardId) const {
    return m_recordService->getTotalDuration(cardId);
}

double RecordController::getTotalCost(const QString& cardId) const {
    return m_recordService->getTotalCost(cardId);
}

double RecordController::getDailyIncome(const QString& date) const {
    return m_recordService->getDailyIncome(date);
}

int RecordController::getDailySessionCount(const QString& date) const {
    return m_recordService->getDailySessionCount(date);
}

int RecordController::getDailyTotalDuration(const QString& date) const {
    return m_recordService->getDailyTotalDuration(date);
}

QString RecordController::getStatisticsSummary(const QString& cardId) const {
    return m_recordService->getStatisticsSummary(cardId);
}

int RecordController::getOnlineCount() const {
    return m_recordService->getOnlineCount();
}

}  // namespace CampusCard
