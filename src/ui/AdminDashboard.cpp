/**
 * @file AdminDashboard.cpp
 * @brief 管理员控制面板实现
 * @author CampusCardSystem
 * @date 2024
 */

#include "AdminDashboard.h"
#include "RechargeDialog.h"
#include "StatisticsWidget.h"

#include "ElaTableView.h"
#include "ElaPushButton.h"
#include "ElaLineEdit.h"
#include "ElaText.h"
#include "ElaMessageBar.h"
#include "ElaContentDialog.h"
#include "ElaScrollPageArea.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QDate>
#include <QDateEdit>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QGroupBox>

namespace CampusCard {

AdminDashboard::AdminDashboard(CardManager* cardManager, 
                               RecordManager* recordManager,
                               QWidget* parent)
    : QWidget(parent)
    , m_cardManager(cardManager)
    , m_recordManager(recordManager)
{
    initUI();
    initConnections();
}

void AdminDashboard::initUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // 标题区域
    auto* titleLabel = new ElaText(QStringLiteral("🔧 管理员控制面板"), this);
    titleLabel->setTextPixelSize(24);
    mainLayout->addWidget(titleLabel);

    // ========== 统计概览区域 ==========
    QGroupBox* statsGroup = new QGroupBox(QStringLiteral("📊 统计概览"), this);
    QGridLayout* statsLayout = new QGridLayout(statsGroup);
    statsLayout->setSpacing(15);
    
    // 总卡数
    auto* totalCardsTitle = new ElaText(QStringLiteral("总卡数："), statsGroup);
    m_totalCardsLabel = new ElaText(QStringLiteral("0 张"), statsGroup);
    m_totalCardsLabel->setTextPixelSize(16);
    statsLayout->addWidget(totalCardsTitle, 0, 0);
    statsLayout->addWidget(m_totalCardsLabel, 0, 1);
    
    // 当前在线
    auto* onlineTitle = new ElaText(QStringLiteral("当前在线："), statsGroup);
    m_onlineCountLabel = new ElaText(QStringLiteral("0 人"), statsGroup);
    m_onlineCountLabel->setTextPixelSize(16);
    m_onlineCountLabel->setStyleSheet(QStringLiteral("color: #27AE60;"));
    statsLayout->addWidget(onlineTitle, 0, 2);
    statsLayout->addWidget(m_onlineCountLabel, 0, 3);
    
    // 收入统计（带日期选择）
    auto* incomeTitle = new ElaText(QStringLiteral("收入统计："), statsGroup);
    m_incomeDateEdit = new QDateEdit(statsGroup);
    m_incomeDateEdit->setDate(QDate::currentDate());
    m_incomeDateEdit->setCalendarPopup(true);
    m_incomeDateEdit->setDisplayFormat(QStringLiteral("yyyy-MM-dd"));
    m_incomeLabel = new ElaText(QStringLiteral("0.00 元"), statsGroup);
    m_incomeLabel->setTextPixelSize(18);
    m_incomeLabel->setStyleSheet(QStringLiteral("color: #E74C3C; font-weight: bold;"));
    statsLayout->addWidget(incomeTitle, 1, 0);
    statsLayout->addWidget(m_incomeDateEdit, 1, 1);
    statsLayout->addWidget(m_incomeLabel, 1, 2, 1, 2);
    
    statsLayout->setColumnStretch(1, 1);
    statsLayout->setColumnStretch(3, 1);
    mainLayout->addWidget(statsGroup);

    // ========== 搜索和操作栏 ==========
    QHBoxLayout* searchLayout = new QHBoxLayout();
    
    m_searchEdit = new ElaLineEdit(this);
    m_searchEdit->setPlaceholderText(QStringLiteral("🔍 搜索卡号/姓名/学号..."));
    m_searchEdit->setFixedWidth(300);
    searchLayout->addWidget(m_searchEdit);
    
