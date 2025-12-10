# 更新日志

本文档记录项目的版本更新历史。

## [1.1.0] - 2025-12-10

### 新增

- **完整的打包支持**
  - 新增 `scripts/package_mingw.ps1` 自动化打包脚本
  - 支持 MSYS2 MinGW64 环境下的完整打包
  - 自动收集 Qt6 库、插件和 MinGW 运行时依赖
  - 递归检查并补充间接 DLL 依赖
  - 创建 ZIP 压缩包（约 35MB）

- **GitHub Actions 打包支持**
  - 新增 `build-windows-mingw` 工作流
  - 使用 `msys2/setup-msys2` action 配置环境
  - 自动收集所有依赖并上传 artifact

### 文档更新

- 新增 [打包指南](getting-started/packaging.md) 详细文档
- 更新 README.md 添加打包发布章节
- 更新开发环境文档添加打包说明

### 改进

- 优化 GitHub Actions 工作流
  - MSVC 构建现在也会收集 Qt 依赖
  - 统一 artifact 命名规范

---

## [1.0.0] - 2024-12-03

### 新增

- 完整的校园卡管理功能
  - 开卡、充值、挂失、解冻
  - 密码管理和重置
- 上机计费系统
  - 自动计时计费
  - 按分钟精确结算
- 双角色认证
  - 学生登录（卡号+密码）
  - 管理员登录（密码）
- 统计报表
  - 每日收入统计
  - 上机记录查询
- 数据管理
  - JSON 文件存储
  - 数据导入导出
  - 模拟数据生成
- 现代化 UI
  - Fluent Design 风格
  - ElaWidgetTools 组件库

### 技术特性

- C++20 标准
- Qt6 框架
- CMake Presets 构建配置
- 跨平台支持（Windows/Linux/macOS）

---

## 版本规范

本项目遵循 [语义化版本](https://semver.org/lang/zh-CN/) 规范。

版本号格式：`主版本号.次版本号.修订号`

- **主版本号**：不兼容的 API 变更
- **次版本号**：向下兼容的功能新增
- **修订号**：向下兼容的问题修复
