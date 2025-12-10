# 系统概述

本文档介绍校园卡管理系统的整体架构设计和核心设计理念。

## 架构概览

系统采用标准的 **MVC（Model-View-Controller）架构**设计，实现了关注点分离和模块化开发。

```mermaid
graph TB
    subgraph View["View 层 (src/view/)"]
        MW[MainWindow]
        LD[LoginDialog]
        AP[AdminPanel]
        SP[StudentPanel]
        RD[RegisterDialog]
        ReD[RechargeDialog]
    end

    subgraph Controller["Controller 层 (src/controller/)"]
        MC[MainController]
        AC[AuthController]
        CC[CardController]
        RC[RecordController]
    end

    subgraph Model["Model 层 (src/model/)"]
        subgraph Services["业务服务"]
            AS[AuthService]
            CS[CardService]
            RS[RecordService]
        end
        subgraph Entities["实体类"]
            Card[Card]
            Record[Record]
            User[User]
        end
        subgraph Repositories["数据仓储"]
            SM[StorageManager]
            JSON[(JSON Files)]
        end
    end

    View --> Controller
    Controller --> Services
    Services --> Entities
    Services --> Repositories
    SM --> JSON
```

## 层次职责

### View 层（视图层）

负责用户界面展示和用户交互，不包含业务逻辑。

| 组件                | 职责                 |
| ------------------- | -------------------- |
| `MainWindow`        | 主窗口框架，页面切换 |
| `LoginDialog`       | 登录认证界面         |
| `RegisterDialog`    | 新卡注册界面         |
| `AdminPanel`        | 管理员控制面板       |
| `StudentPanel`      | 学生操作面板         |
| `RechargeDialog`    | 充值对话框           |
| `StatisticsWidget`  | 统计报表组件         |
| `RecordTableWidget` | 记录表格组件         |

### Controller 层（控制器层）

协调 View 和 Model 的交互，处理用户请求。

| 组件               | 职责                           |
| ------------------ | ------------------------------ |
| `MainController`   | 主控制器，管理所有 Service 和 Controller |
| `AuthController`   | 处理登录/登出请求             |
| `CardController`   | 处理卡管理请求               |
| `RecordController` | 处理上下机和记录查询请求     |

### Model 层（模型层）

包含数据和业务逻辑，分为三个子层：

#### 实体类 (entities/)

| 组件     | 职责           |
| -------- | -------------- |
| `Card`   | 校园卡实体     |
| `Record` | 上机记录实体   |
| `User`   | 用户实体       |

#### 业务服务 (services/)

| 组件            | 职责                   |
| --------------- | ---------------------- |
| `AuthService`   | 用户认证、会话管理     |
| `CardService`   | 卡片 CRUD、状态管理    |
| `RecordService` | 上机会话、记录统计     |

#### 数据仓储 (repositories/)

| 组件             | 职责                        |
| ---------------- | --------------------------- |
| `StorageManager` | JSON 文件读写、数据导入导出 |

## 设计模式

### 单例模式（Singleton）

`StorageManager` 采用单例模式，确保全局唯一的数据访问点。

```cpp
class StorageManager {
public:
    static StorageManager& instance() {
        static StorageManager instance;
        return instance;
    }

private:
    StorageManager() = default;
    StorageManager(const StorageManager&) = delete;
    StorageManager& operator=(const StorageManager&) = delete;
};
```

### 观察者模式（Observer）

通过 Qt 信号槽机制实现模块间的松耦合通信。

```mermaid
sequenceDiagram
    participant User
    participant AdminPanel
    participant CardController
    participant CardService
    participant StorageManager

    User->>AdminPanel: 点击充值按钮
    AdminPanel->>CardController: handleRecharge()
    CardController->>CardService: recharge()
    CardService->>CardService: 更新余额
    CardService->>StorageManager: saveAll()
    CardService-->>CardController: balanceChanged()
    CardController-->>AdminPanel: rechargeSuccess()
    AdminPanel->>AdminPanel: 刷新列表
```

### 依赖注入（Dependency Injection）

Controller 类通过构造函数注入 Service 依赖，便于测试和扩展。

```cpp
class AuthController : public QObject {
public:
    explicit AuthController(AuthService* authService,
                            CardService* cardService,
                            QObject* parent = nullptr);
};

class CardController : public QObject {
public:
    explicit CardController(CardService* cardService,
                            QObject* parent = nullptr);
};
```

