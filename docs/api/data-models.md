# 数据模型

本文档描述系统使用的 JSON 数据格式。

## 文件结构

```text
data/
├── cards.json          # 所有校园卡数据
├── admin.json          # 管理员配置
└── records/
    ├── B17010101.txt   # 学号 B17010101 的上机记录
    ├── B17010102.txt   # 学号 B17010102 的上机记录
    └── ...             # 每个学生对应一个文件（以学号命名）
```

## cards.json

存储所有校园卡信息。

### 格式

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
    },
    {
        "cardId": "C002",
        "name": "李四",
        "studentId": "2024002",
        "totalRecharge": 50.0,
        "balance": 42.0,
        "state": 0,
        "loginAttempts": 0,
        "password": "123456"
    }
]
```

### 字段说明

| 字段            | 类型   | 必填 | 说明               |
| --------------- | ------ | ---- | ------------------ |
| `cardId`        | string | 是   | 卡号，唯一标识     |
| `name`          | string | 是   | 持卡人姓名         |
| `studentId`     | string | 是   | 学号               |
| `totalRecharge` | number | 是   | 累计充值金额       |
| `balance`       | number | 是   | 当前余额           |
| `state`         | number | 是   | 卡状态（见状态码） |
| `loginAttempts` | number | 是   | 密码错误次数       |
| `password`      | string | 是   | 登录密码           |

## admin.json

存储管理员配置。

### 格式

```json
{
    "password": "admin123"  // pragma: allowlist secret
}
```

### 字段说明

| 字段       | 类型   | 必填 | 说明           |
| ---------- | ------ | ---- | -------------- |
| `password` | string | 是   | 管理员登录密码 |

## records/{studentId}.txt

存储单个学生的上机记录，文件以学号命名（如 `B17010101.txt`）。

### 格式

```json
[
    {
        "recordId": "550e8400-e29b-41d4-a716-446655440000",
        "cardId": "C001",
        "date": "2024-12-03",
        "startTime": "2024-12-03T09:00:00",
        "endTime": "2024-12-03T11:30:00",
        "durationMinutes": 150,
        "cost": 2.5,
        "state": 0,
        "location": "机房A"
    },
    {
        "recordId": "550e8400-e29b-41d4-a716-446655440001",
        "cardId": "C001",
        "date": "2024-12-03",
        "startTime": "2024-12-03T14:00:00",
        "endTime": "",
        "durationMinutes": 0,
        "cost": 0,
        "state": 1,
        "location": "机房B"
    }
]
```

### 字段说明

| 字段              | 类型   | 必填 | 说明                   |
| ----------------- | ------ | ---- | ---------------------- |
| `recordId`        | string | 是   | 记录 ID（UUID）        |
| `cardId`          | string | 是   | 关联卡号               |
| `date`            | string | 是   | 上机日期（yyyy-MM-dd） |
| `startTime`       | string | 是   | 开始时间（ISO 8601）   |
| `endTime`         | string | 否   | 结束时间（上机中为空） |
| `durationMinutes` | number | 是   | 上机时长（分钟）       |
| `cost`            | number | 是   | 上机费用               |
| `state`           | number | 是   | 会话状态（见状态码）   |
| `location`        | string | 是   | 上机地点               |

## 状态码

### CardState - 卡状态

| 值  | 枚举     | 说明 |
| --- | -------- | ---- |
| 0   | `Normal` | 正常 |
| 1   | `Lost`   | 挂失 |
| 2   | `Frozen` | 冻结 |

### SessionState - 会话状态

| 值  | 枚举      | 说明        |
| --- | --------- | ----------- |
| 0   | `Offline` | 离线/已结束 |
| 1   | `Online`  | 上机中      |

## 导出数据格式

使用导出功能时，所有数据打包为单个 JSON 文件。

### 格式

```json
{
    "exportTime": "2024-12-03T15:30:00",
    "version": "1.0.0",
    "cards": [...],
    "admin": {...},
    "records": {
        "B17010101": [...],
        "B17010102": [...]
    }
}
```

### 字段说明

| 字段         | 类型   | 说明                   |
| ------------ | ------ | ---------------------- |
| `exportTime` | string | 导出时间               |
| `version`    | string | 数据格式版本           |
| `cards`      | array  | 所有卡数据             |
| `admin`      | object | 管理员配置             |
| `records`    | object | 所有记录（按学号分组） |

## 费用计算规则

### 计费公式

```text
费用 = ceil(上机分钟数) × (COST_PER_HOUR / 60)
     = ceil(分钟数) × (1.0 / 60)
     ≈ 分钟数 / 60 元（向上取整到分钟）
```

### 计算示例

| 上机时长 | 计算过程 | 费用 |
|----------|----------|------|
| 30 分钟 | 30 × (1.0/60) | ¥0.50 |
| 90 分钟 | 90 × (1.0/60) | ¥1.50 |
| 150 分钟 | 150 × (1.0/60) | ¥2.50 |

### 代码实现

```cpp
double Record::calculateCost(int durationMinutes) {
    return std::ceil(durationMinutes) * (CampusCard::COST_PER_HOUR / 60.0);
}

void Record::endSession() {
    m_endTime = QDateTime::currentDateTime();
    m_durationMinutes = m_startTime.secsTo(m_endTime) / 60;
    if (m_startTime.secsTo(m_endTime) % 60 > 0) {
        m_durationMinutes++;  // 不足一分钟按一分钟计算
    }
    m_cost = calculateCost(m_durationMinutes);
    m_state = SessionState::Offline;
}
```

## 数据完整性

### 自动初始化

程序首次运行时自动创建示例数据：

```cpp
void StorageManager::createSampleData() {
    QList<Card> sampleCards = {
        Card("C001", "张三", "2024001", 100.0),
        Card("C002", "李四", "2024002", 50.0),
        Card("C003", "王五", "2024003", 75.0)
    };
    saveAllCards(sampleCards);
    saveAdminPassword(CampusCard::DEFAULT_ADMIN_PASSWORD);
}
```

### 数据验证

| 验证项 | 规则 | 错误处理 |
|--------|------|----------|
| 卡号唯一性 | 创建时检查重复 | 返回失败 |
| 余额非负 | 扣款前检查余额 | 拒绝操作 |
| 密码非空 | 注册时验证 | 提示错误 |
| 时间有效性 | 结束时间 > 开始时间 | 自动修正 |

## 下一步

- [管理器类](managers.md) - 了解业务接口
- [信号与槽](signals-slots.md) - 了解事件通信
