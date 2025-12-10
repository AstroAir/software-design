/**
 * @file CardServiceTest.cpp
 * @brief CardService业务服务层单元测试
 * @author CampusCardSystem
 * @date 2024
 */

#include "model/entities/Card.h"
#include "model/repositories/StorageManager.h"
#include "model/services/CardService.h"

#include <QSignalSpy>
#include <QTemporaryDir>
#include <gtest/gtest.h>

using namespace CampusCard;

class CardServiceTest : public ::testing::Test {
protected:
    QTemporaryDir tempDir;
    QString testDataPath;
    CardService* cardService;

    void SetUp() override {
        ASSERT_TRUE(tempDir.isValid());
        testDataPath = tempDir.path() + "/test_data";
        StorageManager::instance().setDataPath(testDataPath);
        StorageManager::instance().initializeDataDirectory();

        // 清空卡数据
        StorageManager::instance().saveAllCards(QList<Card>());

        cardService = new CardService();
        cardService->initialize();
    }

    void TearDown() override {
        delete cardService;
    }
};

// ========== 初始化测试 ==========

TEST_F(CardServiceTest, Initialize) {
    // 创建一些卡并保存
    QList<Card> cards;
    cards.append(Card("C001", "张三", "B17010101", 100.0));
    cards.append(Card("C002", "李四", "B17010102", 200.0));
    StorageManager::instance().saveAllCards(cards);

    CardService service;
    service.initialize();

    EXPECT_EQ(service.cardCount(), 2);
}

TEST_F(CardServiceTest, SaveAll) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    EXPECT_TRUE(cardService->saveAll());

    // 验证数据已保存
    QList<Card> loadedCards = StorageManager::instance().loadAllCards();
    EXPECT_EQ(loadedCards.size(), 1);
}

// ========== 查询操作测试 ==========

TEST_F(CardServiceTest, GetAllCards) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    cardService->createCard("C002", "李四", "B17010102", 200.0);

    QList<Card> cards = cardService->getAllCards();
    EXPECT_EQ(cards.size(), 2);
}

TEST_F(CardServiceTest, FindCard) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    Card card = cardService->findCard("C001");
    EXPECT_EQ(card.cardId(), "C001");
    EXPECT_EQ(card.name(), "张三");
}

TEST_F(CardServiceTest, FindCardNotFound) {
    Card card = cardService->findCard("C999");
    EXPECT_TRUE(card.cardId().isEmpty());
}

TEST_F(CardServiceTest, GetCardPtr) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    Card* cardPtr = cardService->getCardPtr("C001");
    ASSERT_NE(cardPtr, nullptr);
    EXPECT_EQ(cardPtr->cardId(), "C001");
}

TEST_F(CardServiceTest, GetCardPtrNotFound) {
    Card* cardPtr = cardService->getCardPtr("C999");
    EXPECT_EQ(cardPtr, nullptr);
}

TEST_F(CardServiceTest, FindCardByStudentId) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    Card card = cardService->findCardByStudentId("B17010101");
    EXPECT_EQ(card.cardId(), "C001");
}

TEST_F(CardServiceTest, FindCardByStudentIdNotFound) {
    Card card = cardService->findCardByStudentId("B99999999");
    EXPECT_TRUE(card.cardId().isEmpty());
}

TEST_F(CardServiceTest, CardExists) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    EXPECT_TRUE(cardService->cardExists("C001"));
    EXPECT_FALSE(cardService->cardExists("C999"));
}

TEST_F(CardServiceTest, CardCount) {
    EXPECT_EQ(cardService->cardCount(), 0);

    cardService->createCard("C001", "张三", "B17010101", 100.0);
    EXPECT_EQ(cardService->cardCount(), 1);

    cardService->createCard("C002", "李四", "B17010102", 200.0);
    EXPECT_EQ(cardService->cardCount(), 2);
}

// ========== 创建操作测试 ==========

TEST_F(CardServiceTest, CreateCardWithParameters) {
    EXPECT_TRUE(cardService->createCard("C001", "张三", "B17010101", 100.0));

    Card card = cardService->findCard("C001");
    EXPECT_EQ(card.cardId(), "C001");
    EXPECT_EQ(card.name(), "张三");
    EXPECT_EQ(card.studentId(), "B17010101");
    EXPECT_DOUBLE_EQ(card.balance(), 100.0);
}

TEST_F(CardServiceTest, CreateCardWithObject) {
    Card newCard("C001", "张三", "B17010101", 100.0);
    newCard.setPassword("custompass");

    EXPECT_TRUE(cardService->createCard(newCard));

    Card card = cardService->findCard("C001");
    EXPECT_EQ(card.password(), "custompass");
}

TEST_F(CardServiceTest, CreateCardDuplicate) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    EXPECT_FALSE(cardService->createCard("C001", "李四", "B17010102", 200.0));
}

