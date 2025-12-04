/**
 * @file MainWindow.cpp
 * @brief 主窗口实现
 * @author CampusCardSystem
 * @date 2024
 */

#include "MainWindow.h"

#include "AdminDashboard.h"
#include "ElaContentDialog.h"
#include "ElaMenu.h"
#include "ElaMenuBar.h"
#include "ElaMessageBar.h"
#include "ElaText.h"
#include "ElaTheme.h"
#include "LoginDialog.h"
#include "StudentDashboard.h"
#include "core/StorageManager.h"

#include <QApplication>
#include <QDir>
#include <QFont>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QKeySequence>
#include <QLabel>
#include <QMessageBox>
#include <QPalette>
#include <QShortcut>
#include <QVBoxLayout>


namespace CampusCard {

MainWindow::MainWindow(QWidget* parent) : ElaWindow(parent) {
    // 初始化管理器
    initManagers();

    // 初始化UI
    initUI();

    // 初始化信号槽
    initConnections();

    // 显示登录对话框
    QMetaObject::invokeMethod(this, &MainWindow::showLoginDialog, Qt::QueuedConnection);
}

MainWindow::~MainWindow() {
    // 管理器会自动清理
}

void MainWindow::initManagers() {
    // 设置数据目录
    QString dataPath = QApplication::applicationDirPath() + QStringLiteral("/data");
    StorageManager::instance().setDataPath(dataPath);
    StorageManager::instance().initializeDataDirectory();

    // 创建管理器
    m_cardManager = new CardManager(this);
    m_recordManager = new RecordManager(this);
    m_authManager = new AuthManager(m_cardManager, this);

    // 初始化管理器
    m_cardManager->initialize();
    m_recordManager->initialize();
}

void MainWindow::initUI() {
    // 设置窗口属性
    setWindowTitle(QStringLiteral("校园卡消费记录查询系统"));
    resize(1200, 800);
    setWindowIcon(QIcon(QStringLiteral(":/icons/app.png")));

    // 设置用户信息区域
    setUserInfoCardVisible(true);
    setUserInfoCardTitle(QStringLiteral("校园卡系统"));
    setUserInfoCardSubTitle(QStringLiteral("请登录"));

    // 创建欢迎页面
    showWelcomePage();

    // 创建子界面
    m_adminDashboard = new AdminDashboard(m_cardManager, m_recordManager, this);
    m_studentDashboard = new StudentDashboard(m_cardManager, m_recordManager, this);

    // 添加页面到导航
    addPageNode(QStringLiteral("主页"), m_welcomePage, ElaIconType::House);
    m_homeKey = m_welcomePage->property("ElaPageKey").toString();

    // 添加页脚节点（无页面，通过信号处理点击）
    addFooterNode(QStringLiteral("登录"), m_loginFooterKey, 0, ElaIconType::RightToBracket);
    addFooterNode(QStringLiteral("关于"), m_aboutFooterKey, 0, ElaIconType::CircleInfo);

    // 连接导航信号 - 处理页脚节点点击
    connect(this, &ElaWindow::navigationNodeClicked, this,
            [this](ElaNavigationType::NavigationNodeType nodeType, const QString& nodeKey) {
                Q_UNUSED(nodeType)
                if (nodeKey == m_loginFooterKey) {
                    if (m_authManager->isLoggedIn()) {
                        m_authManager->logout();
                    } else {
                        showLoginDialog();
                    }
                } else if (nodeKey == m_aboutFooterKey) {
                    showAboutDialog();
                }
            });
}

void MainWindow::initConnections() {
    // 登录成功
    connect(m_authManager, &AuthManager::loginSucceeded, this, &MainWindow::onLoginSucceeded);

    // 登出
    connect(m_authManager, &AuthManager::loggedOut, this, &MainWindow::onLogout);

    // 管理员界面登出请求
    connect(m_adminDashboard, &AdminDashboard::logoutRequested, m_authManager,
            &AuthManager::logout);

    // 学生界面登出请求
    connect(m_studentDashboard, &StudentDashboard::logoutRequested, m_authManager,
            &AuthManager::logout);

    // 调试快捷键：Ctrl+Shift+D 跳过登录，以管理员身份进入
    auto* skipLoginShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_D), this);
    skipLoginShortcut->setContext(Qt::ApplicationShortcut);
    connect(skipLoginShortcut, &QShortcut::activated, this, [this]() {
        if (!m_authManager->isLoggedIn()) {
            // 模拟管理员登录
            m_authManager->adminLogin(QStringLiteral("admin123"));
        }
    });
}

