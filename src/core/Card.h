/**
 * @file Card.h
 * @brief 校园卡实体类定义
 * @author CampusCardSystem
 * @date 2024
 */

#ifndef CARD_H
#define CARD_H

#include "Types.h"

#include <QJsonObject>
#include <QString>


namespace CampusCard {

/**
 * @class Card
 * @brief 校园卡实体类，存储卡的基本信息和状态
 *
 * 包含卡号、姓名、学号、充值记录、余额和状态等信息
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
     * @brief 获取登录密码（加密存储）
     * @return 密码字符串
     */
    [[nodiscard]] QString password() const { return m_password; }

    // ========== Setters ==========

    /**
     * @brief 设置持卡人姓名
     * @param name 姓名
     */
    void setName(const QString& name) { m_name = name; }

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

    // ========== 业务方法 ==========

    /**
     * @brief 充值操作
     * @param amount 充值金额（必须为正数）
     * @return 充值是否成功
     */
    bool recharge(double amount);

    /**
     * @brief 消费/扣款操作
     * @param amount 消费金额
     * @return 消费是否成功（余额不足返回false）
     */
    bool deduct(double amount);

    /**
     * @brief 挂失卡
     */
    void reportLost();

    /**
     * @brief 解除挂失
     */
    void cancelLost();

    /**
     * @brief 冻结卡（密码错误达到上限）
     */
    void freeze();

    /**
     * @brief 解冻卡并重置密码错误计数
     */
    void unfreeze();

    /**
     * @brief 增加登录失败次数
     * @return 当前失败次数
     */
    int incrementLoginAttempts();

    /**
     * @brief 重置登录失败次数
     */
    void resetLoginAttempts();

    /**
     * @brief 检查卡是否可用（非挂失、非冻结）
     * @return 是否可用
     */
    [[nodiscard]] bool isUsable() const;

private:
    QString m_cardId;                       ///< 卡号（唯一标识）
    QString m_name;                         ///< 持卡人姓名
    QString m_studentId;                    ///< 学号
    double m_totalRecharge = 0.0;           ///< 累计充值金额
    double m_balance = 0.0;                 ///< 当前余额
    CardState m_state = CardState::Normal;  ///< 卡状态
    int m_loginAttempts = 0;                ///< 密码错误次数
    QString m_password = "123456";          ///< 登录密码（默认123456）
};

}  // namespace CampusCard

#endif  // CARD_H
