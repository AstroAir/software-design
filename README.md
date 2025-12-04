# 校园卡消费记录查询系统

基于 **Qt6 + C++20 + ElaWidgetTools** 开发的模拟校园卡消费记录查询系统，提供美观的现代化 Fluent Design 风格界面和完整的机房上机管理功能。

---

## 目录

- [功能特性](#功能特性)
- [技术栈](#技术栈)
- [项目结构](#项目结构)
- [架构设计](#架构设计)
- [核心类详解](#核心类详解)
- [UI 组件详解](#ui-组件详解)
- [构建指南](#构建指南)
- [使用说明](#使用说明)
- [API 参考](#api-参考)
- [开发说明](#开发说明)
- [许可证](#许可证)

---

## 功能特性

### 登录模块

| 功能 | 说明 |
|------|------|
| **学生登录** | 使用卡号和密码登录系统 |
| **管理员登录** | 使用管理员密码登录系统 |
| **安全机制** | 连续 3 次密码错误自动冻结账户（`MAX_LOGIN_ATTEMPTS = 3`） |
| **新用户注册** | 支持创建新校园卡（自动生成卡号或手动输入） |
| **登录状态提示** | 实时显示剩余登录次数和账户状态 |

### 管理员功能

| 功能 | 说明 |
|------|------|
| **卡列表管理** | 查看所有学生卡，支持实时搜索筛选（按卡号/姓名/学号） |
| **充值** | 为学生卡充值任意金额，显示充值前后余额变化 |
| **挂失/解挂** | 将学生卡设置为挂失状态或解除挂失 |
| **解冻** | 解冻因密码错误被冻结的账户，同时重置错误计数 |
| **重置密码** | 重置学生登录密码为默认值（`123456`） |
| **添加新卡** | 手动创建新的校园卡，支持自定义卡号和初始余额 |
| **统计报表** | 查看指定日期的收入、上机次数、总时长及详细记录明细 |
| **数据导入/导出** | 将所有数据导出为 JSON 或从 JSON 导入（支持合并/覆盖模式） |
| **生成模拟数据** | 一键生成测试用的卡和上机记录（可配置数量） |
| **修改管理员密码** | 更改管理员登录密码（需验证旧密码） |
| **实时统计面板** | 显示当日收入、总卡数、当前在线人数 |

### 学生功能

| 功能 | 说明 |
|------|------|
| **卡片信息** | 查看姓名、学号、余额、卡状态 |
| **开始/结束上机** | 选择上机地点后开始计时，结束时自动扣费 |
| **上机状态显示** | 实时显示当前上机时间、地点、预计费用 |
| **上机记录查询** | 按日期范围和地点筛选历史上机记录 |
| **统计信息** | 查看总上机次数、总时长、总费用 |
| **修改密码** | 修改个人登录密码（需验证旧密码） |

---

## 技术栈

| 类别 | 技术 |
|------|------|
| **编程语言** | C++20 |
| **GUI 框架** | Qt6 (Widgets, Core, Gui) |
| **UI 组件库** | [ElaWidgetTools](https://github.com/AstroAir/ElaWidgetTools) - Fluent Design 风格 |
| **构建系统** | CMake (>= 3.24) |
| **数据存储** | JSON 文件（基于 Qt JSON 模块） |
| **设计模式** | 单例模式、观察者模式（信号槽）、MVC 架构 |

---

## 项目结构

```text
software-design/
├── CMakeLists.txt                  # CMake 配置文件（版本 1.0.0）
├── CMakePresets.json               # CMake 预设配置（MSVC/MinGW/Ninja）
├── CMakeUserPresets.json.example   # 用户预设示例
├── README.md                       # 项目说明文档
├── question.md                     # 题目要求
│
├── src/
│   ├── main.cpp                    # 程序入口
│   │
│   ├── core/                       # 核心业务逻辑层（15 个文件）
│   │   ├── Types.h                 # 全局类型定义（枚举、常量）
│   │   ├── Card.h/cpp              # 校园卡实体类
│   │   ├── Record.h/cpp            # 上机记录实体类
│   │   ├── User.h/cpp              # 用户实体类
│   │   ├── AuthManager.h/cpp       # 认证管理器（登录/登出）
│   │   ├── CardManager.h/cpp       # 卡管理器（CRUD、充值、挂失等）
│   │   ├── RecordManager.h/cpp     # 记录管理器（上下机、统计）
│   │   └── StorageManager.h/cpp    # 存储管理器（JSON 读写，单例模式）
│   │
│   └── ui/                         # 用户界面层（16 个文件）
│       ├── MainWindow.h/cpp        # 主窗口（ElaWindow 基类，导航管理）
│       ├── LoginDialog.h/cpp       # 登录对话框（ElaDialog）
│       ├── RegisterDialog.h/cpp    # 注册对话框（创建新卡）
│       ├── AdminDashboard.h/cpp    # 管理员控制面板
│       ├── StudentDashboard.h/cpp  # 学生控制面板
│       ├── RechargeDialog.h/cpp    # 充值对话框（ElaContentDialog）
│       ├── RecordTableWidget.h/cpp # 上机记录表格组件
│       └── StatisticsWidget.h/cpp  # 统计报表组件
│
├── resources/
│   ├── resources.qrc               # Qt 资源文件
│   └── icons/
│       └── app.png                 # 应用图标
│
├── data/                           # 数据目录（运行时自动创建）
│   ├── cards.json                  # 校园卡数据
│   ├── admin.json                  # 管理员密码配置
│   └── records/                    # 上机记录目录
│       └── <卡号>.json             # 每张卡的上机记录
│
└── third_party/
    └── ElaWidgetTools/             # UI 组件库（Git 子模块）
```

---

## 架构设计

本系统采用**三层架构**，职责分离清晰：

```
┌─────────────────────────────────────────────────────────────┐
│                        UI 层 (src/ui/)                       │
│  MainWindow / LoginDialog / AdminDashboard / StudentDashboard│
│  RegisterDialog / RechargeDialog / RecordTableWidget / ...   │
├─────────────────────────────────────────────────────────────┤
│                      业务逻辑层 (src/core/)                   │
│         AuthManager / CardManager / RecordManager            │
├─────────────────────────────────────────────────────────────┤
│                      数据持久层 (StorageManager)              │
│                    JSON 文件读写 (data/)                      │
└─────────────────────────────────────────────────────────────┘
```

### 设计原则

- **单一职责**：每个类只负责一个功能领域
- **依赖注入**：UI 层通过构造函数注入管理器依赖
- **信号槽解耦**：使用 Qt 信号槽机制实现模块间通信
- **单例模式**：`StorageManager` 使用单例确保数据一致性

### 数据流

1. **登录流程**：`LoginDialog` → `AuthManager.studentLogin()` → `CardManager.findCard()` → 验证密码 → 发射 `loginSucceeded` 信号 → `MainWindow` 切换界面
2. **上机流程**：`StudentDashboard` → `RecordManager.startSession()` → 创建 `Record` → `StorageManager` 保存 → 发射 `sessionStarted` 信号
3. **下机流程**：`StudentDashboard` → `RecordManager.endSession()` → 计算费用 → `CardManager.deductFromCard()` → 更新余额 → 发射 `sessionEnded` 信号

---

## 核心类详解

### Types.h - 全局类型定义

定义系统中使用的枚举类型和常量：

```cpp
namespace CampusCard {
    // 用户角色
    enum class UserRole { Student, Admin };
    
    // 校园卡状态
    enum class CardState { 
        Normal = 0,   // 正常
        Lost = 1,     // 挂失
        Frozen = 2    // 冻结
    };
    
    // 上机状态
    enum class SessionState { 
        Offline = 0,  // 离线/已结束
        Online = 1    // 上机中
    };
    
    // 系统常量
    constexpr double COST_PER_HOUR = 1.0;           // 每小时费用
    const QString DEFAULT_ADMIN_PASSWORD = "admin123";
    constexpr int MAX_LOGIN_ATTEMPTS = 3;           // 最大登录尝试次数
}
```

### Card 类 - 校园卡实体

| 属性 | 类型 | 说明 |
|------|------|------|
| `m_cardId` | `QString` | 卡号（唯一标识） |
| `m_name` | `QString` | 持卡人姓名 |
| `m_studentId` | `QString` | 学号 |
| `m_totalRecharge` | `double` | 累计充值金额 |
| `m_balance` | `double` | 当前余额 |
| `m_state` | `CardState` | 卡状态 |
| `m_loginAttempts` | `int` | 密码错误次数 |
| `m_password` | `QString` | 登录密码（默认 123456） |

**主要方法**：

- `recharge(double amount)` - 充值
- `deduct(double amount)` - 扣款
- `reportLost()` / `cancelLost()` - 挂失/解挂
- `freeze()` / `unfreeze()` - 冻结/解冻
- `incrementLoginAttempts()` - 增加登录失败次数
- `isUsable()` - 检查卡是否可用
- `toJson()` / `fromJson()` - JSON 序列化

### Record 类 - 上机记录实体

| 属性 | 类型 | 说明 |
|------|------|------|
| `m_recordId` | `QString` | 记录唯一 ID（UUID） |
| `m_cardId` | `QString` | 关联卡号 |
| `m_date` | `QString` | 上机日期（yyyy-MM-dd） |
| `m_startTime` | `QDateTime` | 开始时间 |
| `m_endTime` | `QDateTime` | 结束时间 |
| `m_durationMinutes` | `int` | 上机时长（分钟） |
| `m_cost` | `double` | 上机费用 |
| `m_state` | `SessionState` | 上机状态 |
| `m_location` | `QString` | 上机地点 |

**主要方法**：

- `createNew(cardId, location)` - 创建新记录
- `endSession()` - 结束会话，计算费用
- `calculateCurrentCost()` - 计算当前费用（不结束）
- `isOnline()` - 是否在上机中

### AuthManager 类 - 认证管理器

**登录结果枚举**：

```cpp
enum class LoginResult {
    Success,            // 登录成功
    InvalidCredentials, // 密码错误
    CardNotFound,       // 卡号不存在
    CardLost,           // 卡已挂失
    CardFrozen,         // 卡已冻结
    AlreadyLoggedIn     // 已经登录
};
```

**主要方法**：

- `studentLogin(cardId, password)` - 学生登录
- `adminLogin(password)` - 管理员登录
- `logout()` - 登出
- `changeAdminPassword(oldPwd, newPwd)` - 修改管理员密码

**信号**：

- `loginSucceeded(UserRole, QString cardId)` - 登录成功
- `loginFailed(LoginResult)` - 登录失败
- `loggedOut()` - 已登出

### CardManager 类 - 卡管理器

**主要方法**：

- `getAllCards()` - 获取所有卡
- `findCard(cardId)` - 按卡号查找
- `findCardByStudentId(studentId)` - 按学号查找
- `createCard(cardId, name, studentId, balance)` - 创建新卡
- `rechargeCard(cardId, amount)` - 充值
- `deductFromCard(cardId, amount)` - 扣款
- `reportCardLost(cardId)` / `cancelCardLost(cardId)` - 挂失/解挂
- `freezeCard(cardId)` / `unfreezeCard(cardId)` - 冻结/解冻
- `resetPassword(cardId, newPassword)` - 重置密码

**信号**：

- `cardsChanged()` - 卡数据变更
- `cardUpdated(QString cardId)` - 单张卡更新

### RecordManager 类 - 记录管理器

**主要方法**：

- `startSession(cardId, location)` - 开始上机
- `endSession(cardId)` - 结束上机，返回费用
- `isOnline(cardId)` - 检查是否在线
- `getCurrentSession(cardId)` - 获取当前会话
- `getRecords(cardId)` - 获取所有记录
- `getRecordsByDate(cardId, date)` - 按日期查询
- `getTotalDuration(cardId)` - 统计总时长
- `getTotalCost(cardId)` - 统计总费用
- `getDailyIncome(date)` - 统计当日收入
- `getAllRecordsByDate(date)` - 获取所有卡的当日记录

**信号**：

- `recordsChanged(QString cardId)` - 记录变更
- `sessionStarted(QString cardId, QString location)` - 上机开始
- `sessionEnded(QString cardId, double cost)` - 上机结束

### StorageManager 类 - 存储管理器（单例）

**数据文件结构**：

```text
data/
├── cards.json          # 所有校园卡
├── admin.json          # 管理员密码
└── records/
    ├── C001.json       # C001 的上机记录
    ├── C002.json       # C002 的上机记录
    └── ...
```

**主要方法**：

- `instance()` - 获取单例实例
- `loadAllCards()` / `saveAllCards()` - 卡数据读写
- `loadRecords(cardId)` / `saveRecords(cardId, records)` - 记录读写
- `loadAdminPassword()` / `saveAdminPassword()` - 管理员密码
- `initializeDataDirectory()` - 初始化数据目录
- `createSampleData()` - 创建示例数据
- `generateMockData(cardCount, recordsPerCard)` - 生成模拟数据
- `exportAllData(filePath)` - 导出所有数据
- `importData(filePath, merge)` - 导入数据

---

## UI 组件详解

### MainWindow - 主窗口

继承自 `ElaWindow`，提供 Fluent Design 风格的主界面框架。

**功能**：

- 侧边栏导航（主页、管理员、学生）
- 页面切换管理
- 搜索建议框（`ElaSuggestBox`）
- 登录/登出状态管理
- 关于对话框

**关键成员**：

- `m_cardManager` / `m_recordManager` / `m_authManager` - 业务管理器
- `m_loginDialog` / `m_adminDashboard` / `m_studentDashboard` - 子界面
- `m_suggestBox` - 搜索建议框

### LoginDialog - 登录对话框

继承自 `ElaDialog`，提供登录和注册入口。

**UI 组件**：

- `ElaComboBox` - 角色选择（学生/管理员）
- `ElaLineEdit` - 卡号输入（学生模式）
- `ElaLineEdit` - 密码输入
- `ElaPushButton` - 登录/注册按钮
- `ElaText` - 提示信息标签

**功能**：

- 角色切换时动态显示/隐藏卡号输入框
- 登录失败时显示剩余尝试次数
- 跳转到注册对话框

### RegisterDialog - 注册对话框

继承自 `ElaDialog`，用于创建新校园卡。

**UI 组件**：

- 卡号输入框（可手动输入或自动生成）
- 姓名输入框
- 学号输入框
- 密码/确认密码输入框
- 生成卡号按钮

### AdminDashboard - 管理员控制面板

**UI 布局**：

```text
┌─────────────────────────────────────────────────────────┐
│  搜索框  │  当日收入  │  总卡数  │  在线人数  │  日期选择  │
├─────────────────────────────────────────────────────────┤
│                                                         │
│                    卡列表表格                            │
│  (卡号 | 姓名 | 学号 | 余额 | 状态 | 累计充值)           │
│                                                         │
├─────────────────────────────────────────────────────────┤
│ 充值 │ 挂失 │ 解挂 │ 解冻 │ 重置密码 │ 添加新卡 │ 统计   │
│ 导出 │ 导入 │ 生成模拟数据 │ 修改管理员密码 │ 退出登录   │
└─────────────────────────────────────────────────────────┘
```

**功能按钮**：

- 充值、挂失、解挂、解冻、重置密码
- 添加新卡、统计报表
- 导出/导入数据
- 生成模拟数据
- 修改管理员密码
- 退出登录

### StudentDashboard - 学生控制面板

**UI 布局**：

```text
┌─────────────────────────────────────────────────────────┐
│  卡片信息区域                                            │
│  姓名: xxx  │  学号: xxx  │  余额: xxx  │  状态: xxx     │
├─────────────────────────────────────────────────────────┤
│  上机状态区域                                            │
│  当前状态: 上机中/离线  │  开始时间  │  地点  │  预计费用  │
│  [开始上机]  [结束上机]                                  │
├─────────────────────────────────────────────────────────┤
│  统计信息                                               │
│  总上机次数: xxx  │  总时长: xxx  │  总费用: xxx         │
├─────────────────────────────────────────────────────────┤
│  日期筛选: [开始日期] - [结束日期]  │  地点筛选: [下拉框] │
├─────────────────────────────────────────────────────────┤
│                    上机记录表格                          │
│  (日期 | 开始时间 | 结束时间 | 时长 | 费用 | 地点 | 状态) │
├─────────────────────────────────────────────────────────┤
│  [修改密码]  [退出登录]                                  │
└─────────────────────────────────────────────────────────┘
```

### RechargeDialog - 充值对话框

继承自 `ElaContentDialog`，显示卡信息和充值输入。

### StatisticsWidget - 统计报表组件

显示指定日期的统计信息：

- 当日总收入
- 上机次数
- 总时长
- 详细记录表格

### RecordTableWidget - 上机记录表格

可复用的表格组件，显示上机记录列表。

**表格列**：日期、开始时间、结束时间、时长、费用、地点、状态

---

## 构建指南

### 前置要求

- **CMake** >= 3.24
- **Qt6** >= 6.5
- **C++20 编译器**：MSVC 2019+、GCC 10+、Clang 10+

### 克隆项目

```bash
git clone --recursive https://github.com/your-repo/software-design.git
cd software-design
```

如果已克隆但缺少子模块：

```bash
git submodule update --init --recursive
```

### 使用 CMake Presets 构建（推荐）

项目提供了 `CMakePresets.json` 预设配置，支持多种构建环境。

#### 1. 配置环境变量

**MSVC 构建**：

```powershell
$env:QT_MSVC_PATH = "C:/Qt/6.7.0/msvc2019_64"
```

**MinGW64 构建**：

```powershell
$env:MSYS2_ROOT = "C:/msys64"
$env:QT_MINGW_PATH = "C:/msys64/mingw64"
```

#### 2. 配置和构建

**MSVC Release：**

```bash
cmake --preset msvc-release
cmake --build --preset msvc-release
```

**MinGW64 Release (Ninja)：**

```bash
cmake --preset ninja-mingw-release
cmake --build --preset ninja-mingw-release
```

**MinGW64 Release (MinGW Makefiles)：**

```bash
cmake --preset mingw-release
cmake --build --preset mingw-release
```

#### 3. 自定义预设（可选）

复制 `CMakeUserPresets.json.example` 为 `CMakeUserPresets.json`，根据本地环境修改路径。

### 传统构建方式

#### Windows (Visual Studio)

```bash
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -DCMAKE_PREFIX_PATH="C:/Qt/6.7.0/msvc2019_64"
cmake --build . --config Release
```

#### Windows (MSYS2 MinGW64)

```bash
mkdir build && cd build
cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Release
ninja
```

或使用 MinGW Makefiles：

```bash
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
mingw32-make -j$(nproc)
```

#### Linux / macOS

```bash
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH="$HOME/Qt/6.7.0/gcc_64"
make -j$(nproc)
```

### 运行程序

构建完成后，可执行文件位于 `build/` 目录（或对应的 preset 输出目录）。首次运行时会自动在可执行文件同级目录创建 `data/` 文件夹并生成示例数据。

---

## 使用说明

### 默认账户

| 角色 | 凭据 |
|------|------|
| **管理员** | 密码：`admin123` |
| **学生 1** | 卡号：`C001`，姓名：张三，密码：`123456` |
| **学生 2** | 卡号：`C002`，姓名：李四，密码：`123456` |
| **学生 3** | 卡号：`C003`，姓名：王五，密码：`123456` |

### 操作流程

#### 学生上机流程

1. 在登录界面选择「学生」角色
2. 输入卡号和密码，点击「登录」
3. 在学生面板点击「开始上机」，选择上机地点
4. 上机结束后点击「结束上机」，系统自动计算费用并扣款
5. 可在「上机记录」区域查看历史记录和统计信息

#### 管理员操作流程

1. 在登录界面选择「管理员」角色
2. 输入管理员密码，点击「登录」
3. 在管理员面板可进行：
   - **充值**：选中卡后点击「充值」，输入金额
   - **挂失/解挂**：选中卡后点击对应按钮
   - **解冻**：选中被冻结的卡后点击「解冻」
   - **重置密码**：选中卡后点击「重置密码」
   - **统计报表**：点击「统计」查看指定日期的收入明细

### 数据存储

| 文件 | 内容 |
|------|------|
| `data/cards.json` | 所有校园卡信息（卡号、姓名、学号、余额、状态、密码等） |
| `data/admin.json` | 管理员密码 |
| `data/records/<卡号>.json` | 每张卡的上机记录列表 |

- 所有数据以 **JSON 格式**存储，便于查看和调试
- 程序首次运行会自动创建示例数据
- 支持通过管理员面板导入/导出全部数据

### 计费规则

- **费率**：每小时 **1.0 元**
- **计费方式**：按分钟计费，不足一分钟按一分钟计算
- **扣费时机**：结束上机时从余额中扣除

### 卡状态说明

| 状态 | 说明 |
|------|------|
| **正常** | 可正常登录和上机 |
| **挂失** | 无法登录，需管理员解挂 |
| **冻结** | 密码错误 3 次后自动冻结，需管理员解冻 |

---

## API 参考

### JSON 数据格式

#### cards.json - 校园卡数据

```json
[
    {
        "cardId": "C001",
        "name": "张三",
        "studentId": "2024001",
        "totalRecharge": 100.0,
        "balance": 85.5,
        "state": 0,
        "loginAttempts": 0,
        "password": "123456"
    }
]
```

#### admin.json - 管理员配置

```json
{
    "password": "admin123"
}
```

#### records/C001.json - 上机记录

```json
[
    {
        "recordId": "uuid-xxxx-xxxx",
        "cardId": "C001",
        "date": "2024-12-03",
        "startTime": "2024-12-03T09:00:00",
        "endTime": "2024-12-03T11:30:00",
        "durationMinutes": 150,
        "cost": 2.5,
        "state": 0,
        "location": "机房A"
    }
]
```

### 状态码对照

| 枚举 | 值 | 说明 |
|------|------|------|
| `CardState::Normal` | 0 | 正常 |
| `CardState::Lost` | 1 | 挂失 |
| `CardState::Frozen` | 2 | 冻结 |
| `SessionState::Offline` | 0 | 离线/已结束 |
| `SessionState::Online` | 1 | 上机中 |

### 费用计算公式

```text
费用 = ceil(上机分钟数) * (COST_PER_HOUR / 60)
     = ceil(分钟数) * (1.0 / 60)
     = ceil(分钟数) / 60 元
```

---

## 开发说明

### 代码规范

- 所有变量、函数均有 **Doxygen 风格注释**
- 遵循 **Qt 编码规范**
- 类名使用**大驼峰命名**（如 `CardManager`）
- 成员变量使用 **`m_` 前缀**（如 `m_cardId`）
- 使用 `[[nodiscard]]` 标记返回值不应被忽略的函数
- 枚举类型使用 `enum class` 强类型枚举
- 所有类都在 `CampusCard` 命名空间内

### 信号与槽

系统大量使用 Qt 信号槽机制实现模块间解耦：

| 信号 | 发射时机 | 接收者 |
|------|----------|--------|
| `AuthManager::loginSucceeded` | 登录成功 | `MainWindow` 切换界面 |
| `AuthManager::loginFailed` | 登录失败 | `LoginDialog` 显示错误 |
| `AuthManager::loggedOut` | 用户登出 | `MainWindow` 返回欢迎页 |
| `CardManager::cardsChanged` | 卡数据变更 | `AdminDashboard` 刷新列表 |
| `CardManager::cardUpdated` | 单卡更新 | `StudentDashboard` 刷新信息 |
| `RecordManager::sessionStarted` | 上机开始 | UI 更新状态 |
| `RecordManager::sessionEnded` | 上机结束 | UI 更新统计 |
| `RecordManager::recordsChanged` | 记录变更 | 表格刷新 |

### 文件依赖关系

```text
Types.h
    ↓
Card.h ← Record.h ← User.h
    ↓         ↓
CardManager.h  RecordManager.h
    ↓              ↓
    └──── AuthManager.h
              ↓
         StorageManager.h (单例)
              ↓
         UI 层组件
```

### 扩展建议

- **数据库支持**：将 `StorageManager` 改为使用 SQLite 或其他数据库
- **网络功能**：添加服务端，支持多客户端同时使用
- **报表导出**：支持导出 Excel/PDF 格式的统计报表
- **多语言支持**：使用 Qt Linguist 实现国际化
- **主题切换**：ElaWidgetTools 支持明暗主题切换
- **日志系统**：添加操作日志记录功能

### 测试建议

1. **登录测试**：验证正常登录、密码错误、账户冻结等场景
2. **上机测试**：验证开始/结束上机、费用计算、余额扣除
3. **管理员测试**：验证充值、挂失、解冻、密码重置等操作
4. **边界测试**：余额不足、重复上机、并发操作等

---

## 许可证

本项目仅供**学习交流**使用。

UI 组件库 [ElaWidgetTools](https://github.com/AstroAir/ElaWidgetTools) 遵循其原有许可证。