void MainWindow::showWelcomePage() {
    if (!m_welcomePage) {
        m_welcomePage = new QWidget(this);
        m_welcomePage->setObjectName(QStringLiteral("WelcomePage"));

        auto* mainLayout = new QVBoxLayout(m_welcomePage);
        mainLayout->setAlignment(Qt::AlignCenter);
        mainLayout->setContentsMargins(60, 40, 60, 40);
        mainLayout->setSpacing(20);

        mainLayout->addStretch(1);

        // ========== 头部区域 ==========
        // Logo图标（使用emoji替代）
        auto* logoLabel = new QLabel(m_welcomePage);
        logoLabel->setText(QStringLiteral("🎓"));
        QFont logoFont;
        logoFont.setPointSize(60);
        logoLabel->setFont(logoFont);
        logoLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(logoLabel);

        // 欢迎标题
        auto* titleLabel = new QLabel(m_welcomePage);
        titleLabel->setText(QStringLiteral("校园卡消费记录查询系统"));
        QFont titleFont;
        titleFont.setPointSize(32);
        titleFont.setBold(true);
        titleLabel->setFont(titleFont);
        titleLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(titleLabel);

        // 副标题
        auto* subLabel = new QLabel(m_welcomePage);
        subLabel->setText(QStringLiteral("便捷管理 • 高效查询 • 安全可靠"));
        QFont subFont;
        subFont.setPointSize(14);
        subLabel->setFont(subFont);
        subLabel->setAlignment(Qt::AlignCenter);
        QPalette subPal = subLabel->palette();
        subPal.setColor(QPalette::WindowText, QColor(100, 100, 100));
        subLabel->setPalette(subPal);
        mainLayout->addWidget(subLabel);

        mainLayout->addSpacing(30);

        // ========== 功能卡片区域 ==========
        auto* cardsLayout = new QHBoxLayout();
        cardsLayout->setSpacing(20);
        cardsLayout->setAlignment(Qt::AlignCenter);

        // 功能卡片样式
        auto createFeatureCard = [this](const QString& icon, const QString& title,
                                        const QString& desc) -> QFrame* {
            auto* card = new QFrame(m_welcomePage);
            card->setObjectName(QStringLiteral("FeatureCard"));
            card->setFixedSize(200, 160);
            card->setStyleSheet(QStringLiteral("QFrame#FeatureCard {"
                                               "    background-color: rgba(255, 255, 255, 0.8);"
                                               "    border-radius: 12px;"
                                               "    border: 1px solid rgba(0, 0, 0, 0.05);"
                                               "}"
                                               "QFrame#FeatureCard:hover {"
                                               "    background-color: rgba(255, 255, 255, 0.95);"
                                               "    border: 1px solid rgba(64, 158, 255, 0.3);"
                                               "}"));

            auto* cardLayout = new QVBoxLayout(card);
            cardLayout->setAlignment(Qt::AlignCenter);
            cardLayout->setSpacing(8);
            cardLayout->setContentsMargins(15, 20, 15, 20);

            auto* iconLabel = new QLabel(card);
            iconLabel->setText(icon);
            QFont iconFont;
            iconFont.setPointSize(32);
            iconLabel->setFont(iconFont);
            iconLabel->setAlignment(Qt::AlignCenter);
            cardLayout->addWidget(iconLabel);

            auto* titleLbl = new QLabel(card);
            titleLbl->setText(title);
            QFont titleFnt;
            titleFnt.setPointSize(13);
            titleFnt.setBold(true);
            titleLbl->setFont(titleFnt);
            titleLbl->setAlignment(Qt::AlignCenter);
            cardLayout->addWidget(titleLbl);

            auto* descLabel = new QLabel(card);
            descLabel->setText(desc);
            descLabel->setWordWrap(true);
            QFont descFont;
            descFont.setPointSize(10);
            descLabel->setFont(descFont);
            descLabel->setAlignment(Qt::AlignCenter);
            QPalette descPal = descLabel->palette();
            descPal.setColor(QPalette::WindowText, QColor(120, 120, 120));
            descLabel->setPalette(descPal);
            cardLayout->addWidget(descLabel);

            // 添加阴影效果
            auto* shadow = new QGraphicsDropShadowEffect(card);
            shadow->setBlurRadius(20);
            shadow->setColor(QColor(0, 0, 0, 30));
            shadow->setOffset(0, 4);
            card->setGraphicsEffect(shadow);

            return card;
        };

        // 学生功能卡片
        cardsLayout->addWidget(createFeatureCard(QStringLiteral("💳"), QStringLiteral("余额查询"),
                                                 QStringLiteral("实时查看卡内余额")));

        cardsLayout->addWidget(createFeatureCard(QStringLiteral("📊"), QStringLiteral("上机记录"),
                                                 QStringLiteral("查看历史消费明细")));

        cardsLayout->addWidget(createFeatureCard(QStringLiteral("⚙️"), QStringLiteral("卡片管理"),
                                                 QStringLiteral("充值、挂失、解冻")));

        cardsLayout->addWidget(createFeatureCard(QStringLiteral("📈"), QStringLiteral("统计报表"),
                                                 QStringLiteral("数据分析与导出")));

        mainLayout->addLayout(cardsLayout);

        mainLayout->addSpacing(30);

        // ========== 快速开始提示 ==========
        auto* guideFrame = new QFrame(m_welcomePage);
        guideFrame->setObjectName(QStringLiteral("GuideFrame"));
        guideFrame->setStyleSheet(QStringLiteral("QFrame#GuideFrame {"
                                                 "    background-color: rgba(64, 158, 255, 0.1);"
                                                 "    border-radius: 10px;"
                                                 "    border: 1px solid rgba(64, 158, 255, 0.2);"
                                                 "}"));

        auto* guideLayout = new QVBoxLayout(guideFrame);
        guideLayout->setContentsMargins(30, 20, 30, 20);
        guideLayout->setSpacing(10);

        auto* guideTitle = new QLabel(guideFrame);
        guideTitle->setText(QStringLiteral("🚀 快速开始"));
        QFont guideTitleFont;
        guideTitleFont.setPointSize(14);
        guideTitleFont.setBold(true);
        guideTitle->setFont(guideTitleFont);
        guideLayout->addWidget(guideTitle);

        auto* guideText = new QLabel(guideFrame);
        guideText->setText(QStringLiteral(
            "【角色选择】\n"
            "• 学生用户：输入卡号和密码登录，可查询余额、上机记录和统计信息\n"
            "• 管理员用户：选择管理员身份，输入管理员密码(默认: admin123)\n\n"
            "【功能说明】\n"
            "• 学生功能：开始/结束上机、查看余额、查询上机记录、修改密码\n"
            "• 管理员功能：充值、挂失/解挂、解冻、重置密码、统计报表、数据导入导出\n\n"
            "【操作步骤】\n"
            "1. 点击左下角 [登录] 按钮\n"
            "2. 选择角色并输入登录信息\n"
            "3. 新用户可点击 [注册] 按钮创建新卡"));
        QFont guideTextFont;
        guideTextFont.setPointSize(11);
        guideText->setFont(guideTextFont);
        guideText->setWordWrap(true);
        QPalette guidePal = guideText->palette();
        guidePal.setColor(QPalette::WindowText, QColor(80, 80, 80));
        guideText->setPalette(guidePal);
        guideLayout->addWidget(guideText);

        mainLayout->addWidget(guideFrame);

        // 快捷键提示（更小更subtle）
        auto* shortcutLabel = new QLabel(m_welcomePage);
        shortcutLabel->setText(QStringLiteral("💡 开发调试: Ctrl+Shift+D 跳过登录"));
        QFont hintFont;
        hintFont.setPointSize(9);
        shortcutLabel->setFont(hintFont);
        shortcutLabel->setAlignment(Qt::AlignCenter);
        QPalette pal = shortcutLabel->palette();
        pal.setColor(QPalette::WindowText, QColor(180, 180, 180));
        shortcutLabel->setPalette(pal);
        mainLayout->addWidget(shortcutLabel);

        mainLayout->addStretch(2);
    }
}

