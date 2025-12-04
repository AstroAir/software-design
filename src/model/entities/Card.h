/**
 * @file Card.h
 * @brief 校园卡实体类定义
 * @author CampusCardSystem
 * @date 2024
 *
 * MVC架构 - Model层实体类
 * 校园卡实体是纯数据对象，存储卡的基本信息和状态
 */

#ifndef MODEL_ENTITIES_CARD_H
#define MODEL_ENTITIES_CARD_H

#include "model/Types.h"

#include <QJsonObject>
#include <QString>


namespace CampusCard {

/**
 * @class Card
 * @brief 校园卡实体类，存储卡的基本信息和状态
 *
 * 这是一个纯数据对象(Entity)，包含：
 * - 卡号、姓名、学号、充值记录、余额和状态等信息
 * - Getter/Setter方法
 * - 序列化/反序列化方法
 * - 基本的状态检查方法
 *
 * 注意：业务逻辑（如充值、扣款）应在Service层处理
 */
class Card {
public:
    /**
     * @brief 默认构造函数
     */
    Card() = default;

    /**
     * @brief 带参数的构造函数
     * @param cardId 卡号（唯一标识）
     * @param name 持卡人姓名
     * @param studentId 学号
     * @param balance 初始余额，默认为0
     */
    Card(const QString& cardId, const QString& name, const QString& studentId,
         double balance = 0.0);

    /**
     * @brief 拷贝构造函数
     */
    Card(const Card& other) = default;

    /**
     * @brief 移动构造函数
     */
    Card(Card&& other) noexcept = default;

    /**
     * @brief 拷贝赋值运算符
     */
    Card& operator=(const Card& other) = default;

    /**
     * @brief 移动赋值运算符
     */
    Card& operator=(Card&& other) noexcept = default;

    /**
     * @brief 析构函数
     */
    ~Card() = default;

    // ========== 序列化方法 ==========

    /**
     * @brief 从JSON对象反序列化
     * @param json JSON对象
     * @return 解析后的Card对象
     */
    static Card fromJson(const QJsonObject& json);

    /**
     * @brief 序列化为JSON对象
     * @return JSON对象
     */
    [[nodiscard]] QJsonObject toJson() const;

    // ========== Getters ==========

    /**
     * @brief 获取卡号
     * @return 卡号字符串
     */
    [[nodiscard]] QString cardId() const { return m_cardId; }

    /**
     * @brief 获取持卡人姓名
     * @return 姓名字符串
     */
    [[nodiscard]] QString name() const { return m_name; }

    /**
     * @brief 获取学号
     * @return 学号字符串
     */
    [[nodiscard]] QString studentId() const { return m_studentId; }

    /**
     * @brief 获取累计充值金额
     * @return 充值总额
     */
    [[nodiscard]] double totalRecharge() const { return m_totalRecharge; }

    /**
     * @brief 获取当前余额
     * @return 余额
     */
    [[nodiscard]] double balance() const { return m_balance; }

    /**
     * @brief 获取卡状态
     * @return 卡状态枚举值
     */
    [[nodiscard]] CardState state() const { return m_state; }

    /**
     * @brief 获取密码错误次数
     * @return 错误次数
     */
    [[nodiscard]] int loginAttempts() const { return m_loginAttempts; }

    /**
     * @brief 获取登录密码
     * @return 密码字符串
     */
    [[nodiscard]] QString password() const { return m_password; }

    // ========== Setters ==========

    /**
     * @brief 设置卡号
     * @param cardId 卡号
     */
    void setCardId(const QString& cardId) { m_cardId = cardId; }

    /**
     * @brief 设置持卡人姓名
     * @param name 姓名
     */
    void setName(const QString& name) { m_name = name; }

    /**
     * @brief 设置学号
     * @param studentId 学号
     */
    void setStudentId(const QString& studentId) { m_studentId = studentId; }

    /**
     * @brief 设置累计充值金额
     * @param totalRecharge 充值总额
     */
    void setTotalRecharge(double totalRecharge) { m_totalRecharge = totalRecharge; }

    /**
     * @brief 设置余额
     * @param balance 余额
     */
    void setBalance(double balance) { m_balance = balance; }

    /**
     * @brief 设置密码
     * @param password 新密码
     */
    void setPassword(const QString& password) { m_password = password; }

    /**
     * @brief 设置卡状态
     * @param state 新状态
     */
    void setState(CardState state) { m_state = state; }

    /**
     * @brief 设置登录失败次数
     * @param attempts 失败次数
     */
    void setLoginAttempts(int attempts) { m_loginAttempts = attempts; }

    // ========== 状态检查方法 ==========

    /**
     * @brief 检查卡是否可用（非挂失、非冻结）
     * @return 是否可用
     */
    [[nodiscard]] bool isUsable() const { return m_state == CardState::Normal; }

    /**
     * @brief 检查卡是否正常状态
     * @return 是否正常
     */
    [[nodiscard]] bool isNormal() const { return m_state == CardState::Normal; }

    /**
     * @brief 检查卡是否挂失状态
     * @return 是否挂失
     */
    [[nodiscard]] bool isLost() const { return m_state == CardState::Lost; }

    /**
     * @brief 检查卡是否冻结状态
     * @return 是否冻结
     */
    [[nodiscard]] bool isFrozen() const { return m_state == CardState::Frozen; }

    /**
     * @brief 检查是否达到最大登录失败次数
     * @return 是否达到上限
     */
    [[nodiscard]] bool hasReachedMaxLoginAttempts() const {
        return m_loginAttempts >= MAX_LOGIN_ATTEMPTS;
    }

private:
    QString m_cardId;                       ///< 卡号（唯一标识）
    QString m_name;                         ///< 持卡人姓名
    QString m_studentId;                    ///< 学号
    double m_totalRecharge = 0.0;           ///< 累计充值金额
    double m_balance = 0.0;                 ///< 当前余额
    CardState m_state = CardState::Normal;  ///< 卡状态
    int m_loginAttempts = 0;                ///< 密码错误次数
    QString m_password = DEFAULT_STUDENT_PASSWORD;  ///< 登录密码（默认123456）
};

}  // namespace CampusCard

#endif  // MODEL_ENTITIES_CARD_H
