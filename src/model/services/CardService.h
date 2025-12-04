/**
 * @file CardService.h
 * @brief 校园卡业务服务类
 * @author CampusCardSystem
 * @date 2024
 *
 * MVC架构 - Model层业务服务
 * 负责校园卡相关的业务逻辑处理
 */

#ifndef MODEL_SERVICES_CARDSERVICE_H
#define MODEL_SERVICES_CARDSERVICE_H

#include "model/entities/Card.h"
#include "model/repositories/StorageManager.h"

#include <QList>
#include <QMap>
#include <QObject>


namespace CampusCard {

/**
 * @class CardService
 * @brief 校园卡业务服务类，处理卡相关的业务逻辑
 *
 * 作为Service层，负责：
 * - 卡的CRUD操作
 * - 充值、扣款业务逻辑
 * - 挂失、解挂、冻结、解冻业务逻辑
 * - 密码管理
 * - 通过信号通知状态变更
 */
class CardService : public QObject {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父对象
     */
    explicit CardService(QObject* parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~CardService() override = default;

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
     * @return 卡对象（不存在返回空Card）
     */
    [[nodiscard]] Card findCard(const QString& cardId) const;

    /**
     * @brief 根据卡号获取卡指针（用于修改）
     * @param cardId 卡号
     * @return 卡指针（不存在返回nullptr）
     */
    Card* getCardPtr(const QString& cardId);

    /**
     * @brief 根据学号查找卡
     * @param studentId 学号
     * @return 卡对象
     */
    [[nodiscard]] Card findCardByStudentId(const QString& studentId) const;

    /**
     * @brief 检查卡号是否存在
     * @param cardId 卡号
     * @return 是否存在
     */
    [[nodiscard]] bool cardExists(const QString& cardId) const;

    /**
     * @brief 获取卡数量
     * @return 卡总数
     */
    [[nodiscard]] int cardCount() const;

    // ========== 创建操作 ==========

    /**
     * @brief 创建新卡
     * @param cardId 卡号
     * @param name 姓名
     * @param studentId 学号
     * @param initialBalance 初始余额
     * @return 是否成功
     */
    bool createCard(const QString& cardId, const QString& name, const QString& studentId,
                    double initialBalance = 0.0);

    /**
     * @brief 创建新卡（使用Card对象）
     * @param card 卡对象
     * @return 是否成功
     */
    bool createCard(const Card& card);

    // ========== 充值扣款操作 ==========

    /**
     * @brief 充值
     * @param cardId 卡号
     * @param amount 充值金额
     * @return 是否成功
     */
    bool recharge(const QString& cardId, double amount);

    /**
     * @brief 扣款
     * @param cardId 卡号
     * @param amount 扣款金额
     * @return 是否成功
     */
    bool deduct(const QString& cardId, double amount);

    /**
     * @brief 获取卡余额
     * @param cardId 卡号
     * @return 余额（卡不存在返回-1）
     */
    [[nodiscard]] double getBalance(const QString& cardId) const;

    // ========== 状态管理操作 ==========

    /**
     * @brief 挂失卡
     * @param cardId 卡号
     * @return 是否成功
     */
    bool reportLost(const QString& cardId);

    /**
     * @brief 解除挂失
     * @param cardId 卡号
     * @return 是否成功
     */
    bool cancelLost(const QString& cardId);

    /**
     * @brief 冻结卡
     * @param cardId 卡号
     * @return 是否成功
     */
    bool freeze(const QString& cardId);

    /**
     * @brief 解冻卡
     * @param cardId 卡号
     * @return 是否成功
     */
    bool unfreeze(const QString& cardId);

    // ========== 密码管理 ==========

    /**
     * @brief 验证密码
     * @param cardId 卡号
     * @param password 密码
     * @return 是否正确
     */
    [[nodiscard]] bool verifyPassword(const QString& cardId, const QString& password) const;

    /**
     * @brief 修改密码
     * @param cardId 卡号
     * @param oldPassword 旧密码
     * @param newPassword 新密码
     * @return 是否成功
     */
    bool changePassword(const QString& cardId, const QString& oldPassword,
                        const QString& newPassword);

    /**
     * @brief 重置密码（管理员操作）
     * @param cardId 卡号
     * @param newPassword 新密码
     * @return 是否成功
     */
    bool resetPassword(const QString& cardId, const QString& newPassword);

    // ========== 登录尝试管理 ==========

    /**
     * @brief 增加登录失败次数
     * @param cardId 卡号
     * @return 当前失败次数（-1表示卡不存在）
     */
    int incrementLoginAttempts(const QString& cardId);

    /**
     * @brief 重置登录失败次数
     * @param cardId 卡号
     * @return 是否成功
     */
    bool resetLoginAttempts(const QString& cardId);

    /**
     * @brief 获取登录失败次数
     * @param cardId 卡号
     * @return 失败次数（-1表示卡不存在）
     */
    [[nodiscard]] int getLoginAttempts(const QString& cardId) const;

    // ========== 卡信息更新 ==========

    /**
     * @brief 更新卡信息
     * @param card 更新后的卡对象
     * @return 是否成功
     */
    bool updateCard(const Card& card);

signals:
    /**
     * @brief 卡数据变更信号（整体变更）
     */
    void cardsChanged();

    /**
     * @brief 单张卡更新信号
     * @param cardId 被更新的卡号
     */
    void cardUpdated(const QString& cardId);

    /**
     * @brief 卡创建信号
     * @param cardId 新卡号
     */
    void cardCreated(const QString& cardId);

    /**
     * @brief 余额变更信号
     * @param cardId 卡号
     * @param newBalance 新余额
     */
    void balanceChanged(const QString& cardId, double newBalance);

    /**
     * @brief 卡状态变更信号
     * @param cardId 卡号
     * @param newState 新状态
     */
    void cardStateChanged(const QString& cardId, CardState newState);

private:
    QMap<QString, Card> m_cards;  ///< 卡号到卡对象的映射
};

}  // namespace CampusCard

#endif  // MODEL_SERVICES_CARDSERVICE_H
