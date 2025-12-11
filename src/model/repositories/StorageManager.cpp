/**
 * @file StorageManager.cpp
 * @brief 存储管理器实现
 * @author CampusCardSystem
 * @date 2024
 *
 * MVC架构 - Model层数据访问层实现
 */

#include "StorageManager.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRandomGenerator>
#include <QStandardPaths>
#include <QUuid>


namespace CampusCard {

StorageManager& StorageManager::instance() {
    static StorageManager instance;
    return instance;
}

void StorageManager::setDataPath(const QString& path) {
    m_dataPath = path;
}

bool StorageManager::ensureDirectory(const QString& dirPath) {
    QDir dir(dirPath);
    if (!dir.exists()) {
        return dir.mkpath(QStringLiteral("."));
    }
    return true;
}

bool StorageManager::initializeDataDirectory() {
    // 确保数据目录存在
    if (!ensureDirectory(m_dataPath)) {
        return false;
    }
    // 确保记录子目录存在
    if (!ensureDirectory(m_dataPath + QStringLiteral("/records"))) {
        return false;
    }

    // 检查是否需要创建示例数据
    QString cardsFile = m_dataPath + QStringLiteral("/cards.json");
    if (!QFile::exists(cardsFile)) {
        createSampleData();
    }

    return true;
}

void StorageManager::createSampleData() {
    // 创建示例学生卡
    QList<Card> sampleCards;

    // 示例学生1
    Card card1(QStringLiteral("C001"), QStringLiteral("张三"), QStringLiteral("B17010101"), 100.0);
    card1.setPassword(DEFAULT_STUDENT_PASSWORD);
    sampleCards.append(card1);

    // 示例学生2
    Card card2(QStringLiteral("C002"), QStringLiteral("李四"), QStringLiteral("B17010102"), 50.0);
    card2.setPassword(DEFAULT_STUDENT_PASSWORD);
    sampleCards.append(card2);

    // 示例学生3
    Card card3(QStringLiteral("C003"), QStringLiteral("王五"), QStringLiteral("B17010103"), 200.0);
    card3.setPassword(DEFAULT_STUDENT_PASSWORD);
    sampleCards.append(card3);

    saveAllCards(sampleCards);

    // 保存默认管理员密码
    saveAdminPassword(DEFAULT_ADMIN_PASSWORD);

    // 创建示例上机记录
    // 根据文档要求，记录文件以学号命名（如 B17010101.txt）
    Record record1;
    record1.setRecordId(QUuid::createUuid().toString(QUuid::WithoutBraces));
    record1.setCardId(QStringLiteral("C001"));
    record1.setLocation(QStringLiteral("机房A101"));
    record1.setStartTime(QDateTime::currentDateTime().addSecs(-3600));  // 1小时前开始
    record1.setEndTime(QDateTime::currentDateTime());
    record1.setDurationMinutes(60);
    record1.setCost(1.0);
    record1.setState(SessionState::Offline);
    appendRecord(QStringLiteral("B17010101"), record1);  // 使用学号作为文件名
}

// ========== 卡数据操作 ==========

QList<Card> StorageManager::loadAllCards() {
    QList<Card> cards;
    QString filePath = m_dataPath + QStringLiteral("/cards.json");

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return cards;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (!doc.isArray()) {
        return cards;
    }

    QJsonArray array = doc.array();
    for (const auto& item : array) {
        if (item.isObject()) {
            cards.append(Card::fromJson(item.toObject()));
        }
    }

    return cards;
}

bool StorageManager::saveAllCards(const QList<Card>& cards) {
    QString filePath = m_dataPath + QStringLiteral("/cards.json");

    QJsonArray array;
    for (const auto& card : cards) {
        array.append(card.toJson());
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QJsonDocument doc(array);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    return true;
}

Card StorageManager::loadCard(const QString& cardId) {
    QList<Card> cards = loadAllCards();
    for (const auto& card : cards) {
        if (card.cardId() == cardId) {
            return card;
        }
    }
    return Card();  // 返回空卡
}

// ========== 记录数据操作 ==========
// 根据文档要求，每个学生对应一个文本文件（如 B17010101.txt）存放上机记录

QList<Record> StorageManager::loadRecords(const QString& studentId) {
    QList<Record> records;
    QString filePath = m_dataPath + QStringLiteral("/records/") + studentId + QStringLiteral(".txt");

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return records;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (!doc.isArray()) {
        return records;
    }

    QJsonArray array = doc.array();
    for (const auto& item : array) {
        if (item.isObject()) {
            records.append(Record::fromJson(item.toObject()));
        }
    }

    return records;
}

bool StorageManager::saveRecords(const QString& studentId, const QList<Record>& records) {
    QString filePath = m_dataPath + QStringLiteral("/records/") + studentId + QStringLiteral(".txt");

    QJsonArray array;
    for (const auto& record : records) {
        array.append(record.toJson());
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QJsonDocument doc(array);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    return true;
}

bool StorageManager::appendRecord(const QString& studentId, const Record& record) {
    QList<Record> records = loadRecords(studentId);
    records.append(record);
    return saveRecords(studentId, records);
}

bool StorageManager::updateRecord(const QString& studentId, const Record& record) {
    QList<Record> records = loadRecords(studentId);

    for (int i = 0; i < records.size(); ++i) {
        if (records[i].recordId() == record.recordId()) {
            records[i] = record;
            return saveRecords(studentId, records);
        }
    }

    return false;  // 未找到对应记录
}

QMap<QString, QList<Record>> StorageManager::loadAllRecords() {
    QMap<QString, QList<Record>> allRecords;

    QString recordsDir = m_dataPath + QStringLiteral("/records");
    QDir dir(recordsDir);

    // 根据文档要求，记录文件以学号命名，后缀为 .txt
    QStringList files = dir.entryList(QStringList() << QStringLiteral("*.txt"), QDir::Files);
    for (const auto& fileName : files) {
        QString studentId = fileName.left(fileName.length() - 4);  // 去掉 .txt 后缀
        allRecords[studentId] = loadRecords(studentId);
    }

    return allRecords;
}

// ========== 管理员数据 ==========

QString StorageManager::loadAdminPassword() {
    QString filePath = m_dataPath + QStringLiteral("/admin.json");

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return DEFAULT_ADMIN_PASSWORD;  // 返回默认密码
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (!doc.isObject()) {
        return DEFAULT_ADMIN_PASSWORD;
    }

    return doc.object()[QStringLiteral("password")].toString(DEFAULT_ADMIN_PASSWORD);
}

bool StorageManager::saveAdminPassword(const QString& password) {
    QString filePath = m_dataPath + QStringLiteral("/admin.json");

    QJsonObject obj;
    obj[QStringLiteral("password")] = password;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QJsonDocument doc(obj);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    return true;
}

// ========== 模拟数据生成 ==========

void StorageManager::generateMockData(int cardCount, int recordsPerCard) {
    // 姓名池
    QStringList surnames = {QStringLiteral("张"), QStringLiteral("李"), QStringLiteral("王"),
                            QStringLiteral("刘"), QStringLiteral("陈"), QStringLiteral("杨"),
                            QStringLiteral("赵"), QStringLiteral("黄"), QStringLiteral("周"),
                            QStringLiteral("吴"), QStringLiteral("徐"), QStringLiteral("孙")};
    QStringList names = {
        QStringLiteral("伟"), QStringLiteral("芳"), QStringLiteral("娜"), QStringLiteral("敏"),
        QStringLiteral("静"), QStringLiteral("丽"), QStringLiteral("强"), QStringLiteral("磊"),
        QStringLiteral("军"), QStringLiteral("洋"), QStringLiteral("勇"), QStringLiteral("艳"),
        QStringLiteral("杰"), QStringLiteral("涛"), QStringLiteral("明"), QStringLiteral("超")};

    // 地点池
    QStringList locations = {QStringLiteral("机房A101"),         QStringLiteral("机房A102"),
                             QStringLiteral("机房B201"),         QStringLiteral("机房B202"),
                             QStringLiteral("图书馆电子阅览室"), QStringLiteral("实验楼C301")};

    QList<Card> existingCards = loadAllCards();
    int startNum = existingCards.size() + 1;

    for (int i = 0; i < cardCount; ++i) {
        // 生成卡号
        QString cardId = QStringLiteral("C%1").arg(startNum + i, 3, 10, QLatin1Char('0'));

        // 确保卡号不重复
        bool exists = false;
        for (const auto& card : existingCards) {
            if (card.cardId() == cardId) {
                exists = true;
                break;
            }
        }
        if (exists)
            continue;

        // 生成姓名
        QString fullName = surnames[QRandomGenerator::global()->bounded(surnames.size())] +
                           names[QRandomGenerator::global()->bounded(names.size())] +
                           names[QRandomGenerator::global()->bounded(names.size())];

        // 生成学号
        QString studentId = QStringLiteral("B%1%2")
                                .arg(17 + QRandomGenerator::global()->bounded(5))
                                .arg(QRandomGenerator::global()->bounded(10000, 99999));

        // 生成初始余额
        double balance = QRandomGenerator::global()->bounded(50, 500);

        // 创建卡
        Card card(cardId, fullName, studentId, balance);
        card.setPassword(DEFAULT_STUDENT_PASSWORD);
        existingCards.append(card);

        // 生成上机记录
        QList<Record> records;
        QDateTime baseTime = QDateTime::currentDateTime().addDays(-30);

        for (int j = 0; j < recordsPerCard; ++j) {
            // 随机日期（过去30天内）
            QDateTime startTime = baseTime.addDays(QRandomGenerator::global()->bounded(30));
            startTime = startTime.addSecs(QRandomGenerator::global()->bounded(8 * 3600, 20 * 3600));

            // 随机时长（30-180分钟）
            int duration = QRandomGenerator::global()->bounded(30, 180);
            QDateTime endTime = startTime.addSecs(duration * 60);

            // 随机地点
            QString location = locations[QRandomGenerator::global()->bounded(locations.size())];

            // 创建记录
            Record record;
            record.setRecordId(QUuid::createUuid().toString(QUuid::WithoutBraces));
            record.setCardId(cardId);
            record.setLocation(location);
            record.setStartTime(startTime);
            record.setEndTime(endTime);
            record.setDurationMinutes(duration);
            record.setCost(duration * COST_PER_HOUR / 60.0);
            record.setState(SessionState::Offline);

            records.append(record);
        }

        // 保存记录（使用学号作为文件名，符合文档要求）
        if (!records.isEmpty()) {
            saveRecords(studentId, records);
        }
    }

    // 保存所有卡
    saveAllCards(existingCards);
}

// ========== 导入导出 ==========

bool StorageManager::exportAllData(const QString& filePath) {
    QJsonObject root;

    // 导出卡数据
    QList<Card> cards = loadAllCards();
    QJsonArray cardsArray;
    for (const auto& card : cards) {
        cardsArray.append(card.toJson());
    }
    root[QStringLiteral("cards")] = cardsArray;

    // 导出管理员密码
    root[QStringLiteral("adminPassword")] = loadAdminPassword();

    // 导出所有记录
    QJsonObject recordsObj;
    QMap<QString, QList<Record>> allRecords = loadAllRecords();
    for (auto it = allRecords.constBegin(); it != allRecords.constEnd(); ++it) {
        QJsonArray recordsArray;
        for (const auto& record : it.value()) {
            recordsArray.append(record.toJson());
        }
        recordsObj[it.key()] = recordsArray;
    }
    root[QStringLiteral("records")] = recordsObj;

    // 添加导出信息
    root[QStringLiteral("exportTime")] = QDateTime::currentDateTime().toString(Qt::ISODate);
    root[QStringLiteral("version")] = QStringLiteral("1.0");

    // 写入文件
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QJsonDocument doc(root);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    return true;
}

bool StorageManager::importData(const QString& filePath, bool merge) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (!doc.isObject()) {
        return false;
    }

    QJsonObject root = doc.object();

    // 导入卡数据
    if (root.contains(QStringLiteral("cards"))) {
        QJsonArray cardsArray = root[QStringLiteral("cards")].toArray();
        QList<Card> importedCards;
        for (const auto& item : cardsArray) {
            if (item.isObject()) {
                importedCards.append(Card::fromJson(item.toObject()));
            }
        }

        if (merge) {
            // 合并模式：保留现有数据，只添加新卡
            QList<Card> existingCards = loadAllCards();
            for (const auto& newCard : importedCards) {
                bool exists = false;
                for (const auto& existingCard : existingCards) {
                    if (existingCard.cardId() == newCard.cardId()) {
                        exists = true;
                        break;
                    }
                }
                if (!exists) {
                    existingCards.append(newCard);
                }
            }
            saveAllCards(existingCards);
        } else {
            // 覆盖模式
            saveAllCards(importedCards);
        }
    }

    // 导入管理员密码（仅覆盖模式）
    if (!merge && root.contains(QStringLiteral("adminPassword"))) {
        saveAdminPassword(root[QStringLiteral("adminPassword")].toString());
    }

    // 导入记录（记录文件以学号命名，符合文档要求）
    if (root.contains(QStringLiteral("records"))) {
        QJsonObject recordsObj = root[QStringLiteral("records")].toObject();
        for (auto it = recordsObj.begin(); it != recordsObj.end(); ++it) {
            QString studentId = it.key();  // key 为学号
            QJsonArray recordsArray = it.value().toArray();

            QList<Record> importedRecords;
            for (const auto& item : recordsArray) {
                if (item.isObject()) {
                    importedRecords.append(Record::fromJson(item.toObject()));
                }
            }

            if (merge) {
                // 合并模式：追加记录
                QList<Record> existingRecords = loadRecords(studentId);
                existingRecords.append(importedRecords);
                saveRecords(studentId, existingRecords);
            } else {
                // 覆盖模式
                saveRecords(studentId, importedRecords);
            }
        }
    }

    return true;
}

}  // namespace CampusCard
