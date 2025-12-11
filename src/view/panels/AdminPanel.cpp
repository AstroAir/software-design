/**
 * @file AdminPanel.cpp
 * @brief 管理员控制面板实现
 * @author CampusCardSystem
 * @date 2024
 *
 * MVC架构 - View层实现
 */

#include "AdminPanel.h"

#include "ElaLineEdit.h"
#include "ElaMessageBar.h"
#include "ElaPushButton.h"
#include "ElaTableView.h"
#include "ElaText.h"
#include "ElaTheme.h"
#include "view/dialogs/RechargeDialog.h"
#include "view/dialogs/RegisterDialog.h"
#include "view/widgets/StatisticsWidget.h"

#include <QDate>
#include <QDialog>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QInputDialog>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QVBoxLayout>


namespace CampusCard {

AdminPanel::AdminPanel(MainController* mainController, QWidget* parent)
    : QWidget(parent), m_mainController(mainController),
      m_cardController(mainController->cardController()),
      m_recordController(mainController->recordController()),
      m_authController(mainController->authController()) {
    initUI();
    initConnections();
    refresh();
}

void AdminPanel::initUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // 标题
    auto* titleLabel = new ElaText(QStringLiteral("管理员控制面板"), this);
    titleLabel->setTextPixelSize(24);
    mainLayout->addWidget(titleLabel);

    // ========== 统计信息区域 ==========
    QGroupBox* statsGroup = new QGroupBox(QStringLiteral("📊 系统概览"), this);
    QHBoxLayout* statsLayout = new QHBoxLayout(statsGroup);

    m_totalCardsLabel = new ElaText(QStringLiteral("卡总数：0"), statsGroup);
    m_totalCardsLabel->setTextPixelSize(14);
    m_todayIncomeLabel = new ElaText(QStringLiteral("今日收入：0.00 元"), statsGroup);
    m_todayIncomeLabel->setTextPixelSize(14);
    m_onlineCountLabel = new ElaText(QStringLiteral("当前在线：0"), statsGroup);
    m_onlineCountLabel->setTextPixelSize(14);

    statsLayout->addWidget(m_totalCardsLabel);
    statsLayout->addWidget(m_todayIncomeLabel);
    statsLayout->addWidget(m_onlineCountLabel);
    statsLayout->addStretch();
    mainLayout->addWidget(statsGroup);

    // ========== 搜索和卡列表区域 ==========
    QGroupBox* cardGroup = new QGroupBox(QStringLiteral("💳 卡管理"), this);
    QVBoxLayout* cardLayout = new QVBoxLayout(cardGroup);

    // 搜索栏
    QHBoxLayout* searchLayout = new QHBoxLayout();
    m_searchEdit = new ElaLineEdit(cardGroup);
    m_searchEdit->setPlaceholderText(QStringLiteral("搜索卡号、姓名或学号..."));
    searchLayout->addWidget(m_searchEdit);
    searchLayout->addStretch();
    cardLayout->addLayout(searchLayout);

    // 卡列表表格
    m_cardTable = new ElaTableView(cardGroup);
    m_cardModel = new QStandardItemModel(this);
    m_cardModel->setHorizontalHeaderLabels({QStringLiteral("卡号"), QStringLiteral("姓名"),
                                            QStringLiteral("学号"), QStringLiteral("余额"),
                                            QStringLiteral("状态"), QStringLiteral("累计充值")});
    m_cardTable->setModel(m_cardModel);
    m_cardTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_cardTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_cardTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_cardTable->horizontalHeader()->setStretchLastSection(true);
    m_cardTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    cardLayout->addWidget(m_cardTable, 1);

    // 卡操作按钮
    QHBoxLayout* cardBtnLayout = new QHBoxLayout();
    m_rechargeBtn = new ElaPushButton(QStringLiteral("💰 充值"), cardGroup);
    m_reportLostBtn = new ElaPushButton(QStringLiteral("🔒 挂失"), cardGroup);
    m_cancelLostBtn = new ElaPushButton(QStringLiteral("🔓 解挂"), cardGroup);
    m_unfreezeBtn = new ElaPushButton(QStringLiteral("❄️ 解冻"), cardGroup);
    m_resetPasswordBtn = new ElaPushButton(QStringLiteral("🔑 重置密码"), cardGroup);
    m_addCardBtn = new ElaPushButton(QStringLiteral("➕ 添加卡"), cardGroup);

