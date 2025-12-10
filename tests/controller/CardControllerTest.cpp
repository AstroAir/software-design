/**
 * @file CardControllerTest.cpp
 * @brief CardController控制器层单元测试
 * @author CampusCardSystem
 * @date 2024
 */

#include "controller/CardController.h"
#include "model/repositories/StorageManager.h"
#include "model/services/CardService.h"

#include <QSignalSpy>
#include <QTemporaryDir>
#include <gtest/gtest.h>

using namespace CampusCard;

class CardControllerTest : public ::testing::Test {
protected:
    QTemporaryDir tempDir;
    QString testDataPath;
    CardService* cardService;
    CardController* cardController;

    void SetUp() override {
        ASSERT_TRUE(tempDir.isValid());
        testDataPath = tempDir.path() + "/test_data";
        StorageManager::instance().setDataPath(testDataPath);
        StorageManager::instance().initializeDataDirectory();

        StorageManager::instance().saveAllCards(QList<Card>());

        cardService = new CardService();
        cardService->initialize();

        cardController = new CardController(cardService);
    }

    void TearDown() override {
        delete cardController;
        delete cardService;
    }
};

// ========== 查询操作测试 ==========

TEST_F(CardControllerTest, GetAllCards) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    cardService->createCard("C002", "李四", "B17010102", 200.0);

    QList<Card> cards = cardController->getAllCards();
    EXPECT_EQ(cards.size(), 2);
}

TEST_F(CardControllerTest, GetCard) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    Card card = cardController->getCard("C001");
    EXPECT_EQ(card.cardId(), "C001");
    EXPECT_EQ(card.name(), "张三");
}

TEST_F(CardControllerTest, GetCardNotFound) {
    Card card = cardController->getCard("C999");
    EXPECT_TRUE(card.cardId().isEmpty());
}

TEST_F(CardControllerTest, CardExists) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    EXPECT_TRUE(cardController->cardExists("C001"));
    EXPECT_FALSE(cardController->cardExists("C999"));
}

TEST_F(CardControllerTest, GetCardCount) {
    EXPECT_EQ(cardController->getCardCount(), 0);

    cardService->createCard("C001", "张三", "B17010101", 100.0);
    EXPECT_EQ(cardController->getCardCount(), 1);
}

TEST_F(CardControllerTest, SearchCards) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    cardService->createCard("C002", "李四", "B17010102", 200.0);
    cardService->createCard("C003", "张伟", "B17010103", 300.0);

    QList<Card> results = cardController->searchCards("张");
    EXPECT_EQ(results.size(), 2);
}

TEST_F(CardControllerTest, SearchCardsEmpty) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    QList<Card> results = cardController->searchCards("");
    EXPECT_EQ(results.size(), 1);
}

TEST_F(CardControllerTest, SearchCardsNoMatch) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    QList<Card> results = cardController->searchCards("王");
    EXPECT_TRUE(results.isEmpty());
}

TEST_F(CardControllerTest, SearchCardsCaseInsensitive) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    QList<Card> results = cardController->searchCards("c001");
    EXPECT_EQ(results.size(), 1);
}

// ========== 创建操作测试 ==========

TEST_F(CardControllerTest, HandleCreateCardSuccess) {
    QSignalSpy successSpy(cardController, &CardController::cardCreated);

    cardController->handleCreateCard("C001", "张三", "B17010101", 100.0);

    EXPECT_EQ(successSpy.count(), 1);
    EXPECT_EQ(successSpy.takeFirst().at(0).toString(), "C001");
}

TEST_F(CardControllerTest, HandleCreateCardEmptyCardId) {
    QSignalSpy failedSpy(cardController, &CardController::cardCreateFailed);

    cardController->handleCreateCard("", "张三", "B17010101", 100.0);

    EXPECT_EQ(failedSpy.count(), 1);
}

TEST_F(CardControllerTest, HandleCreateCardEmptyName) {
    QSignalSpy failedSpy(cardController, &CardController::cardCreateFailed);

    cardController->handleCreateCard("C001", "", "B17010101", 100.0);

    EXPECT_EQ(failedSpy.count(), 1);
}

TEST_F(CardControllerTest, HandleCreateCardEmptyStudentId) {
    QSignalSpy failedSpy(cardController, &CardController::cardCreateFailed);

    cardController->handleCreateCard("C001", "张三", "", 100.0);

    EXPECT_EQ(failedSpy.count(), 1);
}

TEST_F(CardControllerTest, HandleCreateCardDuplicate) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    QSignalSpy failedSpy(cardController, &CardController::cardCreateFailed);

    cardController->handleCreateCard("C001", "李四", "B17010102", 200.0);

    EXPECT_EQ(failedSpy.count(), 1);
}

TEST_F(CardControllerTest, HandleRegisterCardSuccess) {
    QSignalSpy successSpy(cardController, &CardController::cardCreated);

    cardController->handleRegisterCard("C001", "张三", "B17010101", "password123");

    EXPECT_EQ(successSpy.count(), 1);
}

TEST_F(CardControllerTest, HandleRegisterCardPasswordTooShort) {
    QSignalSpy failedSpy(cardController, &CardController::cardCreateFailed);

    cardController->handleRegisterCard("C001", "张三", "B17010101", "abc");

    EXPECT_EQ(failedSpy.count(), 1);
}

// ========== 充值扣款操作测试 ==========

TEST_F(CardControllerTest, HandleRechargeSuccess) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    QSignalSpy successSpy(cardController, &CardController::rechargeSuccess);

    cardController->handleRecharge("C001", 50.0);

    EXPECT_EQ(successSpy.count(), 1);
    EXPECT_DOUBLE_EQ(successSpy.takeFirst().at(1).toDouble(), 150.0);
}