### MVC 模式

严格遵循 MVC 分层：

- **Model**：不依赖 View 和 Controller，通过信号通知状态变化
- **View**：只依赖 Controller，不直接操作 Model
- **Controller**：协调 View 和 Model，处理用户输入

## 数据流

### 登录流程

```mermaid
flowchart LR
    A[LoginDialog] --> B[AuthController]
    B --> C[AuthService]
    C --> D[CardService]
    D --> E{验证结果}
    E -->|成功| F[loginSuccess 信号]
    E -->|失败| G[loginFailed 信号]
    F --> H[MainWindow 切换面板]
    G --> I[LoginDialog 显示错误]
```

### 上机计费流程

```mermaid
flowchart TB
    A[StudentPanel] --> B[RecordController]
    B --> C[RecordService]
    C --> D[创建 Record]
    D --> E[记录开始时间]
    E --> F[sessionStarted 信号]
    F --> G[用户使用中...]
    G --> H[RecordController]
    H --> I[RecordService]
    I --> J[计算时长和费用]
    J --> K[CardService 扣款]
    K --> L[StorageManager 保存]
    L --> M[sessionEnded 信号]
```

## 文件结构

```text
src/
├── model/                      # Model 层
│   ├── Types.h                 # 全局类型定义
│   ├── entities/               # 实体类
│   │   ├── Card.h/cpp          # 校园卡实体
│   │   ├── Record.h/cpp        # 上机记录实体
│   │   └── User.h/cpp          # 用户实体
│   ├── services/               # 业务服务
│   │   ├── AuthService.h/cpp   # 认证服务
│   │   ├── CardService.h/cpp   # 卡业务服务
│   │   └── RecordService.h/cpp # 记录业务服务
│   └── repositories/           # 数据仓储
│       └── StorageManager.h/cpp # 存储管理器
│
├── view/                       # View 层
│   ├── MainWindow.h/cpp        # 主窗口
│   ├── dialogs/                # 对话框
│   │   ├── LoginDialog.h/cpp   # 登录对话框
│   │   ├── RegisterDialog.h/cpp # 注册对话框
│   │   └── RechargeDialog.h/cpp # 充值对话框
│   ├── panels/                 # 面板
│   │   ├── AdminPanel.h/cpp    # 管理员面板
│   │   └── StudentPanel.h/cpp  # 学生面板
│   └── widgets/                # 可复用组件
│       ├── RecordTableWidget.h/cpp # 记录表格
│       └── StatisticsWidget.h/cpp  # 统计组件
│
├── controller/                 # Controller 层
│   ├── MainController.h/cpp    # 主控制器
│   ├── AuthController.h/cpp    # 认证控制器
│   ├── CardController.h/cpp    # 卡控制器
│   └── RecordController.h/cpp  # 记录控制器
│
└── main.cpp                    # 程序入口
```

## 技术选型

| 技术        | 选择           | 理由                    |
| ----------- | -------------- | ----------------------- |
| **语言**    | C++20          | 现代特性、类型安全      |
| **GUI**     | Qt6            | 跨平台、成熟稳定        |
| **UI 组件** | ElaWidgetTools | Fluent Design、美观现代 |
| **存储**    | JSON           | 轻量级、易于调试        |
| **构建**    | CMake          | 跨平台、标准化          |

### C++20 特性应用

本项目充分利用了 C++20 的现代特性：

| 特性 | 应用场景 | 示例 |
|------|----------|------|
| **`[[nodiscard]]`** | 返回值不应被忽略的函数 | `[[nodiscard]] bool isUsable() const;` |
| **`std::optional`** | 可能为空的返回值 | `std::optional<User> currentUser() const;` |
| **`enum class`** | 强类型枚举 | `enum class CardState { Normal, Lost, Frozen };` |
| **`constexpr`** | 编译期常量 | `constexpr double COST_PER_HOUR = 1.0;` |
| **范围 for 循环** | 集合遍历 | `for (const auto& card : cards)` |
| **结构化绑定** | 解构赋值 | `auto [success, message] = validate();` |
| **`auto` 类型推导** | 简化类型声明 | `auto it = map.find(key);` |

### Qt6 框架集成

项目使用 Qt6 的核心模块：

