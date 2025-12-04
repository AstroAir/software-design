 /**
 * @file main.cpp
 * @brief 应用程序入口
 * @author CampusCardSystem
 * @date 2024
 * 
 * 校园卡消费记录查询系统主入口
 * 基于 Qt6 + ElaWidgetTools 开发
 */

#include "ui/MainWindow.h"
#include "ElaApplication.h"

#include <QApplication>

/**
 * @brief 主函数，应用程序入口点
 * @param argc 命令行参数个数
 * @param argv 命令行参数数组
 * @return 应用程序退出码
 */
int main(int argc, char* argv[])
{
    // 创建应用程序实例
    QApplication app(argc, argv);
    
    // 设置应用程序信息
    app.setApplicationName(QStringLiteral("校园卡消费记录查询系统"));
    app.setApplicationVersion(QStringLiteral("1.0.0"));
    app.setOrganizationName(QStringLiteral("CampusCard"));
    
    // 初始化ElaWidgetTools
    eApp->init();
    
    // 创建并显示主窗口
    CampusCard::MainWindow mainWindow;
    mainWindow.show();
    
    // 进入事件循环
    return app.exec();
}
