/**
 * @file Record.h
 * @brief 上机记录实体类定义
 * @author CampusCardSystem
 * @date 2024
 *
 * MVC架构 - Model层实体类
 * 上机记录实体是纯数据对象，存储单次上机的详细信息
 */

#ifndef MODEL_ENTITIES_RECORD_H
#define MODEL_ENTITIES_RECORD_H

#include "model/Types.h"

#include <QDateTime>
#include <QJsonObject>
#include <QString>


namespace CampusCard {

/**
 * @class Record
 * @brief 上机记录实体类
 *
 * 这是一个纯数据对象(Entity)，存储单次上机的详细信息：
 * - 记录ID、卡号、日期
 * - 开始时间、结束时间
 * - 时长、费用、状态、地点
 *
 * 注意：业务逻辑（如开始/结束会话、费用计算）应在Service层处理
 */
class Record {
public:
    /**
     * @brief 默认构造函数
     */
    Record() = default;

    /**
     * @brief 拷贝构造函数
     */
    Record(const Record& other) = default;

    /**
     * @brief 移动构造函数
     */
    Record(Record&& other) noexcept = default;

    /**
     * @brief 拷贝赋值运算符
     */
    Record& operator=(const Record& other) = default;

    /**
     * @brief 移动赋值运算符
     */
    Record& operator=(Record&& other) noexcept = default;

    /**
     * @brief 析构函数
     */
    ~Record() = default;

    // ========== 序列化方法 ==========

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

    // ========== Setters ==========

    /**
     * @brief 设置记录ID
     * @param recordId 记录ID
     */
    void setRecordId(const QString& recordId) { m_recordId = recordId; }

    /**
     * @brief 设置卡号
     * @param cardId 卡号
     */
    void setCardId(const QString& cardId) { m_cardId = cardId; }

    /**
     * @brief 设置日期
     * @param date 日期字符串
     */
    void setDate(const QString& date) { m_date = date; }

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
    void setDurationMinutes(int minutes) { m_durationMinutes = minutes; }

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

    /**
     * @brief 设置地点
     * @param location 地点
     */
    void setLocation(const QString& location) { m_location = location; }

    // ========== 状态检查方法 ==========

    /**
     * @brief 判断是否在上机中
     * @return 是否在线
     */
    [[nodiscard]] bool isOnline() const { return m_state == SessionState::Online; }

    /**
     * @brief 判断是否已结束
     * @return 是否离线
     */
    [[nodiscard]] bool isOffline() const { return m_state == SessionState::Offline; }

    /**
     * @brief 判断记录是否有效（有记录ID）
     * @return 是否有效
     */
    [[nodiscard]] bool isValid() const { return !m_recordId.isEmpty(); }

private:
    QString m_recordId;                            ///< 记录唯一ID（UUID）
    QString m_cardId;                              ///< 关联卡号
    QString m_date;                                ///< 上机日期（yyyy-MM-dd）
    QDateTime m_startTime;                         ///< 开始时间
    QDateTime m_endTime;                           ///< 结束时间
    int m_durationMinutes = 0;                     ///< 上机时长（分钟）
    double m_cost = 0.0;                           ///< 上机费用
    SessionState m_state = SessionState::Offline;  ///< 上机状态
    QString m_location;                            ///< 上机地点
};

}  // namespace CampusCard

#endif  // MODEL_ENTITIES_RECORD_H
