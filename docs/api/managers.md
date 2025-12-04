# 管理器类

本文档详细描述系统中各管理器类的公共接口。

## AuthManager

认证管理器，处理用户登录和权限管理。

### 构造函数

```cpp
explicit AuthManager(CardManager* cardManager, QObject* parent = nullptr);
```

### 公共方法

#### studentLogin

学生登录验证。

```cpp
LoginResult studentLogin(const QString& cardId, const QString& password);
```

**参数**：

- `cardId` - 卡号
- `password` - 密码

**返回值**：`LoginResult` 枚举值

**示例**：

```cpp
auto result = authManager->studentLogin("C001", "123456");
if (result == LoginResult::Success) {
    // 登录成功
}
```

#### adminLogin

管理员登录验证。

```cpp
LoginResult adminLogin(const QString& password);
```

**参数**：

- `password` - 管理员密码

**返回值**：`LoginResult` 枚举值

#### logout

登出当前用户。

```cpp
void logout();
```

#### changeAdminPassword

修改管理员密码。

```cpp
bool changeAdminPassword(const QString& oldPassword, const QString& newPassword);
```

**参数**：

- `oldPassword` - 当前密码
- `newPassword` - 新密码

**返回值**：修改是否成功

#### currentUser

获取当前登录用户。

```cpp
[[nodiscard]] const User* currentUser() const;
```

**返回值**：当前用户指针，未登录时返回 `nullptr`

#### isLoggedIn

检查是否已登录。

```cpp
[[nodiscard]] bool isLoggedIn() const;
```

## CardManager

卡片管理器，处理卡片的增删改查。

### 构造函数

```cpp
explicit CardManager(QObject* parent = nullptr);
```

### 公共方法

#### getAllCards

获取所有卡片。

```cpp
[[nodiscard]] QList<Card> getAllCards() const;
```

#### findCard

按卡号查找卡片。

```cpp
[[nodiscard]] std::optional<Card> findCard(const QString& cardId) const;
```

**参数**：

- `cardId` - 卡号

**返回值**：找到返回 `Card`，否则返回 `std::nullopt`

#### findCardByStudentId

按学号查找卡片。

```cpp
[[nodiscard]] std::optional<Card> findCardByStudentId(const QString& studentId) const;
```

#### createCard

创建新卡。

```cpp
bool createCard(const QString& cardId, const QString& name,
                const QString& studentId, double balance = 0.0);
```

**参数**：

- `cardId` - 卡号
- `name` - 姓名
- `studentId` - 学号
- `balance` - 初始余额（默认 0）

**返回值**：创建是否成功

#### rechargeCard

充值。

```cpp
bool rechargeCard(const QString& cardId, double amount);
```

**参数**：

- `cardId` - 卡号
- `amount` - 充值金额

**返回值**：充值是否成功

#### deductFromCard

扣款。

```cpp
bool deductFromCard(const QString& cardId, double amount);
```

#### reportCardLost / cancelCardLost

挂失/解挂。

```cpp
bool reportCardLost(const QString& cardId);
bool cancelCardLost(const QString& cardId);
```

#### freezeCard / unfreezeCard

冻结/解冻。

```cpp
bool freezeCard(const QString& cardId);
bool unfreezeCard(const QString& cardId);
```

#### resetPassword

重置密码。

```cpp
bool resetPassword(const QString& cardId, const QString& newPassword);
```

## RecordManager

记录管理器，处理上机会话和统计。

### 构造函数

```cpp
explicit RecordManager(CardManager* cardManager, QObject* parent = nullptr);
```

### 公共方法

#### startSession

开始上机。

```cpp
bool startSession(const QString& cardId, const QString& location);
```

**参数**：

- `cardId` - 卡号
- `location` - 上机地点

**返回值**：是否成功开始

#### endSession

结束上机。

```cpp
double endSession(const QString& cardId);
```

**参数**：

- `cardId` - 卡号

**返回值**：本次上机费用

#### isOnline

检查是否在线。

```cpp
[[nodiscard]] bool isOnline(const QString& cardId) const;
```

#### getCurrentSession

获取当前会话。

```cpp
[[nodiscard]] std::optional<Record> getCurrentSession(const QString& cardId) const;
```

#### getRecords

获取所有记录。

```cpp
[[nodiscard]] QList<Record> getRecords(const QString& cardId) const;
```

#### getRecordsByDate

按日期查询记录。

```cpp
[[nodiscard]] QList<Record> getRecordsByDate(const QString& cardId, 
                                              const QString& date) const;
```

#### getRecordsByDateRange

按日期范围查询记录。

```cpp
[[nodiscard]] QList<Record> getRecordsByDateRange(const QString& cardId,
                                                   const QString& startDate,
                                                   const QString& endDate) const;
```

#### getTotalDuration

统计总时长。

```cpp
[[nodiscard]] int getTotalDuration(const QString& cardId) const;
```

**返回值**：总时长（分钟）

#### getTotalCost

统计总费用。

```cpp
[[nodiscard]] double getTotalCost(const QString& cardId) const;
```

#### getDailyIncome

统计当日收入。

```cpp
[[nodiscard]] double getDailyIncome(const QString& date) const;
```

#### getAllRecordsByDate

获取所有卡的当日记录。

```cpp
[[nodiscard]] QList<Record> getAllRecordsByDate(const QString& date) const;
```

## StorageManager

存储管理器（单例），处理数据持久化。

### 获取实例

```cpp
static StorageManager& instance();
```

### 公共方法

#### loadAllCards / saveAllCards

卡数据读写。

```cpp
QList<Card> loadAllCards();
void saveAllCards(const QList<Card>& cards);
```

#### loadRecords / saveRecords

记录数据读写。

```cpp
QList<Record> loadRecords(const QString& cardId);
void saveRecords(const QString& cardId, const QList<Record>& records);
```

#### loadAdminPassword / saveAdminPassword

管理员密码读写。

```cpp
QString loadAdminPassword();
void saveAdminPassword(const QString& password);
```

#### initializeDataDirectory

初始化数据目录。

```cpp
void initializeDataDirectory();
```

#### createSampleData

创建示例数据。

```cpp
void createSampleData();
```

#### generateMockData

生成模拟数据。

```cpp
void generateMockData(int cardCount, int recordsPerCard);
```

#### exportAllData

导出所有数据。

```cpp
bool exportAllData(const QString& filePath);
```

#### importData

导入数据。

```cpp
bool importData(const QString& filePath, bool merge = false);
```

**参数**：

- `filePath` - 文件路径
- `merge` - 是否合并（false 为覆盖）

## 下一步

- [信号与槽](signals-slots.md) - 了解事件通信
- [开发环境](../development/environment.md) - 配置开发环境
