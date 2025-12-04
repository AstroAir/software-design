/**
 * @file MainWindow.cpp
 * @brief 主窗口实现
 * @author CampusCardSystem
 * @date 2024
 *
 * MVC架构 - View层实现
 */

#include "MainWindow.h"

#include "ElaContentDialog.h"
#include "ElaMessageBar.h"
#include "ElaPushButton.h"
#include "ElaText.h"
#include "view/dialogs/LoginDialog.h"
#include "view/dialogs/RegisterDialog.h"
#include "view/panels/AdminPanel.h"
#include "view/panels/StudentPanel.h"

#include <QCoreApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>


namespace CampusCard {

MainWindow::MainWindow(QWidget* parent) : ElaWindow(parent) {
    // 初始化控制器
    if (!initController()) {
        ElaMessageBar::error(ElaMessageBarType::TopRight, QStringLiteral("错误"),
                             QStringLiteral("初始化失败"), 3000, this);
    }

    initUI();
    initConnections();

    // 显示欢迎页
    showWelcomePage();
}

MainWindow::~MainWindow() = default;

bool MainWindow::initController() {
    m_mainController = new MainController(this);

    // 获取数据目录路径
    QString dataPath = QCoreApplication::applicationDirPath() + QStringLiteral("/data");

    return m_mainController->initialize(dataPath);
}

void MainWindow::initUI() {
    // 设置窗口属性
    setWindowTitle(QStringLiteral("校园卡消费记录查询系统"));
    resize(1200, 800);
    setMinimumSize(1000, 700);

    // 设置导航栏属性
    setNavigationBarDisplayMode(ElaNavigationType::Minimal);
    setWindowButtonFlag(ElaAppBarType::MaximizeButtonHint, false);

    // 创建页面堆叠容器
    m_stackedWidget = new QStackedWidget(this);

    // 创建欢迎页
    m_welcomePage = new QWidget(this);
    QVBoxLayout* welcomeLayout = new QVBoxLayout(m_welcomePage);
    welcomeLayout->setAlignment(Qt::AlignCenter);

    auto* welcomeTitle = new ElaText(QStringLiteral("欢迎使用校园卡消费记录查询系统"), m_welcomePage);
    welcomeTitle->setTextPixelSize(32);
    welcomeLayout->addWidget(welcomeTitle, 0, Qt::AlignCenter);

    auto* welcomeSubtitle =
        new ElaText(QStringLiteral("Campus Card Consumption Record Query System"), m_welcomePage);
    welcomeSubtitle->setTextPixelSize(16);
    welcomeLayout->addWidget(welcomeSubtitle, 0, Qt::AlignCenter);

    welcomeLayout->addSpacing(30);

    // 登录按钮
    auto* loginBtn = new ElaPushButton(QStringLiteral("🔐 登录"), m_welcomePage);
    loginBtn->setFixedSize(200, 50);
    connect(loginBtn, &ElaPushButton::clicked, this, &MainWindow::showLoginDialog);
    welcomeLayout->addWidget(loginBtn, 0, Qt::AlignCenter);

    welcomeLayout->addSpacing(10);

    // 关于按钮
    auto* aboutBtn = new ElaPushButton(QStringLiteral("ℹ️ 关于"), m_welcomePage);
    aboutBtn->setFixedSize(200, 50);
    connect(aboutBtn, &ElaPushButton::clicked, this, &MainWindow::showAboutDialog);
    welcomeLayout->addWidget(aboutBtn, 0, Qt::AlignCenter);

    // 创建管理员面板（不添加到导航栏）
    m_adminPanel = new AdminPanel(m_mainController, this);

    // 创建学生面板（不添加到导航栏）
    m_studentPanel = new StudentPanel(m_mainController->cardController(),
                                      m_mainController->recordController(), this);

    // 添加页面到堆叠容器
    m_stackedWidget->addWidget(m_welcomePage);   // index 0
    m_stackedWidget->addWidget(m_adminPanel);    // index 1
    m_stackedWidget->addWidget(m_studentPanel);  // index 2

    // 只添加堆叠容器到导航（作为唯一的主页面）
    addPageNode(QStringLiteral("校园卡系统"), m_stackedWidget, ElaIconType::CreditCardFront);
    m_homeKey = QStringLiteral("校园卡系统");

    // 显示欢迎页
    m_stackedWidget->setCurrentIndex(0);
    navigation(m_homeKey);
}

void MainWindow::initConnections() {
    // 登录成功
    connect(m_mainController->authController(), &AuthController::loginSuccess, this,
            &MainWindow::onLoginSuccess);

    // 登出
    connect(m_mainController->authController(), &AuthController::logoutSuccess, this,
            &MainWindow::onLogout);

    // 管理员面板登出请求
    connect(m_adminPanel, &AdminPanel::logoutRequested, this, [this]() {
        m_mainController->authController()->handleLogout();
    });

    // 学生面板登出请求
    connect(m_studentPanel, &StudentPanel::logoutRequested, this, [this]() {
        m_mainController->authController()->handleLogout();
    });
}

void MainWindow::showLoginDialog() {
    if (!m_loginDialog) {
        m_loginDialog = new LoginDialog(m_mainController->authController(),
                                        m_mainController->cardController(), this);

        // 连接注册请求
        connect(m_loginDialog, &LoginDialog::registerRequested, this,
                &MainWindow::showRegisterDialog);
    }

    m_loginDialog->exec();
}

void MainWindow::showRegisterDialog() {
    RegisterDialog dialog(m_mainController->cardController(), this);
    dialog.exec();
}

void MainWindow::showAboutDialog() {
    ElaContentDialog* aboutDialog = new ElaContentDialog(this);
    aboutDialog->setWindowTitle(QStringLiteral("关于"));
    aboutDialog->setFixedSize(450, 350);

    // 隐藏默认按钮
    aboutDialog->setLeftButtonText(QString());
    aboutDialog->setMiddleButtonText(QString());
    aboutDialog->setRightButtonText(QStringLiteral("确定"));

    QWidget* content = new QWidget(aboutDialog);
    QVBoxLayout* layout = new QVBoxLayout(content);
    layout->setSpacing(15);
    layout->setContentsMargins(30, 30, 30, 30);

    auto* titleLabel = new ElaText(QStringLiteral("校园卡消费记录查询系统"), content);
    titleLabel->setTextPixelSize(20);
    layout->addWidget(titleLabel, 0, Qt::AlignCenter);

    auto* versionLabel = new ElaText(QStringLiteral("版本 1.0.0"), content);
    versionLabel->setTextPixelSize(14);
    layout->addWidget(versionLabel, 0, Qt::AlignCenter);

    layout->addSpacing(10);

    auto* descLabel = new ElaText(
        QStringLiteral("本系统采用标准MVC架构设计，用于管理校园卡的充值、消费和查询功能。\n\n"
                       "技术栈：\n"
                       "• Qt 6 + C++20\n"
                       "• ElaWidgetTools UI框架\n"
                       "• MVC分层架构"),
        content);
    descLabel->setTextPixelSize(12);
    descLabel->setWordWrap(true);
    layout->addWidget(descLabel);

    layout->addStretch();

    auto* copyrightLabel = new ElaText(QStringLiteral("© 2024 CampusCardSystem"), content);
    copyrightLabel->setTextPixelSize(11);
    layout->addWidget(copyrightLabel, 0, Qt::AlignCenter);

    aboutDialog->setCentralWidget(content);
    aboutDialog->exec();
}

void MainWindow::onLoginSuccess(UserRole role, const QString& cardId, const QString& userName) {
    ElaMessageBar::success(ElaMessageBarType::TopRight, QStringLiteral("登录成功"),
                           QStringLiteral("欢迎，") + userName, 2000, this);

    m_isLoggedIn = true;
    m_currentRole = role;

    if (role == UserRole::Admin) {
        switchToAdminPanel();
    } else {
        switchToStudentPanel(cardId);
    }
}

void MainWindow::onLogout() {
    m_isLoggedIn = false;
    m_studentPanel->setCurrentCard(QString());
    showWelcomePage();
    ElaMessageBar::information(ElaMessageBarType::TopRight, QStringLiteral("提示"),
                               QStringLiteral("已退出登录"), 2000, this);
}

void MainWindow::switchToAdminPanel() {
    m_adminPanel->refresh();
    m_stackedWidget->setCurrentIndex(1);  // 管理员面板索引
}

void MainWindow::switchToStudentPanel(const QString& cardId) {
    m_studentPanel->setCurrentCard(cardId);
    m_stackedWidget->setCurrentIndex(2);  // 学生面板索引
}

void MainWindow::showWelcomePage() {
    m_stackedWidget->setCurrentIndex(0);  // 欢迎页索引
}

}  // namespace CampusCard
