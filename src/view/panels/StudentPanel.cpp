/**
 * @file StudentPanel.cpp
 * @brief 学生控制面板实现
 * @author CampusCardSystem
 * @date 2024
 *
 * MVC架构 - View层实现
 */

#include "StudentPanel.h"

#include "ElaComboBox.h"
#include "ElaMessageBar.h"
#include "ElaPushButton.h"
#include "ElaText.h"

#include <QDate>
#include <QDateEdit>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QSet>
#include <QVBoxLayout>


namespace CampusCard {

StudentPanel::StudentPanel(CardController* cardController, RecordController* recordController,
                           QWidget* parent)
    : QWidget(parent), m_cardController(cardController), m_recordController(recordController) {
    initUI();
    initConnections();
}

void StudentPanel::initUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // 标题
    auto* titleLabel = new ElaText(QStringLiteral("我的校园卡"), this);
    titleLabel->setTextPixelSize(24);
    mainLayout->addWidget(titleLabel);

    // ========== 卡片信息区域 ==========
    QGroupBox* cardGroup = new QGroupBox(QStringLiteral("📋 卡片信息"), this);
    QGridLayout* cardLayout = new QGridLayout(cardGroup);
    cardLayout->setSpacing(10);

    // 姓名
    auto* nameTitleLabel = new ElaText(QStringLiteral("姓名："), cardGroup);
    m_nameLabel = new ElaText(QStringLiteral("--"), cardGroup);
    m_nameLabel->setTextPixelSize(16);
    cardLayout->addWidget(nameTitleLabel, 0, 0);
    cardLayout->addWidget(m_nameLabel, 0, 1);

    // 学号
    auto* studentIdTitleLabel = new ElaText(QStringLiteral("学号："), cardGroup);
    m_studentIdLabel = new ElaText(QStringLiteral("--"), cardGroup);
    cardLayout->addWidget(studentIdTitleLabel, 0, 2);
    cardLayout->addWidget(m_studentIdLabel, 0, 3);

    // 余额
    auto* balanceTitleLabel = new ElaText(QStringLiteral("余额："), cardGroup);
    m_balanceLabel = new ElaText(QStringLiteral("-- 元"), cardGroup);
    m_balanceLabel->setTextPixelSize(20);
    m_balanceLabel->setStyleSheet(QStringLiteral("color: #27AE60; font-weight: bold;"));
    cardLayout->addWidget(balanceTitleLabel, 1, 0);
    cardLayout->addWidget(m_balanceLabel, 1, 1);

    // 状态
    auto* statusTitleLabel = new ElaText(QStringLiteral("状态："), cardGroup);
    m_statusLabel = new ElaText(QStringLiteral("--"), cardGroup);
    cardLayout->addWidget(statusTitleLabel, 1, 2);
    cardLayout->addWidget(m_statusLabel, 1, 3);

    cardLayout->setColumnStretch(1, 1);
    cardLayout->setColumnStretch(3, 1);
    mainLayout->addWidget(cardGroup);

    // ========== 上机操作区域 ==========
    QGroupBox* sessionGroup = new QGroupBox(QStringLiteral("💻 上机操作"), this);
    QVBoxLayout* sessionMainLayout = new QVBoxLayout(sessionGroup);

    // 上机状态信息
    QHBoxLayout* sessionInfoLayout = new QHBoxLayout();
    m_sessionLabel = new ElaText(QStringLiteral("当前状态：离线"), sessionGroup);
    m_sessionLabel->setTextPixelSize(14);
    sessionInfoLayout->addWidget(m_sessionLabel);
    sessionInfoLayout->addStretch();
    sessionMainLayout->addLayout(sessionInfoLayout);

    // 上机时间和地点
    QHBoxLayout* sessionDetailLayout = new QHBoxLayout();
    m_sessionTimeLabel = new ElaText(QStringLiteral(""), sessionGroup);
    m_sessionLocationLabel = new ElaText(QStringLiteral(""), sessionGroup);
    sessionDetailLayout->addWidget(m_sessionTimeLabel);
    sessionDetailLayout->addWidget(m_sessionLocationLabel);
    sessionDetailLayout->addStretch();
    sessionMainLayout->addLayout(sessionDetailLayout);