    cardBtnLayout->addWidget(m_rechargeBtn);
    cardBtnLayout->addWidget(m_reportLostBtn);
    cardBtnLayout->addWidget(m_cancelLostBtn);
    cardBtnLayout->addWidget(m_unfreezeBtn);
    cardBtnLayout->addWidget(m_resetPasswordBtn);
    cardBtnLayout->addWidget(m_addCardBtn);
    cardBtnLayout->addStretch();
    cardLayout->addLayout(cardBtnLayout);

    mainLayout->addWidget(cardGroup, 1);

    // ========== 系统操作区域 ==========
    QGroupBox* sysGroup = new QGroupBox(QStringLiteral("⚙️ 系统操作"), this);
    QHBoxLayout* sysLayout = new QHBoxLayout(sysGroup);

    m_statisticsBtn = new ElaPushButton(QStringLiteral("📈 统计报表"), sysGroup);
    m_exportBtn = new ElaPushButton(QStringLiteral("📤 导出数据"), sysGroup);
    m_importBtn = new ElaPushButton(QStringLiteral("📥 导入数据"), sysGroup);
    m_mockDataBtn = new ElaPushButton(QStringLiteral("🎲 生成测试数据"), sysGroup);
    m_changeAdminPwdBtn = new ElaPushButton(QStringLiteral("🔐 修改管理员密码"), sysGroup);

    sysLayout->addWidget(m_statisticsBtn);
    sysLayout->addWidget(m_exportBtn);
    sysLayout->addWidget(m_importBtn);
    sysLayout->addWidget(m_mockDataBtn);
    sysLayout->addWidget(m_changeAdminPwdBtn);
    sysLayout->addStretch();
    mainLayout->addWidget(sysGroup);

    // ========== 底部登出按钮 ==========
    QHBoxLayout* bottomLayout = new QHBoxLayout();
    bottomLayout->addStretch();
    m_logoutBtn = new ElaPushButton(QStringLiteral("🚪 退出登录"), this);
    bottomLayout->addWidget(m_logoutBtn);
    mainLayout->addLayout(bottomLayout);

    // 初始化按钮状态
    updateButtonStates();
}

