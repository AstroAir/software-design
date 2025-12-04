/**
 * @file CardController.h
 * @brief 校园卡控制器
 * @author CampusCardSystem
 * @date 2024
 *
 * MVC架构 - Controller层
 * 负责协调校园卡相关的View和Model交互
 */

#ifndef CONTROLLER_CARDCONTROLLER_H
#define CONTROLLER_CARDCONTROLLER_H

#include "model/services/CardService.h"

#include <QObject>


namespace CampusCard {

/**
 * @class CardController
 * @brief 校园卡控制器，处理卡管理相关的用户交互
 *
 * 作为Controller层，负责：
 * - 接收View的卡操作请求
 * - 调用CardService处理业务逻辑
 * - 将结果反馈给View
 */
class CardController : public QObject {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param cardService 卡服务
     * @param parent 父对象
     */
    explicit CardController(CardService* cardService, QObject* parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~CardController() override = default;

    // ========== 查询操作 ==========

    /**
     * @brief 获取所有卡列表
     * @return 卡列表
     */
    [[nodiscard]] QList<Card> getAllCards() const;

    /**
     * @brief 获取指定卡
     * @param cardId 卡号
     * @return 卡对象
     */
    [[nodiscard]] Card getCard(const QString& cardId) const;

    /**
     * @brief 检查卡是否存在
     * @param cardId 卡号
     * @return 是否存在
     */
    [[nodiscard]] bool cardExists(const QString& cardId) const;

    /**
     * @brief 获取卡数量
     * @return 卡总数
     */
    [[nodiscard]] int getCardCount() const;

    /**
     * @brief 搜索卡（按卡号、姓名、学号）
     * @param keyword 搜索关键词
     * @return 匹配的卡列表
     */
    [[nodiscard]] QList<Card> searchCards(const QString& keyword) const;

    // ========== 创建操作 ==========

    /**
     * @brief 处理创建新卡请求
     * @param cardId 卡号
     * @param name 姓名
     * @param studentId 学号
     * @param initialBalance 初始余额
     */
    void handleCreateCard(const QString& cardId, const QString& name, const QString& studentId,
                          double initialBalance = 0.0);

    /**
     * @brief 处理注册新卡请求（包含密码）
     * @param cardId 卡号
     * @param name 姓名
     * @param studentId 学号
     * @param password 密码
     */
    void handleRegisterCard(const QString& cardId, const QString& name, const QString& studentId,
                            const QString& password);

    // ========== 充值扣款操作 ==========

    /**
     * @brief 处理充值请求
     * @param cardId 卡号
     * @param amount 充值金额
     */
    void handleRecharge(const QString& cardId, double amount);

    /**
     * @brief 处理扣款请求
     * @param cardId 卡号
     * @param amount 扣款金额
     */
    void handleDeduct(const QString& cardId, double amount);

    /**
     * @brief 获取卡余额
     * @param cardId 卡号
     * @return 余额
     */
    [[nodiscard]] double getBalance(const QString& cardId) const;

    // ========== 状态管理操作 ==========

    /**
     * @brief 处理挂失请求
     * @param cardId 卡号
     */
    void handleReportLost(const QString& cardId);

    /**
     * @brief 处理解挂请求
     * @param cardId 卡号
     */
    void handleCancelLost(const QString& cardId);

    /**
     * @brief 处理冻结请求
     * @param cardId 卡号
     */
    void handleFreeze(const QString& cardId);

    /**
     * @brief 处理解冻请求
     * @param cardId 卡号
     */
    void handleUnfreeze(const QString& cardId);

    // ========== 密码管理 ==========

    /**
     * @brief 处理修改密码请求
     * @param cardId 卡号
     * @param oldPassword 旧密码
     * @param newPassword 新密码
     */
    void handleChangePassword(const QString& cardId, const QString& oldPassword,
                               const QString& newPassword);

    /**
     * @brief 处理重置密码请求（管理员操作）
     * @param cardId 卡号
     * @param newPassword 新密码
     */
    void handleResetPassword(const QString& cardId, const QString& newPassword);

signals:
    // ========== 操作结果信号 ==========

    /**
     * @brief 卡创建成功信号
     * @param cardId 卡号
     */
    void cardCreated(const QString& cardId);

    /**
     * @brief 卡创建失败信号
     * @param message 错误消息
     */
    void cardCreateFailed(const QString& message);

    /**
     * @brief 充值成功信号
     * @param cardId 卡号
     * @param newBalance 新余额
     */
    void rechargeSuccess(const QString& cardId, double newBalance);

    /**
     * @brief 充值失败信号
     * @param message 错误消息
     */
    void rechargeFailed(const QString& message);

    /**
     * @brief 扣款成功信号
     * @param cardId 卡号
     * @param newBalance 新余额
     */
    void deductSuccess(const QString& cardId, double newBalance);

    /**
     * @brief 扣款失败信号
     * @param message 错误消息
     */
    void deductFailed(const QString& message);

    /**
     * @brief 挂失成功信号
     * @param cardId 卡号
     */
    void reportLostSuccess(const QString& cardId);

    /**
     * @brief 解挂成功信号
     * @param cardId 卡号
     */
    void cancelLostSuccess(const QString& cardId);

    /**
     * @brief 冻结成功信号
     * @param cardId 卡号
     */
    void freezeSuccess(const QString& cardId);

    /**
     * @brief 解冻成功信号
     * @param cardId 卡号
     */
    void unfreezeSuccess(const QString& cardId);

    /**
     * @brief 密码修改成功信号
     * @param cardId 卡号
     */
    void passwordChanged(const QString& cardId);

    /**
     * @brief 密码修改失败信号
     * @param message 错误消息
     */
    void passwordChangeFailed(const QString& message);

    /**
     * @brief 密码重置成功信号
     * @param cardId 卡号
     */
    void passwordReset(const QString& cardId);

    /**
     * @brief 操作失败通用信号
     * @param message 错误消息
     */
    void operationFailed(const QString& message);

    /**
     * @brief 卡列表更新信号
     */
    void cardsUpdated();

    /**
     * @brief 单张卡更新信号
     * @param cardId 卡号
     */
    void cardUpdated(const QString& cardId);

private:
    CardService* m_cardService;  ///< 卡服务
};

}  // namespace CampusCard

#endif  // CONTROLLER_CARDCONTROLLER_H