    // 操作按钮
    QHBoxLayout* sessionBtnLayout = new QHBoxLayout();
    m_startSessionBtn = new ElaPushButton(QStringLiteral("🖥️ 开始上机"), sessionGroup);
    m_endSessionBtn = new ElaPushButton(QStringLiteral("⏹️ 结束上机"), sessionGroup);
    m_endSessionBtn->setEnabled(false);
    sessionBtnLayout->addWidget(m_startSessionBtn);
    sessionBtnLayout->addWidget(m_endSessionBtn);
    sessionBtnLayout->addStretch();
    sessionMainLayout->addLayout(sessionBtnLayout);

    mainLayout->addWidget(sessionGroup);

    // ========== 统计信息区域 ==========
    QGroupBox* statsGroup = new QGroupBox(QStringLiteral("📊 统计信息"), this);
    QGridLayout* statsLayout = new QGridLayout(statsGroup);
    statsLayout->setSpacing(10);

    // 总上机次数
    auto* sessionsTitleLabel = new ElaText(QStringLiteral("总上机次数："), statsGroup);
    m_totalSessionsLabel = new ElaText(QStringLiteral("0 次"), statsGroup);
    m_totalSessionsLabel->setTextPixelSize(16);
    statsLayout->addWidget(sessionsTitleLabel, 0, 0);
    statsLayout->addWidget(m_totalSessionsLabel, 0, 1);

    // 总时长
    auto* durationTitleLabel = new ElaText(QStringLiteral("总上机时长："), statsGroup);
    m_totalDurationLabel = new ElaText(QStringLiteral("0 小时 0 分钟"), statsGroup);
    m_totalDurationLabel->setTextPixelSize(16);
    statsLayout->addWidget(durationTitleLabel, 0, 2);
    statsLayout->addWidget(m_totalDurationLabel, 0, 3);

    // 总费用
    auto* costTitleLabel = new ElaText(QStringLiteral("总消费金额："), statsGroup);
    m_totalCostLabel = new ElaText(QStringLiteral("0.00 元"), statsGroup);
    m_totalCostLabel->setTextPixelSize(16);
    m_totalCostLabel->setStyleSheet(QStringLiteral("color: #E74C3C;"));
    statsLayout->addWidget(costTitleLabel, 1, 0);
    statsLayout->addWidget(m_totalCostLabel, 1, 1);

    statsLayout->setColumnStretch(1, 1);
    statsLayout->setColumnStretch(3, 1);
    mainLayout->addWidget(statsGroup);

    // ========== 上机记录区域 ==========
    QGroupBox* recordGroup = new QGroupBox(QStringLiteral("📝 上机记录"), this);
    QVBoxLayout* recordMainLayout = new QVBoxLayout(recordGroup);

    // 筛选区域
    QHBoxLayout* filterLayout = new QHBoxLayout();

    auto* startDateLabel = new ElaText(QStringLiteral("开始日期："), recordGroup);
    m_startDateEdit = new QDateEdit(recordGroup);
    m_startDateEdit->setDate(QDate::currentDate().addDays(-30));
    m_startDateEdit->setCalendarPopup(true);
    m_startDateEdit->setDisplayFormat(QStringLiteral("yyyy-MM-dd"));

    auto* endDateLabel = new ElaText(QStringLiteral("结束日期："), recordGroup);
    m_endDateEdit = new QDateEdit(recordGroup);
    m_endDateEdit->setDate(QDate::currentDate());
    m_endDateEdit->setCalendarPopup(true);
    m_endDateEdit->setDisplayFormat(QStringLiteral("yyyy-MM-dd"));

    auto* locationLabel = new ElaText(QStringLiteral("地点："), recordGroup);
    m_locationFilter = new ElaComboBox(recordGroup);
    m_locationFilter->addItem(QStringLiteral("全部地点"));

    filterLayout->addWidget(startDateLabel);
    filterLayout->addWidget(m_startDateEdit);
    filterLayout->addWidget(endDateLabel);
    filterLayout->addWidget(m_endDateEdit);
    filterLayout->addWidget(locationLabel);
    filterLayout->addWidget(m_locationFilter);
    filterLayout->addStretch();
    recordMainLayout->addLayout(filterLayout);

