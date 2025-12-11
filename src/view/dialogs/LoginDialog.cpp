/**
 * @file LoginDialog.cpp
 * @brief 登录对话框实现
 * @author CampusCardSystem
 * @date 2024
 *
 * MVC架构 - View层实现
 */

#include "LoginDialog.h"

#include "ElaComboBox.h"
#include "ElaLineEdit.h"
#include "ElaMessageBar.h"
#include "ElaPushButton.h"
#include "ElaText.h"
#include "ElaTheme.h"

#include <QFont>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>


namespace CampusCard {

LoginDialog::LoginDialog(AuthController* authController, CardController* cardController,
                         QWidget* parent)
    : ElaDialog(parent), m_authController(authController), m_cardController(cardController) {
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

    // 标题
    auto* titleLabel = new ElaText(QStringLiteral("用户登录"), this);
    titleLabel->setTextPixelSize(22);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // 提示信息标签
    m_hintLabel = new ElaText(QStringLiteral("请选择角色并输入登录信息"), this);
    m_hintLabel->setTextPixelSize(12);
    m_hintLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_hintLabel);

    mainLayout->addSpacing(8);

    // 表单布局
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
    connect(m_roleCombo, QOverload<int>::of(&ElaComboBox::currentIndexChanged), this,
            &LoginDialog::onRoleChanged);

    // 登录按钮
    connect(m_loginButton, &ElaPushButton::clicked, this, &LoginDialog::onLoginClicked);

    // 回车键登录
    connect(m_passwordEdit, &ElaLineEdit::returnPressed, this, &LoginDialog::onLoginClicked);

    // 注册按钮
    connect(m_registerButton, &ElaPushButton::clicked, this, &LoginDialog::onRegisterClicked);

    // 连接Controller信号
    connect(m_authController, &AuthController::loginSuccess, this, &LoginDialog::onLoginSuccess);
    connect(m_authController, &AuthController::loginFailed, this, &LoginDialog::onLoginFailed);
    connect(m_authController, &AuthController::passwordError, this, &LoginDialog::onPasswordError);
    connect(m_authController, &AuthController::cardFrozen, this, &LoginDialog::onCardFrozen);
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
        ElaMessageBar::warning(ElaMessageBarType::TopRight, QStringLiteral("提示"),
                               QStringLiteral("请输入密码"), 2000, this);
        updateHintLabel(QStringLiteral("请输入密码"), true);
        return;
    }

    if (m_roleCombo->currentIndex() == 0) {
        // 学生登录
        QString cardId = m_cardIdEdit->text().trimmed();
        if (cardId.isEmpty()) {
            ElaMessageBar::warning(ElaMessageBarType::TopRight, QStringLiteral("提示"),
                                   QStringLiteral("请输入卡号"), 2000, this);
            updateHintLabel(QStringLiteral("请输入卡号"), true);
            return;
        }

        // 登录前预检
        LoginResult checkResult = m_authController->checkCardStatus(cardId);
        if (checkResult == LoginResult::CardFrozen) {
            updateHintLabel(QStringLiteral("该卡已冻结，请联系管理员解冻"), true);
            ElaMessageBar::error(ElaMessageBarType::TopRight, QStringLiteral("登录失败"),
                                 QStringLiteral("该卡已冻结（密码错误次数过多），请联系管理员"),
                                 3000, this);
            return;
        }
        if (checkResult == LoginResult::CardLost) {
            updateHintLabel(QStringLiteral("该卡已挂失，请联系管理员解挂"), true);
            ElaMessageBar::error(ElaMessageBarType::TopRight, QStringLiteral("登录失败"),
                                 QStringLiteral("该卡已挂失，请联系管理员"), 3000, this);
            return;
        }

        // 显示剩余尝试次数
        int remaining = m_authController->getRemainingAttempts(cardId);
        if (remaining > 0 && remaining < MAX_LOGIN_ATTEMPTS) {
            updateHintLabel(QStringLiteral("剩余尝试次数：%1").arg(remaining), true);
        }

        // 发起登录请求
        m_authController->handleStudentLogin(cardId, password);
    } else {
        // 管理员登录
        m_authController->handleAdminLogin(password);
    }
}

void LoginDialog::onRegisterClicked() {
    emit registerRequested();
}

void LoginDialog::onLoginSuccess(UserRole /*role*/, const QString& /*cardId*/,
                                  const QString& /*userName*/) {
    resetInputs();
    updateHintLabel(QStringLiteral("请选择角色并输入登录信息"), false);
    accept();  // 关闭对话框
}

void LoginDialog::onLoginFailed(LoginResult result, const QString& message) {
    updateHintLabel(message, true);
    ElaMessageBar::error(ElaMessageBarType::TopRight, QStringLiteral("登录失败"), message, 3000,
                         this);
}

void LoginDialog::onPasswordError(int remainingAttempts) {
    QString message;
    if (remainingAttempts > 0) {
        message = QStringLiteral("密码错误，剩余尝试次数：%1").arg(remainingAttempts);
    } else {
        message = QStringLiteral("密码错误次数过多，卡已被冻结");
    }
    updateHintLabel(message, true);
}

void LoginDialog::onCardFrozen(const QString& /*cardId*/) {
    updateHintLabel(QStringLiteral("卡已冻结，请联系管理员解冻"), true);
}

void LoginDialog::updateHintLabel(const QString& message, bool isError) {
    m_hintLabel->setText(message);
    if (isError) {
        QColor errorColor = ElaThemeColor(eTheme->getThemeMode(), StatusDanger);
        m_hintLabel->setStyleSheet(QStringLiteral("color: %1;").arg(errorColor.name()));
    } else {
        m_hintLabel->setStyleSheet(QString());
    }
}

void LoginDialog::resetInputs() {
    m_cardIdEdit->clear();
    m_passwordEdit->clear();
}

}  // namespace CampusCard