    m_addCardBtn = new ElaPushButton(QStringLiteral("➕ 添加新卡"), this);
    searchLayout->addWidget(m_addCardBtn);
    
    searchLayout->addStretch();
    mainLayout->addLayout(searchLayout);

    // 卡列表表格
    m_cardTable = new ElaTableView(this);
    m_cardModel = new QStandardItemModel(this);
    m_cardModel->setHorizontalHeaderLabels({
        QStringLiteral("卡号"),
        QStringLiteral("姓名"),
        QStringLiteral("学号"),
        QStringLiteral("余额"),
        QStringLiteral("累计充值"),
        QStringLiteral("状态")
    });
    m_cardTable->setModel(m_cardModel);
    m_cardTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_cardTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_cardTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_cardTable->horizontalHeader()->setStretchLastSection(true);
    m_cardTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    mainLayout->addWidget(m_cardTable, 1);

    // ========== 卡操作按钮区域 ==========
    QGroupBox* cardOpGroup = new QGroupBox(QStringLiteral("💳 卡片操作（请先选择卡片）"), this);
    QHBoxLayout* cardOpLayout = new QHBoxLayout(cardOpGroup);
    cardOpLayout->setSpacing(10);

    m_rechargeBtn = new ElaPushButton(QStringLiteral("💰 充值"), this);
    m_reportLostBtn = new ElaPushButton(QStringLiteral("🔒 挂失"), this);
    m_cancelLostBtn = new ElaPushButton(QStringLiteral("🔓 解挂"), this);
    m_unfreezeBtn = new ElaPushButton(QStringLiteral("❄️ 解冻"), this);
    m_resetPwdBtn = new ElaPushButton(QStringLiteral("🔑 重置密码"), this);

    cardOpLayout->addWidget(m_rechargeBtn);
    cardOpLayout->addWidget(m_reportLostBtn);
    cardOpLayout->addWidget(m_cancelLostBtn);
    cardOpLayout->addWidget(m_unfreezeBtn);
    cardOpLayout->addWidget(m_resetPwdBtn);
    cardOpLayout->addStretch();
    
    mainLayout->addWidget(cardOpGroup);

    // ========== 系统操作按钮区域 ==========
    QHBoxLayout* sysOpLayout = new QHBoxLayout();
    sysOpLayout->setSpacing(10);

    m_statisticsBtn = new ElaPushButton(QStringLiteral("📈 统计报表"), this);
    m_exportBtn = new ElaPushButton(QStringLiteral("📤 导出"), this);
    m_importBtn = new ElaPushButton(QStringLiteral("📥 导入"), this);
    m_mockDataBtn = new ElaPushButton(QStringLiteral("🎲 生成测试数据"), this);
    m_changeAdminPwdBtn = new ElaPushButton(QStringLiteral("🔐 修改管理员密码"), this);
    m_logoutBtn = new ElaPushButton(QStringLiteral("🚪 退出登录"), this);

    sysOpLayout->addWidget(m_statisticsBtn);
    sysOpLayout->addWidget(m_exportBtn);
    sysOpLayout->addWidget(m_importBtn);
    sysOpLayout->addWidget(m_mockDataBtn);
    sysOpLayout->addStretch();
    sysOpLayout->addWidget(m_changeAdminPwdBtn);
    sysOpLayout->addWidget(m_logoutBtn);

    mainLayout->addLayout(sysOpLayout);

    // 初始禁用卡操作按钮（需要先选择卡）
    m_rechargeBtn->setEnabled(false);
    m_reportLostBtn->setEnabled(false);
    m_cancelLostBtn->setEnabled(false);
    m_unfreezeBtn->setEnabled(false);
    m_resetPwdBtn->setEnabled(false);
}

