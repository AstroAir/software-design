/**
 * @file RegisterDialog.cpp
 * @brief 注册对话框实现
 * @author CampusCardSystem
 * @date 2024
 */

#include "RegisterDialog.h"

#include "ElaLineEdit.h"
#include "ElaMessageBar.h"
#include "ElaPushButton.h"
#include "ElaText.h"

#include <QFont>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QRandomGenerator>
#include <QRegularExpression>
#include <QVBoxLayout>


namespace CampusCard {

RegisterDialog::RegisterDialog(CardManager* cardManager, QWidget* parent)
    : ElaDialog(parent), m_cardManager(cardManager) {
    initUI();
    initConnections();
}

void RegisterDialog::initUI() {
    setWindowTitle(QStringLiteral("新卡注册"));
    setFixedSize(450, 420);

    // 隐藏窗口按钮，只保留关闭按钮
    setWindowButtonFlags(ElaAppBarType::CloseButtonHint);

    // 创建主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(30, 50, 30, 30);

    // 标题 - 使用 ElaText 自动适应主题
    auto* titleLabel = new ElaText(QStringLiteral("新卡注册"), this);
    titleLabel->setTextPixelSize(22);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    mainLayout->addSpacing(10);

    // 表单布局 - 使用 GridLayout 手动控制标签
    QGridLayout* formLayout = new QGridLayout();
    formLayout->setSpacing(12);
    formLayout->setColumnStretch(1, 1);

    // 卡号输入（带生成按钮）
    auto* cardIdLabel = new ElaText(QStringLiteral("卡号："), this);
    cardIdLabel->setTextPixelSize(14);
    QHBoxLayout* cardIdLayout = new QHBoxLayout();
    m_cardIdEdit = new ElaLineEdit(this);
    m_cardIdEdit->setPlaceholderText(QStringLiteral("请输入或生成卡号"));
    m_generateIdBtn = new ElaPushButton(QStringLiteral("生成"), this);
    m_generateIdBtn->setFixedWidth(60);
    cardIdLayout->addWidget(m_cardIdEdit);
    cardIdLayout->addWidget(m_generateIdBtn);
    formLayout->addWidget(cardIdLabel, 0, 0, Qt::AlignRight | Qt::AlignVCenter);
    formLayout->addLayout(cardIdLayout, 0, 1);

    // 姓名输入
    auto* nameLabel = new ElaText(QStringLiteral("姓名："), this);
    nameLabel->setTextPixelSize(14);
    m_nameEdit = new ElaLineEdit(this);
    m_nameEdit->setPlaceholderText(QStringLiteral("请输入姓名"));
    formLayout->addWidget(nameLabel, 1, 0, Qt::AlignRight | Qt::AlignVCenter);
    formLayout->addWidget(m_nameEdit, 1, 1);

    // 学号输入
    auto* studentIdLabel = new ElaText(QStringLiteral("学号："), this);
    studentIdLabel->setTextPixelSize(14);
    m_studentIdEdit = new ElaLineEdit(this);
    m_studentIdEdit->setPlaceholderText(QStringLiteral("请输入学号"));
    formLayout->addWidget(studentIdLabel, 2, 0, Qt::AlignRight | Qt::AlignVCenter);
    formLayout->addWidget(m_studentIdEdit, 2, 1);

    // 密码输入
    auto* passwordLabel = new ElaText(QStringLiteral("密码："), this);
    passwordLabel->setTextPixelSize(14);
    m_passwordEdit = new ElaLineEdit(this);
    m_passwordEdit->setPlaceholderText(QStringLiteral("请输入密码"));
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    formLayout->addWidget(passwordLabel, 3, 0, Qt::AlignRight | Qt::AlignVCenter);
    formLayout->addWidget(m_passwordEdit, 3, 1);

    // 确认密码
    auto* confirmLabel = new ElaText(QStringLiteral("确认密码："), this);
    confirmLabel->setTextPixelSize(14);
    m_confirmPwdEdit = new ElaLineEdit(this);
    m_confirmPwdEdit->setPlaceholderText(QStringLiteral("请再次输入密码"));
    m_confirmPwdEdit->setEchoMode(QLineEdit::Password);
    formLayout->addWidget(confirmLabel, 4, 0, Qt::AlignRight | Qt::AlignVCenter);
    formLayout->addWidget(m_confirmPwdEdit, 4, 1);

    mainLayout->addLayout(formLayout);

    mainLayout->addSpacing(10);

    // 注册按钮
    m_registerBtn = new ElaPushButton(QStringLiteral("注 册"), this);
    m_registerBtn->setFixedHeight(40);
    mainLayout->addWidget(m_registerBtn);

    mainLayout->addStretch();

    // 自动生成一个卡号
    m_cardIdEdit->setText(generateCardId());
}

void RegisterDialog::initConnections() {
    // 生成卡号按钮
    connect(m_generateIdBtn, &ElaPushButton::clicked, this,
            [this]() { m_cardIdEdit->setText(generateCardId()); });

    // 注册按钮
    connect(m_registerBtn, &ElaPushButton::clicked, this, &RegisterDialog::onRegisterClicked);

    // 回车键注册
    connect(m_confirmPwdEdit, &ElaLineEdit::returnPressed, this,
            &RegisterDialog::onRegisterClicked);
}

QString RegisterDialog::generateCardId() {
    // 生成格式：C + 3位数字
    int num = QRandomGenerator::global()->bounded(1, 1000);
    QString cardId = QStringLiteral("C%1").arg(num, 3, 10, QLatin1Char('0'));

    // 确保卡号不重复
    while (m_cardManager->cardExists(cardId)) {
        num = QRandomGenerator::global()->bounded(1, 1000);
        cardId = QStringLiteral("C%1").arg(num, 3, 10, QLatin1Char('0'));
    }

    return cardId;
}

void RegisterDialog::onRegisterClicked() {
    QString cardId = m_cardIdEdit->text().trimmed();
    QString name = m_nameEdit->text().trimmed();
    QString studentId = m_studentIdEdit->text().trimmed();
    QString password = m_passwordEdit->text();
    QString confirmPassword = m_confirmPwdEdit->text();

    // ========== 输入验证 ==========

    // 验证卡号
    if (cardId.isEmpty()) {
        ElaMessageBar::warning(ElaMessageBarType::TopRight, QStringLiteral("提示"),
                               QStringLiteral("请输入卡号"), 2000, this);
        return;
    }

    // 卡号格式验证：只允许字母和数字，长度6-20位
    QRegularExpression cardIdRegex(QStringLiteral("^[A-Za-z0-9]{6,20}$"));
    if (!cardIdRegex.match(cardId).hasMatch()) {
        ElaMessageBar::warning(ElaMessageBarType::TopRight, QStringLiteral("提示"),
                               QStringLiteral("卡号格式错误：只允许字母和数字，长度6-20位"), 3000,
                               this);
        return;
    }

    // 验证姓名
    if (name.isEmpty()) {
        ElaMessageBar::warning(ElaMessageBarType::TopRight, QStringLiteral("提示"),
                               QStringLiteral("请输入姓名"), 2000, this);
        return;
    }

    // 姓名长度验证
    if (name.length() < 2 || name.length() > 20) {
        ElaMessageBar::warning(ElaMessageBarType::TopRight, QStringLiteral("提示"),
                               QStringLiteral("姓名长度应为2-20个字符"), 2000, this);
        return;
    }

    // 验证学号
    if (studentId.isEmpty()) {
        ElaMessageBar::warning(ElaMessageBarType::TopRight, QStringLiteral("提示"),
                               QStringLiteral("请输入学号"), 2000, this);
        return;
    }

    // 学号格式验证：只允许字母和数字，长度6-20位
    QRegularExpression studentIdRegex(QStringLiteral("^[A-Za-z0-9]{6,20}$"));
    if (!studentIdRegex.match(studentId).hasMatch()) {
        ElaMessageBar::warning(ElaMessageBarType::TopRight, QStringLiteral("提示"),
                               QStringLiteral("学号格式错误：只允许字母和数字，长度6-20位"), 3000,
                               this);
        return;
    }

    // 验证密码
    if (password.isEmpty()) {
        ElaMessageBar::warning(ElaMessageBarType::TopRight, QStringLiteral("提示"),
                               QStringLiteral("请输入密码"), 2000, this);
        return;
    }

    // 密码长度验证
    if (password.length() < 4 || password.length() > 20) {
        ElaMessageBar::warning(ElaMessageBarType::TopRight, QStringLiteral("提示"),
                               QStringLiteral("密码长度应为4-20位"), 2000, this);
        return;
    }

    // 确认密码
    if (password != confirmPassword) {
        ElaMessageBar::warning(ElaMessageBarType::TopRight, QStringLiteral("提示"),
                               QStringLiteral("两次输入的密码不一致"), 2000, this);
        return;
    }

    // 检查卡号是否已存在
    if (m_cardManager->cardExists(cardId)) {
        ElaMessageBar::error(ElaMessageBarType::TopRight, QStringLiteral("错误"),
                             QStringLiteral("该卡号已存在"), 2000, this);
        return;
    }

    // 创建新卡
    if (m_cardManager->createCard(cardId, name, studentId, 0.0)) {
        // 设置密码
        Card* card = m_cardManager->findCard(cardId);
        if (card) {
            card->setPassword(password);
            m_cardManager->saveAll();
        }

        ElaMessageBar::success(ElaMessageBarType::TopRight, QStringLiteral("成功"),
                               QStringLiteral("注册成功！卡号：") + cardId, 3000, this);

        // 清空输入
        m_nameEdit->clear();
        m_studentIdEdit->clear();
        m_passwordEdit->clear();
        m_confirmPwdEdit->clear();
        m_cardIdEdit->setText(generateCardId());

        accept();  // 关闭对话框
    } else {
        ElaMessageBar::error(ElaMessageBarType::TopRight, QStringLiteral("错误"),
                             QStringLiteral("注册失败，请重试"), 2000, this);
    }
}

}  // namespace CampusCard