    // 记录表格
    m_recordTable = new RecordTableWidget(recordGroup);
    recordMainLayout->addWidget(m_recordTable, 1);

    mainLayout->addWidget(recordGroup, 1);

    // ========== 底部按钮区域 ==========
    QHBoxLayout* bottomLayout = new QHBoxLayout();

    m_changePasswordBtn = new ElaPushButton(QStringLiteral("🔑 修改密码"), this);
    bottomLayout->addWidget(m_changePasswordBtn);

    bottomLayout->addStretch();

    m_logoutBtn = new ElaPushButton(QStringLiteral("🚪 退出登录"), this);
    bottomLayout->addWidget(m_logoutBtn);

    mainLayout->addLayout(bottomLayout);
}

void StudentPanel::initConnections() {
    // 上机操作按钮
    connect(m_startSessionBtn, &ElaPushButton::clicked, this, &StudentPanel::onStartSessionClicked);
    connect(m_endSessionBtn, &ElaPushButton::clicked, this, &StudentPanel::onEndSessionClicked);

    // 修改密码按钮
    connect(m_changePasswordBtn, &ElaPushButton::clicked, this,
            &StudentPanel::onChangePasswordClicked);

    // 登出按钮
    connect(m_logoutBtn, &ElaPushButton::clicked, this, &StudentPanel::logoutRequested);

    // 日期筛选变化
    connect(m_startDateEdit, &QDateEdit::dateChanged, this, &StudentPanel::onFilterChanged);
    connect(m_endDateEdit, &QDateEdit::dateChanged, this, &StudentPanel::onFilterChanged);
    connect(m_locationFilter, QOverload<int>::of(&ElaComboBox::currentIndexChanged), this,
            &StudentPanel::onFilterChanged);

    // 控制器信号
    connect(m_recordController, &RecordController::sessionStarted, this,
            &StudentPanel::onSessionStarted);
    connect(m_recordController, &RecordController::sessionEnded, this,
            &StudentPanel::onSessionEnded);
    connect(m_recordController, &RecordController::recordsUpdated, this,
            &StudentPanel::onRecordsUpdated);

    // 上下机失败信号
    connect(m_recordController, &RecordController::sessionStartFailed, this,
            [this](const QString& message) {
                ElaMessageBar::error(ElaMessageBarType::TopRight, QStringLiteral("错误"), message,
                                     2000, this);
            });
    connect(m_recordController, &RecordController::sessionEndFailed, this,
            [this](const QString& message) {
                ElaMessageBar::error(ElaMessageBarType::TopRight, QStringLiteral("错误"), message,
                                     2000, this);
            });

    // 密码修改结果
    connect(m_cardController, &CardController::passwordChanged, this,
            [this](const QString& cardId) {
                if (cardId == m_currentCardId) {
                    ElaMessageBar::success(ElaMessageBarType::TopRight, QStringLiteral("成功"),
                                           QStringLiteral("密码修改成功"), 2000, this);
                }
            });
    connect(m_cardController, &CardController::passwordChangeFailed, this,
            [this](const QString& message) {
                ElaMessageBar::error(ElaMessageBarType::TopRight, QStringLiteral("失败"), message,
                                     2000, this);
            });

    // 卡数据更新（余额变化等）
    connect(m_cardController, &CardController::cardUpdated, this,
            [this](const QString& cardId) {
                if (cardId == m_currentCardId) {
                    updateCardInfo();
                }
            });
}

void StudentPanel::setCurrentCard(const QString& cardId) {
    m_currentCardId = cardId;
    if (cardId.isEmpty()) {
        // 清空显示
        m_nameLabel->setText(QStringLiteral("--"));
        m_studentIdLabel->setText(QStringLiteral("--"));
        m_balanceLabel->setText(QStringLiteral("-- 元"));
        m_statusLabel->setText(QStringLiteral("--"));
        m_sessionLabel->setText(QStringLiteral("当前状态：离线"));
        m_sessionTimeLabel->setText(QString());
        m_sessionLocationLabel->setText(QString());
        m_totalSessionsLabel->setText(QStringLiteral("0 次"));
        m_totalDurationLabel->setText(QStringLiteral("0 小时 0 分钟"));
        m_totalCostLabel->setText(QStringLiteral("0.00 元"));
        m_recordTable->clear();
        m_startSessionBtn->setEnabled(false);
        m_endSessionBtn->setEnabled(false);
        m_changePasswordBtn->setEnabled(false);

        m_locationFilter->clear();
        m_locationFilter->addItem(QStringLiteral("全部地点"));
    } else {
        m_changePasswordBtn->setEnabled(true);
        updateLocationFilter();
        refresh();
    }
}

