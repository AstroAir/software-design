# Service 和 Controller 类

本文档详细描述系统中各 Service 和 Controller 类的公共接口。

---

## Controller 层

### AuthController

认证控制器，位于 `src/controller/AuthController.h`，协调 View 和 AuthService。

#### 构造函数

```cpp
explicit AuthController(AuthService* authService, CardService* cardService, QObject* parent = nullptr);
```

#### 公共方法

##### handleStudentLogin

处理学生登录请求。

```cpp
void handleStudentLogin(const QString& cardId, const QString& password);
```

**参数**：

- `cardId` - 卡号
- `password` - 密码

**示例**：

```cpp
authController->handleStudentLogin("C001", "123456");
// 结果通过 loginSuccess 或 loginFailed 信号返回
```

##### handleAdminLogin

处理管理员登录请求。

```cpp
void handleAdminLogin(const QString& password);
```

**参数**：

- `password` - 管理员密码

##### handleLogout

处理登出请求。

```cpp
void handleLogout();
```

##### handleChangeAdminPassword

处理修改管理员密码请求。

```cpp
void handleChangeAdminPassword(const QString& oldPassword, const QString& newPassword);
```

**参数**：

- `oldPassword` - 当前密码
- `newPassword` - 新密码

##### isLoggedIn / currentRole / currentCardId

查询当前登录状态。

```cpp
[[nodiscard]] bool isLoggedIn() const;
[[nodiscard]] UserRole currentRole() const;
[[nodiscard]] QString currentCardId() const;
[[nodiscard]] QString currentUserName() const;
```

#### 信号

```cpp
signals:
    void loginSuccess(UserRole role, const QString& cardId, const QString& userName);
    void loginFailed(LoginResult result, const QString& message);
    void logoutSuccess();
    void passwordError(int remainingAttempts);
    void cardFrozen(const QString& cardId);
    void adminPasswordChanged();
    void adminPasswordChangeFailed(const QString& message);
```

### CardController

卡控制器，位于 `src/controller/CardController.h`，协调 View 和 CardService。

#### 构造函数

```cpp
explicit CardController(CardService* cardService, QObject* parent = nullptr);
```

#### 公共方法

##### getAllCards / getCard / searchCards

查询卡片。

```cpp
[[nodiscard]] QList<Card> getAllCards() const;
[[nodiscard]] Card getCard(const QString& cardId) const;
[[nodiscard]] bool cardExists(const QString& cardId) const;
[[nodiscard]] QList<Card> searchCards(const QString& keyword) const;
```

##### handleCreateCard / handleRegisterCard

处理创建卡请求。

```cpp
void handleCreateCard(const QString& cardId, const QString& name,
                      const QString& studentId, double initialBalance = 0.0);
void handleRegisterCard(const QString& cardId, const QString& name,
                        const QString& studentId, const QString& password);
```

**参数**：

- `cardId` - 卡号
- `name` - 姓名
- `studentId` - 学号
- `initialBalance` - 初始余额（默认 0）
- `password` - 密码（注册时）

##### handleRecharge / handleDeduct

处理充值/扣款请求。

```cpp
void handleRecharge(const QString& cardId, double amount);
void handleDeduct(const QString& cardId, double amount);
```

**参数**：

- `cardId` - 卡号
- `amount` - 金额

##### handleReportLost / handleCancelLost

处理挂失/解挂请求。

```cpp
void handleReportLost(const QString& cardId);
void handleCancelLost(const QString& cardId);
```

##### handleFreeze / handleUnfreeze

处理冻结/解冻请求。

```cpp
void handleFreeze(const QString& cardId);
void handleUnfreeze(const QString& cardId);
```

##### handleChangePassword / handleResetPassword

处理密码操作请求。

```cpp
void handleChangePassword(const QString& cardId, const QString& oldPassword, const QString& newPassword);
void handleResetPassword(const QString& cardId, const QString& newPassword);
```

#### 信号

```cpp
signals:
    void cardCreated(const QString& cardId);
    void cardCreateFailed(const QString& message);
    void rechargeSuccess(const QString& cardId, double newBalance);
    void rechargeFailed(const QString& message);
    void deductSuccess(const QString& cardId, double newBalance);
    void deductFailed(const QString& message);
    void reportLostSuccess(const QString& cardId);
    void cancelLostSuccess(const QString& cardId);
    void freezeSuccess(const QString& cardId);
    void unfreezeSuccess(const QString& cardId);
    void passwordChanged(const QString& cardId);
    void passwordChangeFailed(const QString& message);
    void passwordReset(const QString& cardId);
    void cardsUpdated();
    void cardUpdated(const QString& cardId);
```

### RecordController

记录控制器，位于 `src/controller/RecordController.h`，协调 View 和 RecordService。

#### 构造函数

```cpp
explicit RecordController(RecordService* recordService, CardService* cardService, QObject* parent = nullptr);
```

#### 公共方法

##### handleStartSession

处理开始上机请求。