void AdminPanel::initConnections() {
    // 主题切换时刷新列表以更新颜色
    connect(eTheme, &ElaTheme::themeModeChanged, this, &AdminPanel::refreshCardList);

    // 卡操作按钮
    connect(m_rechargeBtn, &ElaPushButton::clicked, this, &AdminPanel::onRechargeClicked);
    connect(m_reportLostBtn, &ElaPushButton::clicked, this, &AdminPanel::onReportLostClicked);
    connect(m_cancelLostBtn, &ElaPushButton::clicked, this, &AdminPanel::onCancelLostClicked);
    connect(m_unfreezeBtn, &ElaPushButton::clicked, this, &AdminPanel::onUnfreezeClicked);
    connect(m_resetPasswordBtn, &ElaPushButton::clicked, this, &AdminPanel::onResetPasswordClicked);
    connect(m_addCardBtn, &ElaPushButton::clicked, this, &AdminPanel::onAddCardClicked);

    // 系统操作按钮
    connect(m_statisticsBtn, &ElaPushButton::clicked, this, &AdminPanel::onStatisticsClicked);
    connect(m_exportBtn, &ElaPushButton::clicked, this, &AdminPanel::onExportClicked);
    connect(m_importBtn, &ElaPushButton::clicked, this, &AdminPanel::onImportClicked);
    connect(m_mockDataBtn, &ElaPushButton::clicked, this, &AdminPanel::onGenerateMockDataClicked);
    connect(m_changeAdminPwdBtn, &ElaPushButton::clicked, this,
            &AdminPanel::onChangeAdminPasswordClicked);

    // 登出按钮
    connect(m_logoutBtn, &ElaPushButton::clicked, this, &AdminPanel::logoutRequested);

    // 搜索
    connect(m_searchEdit, &ElaLineEdit::textChanged, this, &AdminPanel::onSearchTextChanged);

    // 表格选择变化
    connect(m_cardTable->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &AdminPanel::updateButtonStates);

    // 控制器信号
    connect(m_cardController, &CardController::cardsUpdated, this, &AdminPanel::onCardsUpdated);
    connect(m_cardController, &CardController::cardUpdated, this, &AdminPanel::onCardUpdated);

    // 操作结果信号
    connect(m_cardController, &CardController::reportLostSuccess, this,
            [this](const QString& cardId) {
                ElaMessageBar::success(ElaMessageBarType::TopRight, QStringLiteral("成功"),
                                       QStringLiteral("卡 %1 挂失成功").arg(cardId), 2000, this);
            });
    connect(m_cardController, &CardController::cancelLostSuccess, this,
            [this](const QString& cardId) {
                ElaMessageBar::success(ElaMessageBarType::TopRight, QStringLiteral("成功"),
                                       QStringLiteral("卡 %1 解挂成功").arg(cardId), 2000, this);
            });
    connect(m_cardController, &CardController::unfreezeSuccess, this,
            [this](const QString& cardId) {
                ElaMessageBar::success(ElaMessageBarType::TopRight, QStringLiteral("成功"),
                                       QStringLiteral("卡 %1 解冻成功").arg(cardId), 2000, this);
            });
    connect(m_cardController, &CardController::passwordReset, this, [this](const QString& cardId) {
        ElaMessageBar::success(ElaMessageBarType::TopRight, QStringLiteral("成功"),
                               QStringLiteral("卡 %1 密码重置成功").arg(cardId), 2000, this);
    });
    connect(m_cardController, &CardController::operationFailed, this,
            [this](const QString& message) {
                ElaMessageBar::error(ElaMessageBarType::TopRight, QStringLiteral("失败"), message,
                                     2000, this);
            });

    // 管理员密码修改
    connect(m_authController, &AuthController::adminPasswordChanged, this, [this]() {
        ElaMessageBar::success(ElaMessageBarType::TopRight, QStringLiteral("成功"),
                               QStringLiteral("管理员密码修改成功"), 2000, this);
    });
    connect(m_authController, &AuthController::adminPasswordChangeFailed, this,
            [this](const QString& message) {
                ElaMessageBar::error(ElaMessageBarType::TopRight, QStringLiteral("失败"), message,
                                     2000, this);
            });

    // 数据导入导出
    connect(m_mainController, &MainController::exportSuccess, this, [this]() {
        ElaMessageBar::success(ElaMessageBarType::TopRight, QStringLiteral("成功"),
                               QStringLiteral("数据导出成功"), 2000, this);
    });
    connect(m_mainController, &MainController::importSuccess, this, [this]() {
        ElaMessageBar::success(ElaMessageBarType::TopRight, QStringLiteral("成功"),
                               QStringLiteral("数据导入成功"), 2000, this);
        refresh();
    });
    connect(m_mainController, &MainController::mockDataGenerated, this, [this](int count) {
        ElaMessageBar::success(ElaMessageBarType::TopRight, QStringLiteral("成功"),
                               QStringLiteral("生成 %1 张测试卡成功").arg(count), 2000, this);
        refresh();
    });
}

void AdminPanel::refresh() {
    refreshCardList();
    refreshStatistics();
}

void AdminPanel::refreshCardList() {
    QString keyword = m_searchEdit->text().trimmed();
    QList<Card> cards = m_cardController->searchCards(keyword);

    m_cardModel->removeRows(0, m_cardModel->rowCount());

    for (const auto& card : cards) {
        QList<QStandardItem*> row;
        row << new QStandardItem(card.cardId());
        row << new QStandardItem(card.name());
        row << new QStandardItem(card.studentId());
        row << new QStandardItem(QString::number(card.balance(), 'f', 2));
        row << new QStandardItem(cardStateToString(card.state()));
        row << new QStandardItem(QString::number(card.totalRecharge(), 'f', 2));

        // 根据状态设置颜色（适配明暗主题）
        if (card.state() == CardState::Lost) {
            // 挂失状态：明亮模式用深橙色，暗黑模式用浅橙色
            QColor lostColor = (eTheme->getThemeMode() == ElaThemeType::Light)
                                   ? QColor(230, 126, 34)   // 深橙色
                                   : QColor(255, 183, 77);  // 浅橙色
            for (auto* item : row) {
                item->setForeground(lostColor);
            }
        } else if (card.state() == CardState::Frozen) {
            // 冻结状态：使用主题的危险色
            QColor frozenColor = ElaThemeColor(eTheme->getThemeMode(), StatusDanger);
            for (auto* item : row) {
                item->setForeground(frozenColor);
            }
        }

        m_cardModel->appendRow(row);
    }

    updateButtonStates();
}