void AdminDashboard::initConnections() {
    // 卡操作按钮
    connect(m_rechargeBtn, &ElaPushButton::clicked, this, &AdminDashboard::onRechargeClicked);
    connect(m_reportLostBtn, &ElaPushButton::clicked, this, &AdminDashboard::onReportLostClicked);
    connect(m_cancelLostBtn, &ElaPushButton::clicked, this, &AdminDashboard::onCancelLostClicked);
    connect(m_unfreezeBtn, &ElaPushButton::clicked, this, &AdminDashboard::onUnfreezeClicked);
    connect(m_resetPwdBtn, &ElaPushButton::clicked, this, &AdminDashboard::onResetPasswordClicked);
    
    // 系统操作按钮
    connect(m_statisticsBtn, &ElaPushButton::clicked, this, &AdminDashboard::onStatisticsClicked);
    connect(m_exportBtn, &ElaPushButton::clicked, this, &AdminDashboard::onExportClicked);
    connect(m_importBtn, &ElaPushButton::clicked, this, &AdminDashboard::onImportClicked);
    connect(m_mockDataBtn, &ElaPushButton::clicked, this, &AdminDashboard::onGenerateMockDataClicked);
    connect(m_changeAdminPwdBtn, &ElaPushButton::clicked, this, &AdminDashboard::onChangeAdminPasswordClicked);
    connect(m_addCardBtn, &ElaPushButton::clicked, this, &AdminDashboard::onAddCardClicked);
    connect(m_logoutBtn, &ElaPushButton::clicked, this, &AdminDashboard::logoutRequested);
    
    // 表格选择和搜索
    connect(m_cardTable->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &AdminDashboard::onSelectionChanged);
    connect(m_searchEdit, &ElaLineEdit::textChanged, this, &AdminDashboard::onSearchChanged);
    
    // 收入日期变化
    connect(m_incomeDateEdit, &QDateEdit::dateChanged, this, &AdminDashboard::onIncomeDateChanged);
    
    // 数据变更信号
    connect(m_cardManager, &CardManager::cardsChanged, this, &AdminDashboard::refreshCardList);
    connect(m_cardManager, &CardManager::cardUpdated, this, &AdminDashboard::refreshCardList);
}

void AdminDashboard::refresh() {
    refreshCardList();
    
    // 更新总卡数
    int totalCards = m_cardManager->getAllCards().size();
    m_totalCardsLabel->setText(QString::number(totalCards) + QStringLiteral(" 张"));
    
    // 更新当前在线人数
    int onlineCount = 0;
    for (const auto& card : m_cardManager->getAllCards()) {
        if (m_recordManager->isOnline(card.cardId())) {
            onlineCount++;
        }
    }
    m_onlineCountLabel->setText(QString::number(onlineCount) + QStringLiteral(" 人"));
    
    // 更新收入（根据选择的日期）
    QString selectedDate = m_incomeDateEdit->date().toString(QStringLiteral("yyyy-MM-dd"));
    double income = m_recordManager->getDailyIncome(selectedDate);
    m_incomeLabel->setText(QString::number(income, 'f', 2) + QStringLiteral(" 元"));
}

void AdminDashboard::onIncomeDateChanged() {
    // 更新收入显示
    QString selectedDate = m_incomeDateEdit->date().toString(QStringLiteral("yyyy-MM-dd"));
    double income = m_recordManager->getDailyIncome(selectedDate);
    m_incomeLabel->setText(QString::number(income, 'f', 2) + QStringLiteral(" 元"));
}