void MainWindow::showLoginDialog() {
    if (!m_loginDialog) {
        m_loginDialog = new LoginDialog(m_authManager, this);
        m_loginDialog->setCardManager(m_cardManager);
    }
    m_loginDialog->exec();
}

void MainWindow::showAboutDialog() {
    // 使用QMessageBox替代ElaContentDialog，避免显示问题
    QMessageBox aboutBox(this);
    aboutBox.setWindowTitle(QStringLiteral("关于"));
    aboutBox.setIcon(QMessageBox::Information);
    aboutBox.setText(QStringLiteral("校园卡消费记录查询系统"));
    aboutBox.setInformativeText(QStringLiteral("版本 1.0.0\n\n"
                                               "基于 Qt6 + ElaWidgetTools 开发\n"
                                               "支持学生上机记录查询、管理员充值挂失等功能\n\n"
                                               "功能特性：\n"
                                               "• 学生登录查询余额和上机记录\n"
                                               "• 管理员充值、挂失、解冻、重置密码\n"
                                               "• 上机计费和统计报表\n"
                                               "• 数据导入导出"));
    aboutBox.setStandardButtons(QMessageBox::Ok);
    aboutBox.exec();
}

void MainWindow::onLoginSucceeded(UserRole role, const QString& cardId) {
    if (role == UserRole::Admin) {
        setUserInfoCardSubTitle(QStringLiteral("管理员"));
        switchToAdminDashboard();
        ElaMessageBar::success(ElaMessageBarType::TopRight, QStringLiteral("登录成功"),
                               QStringLiteral("欢迎管理员"), 2000, this);
    } else {
        // 获取学生信息
        Card* card = m_cardManager->findCard(cardId);
        if (card) {
            setUserInfoCardSubTitle(card->name() + QStringLiteral(" (") + cardId +
                                    QStringLiteral(")"));
            m_studentDashboard->setCurrentCard(cardId);
            switchToStudentDashboard();
            ElaMessageBar::success(ElaMessageBarType::TopRight, QStringLiteral("登录成功"),
                                   QStringLiteral("欢迎 ") + card->name(), 2000, this);
        }
    }
}