TEST_F(CardServiceTest, CreateCardSignals) {
    QSignalSpy createdSpy(cardService, &CardService::cardCreated);
    QSignalSpy changedSpy(cardService, &CardService::cardsChanged);

    cardService->createCard("C001", "张三", "B17010101", 100.0);

    EXPECT_EQ(createdSpy.count(), 1);
    EXPECT_EQ(changedSpy.count(), 1);
    EXPECT_EQ(createdSpy.takeFirst().at(0).toString(), "C001");
}

// ========== 充值扣款操作测试 ==========

TEST_F(CardServiceTest, Recharge) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    EXPECT_TRUE(cardService->recharge("C001", 50.0));

    Card card = cardService->findCard("C001");
    EXPECT_DOUBLE_EQ(card.balance(), 150.0);
    EXPECT_DOUBLE_EQ(card.totalRecharge(), 150.0);
}

TEST_F(CardServiceTest, RechargeCardNotFound) {
    EXPECT_FALSE(cardService->recharge("C999", 50.0));
}

TEST_F(CardServiceTest, RechargeNegativeAmount) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    EXPECT_FALSE(cardService->recharge("C001", -50.0));
}

TEST_F(CardServiceTest, RechargeZeroAmount) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    EXPECT_FALSE(cardService->recharge("C001", 0.0));
}

TEST_F(CardServiceTest, RechargeSignals) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    QSignalSpy updatedSpy(cardService, &CardService::cardUpdated);
    QSignalSpy balanceSpy(cardService, &CardService::balanceChanged);

    cardService->recharge("C001", 50.0);

    EXPECT_EQ(updatedSpy.count(), 1);
    EXPECT_EQ(balanceSpy.count(), 1);
    EXPECT_DOUBLE_EQ(balanceSpy.takeFirst().at(1).toDouble(), 150.0);
}

TEST_F(CardServiceTest, Deduct) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    EXPECT_TRUE(cardService->deduct("C001", 30.0));

    Card card = cardService->findCard("C001");
    EXPECT_DOUBLE_EQ(card.balance(), 70.0);
}

TEST_F(CardServiceTest, DeductCardNotFound) {
    EXPECT_FALSE(cardService->deduct("C999", 30.0));
}

TEST_F(CardServiceTest, DeductInsufficientBalance) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    EXPECT_FALSE(cardService->deduct("C001", 150.0));
}

TEST_F(CardServiceTest, DeductNegativeAmount) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    EXPECT_FALSE(cardService->deduct("C001", -30.0));
}

TEST_F(CardServiceTest, DeductZeroAmount) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    EXPECT_FALSE(cardService->deduct("C001", 0.0));
}

TEST_F(CardServiceTest, DeductCardNotUsable) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    cardService->reportLost("C001");

    EXPECT_FALSE(cardService->deduct("C001", 30.0));
}

TEST_F(CardServiceTest, GetBalance) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    EXPECT_DOUBLE_EQ(cardService->getBalance("C001"), 100.0);
}

TEST_F(CardServiceTest, GetBalanceCardNotFound) {
    EXPECT_DOUBLE_EQ(cardService->getBalance("C999"), -1.0);
}

// ========== 状态管理操作测试 ==========

TEST_F(CardServiceTest, ReportLost) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    EXPECT_TRUE(cardService->reportLost("C001"));

    Card card = cardService->findCard("C001");
    EXPECT_EQ(card.state(), CardState::Lost);
}

TEST_F(CardServiceTest, ReportLostCardNotFound) {
    EXPECT_FALSE(cardService->reportLost("C999"));
}

TEST_F(CardServiceTest, ReportLostSignals) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    QSignalSpy stateSpy(cardService, &CardService::cardStateChanged);

    cardService->reportLost("C001");

    EXPECT_EQ(stateSpy.count(), 1);
    EXPECT_EQ(stateSpy.takeFirst().at(1).value<CardState>(), CardState::Lost);
}

TEST_F(CardServiceTest, CancelLost) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    cardService->reportLost("C001");

    EXPECT_TRUE(cardService->cancelLost("C001"));

    Card card = cardService->findCard("C001");
    EXPECT_EQ(card.state(), CardState::Normal);
}

TEST_F(CardServiceTest, CancelLostNotLost) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    // 卡不是挂失状态
    EXPECT_FALSE(cardService->cancelLost("C001"));
}

TEST_F(CardServiceTest, CancelLostCardNotFound) {
    EXPECT_FALSE(cardService->cancelLost("C999"));
}

TEST_F(CardServiceTest, Freeze) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    EXPECT_TRUE(cardService->freeze("C001"));

    Card card = cardService->findCard("C001");
    EXPECT_EQ(card.state(), CardState::Frozen);
}

TEST_F(CardServiceTest, FreezeCardNotFound) {
    EXPECT_FALSE(cardService->freeze("C999"));
}

