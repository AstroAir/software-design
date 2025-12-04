/**
 * @file RechargeDialog.cpp
 * @brief 充值对话框实现
 * @author CampusCardSystem
 * @date 2024
 *
 * MVC架构 - View层实现
 */

#include "RechargeDialog.h"

#include "ElaLineEdit.h"
#include "ElaMessageBar.h"
#include "ElaPushButton.h"
#include "ElaText.h"

#include <QDoubleValidator>
#include <QVBoxLayout>


namespace CampusCard {

RechargeDialog::RechargeDialog(CardController* cardController, const QString& cardId,
                               QWidget* parent)
    : ElaContentDialog(parent), m_cardController(cardController), m_cardId(cardId) {
    initUI();
    initConnections();
}

void RechargeDialog::initUI() {
    setWindowTitle(QStringLiteral("充值"));
    setFixedSize(400, 280);

    // 隐藏默认按钮
    setLeftButtonText(QString());
    setMiddleButtonText(QString());
    setRightButtonText(QString());

    // 创建中心部件
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);
    layout->setSpacing(15);
    layout->setContentsMargins(30, 20, 30, 20);

    // 标题
    auto* titleLabel = new ElaText(QStringLiteral("充值"), centralWidget);
    titleLabel->setTextPixelSize(24);
    layout->addWidget(titleLabel, 0, Qt::AlignCenter);

    // 获取卡信息
    Card card = m_cardController->getCard(m_cardId);
    if (!card.cardId().isEmpty()) {
        // 卡信息
        m_cardInfoLabel = new ElaText(QStringLiteral("卡号：") + m_cardId +
                                          QStringLiteral("  姓名：") + card.name(),
                                      centralWidget);
        layout->addWidget(m_cardInfoLabel);

        // 当前余额
        m_balanceLabel =
            new ElaText(QStringLiteral("当前余额：") + QString::number(card.balance(), 'f', 2) +
                            QStringLiteral(" 元"),
                        centralWidget);
        layout->addWidget(m_balanceLabel);
    }

    // 充值金额输入
    auto* amountLabel = new ElaText(QStringLiteral("充值金额："), centralWidget);
    layout->addWidget(amountLabel);

    m_amountEdit = new ElaLineEdit(centralWidget);
    m_amountEdit->setPlaceholderText(QStringLiteral("请输入充值金额"));
    // 设置数字验证器
    auto* validator = new QDoubleValidator(0.01, 10000.0, 2, this);
    validator->setNotation(QDoubleValidator::StandardNotation);
    m_amountEdit->setValidator(validator);
    layout->addWidget(m_amountEdit);

    // 充值按钮
    m_rechargeBtn = new ElaPushButton(QStringLiteral("确认充值"), centralWidget);
    m_rechargeBtn->setFixedHeight(40);
    layout->addWidget(m_rechargeBtn);

    layout->addStretch();

    setCentralWidget(centralWidget);
}

void RechargeDialog::initConnections() {
    // 充值按钮
    connect(m_rechargeBtn, &ElaPushButton::clicked, this, &RechargeDialog::onRechargeClicked);

    // 连接Controller信号
    connect(m_cardController, &CardController::rechargeSuccess, this,
            &RechargeDialog::onRechargeSuccess);
    connect(m_cardController, &CardController::rechargeFailed, this,
            &RechargeDialog::onRechargeFailed);
}

void RechargeDialog::onRechargeClicked() {
    QString amountStr = m_amountEdit->text().trimmed();

    if (amountStr.isEmpty()) {
        ElaMessageBar::warning(ElaMessageBarType::TopRight, QStringLiteral("提示"),
                               QStringLiteral("请输入充值金额"), 2000, this);
        return;
    }

    bool ok;
    double amount = amountStr.toDouble(&ok);

    if (!ok || amount <= 0) {
        ElaMessageBar::warning(ElaMessageBarType::TopRight, QStringLiteral("提示"),
                               QStringLiteral("请输入有效的金额"), 2000, this);
        return;
    }

    // 发起充值请求
    m_cardController->handleRecharge(m_cardId, amount);
}

void RechargeDialog::onRechargeSuccess(const QString& cardId, double newBalance) {
    if (cardId != m_cardId) {
        return;  // 不是当前卡的充值结果
    }

    // 更新余额显示
    m_balanceLabel->setText(QStringLiteral("当前余额：") + QString::number(newBalance, 'f', 2) +
                            QStringLiteral(" 元"));

    ElaMessageBar::success(ElaMessageBarType::TopRight, QStringLiteral("成功"),
                           QStringLiteral("充值成功"), 2000, this);
    accept();  // 关闭对话框
}

void RechargeDialog::onRechargeFailed(const QString& message) {
    ElaMessageBar::error(ElaMessageBarType::TopRight, QStringLiteral("错误"), message, 2000, this);
}

}  // namespace CampusCard