void StudentPanel::refresh() {
    if (m_currentCardId.isEmpty())
        return;

    updateCardInfo();
    updateSessionStatus();
    updateStatistics();
    updateRecordTable();
}

void StudentPanel::updateCardInfo() {
    Card card = m_cardController->getCard(m_currentCardId);
    if (card.cardId().isEmpty())
        return;

    m_nameLabel->setText(card.name());
    m_studentIdLabel->setText(card.studentId());
    m_balanceLabel->setText(QString::number(card.balance(), 'f', 2) + QStringLiteral(" 元"));

    // 根据状态设置不同颜色
    QString stateText = cardStateToString(card.state());
    m_statusLabel->setText(stateText);
    if (card.state() == CardState::Normal) {
        m_statusLabel->setStyleSheet(QStringLiteral("color: #27AE60;"));
    } else if (card.state() == CardState::Lost) {
        m_statusLabel->setStyleSheet(QStringLiteral("color: #F39C12;"));
    } else {
        m_statusLabel->setStyleSheet(QStringLiteral("color: #E74C3C;"));
    }
}

void StudentPanel::updateSessionStatus() {
    Card card = m_cardController->getCard(m_currentCardId);
    if (card.cardId().isEmpty() || !card.isUsable()) {
        m_sessionLabel->setText(QStringLiteral("当前状态：卡片不可用"));
        m_sessionLabel->setStyleSheet(QStringLiteral("color: #E74C3C;"));
        m_sessionTimeLabel->setText(QString());
        m_sessionLocationLabel->setText(QString());
        m_startSessionBtn->setEnabled(false);
        m_endSessionBtn->setEnabled(false);
        return;
    }

    bool isOnline = m_recordController->isOnline(m_currentCardId);

    if (isOnline) {
        Record session = m_recordController->getCurrentSession(m_currentCardId);
        if (session.isValid()) {
            m_sessionLabel->setText(QStringLiteral("当前状态：🟢 上机中"));
            m_sessionLabel->setStyleSheet(QStringLiteral("color: #27AE60; font-weight: bold;"));
            QString startTime = session.startTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"));
            m_sessionTimeLabel->setText(QStringLiteral("开始时间：") + startTime);
            m_sessionLocationLabel->setText(QStringLiteral("  地点：") + session.location());

            double currentCost = m_recordController->getCurrentCost(m_currentCardId);
            m_sessionTimeLabel->setText(m_sessionTimeLabel->text() +
                                        QStringLiteral("  当前费用：") +
                                        QString::number(currentCost, 'f', 2) + QStringLiteral(" 元"));
        }
        m_startSessionBtn->setEnabled(false);
        m_endSessionBtn->setEnabled(true);
    } else {
        m_sessionLabel->setText(QStringLiteral("当前状态：⚪ 离线"));
        m_sessionLabel->setStyleSheet(QStringLiteral("color: #7F8C8D;"));
        m_sessionTimeLabel->setText(QString());
        m_sessionLocationLabel->setText(QString());
        m_startSessionBtn->setEnabled(card.balance() > 0);
        m_endSessionBtn->setEnabled(false);
    }
}

void StudentPanel::updateStatistics() {
    int totalSessions = m_recordController->getTotalSessionCount(m_currentCardId);
    int totalDuration = m_recordController->getTotalDuration(m_currentCardId);
    double totalCost = m_recordController->getTotalCost(m_currentCardId);

    m_totalSessionsLabel->setText(QString::number(totalSessions) + QStringLiteral(" 次"));

    int hours = totalDuration / 60;
    int minutes = totalDuration % 60;
    m_totalDurationLabel->setText(QString::number(hours) + QStringLiteral(" 小时 ") +
                                  QString::number(minutes) + QStringLiteral(" 分钟"));

    m_totalCostLabel->setText(QString::number(totalCost, 'f', 2) + QStringLiteral(" 元"));
}