TEST_F(CardServiceTest, Unfreeze) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    cardService->freeze("C001");

    EXPECT_TRUE(cardService->unfreeze("C001"));

    Card card = cardService->findCard("C001");
    EXPECT_EQ(card.state(), CardState::Normal);
    EXPECT_EQ(card.loginAttempts(), 0);
}

TEST_F(CardServiceTest, UnfreezeCardNotFound) {
    EXPECT_FALSE(cardService->unfreeze("C999"));
}

// ========== 密码管理测试 ==========

TEST_F(CardServiceTest, VerifyPassword) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    EXPECT_TRUE(cardService->verifyPassword("C001", DEFAULT_STUDENT_PASSWORD));
    EXPECT_FALSE(cardService->verifyPassword("C001", "wrongpassword"));
}

TEST_F(CardServiceTest, VerifyPasswordCardNotFound) {
    EXPECT_FALSE(cardService->verifyPassword("C999", "anypassword"));
}

TEST_F(CardServiceTest, ChangePassword) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    EXPECT_TRUE(cardService->changePassword("C001", DEFAULT_STUDENT_PASSWORD, "newpass"));

    EXPECT_TRUE(cardService->verifyPassword("C001", "newpass"));
    EXPECT_FALSE(cardService->verifyPassword("C001", DEFAULT_STUDENT_PASSWORD));
}

TEST_F(CardServiceTest, ChangePasswordWrongOld) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    EXPECT_FALSE(cardService->changePassword("C001", "wrongold", "newpass"));
}

TEST_F(CardServiceTest, ChangePasswordCardNotFound) {
    EXPECT_FALSE(cardService->changePassword("C999", "old", "new"));
}

TEST_F(CardServiceTest, ResetPassword) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    cardService->freeze("C001");

    EXPECT_TRUE(cardService->resetPassword("C001", "resetpass"));

    Card card = cardService->findCard("C001");
    EXPECT_EQ(card.state(), CardState::Normal);
    EXPECT_EQ(card.loginAttempts(), 0);
    EXPECT_TRUE(cardService->verifyPassword("C001", "resetpass"));
}

TEST_F(CardServiceTest, ResetPasswordCardNotFound) {
    EXPECT_FALSE(cardService->resetPassword("C999", "newpass"));
}

// ========== 登录尝试管理测试 ==========

TEST_F(CardServiceTest, IncrementLoginAttempts) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    EXPECT_EQ(cardService->incrementLoginAttempts("C001"), 1);
    EXPECT_EQ(cardService->incrementLoginAttempts("C001"), 2);
    EXPECT_EQ(cardService->incrementLoginAttempts("C001"), 3);

    Card card = cardService->findCard("C001");
    EXPECT_EQ(card.state(), CardState::Frozen);
}

TEST_F(CardServiceTest, IncrementLoginAttemptsCardNotFound) {
    EXPECT_EQ(cardService->incrementLoginAttempts("C999"), -1);
}

TEST_F(CardServiceTest, ResetLoginAttempts) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    cardService->incrementLoginAttempts("C001");
    cardService->incrementLoginAttempts("C001");

    EXPECT_TRUE(cardService->resetLoginAttempts("C001"));

    EXPECT_EQ(cardService->getLoginAttempts("C001"), 0);
}

TEST_F(CardServiceTest, ResetLoginAttemptsCardNotFound) {
    EXPECT_FALSE(cardService->resetLoginAttempts("C999"));
}

TEST_F(CardServiceTest, GetLoginAttempts) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    EXPECT_EQ(cardService->getLoginAttempts("C001"), 0);

    cardService->incrementLoginAttempts("C001");
    EXPECT_EQ(cardService->getLoginAttempts("C001"), 1);
}

TEST_F(CardServiceTest, GetLoginAttemptsCardNotFound) {
    EXPECT_EQ(cardService->getLoginAttempts("C999"), -1);
}

// ========== 卡信息更新测试 ==========

TEST_F(CardServiceTest, UpdateCard) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    Card updatedCard = cardService->findCard("C001");
    updatedCard.setName("张三丰");
    updatedCard.setBalance(500.0);

    EXPECT_TRUE(cardService->updateCard(updatedCard));

    Card card = cardService->findCard("C001");
    EXPECT_EQ(card.name(), "张三丰");
    EXPECT_DOUBLE_EQ(card.balance(), 500.0);
}

TEST_F(CardServiceTest, UpdateCardNotFound) {
    Card nonExistentCard("C999", "不存在", "B99999999", 100.0);
    EXPECT_FALSE(cardService->updateCard(nonExistentCard));
}

TEST_F(CardServiceTest, UpdateCardSignals) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    QSignalSpy updatedSpy(cardService, &CardService::cardUpdated);

    Card card = cardService->findCard("C001");
    card.setName("李四");
    cardService->updateCard(card);

    EXPECT_EQ(updatedSpy.count(), 1);
    EXPECT_EQ(updatedSpy.takeFirst().at(0).toString(), "C001");
}
