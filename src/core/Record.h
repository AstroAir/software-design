/**
 * @file Record.h
 * @brief 上机记录实体类定义
 * @author CampusCardSystem
 * @date 2024
 */

#ifndef RECORD_H
#define RECORD_H

#include "Types.h"
#include <QString>
#include <QDateTime>
#include <QJsonObject>

namespace CampusCard {

/**
 * @class Record
 * @brief 上机记录实体类
 * 
 * 存储单次上机的详细信息：时间、地点、时长、费用等
 */
class Record {
public:
    /**
     * @brief 默认构造函数
     */
    Record() = default;

    /**
     * @brief 创建新的上机记录
     * @param cardId 卡号
     * @param location 上机地点
     * @return 新建的Record对象（状态为Online）
     */
    static Record createNew(const QString& cardId, const QString& location);

    /**
     * @brief 从JSON对象反序列化
     * @param json JSON对象
     * @return 解析后的Record对象
     */
    static Record fromJson(const QJsonObject& json);

    /**
     * @brief 序列化为JSON对象
     * @return JSON对象
     */
    [[nodiscard]] QJsonObject toJson() const;

    // ========== Getters ==========

    /**
     * @brief 获取记录唯一ID
     * @return 记录ID
     */
    [[nodiscard]] QString recordId() const { return m_recordId; }

    /**
     * @brief 获取关联的卡号
     * @return 卡号
     */
    [[nodiscard]] QString cardId() const { return m_cardId; }

    /**
     * @brief 获取上机日期
     * @return 日期字符串（格式：yyyy-MM-dd）
     */
    [[nodiscard]] QString date() const { return m_date; }

    /**
     * @brief 获取开始时间
     * @return 开始时间
     */
    [[nodiscard]] QDateTime startTime() const { return m_startTime; }

    /**
     * @brief 获取结束时间
     * @return 结束时间（如果还在上机中则返回无效时间）
     */
    [[nodiscard]] QDateTime endTime() const { return m_endTime; }

    /**
     * @brief 获取上机时长（分钟）
     * @return 时长分钟数
     */
    [[nodiscard]] int durationMinutes() const { return m_durationMinutes; }

    /**
     * @brief 获取上机费用
     * @return 费用金额
     */
    [[nodiscard]] double cost() const { return m_cost; }

    /**
     * @brief 获取上机状态
     * @return 状态枚举
     */
    [[nodiscard]] SessionState state() const { return m_state; }

    /**
     * @brief 获取上机地点
     * @return 地点字符串
     */
    [[nodiscard]] QString location() const { return m_location; }

    // ========== Setters（用于模拟数据生成） ==========

    /**
     * @brief 设置开始时间
     * @param time 开始时间
     */
    void setStartTime(const QDateTime& time) { 
        m_startTime = time; 
        m_date = time.date().toString(QStringLiteral("yyyy-MM-dd"));
    }

    /**
     * @brief 设置结束时间
     * @param time 结束时间
     */
    void setEndTime(const QDateTime& time) { m_endTime = time; }

    /**
     * @brief 设置时长
     * @param minutes 分钟数
     */
    void setDuration(int minutes) { m_durationMinutes = minutes; }

    /**
     * @brief 设置费用
     * @param cost 费用
     */
    void setCost(double cost) { m_cost = cost; }

    /**
     * @brief 设置状态
     * @param state 状态
     */
    void setState(SessionState state) { m_state = state; }

    // ========== 业务方法 ==========

    /**
     * @brief 结束上机会话
     * @details 计算时长和费用，更新结束时间和状态
     * @return 本次上机费用
     */
    double endSession();

    /**
     * @brief 计算当前会话费用（不结束会话）
     * @return 截至当前的费用
     */
    [[nodiscard]] double calculateCurrentCost() const;

    /**
     * @brief 判断是否在上机中
     * @return 是否在线
     */
    [[nodiscard]] bool isOnline() const { return m_state == SessionState::Online; }

private:
    QString m_recordId;             ///< 记录唯一ID（UUID）
    QString m_cardId;               ///< 关联卡号
    QString m_date;                 ///< 上机日期（yyyy-MM-dd）
    QDateTime m_startTime;          ///< 开始时间
    QDateTime m_endTime;            ///< 结束时间
    int m_durationMinutes = 0;      ///< 上机时长（分钟）
    double m_cost = 0.0;            ///< 上机费用
    SessionState m_state = SessionState::Offline;  ///< 上机状态
    QString m_location;             ///< 上机地点
};

} // namespace CampusCard

#endif // RECORD_H
