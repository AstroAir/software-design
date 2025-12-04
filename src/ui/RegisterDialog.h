/**
 * @file RegisterDialog.h
 * @brief 注册对话框（创建新卡）
 * @author CampusCardSystem
 * @date 2024
 */

#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include "ElaDialog.h"
#include "core/CardManager.h"

class ElaLineEdit;
class ElaPushButton;

namespace CampusCard {

/**
 * @class RegisterDialog
 * @brief 注册对话框，用于创建新的校园卡
 */
class RegisterDialog : public ElaDialog {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param cardManager 卡管理器
     * @param parent 父窗口
     */
    explicit RegisterDialog(CardManager* cardManager, QWidget* parent = nullptr);

private slots:
    /**
     * @brief 处理注册按钮点击
     */
    void onRegisterClicked();

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

    CardManager* m_cardManager;     ///< 卡管理器

    ElaLineEdit* m_cardIdEdit;      ///< 卡号输入框
    ElaLineEdit* m_nameEdit;        ///< 姓名输入框
    ElaLineEdit* m_studentIdEdit;   ///< 学号输入框
    ElaLineEdit* m_passwordEdit;    ///< 密码输入框
    ElaLineEdit* m_confirmPwdEdit;  ///< 确认密码输入框
    ElaPushButton* m_registerBtn;   ///< 注册按钮
    ElaPushButton* m_generateIdBtn; ///< 生成卡号按钮
};

} // namespace CampusCard

#endif // REGISTERDIALOG_H
