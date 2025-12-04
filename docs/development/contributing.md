# 贡献指南

感谢你对本项目的关注！本文档将指导你如何参与项目开发。

## 开始之前

1. 阅读 [开发环境](environment.md) 配置开发环境
2. 阅读 [代码规范](code-style.md) 了解编码规范
3. 阅读 [系统概述](../architecture/overview.md) 了解项目架构

## 贡献流程

### 1. Fork 仓库

点击 GitHub 页面右上角的 Fork 按钮，将仓库复制到你的账户。

### 2. 克隆仓库

```bash
git clone --recursive https://github.com/YOUR_USERNAME/software-design.git
cd software-design
```

### 3. 创建分支

```bash
git checkout -b feature/your-feature-name
```

分支命名规范：

- `feature/xxx` - 新功能
- `fix/xxx` - Bug 修复
- `docs/xxx` - 文档更新
- `refactor/xxx` - 代码重构

### 4. 开发

- 遵循 [代码规范](code-style.md)
- 编写必要的注释
- 确保代码可以编译通过

### 5. 提交

```bash
git add .
git commit -m "feat: 添加新功能描述"
```

提交信息规范（遵循 Conventional Commits）：

| 类型       | 说明                   |
| ---------- | ---------------------- |
| `feat`     | 新功能                 |
| `fix`      | Bug 修复               |
| `docs`     | 文档更新               |
| `style`    | 代码格式（不影响功能） |
| `refactor` | 代码重构               |
| `test`     | 测试相关               |
| `chore`    | 构建/工具相关          |

### 6. 推送

```bash
git push origin feature/your-feature-name
```

### 7. 创建 Pull Request

1. 访问你的 Fork 仓库
2. 点击 "Compare & pull request"
3. 填写 PR 描述
4. 提交 PR

## Pull Request 规范

### 标题

使用 Conventional Commits 格式：

```text
feat: 添加卡片批量导入功能
fix: 修复余额计算精度问题
docs: 更新 API 文档
```

### 描述模板

```markdown
## 变更说明

简要描述本次变更的内容。

## 变更类型

-   [ ] 新功能
-   [ ] Bug 修复
-   [ ] 文档更新
-   [ ] 代码重构
-   [ ] 其他

## 测试

描述如何测试这些变更。

## 相关 Issue

关联的 Issue 编号（如有）。
```

## Issue 规范

### Bug 报告

请包含以下信息：

1. **环境信息**

    - 操作系统
    - Qt 版本
    - 编译器版本

2. **复现步骤**

    - 详细的操作步骤

3. **预期行为**

    - 你期望发生什么

4. **实际行为**

    - 实际发生了什么

5. **截图/日志**
    - 如有相关截图或错误日志

### 功能请求

请包含以下信息：

1. **功能描述**

    - 你希望添加什么功能

2. **使用场景**

    - 这个功能解决什么问题

3. **可能的实现方式**
    - 如果你有想法的话

## 代码审查

所有 PR 都需要经过代码审查。审查重点：

1. **功能正确性** - 代码是否实现了预期功能
2. **代码质量** - 是否遵循代码规范
3. **性能** - 是否有明显的性能问题
4. **安全性** - 是否有安全隐患
5. **可维护性** - 代码是否易于理解和维护

## 开发建议

### 添加新功能

1. 先在 Issue 中讨论
2. 设计接口和数据结构
3. 实现核心逻辑
4. 添加 UI（如需要）
5. 编写文档

### 修复 Bug

1. 先复现问题
2. 定位问题根源
3. 编写修复代码
4. 验证修复效果
5. 确保不引入新问题

### 重构代码

1. 确保有足够的测试覆盖
2. 小步重构，频繁提交
3. 保持功能不变
4. 更新相关文档

## 联系方式

如有问题，可以通过以下方式联系：

- 在 GitHub Issue 中提问
- 发送邮件至项目维护者

感谢你的贡献！
