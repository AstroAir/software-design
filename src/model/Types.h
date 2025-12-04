/**
 * @file Types.h
 * @brief 全局类型定义和枚举
 * @author CampusCardSystem
 * @date 2024
 *
 * MVC架构 - Model层公共类型定义
 */

#ifndef MODEL_TYPES_H
#define MODEL_TYPES_H

#include <QDateTime>
#include <QString>


namespace CampusCard {

/**
 * @brief 用户角色枚举
 */
enum class UserRole {
    Student,  ///< 学生角色
    Admin     ///< 管理员角色
};

/**
 * @brief 校园卡状态枚举
 */
enum class CardState {
    Normal = 0,  ///< 正常状态
    Lost = 1,    ///< 挂失状态
    Frozen = 2   ///< 冻结状态（密码错误3次）
};

/**
 * @brief 上机状态枚举
 */
enum class SessionState {
    Offline = 0,  ///< 离线/已结束
    Online = 1    ///< 上机中
};

/**
 * @brief 登录结果枚举
 */
enum class LoginResult {
    Success,             ///< 登录成功
    InvalidCredentials,  ///< 密码错误
    CardNotFound,        ///< 卡号不存在
    CardLost,            ///< 卡已挂失
    CardFrozen,          ///< 卡已冻结
    AlreadyLoggedIn      ///< 已经登录
};

/**
 * @brief 将卡状态转换为可读字符串
 * @param state 卡状态
 * @return 状态的中文描述
 */
inline QString cardStateToString(CardState state) {
    switch (state) {
    case CardState::Normal:
        return QStringLiteral("正常");
    case CardState::Lost:
        return QStringLiteral("挂失");
    case CardState::Frozen:
        return QStringLiteral("冻结");
    default:
        return QStringLiteral("未知");
    }
}

/**
 * @brief 将上机状态转换为可读字符串
 * @param state 上机状态
 * @return 状态的中文描述
 */
inline QString sessionStateToString(SessionState state) {
    switch (state) {
    case SessionState::Offline:
        return QStringLiteral("离线");
    case SessionState::Online:
        return QStringLiteral("上机中");
    default:
        return QStringLiteral("未知");
    }
}

/**
 * @brief 将登录结果转换为中文描述
 * @param result 登录结果
 * @return 描述字符串
 */
inline QString loginResultToString(LoginResult result) {
    switch (result) {
    case LoginResult::Success:
        return QStringLiteral("登录成功");
    case LoginResult::InvalidCredentials:
        return QStringLiteral("密码错误");
    case LoginResult::CardNotFound:
        return QStringLiteral("卡号不存在");
    case LoginResult::CardLost:
        return QStringLiteral("该卡已挂失，请联系管理员");
    case LoginResult::CardFrozen:
        return QStringLiteral("该卡已冻结（密码错误次数过多），请联系管理员");
    case LoginResult::AlreadyLoggedIn:
        return QStringLiteral("已经登录，请先登出");
    default:
        return QStringLiteral("未知错误");
    }
}

/**
 * @brief 每小时上机费用（元）
 */
constexpr double COST_PER_HOUR = 1.0;

/**
 * @brief 默认管理员密码
 */
const QString DEFAULT_ADMIN_PASSWORD = QStringLiteral("admin123");

/**
 * @brief 最大密码错误次数
 */
constexpr int MAX_LOGIN_ATTEMPTS = 3;

/**
 * @brief 默认学生密码
 */
const QString DEFAULT_STUDENT_PASSWORD = QStringLiteral("123456");

}  // namespace CampusCard

#endif  // MODEL_TYPES_H
