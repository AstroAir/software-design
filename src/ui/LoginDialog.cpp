/**
 * @file LoginDialog.cpp
 * @brief 登录对话框实现
 * @author CampusCardSystem
 * @date 2024
 */

#include "LoginDialog.h"
#include "RegisterDialog.h"
#include "ElaLineEdit.h"
#include "ElaComboBox.h"
#include "ElaPushButton.h"
#include "ElaText.h"
#include "ElaMessageBar.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QFont>

namespace CampusCard {

LoginDialog::LoginDialog(AuthManager* authManager, QWidget* parent)
    : ElaDialog(parent)
    , m_authManager(authManager)
    , m_cardManager(nullptr)
{
    initUI();
    initConnections();
}

void LoginDialog::initUI() {
    setWindowTitle(QStringLiteral("用户登录"));
    setFixedSize(400, 380);
    
    // 隐藏窗口按钮，只保留关闭按钮
    setWindowButtonFlags(ElaAppBarType::CloseButtonHint);
    
    // 创建主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12);
    mainLayout->setContentsMargins(30, 50, 30, 30);
    
    // 标题 - 使用 ElaText 自动适应主题
    auto* titleLabel = new ElaText(QStringLiteral("用户登录"), this);
    titleLabel->setTextPixelSize(22);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    
    // 提示信息标签（显示剩余尝试次数、冻结状态等）
    m_hintLabel = new ElaText(QStringLiteral("请选择角色并输入登录信息"), this);
    m_hintLabel->setTextPixelSize(12);
    m_hintLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_hintLabel);
    
    mainLayout->addSpacing(8);
    
    // 表单布局 - 使用 GridLayout 手动控制标签
    QGridLayout* formLayout = new QGridLayout();
    formLayout->setSpacing(12);
    formLayout->setColumnStretch(1, 1);
    
    // 角色选择
    auto* roleLabel = new ElaText(QStringLiteral("角色："), this);
    roleLabel->setTextPixelSize(14);
    m_roleCombo = new ElaComboBox(this);
    m_roleCombo->addItem(QStringLiteral("学生"));
    m_roleCombo->addItem(QStringLiteral("管理员"));
    formLayout->addWidget(roleLabel, 0, 0, Qt::AlignRight | Qt::AlignVCenter);
    formLayout->addWidget(m_roleCombo, 0, 1);
    
    // 卡号输入
    m_cardIdLabel = new ElaText(QStringLiteral("卡号："), this);
    m_cardIdLabel->setTextPixelSize(14);
    m_cardIdEdit = new ElaLineEdit(this);
    m_cardIdEdit->setPlaceholderText(QStringLiteral("请输入卡号"));
    formLayout->addWidget(m_cardIdLabel, 1, 0, Qt::AlignRight | Qt::AlignVCenter);
    formLayout->addWidget(m_cardIdEdit, 1, 1);
    
    // 密码输入
    auto* passwordLabel = new ElaText(QStringLiteral("密码："), this);
    passwordLabel->setTextPixelSize(14);
    m_passwordEdit = new ElaLineEdit(this);
    m_passwordEdit->setPlaceholderText(QStringLiteral("请输入密码"));
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    formLayout->addWidget(passwordLabel, 2, 0, Qt::AlignRight | Qt::AlignVCenter);
    formLayout->addWidget(m_passwordEdit, 2, 1);
    
    mainLayout->addLayout(formLayout);
    
    mainLayout->addSpacing(10);
    
    // 按钮布局
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);
    
    // 登录按钮
    m_loginButton = new ElaPushButton(QStringLiteral("登 录"), this);
    m_loginButton->setFixedHeight(40);
    buttonLayout->addWidget(m_loginButton, 2);
    
    // 注册按钮
    m_registerButton = new ElaPushButton(QStringLiteral("注 册"), this);
    m_registerButton->setFixedHeight(40);
    buttonLayout->addWidget(m_registerButton, 1);
    
    mainLayout->addLayout(buttonLayout);
    
    mainLayout->addStretch();
}

void LoginDialog::initConnections() {
    // 角色切换
    connect(m_roleCombo, QOverload<int>::of(&ElaComboBox::currentIndexChanged),
            this, &LoginDialog::onRoleChanged);
    
    // 登录按钮
    connect(m_loginButton, &ElaPushButton::clicked,
            this, &LoginDialog::onLoginClicked);
    
    // 回车键登录
    connect(m_passwordEdit, &ElaLineEdit::returnPressed,
            this, &LoginDialog::onLoginClicked);
    
    // 注册按钮
    connect(m_registerButton, &ElaPushButton::clicked,
            this, &LoginDialog::onRegisterClicked);
}

void LoginDialog::onRoleChanged(int index) {
    // 管理员不需要卡号
    bool isAdmin = (index == 1);
    m_cardIdLabel->setVisible(!isAdmin);
    m_cardIdEdit->setVisible(!isAdmin);
    
    if (isAdmin) {
        m_cardIdEdit->clear();
    }
}