void AdminDashboard::onChangeAdminPasswordClicked() {
    // 输入旧密码
    bool ok;
    QString oldPassword = QInputDialog::getText(this,
        QStringLiteral("修改管理员密码"),
        QStringLiteral("请输入当前管理员密码："),
        QLineEdit::Password, QString(), &ok);
    
    if (!ok) return;
    
    // 验证旧密码
    QString currentPassword = StorageManager::instance().loadAdminPassword();
    if (oldPassword != currentPassword) {
        ElaMessageBar::error(ElaMessageBarType::TopRight,
            QStringLiteral("错误"), QStringLiteral("当前密码错误"), 2000, this);
        return;
    }
    
    // 输入新密码
    QString newPassword = QInputDialog::getText(this,
        QStringLiteral("修改管理员密码"),
        QStringLiteral("请输入新密码（至少4位）："),
        QLineEdit::Password, QString(), &ok);
    
    if (!ok || newPassword.isEmpty()) return;
    
    if (newPassword.length() < 4) {
        ElaMessageBar::warning(ElaMessageBarType::TopRight,
            QStringLiteral("提示"), QStringLiteral("密码长度至少4位"), 2000, this);
        return;
    }
    
    // 确认新密码
    QString confirmPassword = QInputDialog::getText(this,
        QStringLiteral("修改管理员密码"),
        QStringLiteral("请再次输入新密码："),
        QLineEdit::Password, QString(), &ok);
    
    if (!ok) return;
    
    if (newPassword != confirmPassword) {
        ElaMessageBar::warning(ElaMessageBarType::TopRight,
            QStringLiteral("提示"), QStringLiteral("两次输入的密码不一致"), 2000, this);
        return;
    }
    
    // 保存新密码
    if (StorageManager::instance().saveAdminPassword(newPassword)) {
        ElaMessageBar::success(ElaMessageBarType::TopRight,
            QStringLiteral("成功"), QStringLiteral("管理员密码修改成功"), 2000, this);
    } else {
        ElaMessageBar::error(ElaMessageBarType::TopRight,
            QStringLiteral("错误"), QStringLiteral("密码保存失败"), 2000, this);
    }
}

void AdminDashboard::onAddCardClicked() {
    // 输入卡号
    bool ok;
    QString cardId = QInputDialog::getText(this,
        QStringLiteral("添加新卡"),
        QStringLiteral("请输入卡号："),
        QLineEdit::Normal, QString(), &ok);
    
    if (!ok || cardId.isEmpty()) return;
    
    // 检查卡号是否已存在
    if (m_cardManager->cardExists(cardId)) {
        ElaMessageBar::error(ElaMessageBarType::TopRight,
            QStringLiteral("错误"), QStringLiteral("该卡号已存在"), 2000, this);
        return;
    }
    
    // 输入姓名
    QString name = QInputDialog::getText(this,
        QStringLiteral("添加新卡"),
        QStringLiteral("请输入持卡人姓名："),
        QLineEdit::Normal, QString(), &ok);
    
    if (!ok || name.isEmpty()) return;
    
    // 输入学号
    QString studentId = QInputDialog::getText(this,
        QStringLiteral("添加新卡"),
        QStringLiteral("请输入学号："),
        QLineEdit::Normal, QString(), &ok);
    
    if (!ok || studentId.isEmpty()) return;
    
    // 输入初始余额
    double balance = QInputDialog::getDouble(this,
        QStringLiteral("添加新卡"),
        QStringLiteral("请输入初始余额："),
        0.0, 0.0, 10000.0, 2, &ok);
    
    if (!ok) return;
    
    // 创建新卡
    if (m_cardManager->createCard(cardId, name, studentId, balance)) {
        ElaMessageBar::success(ElaMessageBarType::TopRight,
            QStringLiteral("成功"), 
            QStringLiteral("新卡创建成功，卡号：") + cardId + QStringLiteral("，默认密码：123456"), 
            3000, this);
        refresh();
    } else {
        ElaMessageBar::error(ElaMessageBarType::TopRight,
            QStringLiteral("错误"), QStringLiteral("创建失败"), 2000, this);
    }
}

void AdminDashboard::refreshCardList() {
    m_cardModel->removeRows(0, m_cardModel->rowCount());
    
    QString searchText = m_searchEdit->text().trimmed().toLower();
    QList<Card> cards = m_cardManager->getAllCards();
    
    for (const auto& card : cards) {
        // 过滤搜索
        if (!searchText.isEmpty()) {
            if (!card.cardId().toLower().contains(searchText) &&
                !card.name().toLower().contains(searchText) &&
                !card.studentId().toLower().contains(searchText)) {
                continue;
            }
        }
        
        QList<QStandardItem*> row;
        row << new QStandardItem(card.cardId());
        row << new QStandardItem(card.name());
        row << new QStandardItem(card.studentId());
        row << new QStandardItem(QString::number(card.balance(), 'f', 2));
        row << new QStandardItem(QString::number(card.totalRecharge(), 'f', 2));
        row << new QStandardItem(cardStateToString(card.state()));
        
        m_cardModel->appendRow(row);
    }
}

