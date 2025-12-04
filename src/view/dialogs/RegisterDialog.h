/**
 * @file RegisterDialog.h
 * @brief 注册对话框
 * @author CampusCardSystem
 * @date 2024
 *
 * MVC架构 - View层
 * 负责新卡注册界面的显示和用户交互
 */

#ifndef VIEW_DIALOGS_REGISTERDIALOG_H
#define VIEW_DIALOGS_REGISTERDIALOG_H

#include "ElaDialog.h"
#include "controller/CardController.h"

class ElaLineEdit;
class ElaPushButton;

namespace CampusCard {

/**
 * @class RegisterDialog
 * @brief 注册对话框，用于创建新的校园卡
 *
 * 作为View层，负责：
 * - 显示注册界面
 * - 收集用户输入
 * - 通过Controller处理注册请求
 * - 显示注册结果
 */
class RegisterDialog : public ElaDialog {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param cardController 卡控制器
     * @param parent 父窗口
     */
    explicit RegisterDialog(CardController* cardController, QWidget* parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~RegisterDialog() override = default;

private slots:
    /**
     * @brief 处理注册按钮点击
     */
    void onRegisterClicked();

    /**
     * @brief 生成卡号
     */
    void onGenerateCardId();

    /**
     * @brief 处理卡创建成功
     * @param cardId 卡号
     */
    void onCardCreated(const QString& cardId);

    /**
     * @brief 处理卡创建失败
     * @param message 错误消息
     */
    void onCardCreateFailed(const QString& message);

private:
    /**
     * @brief 初始化UI
     */
    void initUI();

    /**
     * @brief 初始化信号槽
     */
    void initConnections();

    /**
     * @brief 生成新卡号
     * @return 新卡号
     */
    QString generateCardId();

    /**
     * @brief 验证输入
     * @return 是否有效
     */
    bool validateInputs();

    /**
     * @brief 重置输入框
     */
    void resetInputs();

    CardController* m_cardController;  ///< 卡控制器

    ElaLineEdit* m_cardIdEdit;       ///< 卡号输入框
    ElaLineEdit* m_nameEdit;         ///< 姓名输入框
    ElaLineEdit* m_studentIdEdit;    ///< 学号输入框
    ElaLineEdit* m_passwordEdit;     ///< 密码输入框
    ElaLineEdit* m_confirmPwdEdit;   ///< 确认密码输入框
    ElaPushButton* m_registerBtn;    ///< 注册按钮
    ElaPushButton* m_generateIdBtn;  ///< 生成卡号按钮
};

}  // namespace CampusCard

#endif  // VIEW_DIALOGS_REGISTERDIALOG_H