void LoginDialog::onLoginClicked() {
    QString password = m_passwordEdit->text().trimmed();
    
    // 输入验证
    if (password.isEmpty()) {
        ElaMessageBar::warning(ElaMessageBarType::TopRight,
            QStringLiteral("提示"), QStringLiteral("请输入密码"), 2000, this);
        updateHintLabel(QStringLiteral("请输入密码"), true);
        return;
    }
    
    AuthManager::LoginResult result;
    QString cardId;
    
    if (m_roleCombo->currentIndex() == 0) {
        // 学生登录
        cardId = m_cardIdEdit->text().trimmed();
        if (cardId.isEmpty()) {
            ElaMessageBar::warning(ElaMessageBarType::TopRight,
                QStringLiteral("提示"), QStringLiteral("请输入卡号"), 2000, this);
            updateHintLabel(QStringLiteral("请输入卡号"), true);
            return;
        }
        
        // 登录前检查卡状态，显示剩余尝试次数
        if (m_cardManager) {
            Card* card = m_cardManager->findCard(cardId);
            if (card) {
                if (card->state() == CardState::Frozen) {
                    updateHintLabel(QStringLiteral("该卡已冻结，请联系管理员解冻"), true);
                    ElaMessageBar::error(ElaMessageBarType::TopRight,
                        QStringLiteral("登录失败"), 
                        QStringLiteral("该卡已冻结（密码错误次数过多），请联系管理员"), 3000, this);
                    return;
                }
                if (card->state() == CardState::Lost) {
                    updateHintLabel(QStringLiteral("该卡已挂失，请联系管理员解挂"), true);
                    ElaMessageBar::error(ElaMessageBarType::TopRight,
                        QStringLiteral("登录失败"), 
                        QStringLiteral("该卡已挂失，请联系管理员"), 3000, this);
                    return;
                }
                // 显示剩余尝试次数
                int remaining = MAX_LOGIN_ATTEMPTS - card->loginAttempts();
                if (remaining < MAX_LOGIN_ATTEMPTS && remaining > 0) {
                    updateHintLabel(QStringLiteral("剩余尝试次数：%1").arg(remaining), true);
                }
            }
        }
        
        result = m_authManager->studentLogin(cardId, password);
    } else {
        // 管理员登录
        result = m_authManager->adminLogin(password);
    }
    
    if (result == AuthManager::LoginResult::Success) {
        // 清空输入并重置提示
        m_cardIdEdit->clear();
        m_passwordEdit->clear();
        updateHintLabel(QStringLiteral("请选择角色并输入登录信息"), false);
        accept();  // 关闭对话框
    } else {
        // 显示错误信息
        QString errorMsg = AuthManager::loginResultToString(result);
        
        // 如果是密码错误，显示剩余尝试次数
        if (result == AuthManager::LoginResult::InvalidCredentials && 
            m_roleCombo->currentIndex() == 0 && m_cardManager) {
            Card* card = m_cardManager->findCard(cardId);
            if (card) {
                int remaining = MAX_LOGIN_ATTEMPTS - card->loginAttempts();
                if (remaining > 0) {
                    errorMsg = QStringLiteral("密码错误，剩余尝试次数：%1").arg(remaining);
                    updateHintLabel(errorMsg, true);
                } else {
                    errorMsg = QStringLiteral("密码错误次数过多，卡已被冻结");
                    updateHintLabel(QStringLiteral("卡已冻结，请联系管理员解冻"), true);
                }
            }
        } else if (result == AuthManager::LoginResult::CardFrozen) {
            updateHintLabel(QStringLiteral("卡已冻结，请联系管理员解冻"), true);
        } else if (result == AuthManager::LoginResult::CardLost) {
            updateHintLabel(QStringLiteral("卡已挂失，请联系管理员解挂"), true);
        } else {
            updateHintLabel(errorMsg, true);
        }
        
        ElaMessageBar::error(ElaMessageBarType::TopRight,
            QStringLiteral("登录失败"), errorMsg, 3000, this);
    }
}

void LoginDialog::updateHintLabel(const QString& message, bool isError) {
    m_hintLabel->setText(message);
    // 根据是否为错误信息设置不同颜色
    if (isError) {
        m_hintLabel->setStyleSheet(QStringLiteral("color: #E74C3C;"));  // 红色
    } else {
        m_hintLabel->setStyleSheet(QString());  // 恢复默认颜色
    }
}

void LoginDialog::onRegisterClicked() {
    if (!m_cardManager) {
        ElaMessageBar::error(ElaMessageBarType::TopRight,
            QStringLiteral("错误"), QStringLiteral("无法打开注册界面"), 2000, this);
        return;
    }
    
    RegisterDialog registerDialog(m_cardManager, this);
    registerDialog.exec();
}

void LoginDialog::setCardManager(CardManager* cardManager) {
    m_cardManager = cardManager;
}

} // namespace CampusCard