QString AdminDashboard::getSelectedCardId() const {
    QModelIndexList selection = m_cardTable->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        return QString();
    }
    return m_cardModel->item(selection.first().row(), 0)->text();
}

void AdminDashboard::onSelectionChanged() {
    QString cardId = getSelectedCardId();
    bool hasSelection = !cardId.isEmpty();
    
    if (hasSelection) {
        Card* card = m_cardManager->findCard(cardId);
        if (card) {
            // 只有正常状态的卡才能充值
            m_rechargeBtn->setEnabled(card->state() == CardState::Normal);
            m_resetPwdBtn->setEnabled(true);
            m_reportLostBtn->setEnabled(card->state() == CardState::Normal);
            m_cancelLostBtn->setEnabled(card->state() == CardState::Lost);
            m_unfreezeBtn->setEnabled(card->state() == CardState::Frozen);
        } else {
            // 卡不存在，禁用所有按钮
            m_rechargeBtn->setEnabled(false);
            m_resetPwdBtn->setEnabled(false);
            m_reportLostBtn->setEnabled(false);
            m_cancelLostBtn->setEnabled(false);
            m_unfreezeBtn->setEnabled(false);
        }
    } else {
        m_rechargeBtn->setEnabled(false);
        m_resetPwdBtn->setEnabled(false);
        m_reportLostBtn->setEnabled(false);
        m_cancelLostBtn->setEnabled(false);
        m_unfreezeBtn->setEnabled(false);
    }
}

void AdminDashboard::onSearchChanged(const QString& /*text*/) {
    refreshCardList();
}

void AdminDashboard::onRechargeClicked() {
    QString cardId = getSelectedCardId();
    if (cardId.isEmpty()) return;
    
    RechargeDialog dialog(m_cardManager, cardId, this);
    if (dialog.exec() == QDialog::Accepted) {
        refresh();
        ElaMessageBar::success(ElaMessageBarType::TopRight,
            QStringLiteral("成功"), QStringLiteral("充值成功"), 2000, this);
    }
}

void AdminDashboard::onReportLostClicked() {
    QString cardId = getSelectedCardId();
    if (cardId.isEmpty()) return;
    
    if (m_cardManager->reportCardLost(cardId)) {
        refresh();
        ElaMessageBar::success(ElaMessageBarType::TopRight,
            QStringLiteral("成功"), QStringLiteral("挂失成功"), 2000, this);
    }
}

void AdminDashboard::onCancelLostClicked() {
    QString cardId = getSelectedCardId();
    if (cardId.isEmpty()) return;
    
    if (m_cardManager->cancelCardLost(cardId)) {
        refresh();
        ElaMessageBar::success(ElaMessageBarType::TopRight,
            QStringLiteral("成功"), QStringLiteral("解挂成功"), 2000, this);
    }
}

void AdminDashboard::onUnfreezeClicked() {
    QString cardId = getSelectedCardId();
    if (cardId.isEmpty()) return;
    
    if (m_cardManager->unfreezeCard(cardId)) {
        refresh();
        ElaMessageBar::success(ElaMessageBarType::TopRight,
            QStringLiteral("成功"), QStringLiteral("解冻成功"), 2000, this);
    }
}

