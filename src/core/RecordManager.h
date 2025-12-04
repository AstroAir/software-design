/**
 * @file RecordManager.h
 * @brief 上机记录管理器
 * @author CampusCardSystem
 * @date 2024
 */

#ifndef RECORDMANAGER_H
#define RECORDMANAGER_H

#include "Record.h"
#include "StorageManager.h"

#include <QList>
#include <QMap>
#include <QObject>


namespace CampusCard {

/**
 * @class RecordManager
 * @brief 上机记录管理器，处理上下机和记录统计
 */
class RecordManager : public QObject {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父对象
     */
    explicit RecordManager(QObject* parent = nullptr);

    /**
     * @brief 初始化，加载所有记录
     */
    void initialize();

    // ========== 上下机操作 ==========

    /**
     * @brief 开始上机
     * @param cardId 卡号
     * @param location 上机地点
     * @return 新创建的记录（失败返回空Record）
     */
    Record startSession(const QString& cardId, const QString& location);

    /**
     * @brief 结束上机
     * @param cardId 卡号
     * @return 本次上机费用（失败返回-1）
     */
    double endSession(const QString& cardId);

    /**
     * @brief 检查是否正在上机
     * @param cardId 卡号
     * @return 是否上机中
     */
    [[nodiscard]] bool isOnline(const QString& cardId) const;

    /**
     * @brief 获取当前上机记录
     * @param cardId 卡号
     * @return 当前记录指针（未上机返回nullptr）
     */
    [[nodiscard]] Record* getCurrentSession(const QString& cardId);

    // ========== 记录查询 ==========

    /**
     * @brief 获取指定卡的所有记录
     * @param cardId 卡号
     * @return 记录列表
     */
    [[nodiscard]] QList<Record> getRecords(const QString& cardId) const;

    /**
     * @brief 获取指定日期的记录
     * @param cardId 卡号
     * @param date 日期字符串（yyyy-MM-dd）
     * @return 记录列表
     */
    [[nodiscard]] QList<Record> getRecordsByDate(const QString& cardId, const QString& date) const;

    // ========== 统计功能 ==========

    /**
     * @brief 统计某卡的总上机时长（分钟）
     * @param cardId 卡号
     * @return 总时长
     */
    [[nodiscard]] int getTotalDuration(const QString& cardId) const;

    /**
     * @brief 统计某卡的总费用
     * @param cardId 卡号
     * @return 总费用
     */
    [[nodiscard]] double getTotalCost(const QString& cardId) const;

    /**
     * @brief 统计某日期的总收入（管理员功能）
     * @param date 日期字符串（yyyy-MM-dd）
     * @return 当日总收入
     */
    [[nodiscard]] double getDailyIncome(const QString& date) const;

    /**
     * @brief 获取所有卡的当日上机记录
     * @param date 日期
     * @return 记录列表
     */
    [[nodiscard]] QList<Record> getAllRecordsByDate(const QString& date) const;

    /**
     * @brief 获取统计摘要
     * @param cardId 卡号
     * @return 包含总时长、总费用、总次数的字符串
     */
    [[nodiscard]] QString getStatisticsSummary(const QString& cardId) const;

signals:
    /**
     * @brief 记录变更信号
     * @param cardId 变更的卡号
     */
    void recordsChanged(const QString& cardId);

    /**
     * @brief 上机开始信号
     * @param cardId 卡号
     * @param location 地点
     */
    void sessionStarted(const QString& cardId, const QString& location);

    /**
     * @brief 上机结束信号
     * @param cardId 卡号
     * @param cost 费用
     */
    void sessionEnded(const QString& cardId, double cost);

private:
    /**
     * @brief 加载指定卡的记录到缓存
     * @param cardId 卡号
     */
    void loadRecordsForCard(const QString& cardId);

    /**
     * @brief 保存指定卡的记录
     * @param cardId 卡号
     */
    void saveRecordsForCard(const QString& cardId);

    QMap<QString, QList<Record>> m_records;   ///< 卡号到记录列表的映射
    QMap<QString, QString> m_activeSessions;  ///< 卡号到当前活动会话记录ID的映射
};

}  // namespace CampusCard

#endif  // RECORDMANAGER_H