```cpp
void handleStartSession(const QString& cardId, const QString& location);
```

**参数**：

- `cardId` - 卡号
- `location` - 上机地点

##### handleEndSession

处理结束上机请求。

```cpp
void handleEndSession(const QString& cardId);
```

**参数**：

- `cardId` - 卡号

##### isOnline / getCurrentSession / getCurrentCost

查询当前会话状态。

```cpp
[[nodiscard]] bool isOnline(const QString& cardId) const;
[[nodiscard]] Record getCurrentSession(const QString& cardId) const;
[[nodiscard]] double getCurrentCost(const QString& cardId) const;
```

##### getRecords / getRecordsByDateRange / getFilteredRecords

查询记录。

```cpp
[[nodiscard]] QList<Record> getRecords(const QString& cardId) const;
[[nodiscard]] QList<Record> getRecordsByDateRange(const QString& cardId,
                                                   const QString& startDate,
                                                   const QString& endDate) const;
[[nodiscard]] QList<Record> getFilteredRecords(const QString& cardId,
                                                const QString& startDate,
                                                const QString& endDate,
                                                const QString& location) const;
[[nodiscard]] QList<Record> getAllRecordsByDate(const QString& date) const;
```

##### 统计查询

```cpp
[[nodiscard]] int getTotalSessionCount(const QString& cardId) const;
[[nodiscard]] int getTotalDuration(const QString& cardId) const;
[[nodiscard]] double getTotalCost(const QString& cardId) const;
[[nodiscard]] double getDailyIncome(const QString& date) const;
[[nodiscard]] int getDailySessionCount(const QString& date) const;
[[nodiscard]] int getOnlineCount() const;
```

#### 信号

```cpp
signals:
    void sessionStarted(const QString& cardId, const QString& location);
    void sessionStartFailed(const QString& message);
    void sessionEnded(const QString& cardId, double cost, int duration);
    void sessionEndFailed(const QString& message);
    void recordsUpdated(const QString& cardId);
```

---

## Service 层

### AuthService

认证业务服务，位于 `src/model/services/AuthService.h`。

#### 主要方法

```cpp
LoginResult studentLogin(const QString& cardId, const QString& password);
LoginResult adminLogin(const QString& password);
void logout();
bool isLoggedIn() const;
UserRole currentRole() const;
QString currentCardId() const;
bool changeAdminPassword(const QString& oldPassword, const QString& newPassword);
```

### CardService

卡业务服务，位于 `src/model/services/CardService.h`。

#### 主要方法

```cpp
void initialize();
bool saveAll();
QList<Card> getAllCards() const;
Card findCard(const QString& cardId) const;
bool createCard(const QString& cardId, const QString& name,
                const QString& studentId, double initialBalance = 0.0);
bool recharge(const QString& cardId, double amount);
bool deduct(const QString& cardId, double amount);
bool reportLost(const QString& cardId);
bool cancelLost(const QString& cardId);
bool freeze(const QString& cardId);
bool unfreeze(const QString& cardId);
bool resetPassword(const QString& cardId, const QString& newPassword);
```

### RecordService

记录业务服务，位于 `src/model/services/RecordService.h`。

#### 主要方法

```cpp
void initialize();
Record startSession(const QString& cardId, const QString& location);
double endSession(const QString& cardId);
bool isOnline(const QString& cardId) const;
Record getCurrentSession(const QString& cardId) const;
QList<Record> getRecords(const QString& cardId) const;
QList<Record> getRecordsByDateRange(const QString& cardId,
                                     const QString& startDate,
                                     const QString& endDate) const;
int getTotalDuration(const QString& cardId) const;
double getTotalCost(const QString& cardId) const;
double getDailyIncome(const QString& date) const;
```

---

## Repository 层

### StorageManager

存储管理器（单例），位于 `src/model/repositories/StorageManager.h`，处理数据持久化。

#### 获取实例

```cpp
static StorageManager& instance();
```

#### 公共方法

##### 初始化

```cpp
void setDataPath(const QString& path);
void initializeDataDirectory();
void createSampleData();
```

##### 卡数据读写

```cpp
QList<Card> loadAllCards();
void saveAllCards(const QList<Card>& cards);
```

##### 记录数据读写

```cpp
// 根据文档要求，记录文件以学号命名（如 B17010101.txt）
QList<Record> loadRecords(const QString& studentId);
void saveRecords(const QString& studentId, const QList<Record>& records);
```

##### 管理员密码读写

```cpp
QString loadAdminPassword();
void saveAdminPassword(const QString& password);
```

##### 模拟数据

```cpp
void generateMockData(int cardCount, int recordsPerCard);
```

##### 导入导出

```cpp
bool exportAllData(const QString& filePath);
bool importData(const QString& filePath, bool merge = false);
```

**参数**：

- `filePath` - 文件路径
- `merge` - 是否合并（false 为覆盖）

---

## 下一步

- [信号与槽](signals-slots.md) - 了解事件通信
- [开发环境](../development/environment.md) - 配置开发环境