void AdminDashboard::onResetPasswordClicked() {
    QString cardId = getSelectedCardId();
    if (cardId.isEmpty()) return;
    
    bool ok;
    QString newPassword = QInputDialog::getText(this, 
        QStringLiteral("重置密码"),
        QStringLiteral("请输入新密码："),
        QLineEdit::Password, QString(), &ok);
    
    if (ok && !newPassword.isEmpty()) {
        if (m_cardManager->resetPassword(cardId, newPassword)) {
            ElaMessageBar::success(ElaMessageBarType::TopRight,
                QStringLiteral("成功"), QStringLiteral("密码重置成功"), 2000, this);
        }
    }
}

void AdminDashboard::onStatisticsClicked() {
    StatisticsWidget* statsWidget = new StatisticsWidget(m_recordManager, m_cardManager, this);
    
    ElaContentDialog* dialog = new ElaContentDialog(this);
    dialog->setWindowTitle(QStringLiteral("统计报表"));
    dialog->setCentralWidget(statsWidget);
    dialog->setLeftButtonText(QStringLiteral("关闭"));
    dialog->setMiddleButtonText(QString());
    dialog->setRightButtonText(QString());
    dialog->resize(800, 600);
    dialog->exec();
}

void AdminDashboard::onExportClicked() {
    QString filePath = QFileDialog::getSaveFileName(this,
        QStringLiteral("导出数据"),
        QStringLiteral("campus_card_data.json"),
        QStringLiteral("JSON文件 (*.json)"));
    
    if (filePath.isEmpty()) return;
    
    if (StorageManager::instance().exportAllData(filePath)) {
        ElaMessageBar::success(ElaMessageBarType::TopRight,
            QStringLiteral("成功"), QStringLiteral("数据导出成功"), 2000, this);
    } else {
        ElaMessageBar::error(ElaMessageBarType::TopRight,
            QStringLiteral("错误"), QStringLiteral("数据导出失败"), 2000, this);
    }
}

void AdminDashboard::onImportClicked() {
    QString filePath = QFileDialog::getOpenFileName(this,
        QStringLiteral("导入数据"),
        QString(),
        QStringLiteral("JSON文件 (*.json)"));
    
    if (filePath.isEmpty()) return;
    
    // 询问导入模式
    QMessageBox::StandardButton reply = QMessageBox::question(this,
        QStringLiteral("导入模式"),
        QStringLiteral("是否合并数据？\n\n选择\"是\"将保留现有数据并添加新数据\n选择\"否\"将覆盖现有数据"),
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    
    if (reply == QMessageBox::Cancel) return;
    
    bool merge = (reply == QMessageBox::Yes);
    
    if (StorageManager::instance().importData(filePath, merge)) {
        // 重新加载数据
        m_cardManager->initialize();
        m_recordManager->initialize();
        refresh();
        
        ElaMessageBar::success(ElaMessageBarType::TopRight,
            QStringLiteral("成功"), QStringLiteral("数据导入成功"), 2000, this);
    } else {
        ElaMessageBar::error(ElaMessageBarType::TopRight,
            QStringLiteral("错误"), QStringLiteral("数据导入失败"), 2000, this);
    }
}

void AdminDashboard::onGenerateMockDataClicked() {
    bool ok;
    int cardCount = QInputDialog::getInt(this,
        QStringLiteral("生成测试数据"),
        QStringLiteral("请输入要生成的学生卡数量："),
        5, 1, 50, 1, &ok);
    
    if (!ok) return;
    
    int recordsPerCard = QInputDialog::getInt(this,
        QStringLiteral("生成测试数据"),
        QStringLiteral("请输入每张卡的上机记录数量："),
        3, 0, 20, 1, &ok);
    
    if (!ok) return;
    
    StorageManager::instance().generateMockData(cardCount, recordsPerCard);
    
    // 重新加载数据
    m_cardManager->initialize();
    m_recordManager->initialize();
    refresh();
    
    ElaMessageBar::success(ElaMessageBarType::TopRight,
        QStringLiteral("成功"), 
        QStringLiteral("已生成 %1 张测试卡").arg(cardCount), 2000, this);
}

} // namespace CampusCard