void StudentPanel::updateLocationFilter() {
    QStringList locations = m_recordController->getLocations(m_currentCardId);

    QString currentSelection = m_locationFilter->currentText();
    m_locationFilter->clear();
    m_locationFilter->addItem(QStringLiteral("全部地点"));
    for (const auto& loc : locations) {
        m_locationFilter->addItem(loc);
    }

    int index = m_locationFilter->findText(currentSelection);
    if (index >= 0) {
        m_locationFilter->setCurrentIndex(index);
    }
}

void StudentPanel::updateRecordTable() {
    QString startDate = m_startDateEdit->date().toString(QStringLiteral("yyyy-MM-dd"));
    QString endDate = m_endDateEdit->date().toString(QStringLiteral("yyyy-MM-dd"));
    QString location = m_locationFilter->currentText();

    if (location == QStringLiteral("全部地点")) {
        location.clear();
    }

    QList<Record> records =
        m_recordController->getFilteredRecords(m_currentCardId, startDate, endDate, location);
    m_recordTable->setRecords(records);
}

void StudentPanel::onFilterChanged() {
    updateRecordTable();
}

void StudentPanel::onStartSessionClicked() {
    if (m_currentCardId.isEmpty())
        return;

    bool ok;
    QString location =
        QInputDialog::getText(this, QStringLiteral("开始上机"), QStringLiteral("请输入上机地点："),
                              QLineEdit::Normal, QStringLiteral("机房A101"), &ok);

    if (ok && !location.isEmpty()) {
        m_recordController->handleStartSession(m_currentCardId, location);
    }
}

void StudentPanel::onEndSessionClicked() {
    if (m_currentCardId.isEmpty())
        return;

    m_recordController->handleEndSession(m_currentCardId);
}

void StudentPanel::onChangePasswordClicked() {
    if (m_currentCardId.isEmpty())
        return;

    bool ok;
    QString oldPassword =
        QInputDialog::getText(this, QStringLiteral("修改密码"), QStringLiteral("请输入当前密码："),
                              QLineEdit::Password, QString(), &ok);

    if (!ok)
        return;

    QString newPassword = QInputDialog::getText(this, QStringLiteral("修改密码"),
                                                QStringLiteral("请输入新密码（至少4位）："),
                                                QLineEdit::Password, QString(), &ok);

    if (!ok || newPassword.isEmpty())
        return;

    if (newPassword.length() < 4) {
        ElaMessageBar::warning(ElaMessageBarType::TopRight, QStringLiteral("提示"),
                               QStringLiteral("密码长度至少4位"), 2000, this);
        return;
    }

    QString confirmPassword = QInputDialog::getText(this, QStringLiteral("修改密码"),
                                                    QStringLiteral("请再次输入新密码："),
                                                    QLineEdit::Password, QString(), &ok);

    if (!ok)
        return;

    if (newPassword != confirmPassword) {
        ElaMessageBar::warning(ElaMessageBarType::TopRight, QStringLiteral("提示"),
                               QStringLiteral("两次输入的密码不一致"), 2000, this);
        return;
    }

    m_cardController->handleChangePassword(m_currentCardId, oldPassword, newPassword);
}

void StudentPanel::onSessionStarted(const QString& cardId, const QString& /*location*/) {
    if (cardId == m_currentCardId) {
        ElaMessageBar::success(ElaMessageBarType::TopRight, QStringLiteral("成功"),
                               QStringLiteral("上机开始"), 2000, this);
        refresh();
    }
}

void StudentPanel::onSessionEnded(const QString& cardId, double cost, int /*duration*/) {
    if (cardId == m_currentCardId) {
        ElaMessageBar::success(ElaMessageBarType::TopRight, QStringLiteral("成功"),
                               QStringLiteral("上机结束，本次费用：") +
                                   QString::number(cost, 'f', 2) + QStringLiteral(" 元"),
                               3000, this);
        refresh();
    }
}

void StudentPanel::onRecordsUpdated(const QString& cardId) {
    if (cardId == m_currentCardId) {
        refresh();
    }
}

}  // namespace CampusCard