void AdminPanel::refreshStatistics() {
    int totalCards = m_cardController->getCardCount();
    QString today = QDate::currentDate().toString(QStringLiteral("yyyy-MM-dd"));
    double todayIncome = m_recordController->getDailyIncome(today);
    int onlineCount = m_recordController->getOnlineCount();

    m_totalCardsLabel->setText(QStringLiteral("卡总数：%1").arg(totalCards));
    m_todayIncomeLabel->setText(
        QStringLiteral("今日收入：%1 元").arg(QString::number(todayIncome, 'f', 2)));
    m_onlineCountLabel->setText(QStringLiteral("当前在线：%1").arg(onlineCount));
}

QString AdminPanel::getSelectedCardId() const {
    QModelIndexList selection = m_cardTable->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        return QString();
    }
    return m_cardModel->item(selection.first().row(), 0)->text();
}

void AdminPanel::updateButtonStates() {
    QString cardId = getSelectedCardId();
    bool hasSelection = !cardId.isEmpty();

    m_rechargeBtn->setEnabled(hasSelection);
    m_resetPasswordBtn->setEnabled(hasSelection);

    if (hasSelection) {
        Card card = m_cardController->getCard(cardId);
        m_reportLostBtn->setEnabled(card.state() == CardState::Normal);
        m_cancelLostBtn->setEnabled(card.state() == CardState::Lost);
        m_unfreezeBtn->setEnabled(card.state() == CardState::Frozen);
    } else {
        m_reportLostBtn->setEnabled(false);
        m_cancelLostBtn->setEnabled(false);
        m_unfreezeBtn->setEnabled(false);
    }
}

void AdminPanel::onCardsUpdated() {
    refreshCardList();
    refreshStatistics();
}

void AdminPanel::onCardUpdated(const QString& /*cardId*/) {
    refreshCardList();
    refreshStatistics();
}

void AdminPanel::onSearchTextChanged(const QString& /*text*/) {
    refreshCardList();
}

// ========== 卡操作槽实现 ==========

void AdminPanel::onRechargeClicked() {
    QString cardId = getSelectedCardId();
    if (cardId.isEmpty()) {
        ElaMessageBar::warning(ElaMessageBarType::TopRight, QStringLiteral("提示"),
                               QStringLiteral("请先选择一张卡"), 2000, this);
        return;
    }

    RechargeDialog dialog(m_cardController, cardId, this);
    dialog.exec();
}

void AdminPanel::onReportLostClicked() {
    QString cardId = getSelectedCardId();
    if (cardId.isEmpty()) {
        ElaMessageBar::warning(ElaMessageBarType::TopRight, QStringLiteral("提示"),
                               QStringLiteral("请先选择一张卡"), 2000, this);
        return;
    }

    m_cardController->handleReportLost(cardId);
}

void AdminPanel::onCancelLostClicked() {
    QString cardId = getSelectedCardId();
    if (cardId.isEmpty()) {
        ElaMessageBar::warning(ElaMessageBarType::TopRight, QStringLiteral("提示"),
                               QStringLiteral("请先选择一张卡"), 2000, this);
        return;
    }

    m_cardController->handleCancelLost(cardId);
}

void AdminPanel::onUnfreezeClicked() {
    QString cardId = getSelectedCardId();
    if (cardId.isEmpty()) {
        ElaMessageBar::warning(ElaMessageBarType::TopRight, QStringLiteral("提示"),
                               QStringLiteral("请先选择一张卡"), 2000, this);
        return;
    }

    m_cardController->handleUnfreeze(cardId);
}

