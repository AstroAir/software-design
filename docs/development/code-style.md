# 代码规范

本文档描述项目的编码规范和最佳实践。

## 命名规范

### 类名

使用**大驼峰命名法**（PascalCase）。

```cpp
// ✅ 正确
class CardManager;
class StudentDashboard;

// ❌ 错误
class cardManager;
class student_dashboard;
```

### 成员变量

使用 **`m_` 前缀** + 小驼峰命名法。

```cpp
// ✅ 正确
QString m_cardId;
double m_balance;

// ❌ 错误
QString cardId;
QString _cardId;
```

### 函数名

使用**小驼峰命名法**（camelCase）。

```cpp
// ✅ 正确
void rechargeCard();
bool isUsable() const;

// ❌ 错误
void RechargeCard();
void recharge_card();
```

### 常量

使用**全大写 + 下划线**。

```cpp
// ✅ 正确
constexpr double COST_PER_HOUR = 1.0;
constexpr int MAX_LOGIN_ATTEMPTS = 3;

// ❌ 错误
constexpr double costPerHour = 1.0;
```

### 枚举

使用 `enum class` 强类型枚举，枚举值使用大驼峰。

```cpp
// ✅ 正确
enum class CardState {
    Normal,
    Lost,
    Frozen
};

// ❌ 错误
enum CardState {
    NORMAL,
    LOST,
    FROZEN
};
```

## 代码格式

### 缩进

使用 **4 个空格**，不使用 Tab。

### 大括号

使用 **K&R 风格**（左括号不换行）。

```cpp
// ✅ 正确
if (condition) {
    doSomething();
} else {
    doOther();
}

// ❌ 错误
if (condition)
{
    doSomething();
}
```

### 行宽

每行不超过 **100 个字符**。

### 空行

-   函数之间空一行
-   逻辑块之间空一行
-   类成员分组之间空一行

## 注释规范

### 文件头

```cpp
/**
 * @file CardManager.h
 * @brief 校园卡管理器
 * @author Your Name
 * @date 2024-12-03
 */
```

### 类注释

```cpp
/**
 * @class CardManager
 * @brief 管理所有校园卡的增删改查操作
 *
 * CardManager 负责卡片的生命周期管理，包括创建、查询、
 * 充值、挂失、冻结等操作。
 */
class CardManager : public QObject {
    // ...
};
```

### 函数注释

```cpp
/**
 * @brief 为指定卡充值
 * @param cardId 卡号
 * @param amount 充值金额（必须大于 0）
 * @return 充值是否成功
 * @throws std::invalid_argument 如果金额小于等于 0
 */
bool rechargeCard(const QString& cardId, double amount);
```

### 成员变量注释

```cpp
class Card {
private:
    QString m_cardId;       ///< 卡号（唯一标识）
    QString m_name;         ///< 持卡人姓名
    double m_balance;       ///< 当前余额
    CardState m_state;      ///< 卡状态
};
```

## C++ 特性使用

### 使用 `[[nodiscard]]`

对于返回值不应被忽略的函数，使用 `[[nodiscard]]` 属性。

```cpp
[[nodiscard]] bool isUsable() const;
[[nodiscard]] double calculateCost() const;
```

### 使用 `const` 正确性

-   不修改对象的成员函数标记为 `const`
-   不修改的参数使用 `const` 引用

```cpp
[[nodiscard]] QString cardId() const { return m_cardId; }
bool verifyPassword(const QString& password) const;
```

### 使用 `std::optional`

对于可能不存在的返回值，使用 `std::optional`。

```cpp
[[nodiscard]] std::optional<Card> findCard(const QString& cardId) const;
```

### 使用智能指针

优先使用智能指针管理动态内存。

```cpp
std::unique_ptr<Card> createCard();
std::shared_ptr<Record> getRecord();
```

## Qt 特性使用

### 信号槽

使用新式连接语法。

```cpp
// ✅ 正确
connect(button, &QPushButton::clicked, this, &MyClass::onClicked);

// ❌ 避免
connect(button, SIGNAL(clicked()), this, SLOT(onClicked()));
```

### 属性系统

使用 `Q_PROPERTY` 暴露属性。

```cpp
class Card : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString cardId READ cardId CONSTANT)
    Q_PROPERTY(double balance READ balance NOTIFY balanceChanged)

public:
    QString cardId() const { return m_cardId; }
    double balance() const { return m_balance; }

signals:
    void balanceChanged();
};
```

### 父子关系

利用 Qt 的父子关系管理内存。

```cpp
auto* button = new QPushButton("Click", this);  // this 作为父对象
```

## 错误处理

### 使用返回值

对于可预期的错误，使用返回值。

```cpp
bool rechargeCard(const QString& cardId, double amount) {
    if (amount <= 0) {
        return false;
    }
    // ...
    return true;
}
```

### 使用枚举结果

对于多种错误情况，使用枚举。

```cpp
enum class LoginResult {
    Success,
    InvalidCredentials,
    CardNotFound,
    CardFrozen
};

LoginResult login(const QString& cardId, const QString& password);
```

## 文件组织

### 头文件

```cpp
#ifndef CARDMANAGER_H
#define CARDMANAGER_H

// 1. Qt 头文件
#include <QObject>
#include <QString>

// 2. 标准库头文件
#include <optional>

// 3. 项目头文件
#include "Card.h"

namespace CampusCard {

class CardManager : public QObject {
    // ...
};

} // namespace CampusCard

#endif // CARDMANAGER_H
```

### 源文件

```cpp
#include "CardManager.h"

// 1. 对应头文件
// 2. Qt 头文件
// 3. 标准库头文件
// 4. 其他项目头文件

namespace CampusCard {

// 实现...

} // namespace CampusCard
```

## 下一步

-   [贡献指南](contributing.md) - 参与项目开发
