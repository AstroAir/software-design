# 贡献指南

感谢你对校园卡管理系统的关注！我们欢迎各种形式的贡献。

## 如何贡献

### 报告 Bug

1. 在 [Issues](https://github.com/your-repo/software-design/issues) 中搜索是否已有相同问题
2. 如果没有，创建新 Issue，使用 Bug 报告模板
3. 提供详细的复现步骤和环境信息

### 提交功能请求

1. 在 Issues 中搜索是否已有相同请求
2. 创建新 Issue，使用功能请求模板
3. 描述功能的使用场景和预期效果

### 提交代码

1. Fork 本仓库
2. 创建功能分支：`git checkout -b feature/your-feature`
3. 提交更改：`git commit -m 'feat: 添加新功能'`
4. 推送分支：`git push origin feature/your-feature`
5. 创建 Pull Request

## 开发指南

详细的开发指南请参阅 [开发文档](docs/development/environment.md)。

### 快速开始

```bash
# 克隆仓库
git clone --recursive https://github.com/your-repo/software-design.git
cd software-design

# 构建项目
cmake --preset msvc-release  # 或其他预设
cmake --build --preset msvc-release
```

### 代码规范

- 遵循 [代码规范](docs/development/code-style.md)
- 使用 Doxygen 风格注释
- 提交信息遵循 Conventional Commits

## 提交信息规范

```text
<type>: <description>

[optional body]

[optional footer]
```

类型：

- `feat`: 新功能
- `fix`: Bug 修复
- `docs`: 文档更新
- `style`: 代码格式
- `refactor`: 代码重构
- `test`: 测试相关
- `chore`: 构建/工具

## 行为准则

请阅读并遵守我们的 [行为准则](CODE_OF_CONDUCT.md)。

## 许可

通过提交代码，你同意你的贡献将按照项目许可证进行授权。
