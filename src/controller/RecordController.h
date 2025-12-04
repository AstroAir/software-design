/**
 * @file RecordController.h
 * @brief 上机记录控制器
 * @author CampusCardSystem
 * @date 2024
 *
 * MVC架构 - Controller层
 * 负责协调上机记录相关的View和Model交互
 */

#ifndef CONTROLLER_RECORDCONTROLLER_H
#define CONTROLLER_RECORDCONTROLLER_H

#include "model/services/CardService.h"
#include "model/services/RecordService.h"

#include <QObject>


namespace CampusCard {

/**
 * @class RecordController
 * @brief 上机记录控制器，处理上下机和记录查询相关的用户交互
 *
 * 作为Controller层，负责：
 * - 接收View的上下机请求
 * - 调用RecordService处理业务逻辑
 * - 协调CardService进行扣款
 * - 将结果反馈给View
 */
class RecordController : public QObject {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param recordService 记录服务
     * @param cardService 卡服务
     * @param parent 父对象
     */
    explicit RecordController(RecordService* recordService, CardService* cardService,
                              QObject* parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~RecordController() override = default;

    // ========== 上下机操作 ==========

    /**
     * @brief 处理开始上机请求
     * @param cardId 卡号
     * @param location 上机地点
     */
    void handleStartSession(const QString& cardId, const QString& location);

    /**
     * @brief 处理结束上机请求
     * @param cardId 卡号
     */
    void handleEndSession(const QString& cardId);

    /**
     * @brief 检查是否正在上机
     * @param cardId 卡号
     * @return 是否上机中
     */
    [[nodiscard]] bool isOnline(const QString& cardId) const;

    /**
     * @brief 获取当前会话信息
     * @param cardId 卡号
     * @return 当前记录
     */
    [[nodiscard]] Record getCurrentSession(const QString& cardId) const;

    /**
     * @brief 获取当前会话费用
     * @param cardId 卡号
     * @return 当前费用
     */
    [[nodiscard]] double getCurrentCost(const QString& cardId) const;

    // ========== 记录查询 ==========

    /**
     * @brief 获取指定卡的所有记录
     * @param cardId 卡号
     * @return 记录列表
     */
    [[nodiscard]] QList<Record> getRecords(const QString& cardId) const;

    /**
     * @brief 获取指定日期范围的记录
     * @param cardId 卡号
     * @param startDate 开始日期
     * @param endDate 结束日期
     * @return 记录列表
     */
    [[nodiscard]] QList<Record> getRecordsByDateRange(const QString& cardId,
                                                       const QString& startDate,
                                                       const QString& endDate) const;

    /**
     * @brief 获取指定地点的记录
     * @param cardId 卡号
     * @param location 地点
     * @return 记录列表
     */
    [[nodiscard]] QList<Record> getRecordsByLocation(const QString& cardId,
                                                      const QString& location) const;

    /**
     * @brief 获取筛选后的记录
     * @param cardId 卡号
     * @param startDate 开始日期
     * @param endDate 结束日期
     * @param location 地点（空表示不筛选）
     * @return 记录列表
     */
    [[nodiscard]] QList<Record> getFilteredRecords(const QString& cardId, const QString& startDate,
                                                    const QString& endDate,
                                                    const QString& location) const;

    /**
     * @brief 获取所有上机地点
     * @param cardId 卡号
     * @return 地点列表
     */
    [[nodiscard]] QStringList getLocations(const QString& cardId) const;

    /**
     * @brief 获取所有卡的指定日期记录
     * @param date 日期
     * @return 记录列表
     */
    [[nodiscard]] QList<Record> getAllRecordsByDate(const QString& date) const;

    // ========== 统计查询 ==========

    /**
     * @brief 获取总上机次数
     * @param cardId 卡号
     * @return 次数
     */
    [[nodiscard]] int getTotalSessionCount(const QString& cardId) const;

    /**
     * @brief 获取总上机时长
     * @param cardId 卡号
     * @return 时长（分钟）
     */
    [[nodiscard]] int getTotalDuration(const QString& cardId) const;

    /**
     * @brief 获取总费用
     * @param cardId 卡号
     * @return 费用
     */
    [[nodiscard]] double getTotalCost(const QString& cardId) const;

    /**
     * @brief 获取当日收入
     * @param date 日期
     * @return 收入
     */
    [[nodiscard]] double getDailyIncome(const QString& date) const;

    /**
     * @brief 获取当日上机次数
     * @param date 日期
     * @return 次数
     */
    [[nodiscard]] int getDailySessionCount(const QString& date) const;

    /**
     * @brief 获取当日总时长
     * @param date 日期
     * @return 时长（分钟）
     */
    [[nodiscard]] int getDailyTotalDuration(const QString& date) const;

    /**
     * @brief 获取统计摘要
     * @param cardId 卡号
     * @return 摘要字符串
     */
    [[nodiscard]] QString getStatisticsSummary(const QString& cardId) const;

    /**
     * @brief 获取当前在线人数
     * @return 在线人数
     */
    [[nodiscard]] int getOnlineCount() const;

signals:
    // ========== 上下机结果信号 ==========

    /**
     * @brief 上机成功信号
     * @param cardId 卡号
     * @param location 地点
     */
    void sessionStarted(const QString& cardId, const QString& location);

    /**
     * @brief 上机失败信号
     * @param message 错误消息
     */
    void sessionStartFailed(const QString& message);

    /**
     * @brief 下机成功信号
     * @param cardId 卡号
     * @param cost 费用
     * @param duration 时长（分钟）
     */
    void sessionEnded(const QString& cardId, double cost, int duration);

    /**
     * @brief 下机失败信号
     * @param message 错误消息
     */
    void sessionEndFailed(const QString& message);

    /**
     * @brief 记录更新信号
     * @param cardId 卡号
     */
    void recordsUpdated(const QString& cardId);

private:
    RecordService* m_recordService;  ///< 记录服务
    CardService* m_cardService;      ///< 卡服务
};

}  // namespace CampusCard

#endif  // CONTROLLER_RECORDCONTROLLER_H