void MainWindow::onLogout() {
    setUserInfoCardSubTitle(QStringLiteral("请登录"));

    // 先导航到主页
    navigation(m_homeKey);

    // 清理学生界面状态
    m_studentDashboard->setCurrentCard(QString());

    ElaMessageBar::information(ElaMessageBarType::TopRight, QStringLiteral("已登出"),
                               QStringLiteral("您已安全退出系统"), 2000, this);
}

void MainWindow::switchToAdminDashboard() {
    // 如果还没添加管理员页面，添加它
    if (m_adminKey.isEmpty()) {
        addPageNode(QStringLiteral("管理员面板"), m_adminDashboard, ElaIconType::UserGear);
        m_adminKey = m_adminDashboard->property("ElaPageKey").toString();
    }
    m_adminDashboard->refresh();
    navigation(m_adminKey);
}

void MainWindow::switchToStudentDashboard() {
    // 如果还没添加学生页面，添加它
    if (m_studentKey.isEmpty()) {
        addPageNode(QStringLiteral("我的校园卡"), m_studentDashboard, ElaIconType::User);
        m_studentKey = m_studentDashboard->property("ElaPageKey").toString();
    }
    m_studentDashboard->refresh();
    navigation(m_studentKey);
}

void MainWindow::initSuggestBox() {
    // ElaWindow 导航栏已内置搜索功能，此处留空
}

void MainWindow::updateSuggestions() {
    // ElaWindow 导航栏已内置搜索功能，此处留空
}

}  // namespace CampusCard