void AdminPanel::onResetPasswordClicked() {
    QString cardId = getSelectedCardId();
    if (cardId.isEmpty()) {
        ElaMessageBar::warning(ElaMessageBarType::TopRight, QStringLiteral("提示"),
                               QStringLiteral("请先选择一张卡"), 2000, this);
        return;
    }

    bool ok;
    QString newPassword = QInputDialog::getText(this, QStringLiteral("重置密码"),
                                                QStringLiteral("请输入新密码（至少4位）："),
                                                QLineEdit::Password, QString(), &ok);

    if (ok && !newPassword.isEmpty()) {
        m_cardController->handleResetPassword(cardId, newPassword);
    }
}

void AdminPanel::onAddCardClicked() {
    RegisterDialog dialog(m_cardController, this);
    dialog.exec();
}

// ========== 系统操作槽实现 ==========

void AdminPanel::onStatisticsClicked() {
    QDialog dialog(this);
    dialog.setWindowTitle(QStringLiteral("统计报表"));
    dialog.setMinimumSize(800, 600);

    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    auto* statsWidget = new StatisticsWidget(m_recordController, m_cardController, &dialog);
    layout->addWidget(statsWidget);

    dialog.exec();
}

void AdminPanel::onExportClicked() {
    QString filePath = QFileDialog::getSaveFileName(this, QStringLiteral("导出数据"),
                                                    QStringLiteral("campus_card_data.json"),
                                                    QStringLiteral("JSON Files (*.json)"));
    if (!filePath.isEmpty()) {
        m_mainController->exportData(filePath);
    }
}

void AdminPanel::onImportClicked() {
    QString filePath = QFileDialog::getOpenFileName(this, QStringLiteral("导入数据"), QString(),
                                                    QStringLiteral("JSON Files (*.json)"));
    if (!filePath.isEmpty()) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, QStringLiteral("导入方式"),
            QStringLiteral("选择导入方式：\n\n是 - 合并（保留现有数据）\n否 - 覆盖（清空现有数据）"),
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);

        if (reply == QMessageBox::Cancel) {
            return;
        }

        bool merge = (reply == QMessageBox::Yes);
        m_mainController->importData(filePath, merge);
    }
}

void AdminPanel::onGenerateMockDataClicked() {
    bool ok;
    int count = QInputDialog::getInt(this, QStringLiteral("生成测试数据"),
                                     QStringLiteral("请输入要生成的卡数量："), 10, 1, 100, 1, &ok);
    if (ok) {
        int recordsPerCard = QInputDialog::getInt(this, QStringLiteral("生成测试数据"),
                                                  QStringLiteral("每张卡的记录数量："), 5, 0, 50,
                                                  1, &ok);
        if (ok) {
            m_mainController->generateMockData(count, recordsPerCard);
        }
    }
}

void AdminPanel::onChangeAdminPasswordClicked() {
    bool ok;
    QString oldPassword = QInputDialog::getText(this, QStringLiteral("修改管理员密码"),
                                                QStringLiteral("请输入当前密码："),
                                                QLineEdit::Password, QString(), &ok);
    if (!ok || oldPassword.isEmpty()) {
        return;
    }

    QString newPassword = QInputDialog::getText(this, QStringLiteral("修改管理员密码"),
                                                QStringLiteral("请输入新密码（至少4位）："),
                                                QLineEdit::Password, QString(), &ok);
    if (!ok || newPassword.isEmpty()) {
        return;
    }

    QString confirmPassword = QInputDialog::getText(this, QStringLiteral("修改管理员密码"),
                                                    QStringLiteral("请再次输入新密码："),
                                                    QLineEdit::Password, QString(), &ok);
    if (!ok) {
        return;
    }

    if (newPassword != confirmPassword) {
        ElaMessageBar::warning(ElaMessageBarType::TopRight, QStringLiteral("提示"),
                               QStringLiteral("两次输入的密码不一致"), 2000, this);
        return;
    }

    m_authController->handleChangeAdminPassword(oldPassword, newPassword);
}

}  // namespace CampusCard