TEST_F(CardControllerTest, HandleRechargeNegativeAmount) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    QSignalSpy failedSpy(cardController, &CardController::rechargeFailed);

    cardController->handleRecharge("C001", -50.0);

    EXPECT_EQ(failedSpy.count(), 1);
}

TEST_F(CardControllerTest, HandleRechargeZeroAmount) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    QSignalSpy failedSpy(cardController, &CardController::rechargeFailed);

    cardController->handleRecharge("C001", 0.0);

    EXPECT_EQ(failedSpy.count(), 1);
}

TEST_F(CardControllerTest, HandleDeductSuccess) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    QSignalSpy successSpy(cardController, &CardController::deductSuccess);

    cardController->handleDeduct("C001", 30.0);

    EXPECT_EQ(successSpy.count(), 1);
    EXPECT_DOUBLE_EQ(successSpy.takeFirst().at(1).toDouble(), 70.0);
}

TEST_F(CardControllerTest, HandleDeductInsufficientBalance) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    QSignalSpy failedSpy(cardController, &CardController::deductFailed);

    cardController->handleDeduct("C001", 150.0);

    EXPECT_EQ(failedSpy.count(), 1);
}

TEST_F(CardControllerTest, HandleDeductNegativeAmount) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    QSignalSpy failedSpy(cardController, &CardController::deductFailed);

    cardController->handleDeduct("C001", -30.0);

    EXPECT_EQ(failedSpy.count(), 1);
}

TEST_F(CardControllerTest, GetBalance) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    EXPECT_DOUBLE_EQ(cardController->getBalance("C001"), 100.0);
}

// ========== 状态管理操作测试 ==========

TEST_F(CardControllerTest, HandleReportLostSuccess) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    QSignalSpy successSpy(cardController, &CardController::reportLostSuccess);

    cardController->handleReportLost("C001");

    EXPECT_EQ(successSpy.count(), 1);
}

TEST_F(CardControllerTest, HandleReportLostCardNotFound) {
    QSignalSpy failedSpy(cardController, &CardController::operationFailed);

    cardController->handleReportLost("C999");

    EXPECT_EQ(failedSpy.count(), 1);
}

TEST_F(CardControllerTest, HandleCancelLostSuccess) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    cardService->reportLost("C001");

    QSignalSpy successSpy(cardController, &CardController::cancelLostSuccess);

    cardController->handleCancelLost("C001");

    EXPECT_EQ(successSpy.count(), 1);
}

TEST_F(CardControllerTest, HandleCancelLostNotLost) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    QSignalSpy failedSpy(cardController, &CardController::operationFailed);

    cardController->handleCancelLost("C001");

    EXPECT_EQ(failedSpy.count(), 1);
}

TEST_F(CardControllerTest, HandleFreezeSuccess) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    QSignalSpy successSpy(cardController, &CardController::freezeSuccess);

    cardController->handleFreeze("C001");

    EXPECT_EQ(successSpy.count(), 1);
}

TEST_F(CardControllerTest, HandleUnfreezeSuccess) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);
    cardService->freeze("C001");

    QSignalSpy successSpy(cardController, &CardController::unfreezeSuccess);

    cardController->handleUnfreeze("C001");

    EXPECT_EQ(successSpy.count(), 1);
}

// ========== 密码管理测试 ==========

TEST_F(CardControllerTest, HandleChangePasswordSuccess) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    QSignalSpy successSpy(cardController, &CardController::passwordChanged);

    cardController->handleChangePassword("C001", DEFAULT_STUDENT_PASSWORD, "newpassword");

    EXPECT_EQ(successSpy.count(), 1);
}

TEST_F(CardControllerTest, HandleChangePasswordWrongOld) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    QSignalSpy failedSpy(cardController, &CardController::passwordChangeFailed);

    cardController->handleChangePassword("C001", "wrongold", "newpassword");

    EXPECT_EQ(failedSpy.count(), 1);
}

TEST_F(CardControllerTest, HandleChangePasswordTooShort) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    QSignalSpy failedSpy(cardController, &CardController::passwordChangeFailed);

    cardController->handleChangePassword("C001", DEFAULT_STUDENT_PASSWORD, "abc");

    EXPECT_EQ(failedSpy.count(), 1);
}

TEST_F(CardControllerTest, HandleResetPasswordSuccess) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    QSignalSpy successSpy(cardController, &CardController::passwordReset);

    cardController->handleResetPassword("C001", "newpassword");

    EXPECT_EQ(successSpy.count(), 1);
}

TEST_F(CardControllerTest, HandleResetPasswordEmpty) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    QSignalSpy failedSpy(cardController, &CardController::passwordChangeFailed);

    cardController->handleResetPassword("C001", "");

    EXPECT_EQ(failedSpy.count(), 1);
}

// ========== 信号转发测试 ==========

TEST_F(CardControllerTest, CardsUpdatedSignal) {
    QSignalSpy updatedSpy(cardController, &CardController::cardsUpdated);

    cardService->createCard("C001", "张三", "B17010101", 100.0);

    EXPECT_EQ(updatedSpy.count(), 1);
}

TEST_F(CardControllerTest, CardUpdatedSignal) {
    cardService->createCard("C001", "张三", "B17010101", 100.0);

    QSignalSpy updatedSpy(cardController, &CardController::cardUpdated);

    cardService->recharge("C001", 50.0);

    EXPECT_EQ(updatedSpy.count(), 1);
    EXPECT_EQ(updatedSpy.takeFirst().at(0).toString(), "C001");
}
