/**
 * @file CardManager.h
 * @brief 校园卡管理器，负责卡的业务逻辑
 * @author CampusCardSystem
 * @date 2024
 */

#ifndef CARDMANAGER_H
#define CARDMANAGER_H

#include "Card.h"
#include "StorageManager.h"
#include <QObject>
#include <QList>
#include <QMap>

namespace CampusCard {

/**
 * @class CardManager
 * @brief 校园卡管理器，提供卡的CRUD和业务操作
 * 
 * 继承QObject以支持信号槽机制
 */
class CardManager : public QObject {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父对象
     */
    explicit CardManager(QObject* parent = nullptr);

    /**
     * @brief 初始化，从存储加载数据
     */
    void initialize();

    /**
     * @brief 保存所有数据到存储
     * @return 是否成功
     */
    bool saveAll();

    // ========== 查询操作 ==========

    /**
     * @brief 获取所有校园卡
     * @return 卡列表
     */
    [[nodiscard]] QList<Card> getAllCards() const;

    /**
     * @brief 根据卡号查找卡
     * @param cardId 卡号
     * @return 卡指针（不存在返回nullptr）
     */
    [[nodiscard]] Card* findCard(const QString& cardId);

    /**
     * @brief 根据学号查找卡
     * @param studentId 学号
     * @return 卡指针
     */
    [[nodiscard]] Card* findCardByStudentId(const QString& studentId);

    /**
     * @brief 检查卡号是否存在
     * @param cardId 卡号
     * @return 是否存在
     */
    [[nodiscard]] bool cardExists(const QString& cardId) const;

    // ========== 管理操作 ==========

    /**
     * @brief 创建新卡
     * @param cardId 卡号
     * @param name 姓名
     * @param studentId 学号
     * @param initialBalance 初始余额
     * @return 是否成功
     */
    bool createCard(const QString& cardId, const QString& name,
                    const QString& studentId, double initialBalance = 0.0);

    /**
     * @brief 充值
     * @param cardId 卡号
     * @param amount 充值金额
     * @return 是否成功
     */
    bool rechargeCard(const QString& cardId, double amount);

    /**
     * @brief 扣款
     * @param cardId 卡号
     * @param amount 扣款金额
     * @return 是否成功
     */
    bool deductFromCard(const QString& cardId, double amount);

    /**
     * @brief 挂失卡
     * @param cardId 卡号
     * @return 是否成功
     */
    bool reportCardLost(const QString& cardId);

    /**
     * @brief 解除挂失
     * @param cardId 卡号
     * @return 是否成功
     */
    bool cancelCardLost(const QString& cardId);

    /**
     * @brief 冻结卡
     * @param cardId 卡号
     * @return 是否成功
     */
    bool freezeCard(const QString& cardId);

    /**
     * @brief 解冻卡
     * @param cardId 卡号
     * @return 是否成功
     */
    bool unfreezeCard(const QString& cardId);

    /**
     * @brief 重置学生密码
     * @param cardId 卡号
     * @param newPassword 新密码
     * @return 是否成功
     */
    bool resetPassword(const QString& cardId, const QString& newPassword);

    /**
     * @brief 更新卡信息
     * @param card 更新后的卡对象
     * @return 是否成功
     */
    bool updateCard(const Card& card);

signals:
    /**
     * @brief 卡数据变更信号
     */
    void cardsChanged();

    /**
     * @brief 单张卡更新信号
     * @param cardId 被更新的卡号
     */
    void cardUpdated(const QString& cardId);

private:
    QMap<QString, Card> m_cards;  ///< 卡号到卡对象的映射
};

} // namespace CampusCard

#endif // CARDMANAGER_H
