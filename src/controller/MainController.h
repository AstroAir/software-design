/**
 * @file MainController.h
 * @brief 主控制器
 * @author CampusCardSystem
 * @date 2024
 *
 * MVC架构 - Controller层
 * 负责协调整个应用程序的控制器和服务
 */

#ifndef CONTROLLER_MAINCONTROLLER_H
#define CONTROLLER_MAINCONTROLLER_H

#include "AuthController.h"
#include "CardController.h"
#include "RecordController.h"
#include "model/repositories/StorageManager.h"
#include "model/services/AuthService.h"
#include "model/services/CardService.h"
#include "model/services/RecordService.h"

#include <QObject>


namespace CampusCard {

/**
 * @class MainController
 * @brief 主控制器，管理所有服务和子控制器
 *
 * 作为Controller层的入口点，负责：
 * - 创建和管理所有Service
 * - 创建和管理所有Controller
 * - 初始化整个应用程序的业务层
 * - 提供对子控制器的访问
 */
class MainController : public QObject {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父对象
     */
    explicit MainController(QObject* parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~MainController() override = default;

    /**
     * @brief 初始化控制器和服务
     * @param dataPath 数据目录路径
     * @return 是否成功
     */
    bool initialize(const QString& dataPath);

    // ========== 获取子控制器 ==========

    /**
     * @brief 获取认证控制器
     * @return 认证控制器指针
     */
    [[nodiscard]] AuthController* authController() const { return m_authController; }

    /**
     * @brief 获取卡控制器
     * @return 卡控制器指针
     */
    [[nodiscard]] CardController* cardController() const { return m_cardController; }

    /**
     * @brief 获取记录控制器
     * @return 记录控制器指针
     */
    [[nodiscard]] RecordController* recordController() const { return m_recordController; }

    // ========== 获取服务（用于View直接查询） ==========

    /**
     * @brief 获取卡服务
     * @return 卡服务指针
     */
    [[nodiscard]] CardService* cardService() const { return m_cardService; }

    /**
     * @brief 获取记录服务
     * @return 记录服务指针
     */
    [[nodiscard]] RecordService* recordService() const { return m_recordService; }

    /**
     * @brief 获取认证服务
     * @return 认证服务指针
     */
    [[nodiscard]] AuthService* authService() const { return m_authService; }

    // ========== 数据管理 ==========

    /**
     * @brief 生成模拟数据
     * @param cardCount 卡数量
     * @param recordsPerCard 每张卡的记录数
     */
    void generateMockData(int cardCount, int recordsPerCard);

    /**
     * @brief 导出所有数据
     * @param filePath 文件路径
     * @return 是否成功
     */
    bool exportData(const QString& filePath);

    /**
     * @brief 导入数据
     * @param filePath 文件路径
     * @param merge 是否合并
     * @return 是否成功
     */
    bool importData(const QString& filePath, bool merge);

    /**
     * @brief 重新加载所有数据
     */
    void reloadData();

signals:
    /**
     * @brief 初始化完成信号
     */
    void initialized();

    /**
     * @brief 数据重新加载完成信号
     */
    void dataReloaded();

    /**
     * @brief 导出成功信号
     */
    void exportSuccess();

    /**
     * @brief 导出失败信号
     * @param message 错误消息
     */
    void exportFailed(const QString& message);

    /**
     * @brief 导入成功信号
     */
    void importSuccess();

    /**
     * @brief 导入失败信号
     * @param message 错误消息
     */
    void importFailed(const QString& message);

    /**
     * @brief 模拟数据生成完成信号
     * @param count 生成的卡数量
     */
    void mockDataGenerated(int count);

private:
    // ========== 服务层 ==========
    CardService* m_cardService = nullptr;      ///< 卡服务
    RecordService* m_recordService = nullptr;  ///< 记录服务
    AuthService* m_authService = nullptr;      ///< 认证服务

    // ========== 控制器层 ==========
    AuthController* m_authController = nullptr;      ///< 认证控制器
    CardController* m_cardController = nullptr;      ///< 卡控制器
    RecordController* m_recordController = nullptr;  ///< 记录控制器
};

}  // namespace CampusCard

#endif  // CONTROLLER_MAINCONTROLLER_H
