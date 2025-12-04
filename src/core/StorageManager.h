/**
 * @file StorageManager.h
 * @brief 存储管理器，负责数据的持久化
 * @author CampusCardSystem
 * @date 2024
 */

#ifndef STORAGEMANAGER_H
#define STORAGEMANAGER_H

#include "Card.h"
#include "Record.h"
#include <QString>
#include <QList>
#include <QMap>
#include <memory>

namespace CampusCard {

/**
 * @class StorageManager
 * @brief 单例存储管理器，负责所有数据的文件读写
 * 
 * 数据存储结构：
 * - data/cards.json: 所有校园卡信息
 * - data/admin.json: 管理员密码
 * - data/records/<cardId>.json: 每个学生的上机记录
 */
class StorageManager {
public:
    /**
     * @brief 获取单例实例
     * @return 存储管理器引用
     */
    static StorageManager& instance();

    /**
     * @brief 设置数据目录路径
     * @param path 数据目录路径
     */
    void setDataPath(const QString& path);

    /**
     * @brief 获取数据目录路径
     * @return 数据目录路径
     */
    [[nodiscard]] QString dataPath() const { return m_dataPath; }

    // ========== 卡数据操作 ==========

    /**
     * @brief 加载所有校园卡数据
     * @return 卡列表
     */
    QList<Card> loadAllCards();

    /**
     * @brief 保存所有校园卡数据
     * @param cards 卡列表
     * @return 是否成功
     */
    bool saveAllCards(const QList<Card>& cards);

    /**
     * @brief 根据卡号加载单张卡
     * @param cardId 卡号
     * @return 卡对象（如果不存在则返回空Card）
     */
    Card loadCard(const QString& cardId);

    // ========== 记录数据操作 ==========

    /**
     * @brief 加载指定卡号的所有上机记录
     * @param cardId 卡号
     * @return 记录列表
     */
    QList<Record> loadRecords(const QString& cardId);

    /**
     * @brief 保存指定卡号的所有上机记录
     * @param cardId 卡号
     * @param records 记录列表
     * @return 是否成功
     */
    bool saveRecords(const QString& cardId, const QList<Record>& records);

    /**
     * @brief 追加一条上机记录
     * @param cardId 卡号
     * @param record 记录对象
     * @return 是否成功
     */
    bool appendRecord(const QString& cardId, const Record& record);

    /**
     * @brief 更新一条上机记录
     * @param cardId 卡号
     * @param record 更新后的记录
     * @return 是否成功
     */
    bool updateRecord(const QString& cardId, const Record& record);

    /**
     * @brief 加载所有卡的所有记录（用于管理员统计）
     * @return 卡号到记录列表的映射
     */
    QMap<QString, QList<Record>> loadAllRecords();

    // ========== 管理员数据 ==========

    /**
     * @brief 加载管理员密码
     * @return 管理员密码
     */
    QString loadAdminPassword();

    /**
     * @brief 保存管理员密码
     * @param password 新密码
     * @return 是否成功
     */
    bool saveAdminPassword(const QString& password);

    // ========== 初始化 ==========

    /**
     * @brief 初始化数据目录和默认数据
     * @return 是否成功
     */
    bool initializeDataDirectory();

    /**
     * @brief 创建示例数据（首次运行时）
     */
    void createSampleData();

    /**
     * @brief 生成模拟数据（更多测试数据）
     * @param cardCount 要生成的卡数量
     * @param recordsPerCard 每张卡的记录数量
     */
    void generateMockData(int cardCount, int recordsPerCard);

    // ========== 导入导出 ==========

    /**
     * @brief 导出所有数据到JSON文件
     * @param filePath 导出文件路径
     * @return 是否成功
     */
    bool exportAllData(const QString& filePath);

    /**
     * @brief 从JSON文件导入数据
     * @param filePath 导入文件路径
     * @param merge 是否合并（true合并，false覆盖）
     * @return 是否成功
     */
    bool importData(const QString& filePath, bool merge = false);

private:
    /**
     * @brief 私有构造函数（单例模式）
     */
    StorageManager() = default;

    /**
     * @brief 确保目录存在
     * @param dirPath 目录路径
     * @return 是否成功
     */
    bool ensureDirectory(const QString& dirPath);

    QString m_dataPath;  ///< 数据目录路径
};

} // namespace CampusCard

#endif // STORAGEMANAGER_H