```mermaid
graph LR
    subgraph Qt6Modules["Qt6 模块"]
        Core[Qt6::Core]
        Gui[Qt6::Gui]
        Widgets[Qt6::Widgets]
    end

    subgraph Features["使用的功能"]
        Signals[信号槽机制]
        JSON[JSON 序列化]
        DateTime[日期时间处理]
        Layout[布局管理]
        Model[Model/View 框架]
    end

    Core --> Signals
    Core --> JSON
    Core --> DateTime
    Widgets --> Layout
    Widgets --> Model
```

**关键 Qt6 特性**：

- **信号槽（Signals & Slots）** - 松耦合的事件通信机制
- **QJsonDocument/QJsonObject** - 原生 JSON 序列化支持
- **QDateTime** - 时间处理和格式化
- **QStandardItemModel** - 表格数据模型
- **QStackedWidget** - 页面切换容器
- **Qt Resource System** - 资源文件管理

### ElaWidgetTools UI 组件库

[ElaWidgetTools](https://github.com/AstroAir/ElaWidgetTools) 是一个 Fluent Design 风格的 Qt 组件库。

**使用的组件**：

| 组件 | 用途 | 替代的 Qt 组件 |
|------|------|----------------|
| `ElaWindow` | 主窗口框架 | `QMainWindow` |
| `ElaDialog` | 对话框 | `QDialog` |
| `ElaContentDialog` | 内容对话框 | `QDialog` |
| `ElaPushButton` | 按钮 | `QPushButton` |
| `ElaLineEdit` | 输入框 | `QLineEdit` |
| `ElaComboBox` | 下拉框 | `QComboBox` |
| `ElaTableView` | 表格视图 | `QTableView` |
| `ElaText` | 文本标签 | `QLabel` |
| `ElaScrollPage` | 滚动页面 | `QScrollArea` |
| `ElaMessageBar` | 消息提示 | `QMessageBox` |
| `ElaIconButton` | 图标按钮 | `QToolButton` |

**Fluent Design 特性**：

- 圆角边框和阴影效果
- 动画过渡效果
- 亚克力/云母材质背景
- 现代化配色方案
- 响应式悬停效果

### JSON 数据存储

使用 Qt 内置的 JSON 模块实现轻量级数据持久化：

```cpp
// 序列化示例
QJsonObject Card::toJson() const {
    return QJsonObject{
        {"cardId", m_cardId},
        {"name", m_name},
        {"studentId", m_studentId},
        {"balance", m_balance},
        {"state", static_cast<int>(m_state)},
        {"password", m_password}
    };
}

// 反序列化示例
Card Card::fromJson(const QJsonObject& json) {
    Card card;
    card.m_cardId = json["cardId"].toString();
    card.m_name = json["name"].toString();
    card.m_balance = json["balance"].toDouble();
    card.m_state = static_cast<CardState>(json["state"].toInt());
    return card;
}
```

### CMake 构建系统

项目使用 CMake Presets 实现标准化的跨平台构建：

```mermaid
graph TB
    subgraph Presets["CMake Presets"]
        MSVC[msvc-release]
        MinGW[mingw-release]
        Ninja[ninja-mingw-release]
    end

    subgraph Targets["构建目标"]
        EXE[CampusCardSystem.exe]
        DLL[ElaWidgetTools.dll]
    end

    subgraph Dependencies["依赖"]
        Qt6[Qt6::Widgets/Core/Gui]
        Ela[ElaWidgetTools]
    end

    MSVC --> EXE
    MinGW --> EXE
    Ninja --> EXE
    EXE --> Qt6
    EXE --> Ela
    Ela --> DLL
```

**CMake 配置特性**：

- `CMAKE_CXX_STANDARD 20` - C++20 标准
- `CMAKE_AUTOMOC ON` - 自动处理 Qt MOC
- `CMAKE_AUTORCC ON` - 自动处理资源文件
- `CMAKE_EXPORT_COMPILE_COMMANDS ON` - 生成 clangd 配置

## 扩展性设计

系统预留了以下扩展点：

1. **数据库支持** - `StorageManager` 可替换为 SQLite/MySQL 实现
2. **网络功能** - 可添加 REST API 层支持远程访问
3. **多语言** - 使用 Qt Linguist 实现国际化
4. **主题切换** - ElaWidgetTools 支持明暗主题

## 下一步

- [核心模块](core-modules.md) - 详细了解核心类
- [UI 组件](ui-components.md) - 了解界面组件
