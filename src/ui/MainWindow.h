/**
 * @file MainWindow.h
 * @brief 主窗口类定义
 * @author CampusCardSystem
 * @date 2024
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ElaWindow.h"
#include "core/AuthManager.h"
#include "core/CardManager.h"
#include "core/RecordManager.h"
#include "core/Types.h"

#include <QMap>


class ElaContentDialog;
class ElaSuggestBox;

namespace CampusCard {

class LoginDialog;
class AdminDashboard;
class StudentDashboard;

/**
 * @class MainWindow
 * @brief 应用程序主窗口，使用ElaWindow作为基类
 *
 * 负责整体界面布局、导航和子模块管理
 */
class MainWindow : public ElaWindow {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口
     */
    explicit MainWindow(QWidget* parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~MainWindow() override;

private slots:
    /**
     * @brief 处理登录成功
     * @param role 用户角色
     * @param cardId 卡号
     */
    void onLoginSucceeded(UserRole role, const QString& cardId);

    /**
     * @brief 处理登出
     */
    void onLogout();

    /**
     * @brief 显示登录对话框
     */
    void showLoginDialog();

    /**
     * @brief 显示关于对话框
     */
    void showAboutDialog();

private:
    /**
     * @brief 初始化UI组件
     */
    void initUI();

    /**
     * @brief 初始化管理器
     */
    void initManagers();

    /**
     * @brief 初始化信号槽连接
     */
    void initConnections();

    /**
     * @brief 切换到管理员界面
     */
    void switchToAdminDashboard();

    /**
     * @brief 切换到学生界面
     */
    void switchToStudentDashboard();

    /**
     * @brief 显示欢迎界面
     */
    void showWelcomePage();

    // ========== 管理器 ==========
    CardManager* m_cardManager = nullptr;      ///< 卡管理器
    RecordManager* m_recordManager = nullptr;  ///< 记录管理器
    AuthManager* m_authManager = nullptr;      ///< 认证管理器

    // ========== UI组件 ==========
    LoginDialog* m_loginDialog = nullptr;            ///< 登录对话框
    AdminDashboard* m_adminDashboard = nullptr;      ///< 管理员界面
    StudentDashboard* m_studentDashboard = nullptr;  ///< 学生界面
    QWidget* m_welcomePage = nullptr;                ///< 欢迎页面

    QString m_homeKey;         ///< 主页导航键
    QString m_adminKey;        ///< 管理员导航键
    QString m_studentKey;      ///< 学生导航键
    QString m_loginFooterKey;  ///< 登录页脚导航键
    QString m_aboutFooterKey;  ///< 关于页脚导航键

    // ========== 搜索建议 ==========
    ElaSuggestBox* m_suggestBox = nullptr;    ///< 搜索建议框
    QMap<QString, QString> m_suggestionKeys;  ///< 建议项key映射 (pageKey -> suggestionKey)

    /**
     * @brief 初始化搜索建议框
     */
    void initSuggestBox();

    /**
     * @brief 更新搜索建议（登录/登出后调用）
     */
    void updateSuggestions();
};

}  // namespace CampusCard

#endif  // MAINWINDOW_H
