/**
 * @file MainController.cpp
 * @brief 主控制器实现
 * @author CampusCardSystem
 * @date 2024
 *
 * MVC架构 - Controller层实现
 */

#include "MainController.h"

namespace CampusCard {

MainController::MainController(QObject* parent) : QObject(parent) {}

bool MainController::initialize(const QString& dataPath) {
    // 初始化存储管理器
    StorageManager::instance().setDataPath(dataPath);
    if (!StorageManager::instance().initializeDataDirectory()) {
        return false;
    }

    // 创建服务层
    m_cardService = new CardService(this);
    m_recordService = new RecordService(this);
    m_authService = new AuthService(m_cardService, this);

    // 初始化服务
    m_cardService->initialize();
    m_recordService->initialize();

    // 创建控制器层
    m_authController = new AuthController(m_authService, m_cardService, this);
    m_cardController = new CardController(m_cardService, this);
    m_recordController = new RecordController(m_recordService, m_cardService, this);

    // 连接信号：创建新卡时更新 RecordService 的卡号到学号映射
    connect(m_cardService, &CardService::cardCreated, this, [this](const QString& cardId) {
        Card card = m_cardService->findCard(cardId);
        if (!card.cardId().isEmpty()) {
            m_recordService->registerCardStudentMapping(cardId, card.studentId());
        }
    });

    emit initialized();
    return true;
}

// ========== 数据管理 ==========

void MainController::generateMockData(int cardCount, int recordsPerCard) {
    StorageManager::instance().generateMockData(cardCount, recordsPerCard);

    // 重新加载数据
    reloadData();

    emit mockDataGenerated(cardCount);
}

bool MainController::exportData(const QString& filePath) {
    if (StorageManager::instance().exportAllData(filePath)) {
        emit exportSuccess();
        return true;
    } else {
        emit exportFailed(QStringLiteral("导出失败"));
        return false;
    }
}

bool MainController::importData(const QString& filePath, bool merge) {
    if (StorageManager::instance().importData(filePath, merge)) {
        // 重新加载数据
        reloadData();
        emit importSuccess();
        return true;
    } else {
        emit importFailed(QStringLiteral("导入失败"));
        return false;
    }
}

void MainController::reloadData() {
    m_cardService->initialize();
    m_recordService->initialize();
    emit dataReloaded();
}

}  // namespace CampusCard
