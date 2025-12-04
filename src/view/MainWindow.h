/**
 * @file MainWindow.h
 * @brief 主窗口
 * @author CampusCardSystem
 * @date 2024
 *
 * MVC架构 - View层
 * 应用程序主窗口，负责整体界面布局和导航
 */

#ifndef VIEW_MAINWINDOW_H
#define VIEW_MAINWINDOW_H

#include "ElaWindow.h"
#include "controller/MainController.h"
#include "model/Types.h"

#include <QStackedWidget>

namespace CampusCard {

class LoginDialog;
class RegisterDialog;
class AdminPanel;
class StudentPanel;

/**
 * @class MainWindow
 * @brief 主窗口类，管理整个应用程序的界面
 *
 * 作为View层的顶层组件，负责：
 * - 初始化和管理MainController
 * - 显示登录界面
 * - 根据登录状态切换管理员/学生面板
 * - 处理全局导航
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
     * @param userName 用户名称
     */
    void onLoginSuccess(UserRole role, const QString& cardId, const QString& userName);

    /**
     * @brief 处理登出
     */
    void onLogout();

    /**
     * @brief 显示登录对话框
     */
    void showLoginDialog();

    /**
     * @brief 显示注册对话框
     */
    void showRegisterDialog();

    /**
     * @brief 显示关于对话框
     */
    void showAboutDialog();

private:
    /**
     * @brief 初始化控制器
     * @return 是否成功
     */
    bool initController();

    /**
     * @brief 初始化UI
     */
    void initUI();

    /**
     * @brief 初始化信号槽
     */
    void initConnections();

    /**
     * @brief 切换到管理员面板
     */
    void switchToAdminPanel();

    /**
     * @brief 切换到学生面板
     * @param cardId 学生卡号
     */
    void switchToStudentPanel(const QString& cardId);

    /**
     * @brief 显示欢迎页面
     */
    void showWelcomePage();

    // ========== 控制器 ==========
    MainController* m_mainController = nullptr;

    // ========== View组件 ==========
    LoginDialog* m_loginDialog = nullptr;
    AdminPanel* m_adminPanel = nullptr;
    StudentPanel* m_studentPanel = nullptr;
    QWidget* m_welcomePage = nullptr;
    QStackedWidget* m_stackedWidget = nullptr;  ///< 页面堆叠容器

    // ========== 状态跟踪 ==========
    bool m_isLoggedIn = false;                  ///< 是否已登录
    UserRole m_currentRole = UserRole::Student; ///< 当前用户角色

    // ========== 导航键 ==========
    QString m_homeKey;
};

}  // namespace CampusCard

#endif  // VIEW_MAINWINDOW_H
