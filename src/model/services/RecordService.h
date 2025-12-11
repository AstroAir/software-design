/**
 * @file RecordService.h
 * @brief 上机记录业务服务类
 * @author CampusCardSystem
 * @date 2024
 *
 * MVC架构 - Model层业务服务
 * 负责上机记录相关的业务逻辑处理
 */

#ifndef MODEL_SERVICES_RECORDSERVICE_H
#define MODEL_SERVICES_RECORDSERVICE_H

#include "model/entities/Card.h"
#include "model/entities/Record.h"
#include "model/repositories/StorageManager.h"

#include <QList>
#include <QMap>
#include <QObject>


namespace CampusCard {

/**
 * @class RecordService
 * @brief 上机记录业务服务类，处理上下机和记录统计
 *
 * 作为Service层，负责：
 * - 上机/下机会话管理
 * - 费用计算
 * - 记录查询和统计
 * - 通过信号通知状态变更
 */
class RecordService : public QObject {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父对象
     */
    explicit RecordService(QObject* parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~RecordService() override = default;

    /**
     * @brief 初始化，加载所有记录
     */
    void initialize();

    /**
     * @brief 注册新卡的学号映射（创建新卡时调用）
     * @param cardId 卡号
     * @param studentId 学号
     */
    void registerCardStudentMapping(const QString& cardId, const QString& studentId);

    // ========== 上下机操作 ==========

    /**
     * @brief 开始上机
     * @param cardId 卡号
     * @param location 上机地点
     * @return 新创建的记录（失败返回无效Record）
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
     * @return 当前记录（未上机返回无效Record）
     */
    [[nodiscard]] Record getCurrentSession(const QString& cardId) const;

    /**
     * @brief 获取当前上机记录指针（用于修改）
     * @param cardId 卡号
     * @return 记录指针（未上机返回nullptr）
     */
    Record* getCurrentSessionPtr(const QString& cardId);

    /**
     * @brief 计算当前会话费用（不结束会话）
     * @param cardId 卡号
     * @return 截至当前的费用（-1表示未上机）
     */
    [[nodiscard]] double calculateCurrentCost(const QString& cardId) const;

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

    /**
     * @brief 获取日期范围内的记录
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
     * @brief 获取所有卡的当日上机记录
     * @param date 日期
     * @return 记录列表
     */
    [[nodiscard]] QList<Record> getAllRecordsByDate(const QString& date) const;

    /**
     * @brief 获取指定卡的所有上机地点
     * @param cardId 卡号
     * @return 地点列表（去重）
     */
    [[nodiscard]] QStringList getLocations(const QString& cardId) const;

    // ========== 统计功能 ==========

    /**
     * @brief 统计某卡的总上机次数（已完成的）
     * @param cardId 卡号
     * @return 总次数
     */
    [[nodiscard]] int getTotalSessionCount(const QString& cardId) const;

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
     * @brief 统计某日期的上机次数
     * @param date 日期字符串
     * @return 当日上机次数
     */
    [[nodiscard]] int getDailySessionCount(const QString& date) const;

    /**
     * @brief 统计某日期的总时长
     * @param date 日期字符串
     * @return 当日总时长（分钟）
     */
    [[nodiscard]] int getDailyTotalDuration(const QString& date) const;

    /**
     * @brief 获取统计摘要
     * @param cardId 卡号
     * @return 包含总时长、总费用、总次数的字符串
     */
    [[nodiscard]] QString getStatisticsSummary(const QString& cardId) const;

    /**
     * @brief 获取当前在线人数
     * @return 在线人数
     */
    [[nodiscard]] int getOnlineCount() const;

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
     * @param duration 时长（分钟）
     */
    void sessionEnded(const QString& cardId, double cost, int duration);

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

    /**
     * @brief 计算费用
     * @param durationMinutes 时长（分钟）
     * @return 费用
     */
    [[nodiscard]] double calculateCost(int durationMinutes) const;

    /**
     * @brief 根据卡号获取学号
     * @param cardId 卡号
     * @return 学号（用于记录文件命名）
     */
    [[nodiscard]] QString getStudentIdByCardId(const QString& cardId) const;

    QMap<QString, QList<Record>> m_records;   ///< 卡号到记录列表的映射（内存缓存仍用卡号索引）
    QMap<QString, QString> m_activeSessions;  ///< 卡号到当前活动会话记录ID的映射
    QMap<QString, QString> m_cardToStudentId; ///< 卡号到学号的映射（用于文件命名）
};

}  // namespace CampusCard

#endif  // MODEL_SERVICES_RECORDSERVICE_H
