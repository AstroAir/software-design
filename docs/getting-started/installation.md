# 安装指南

本指南将帮助你在本地环境中构建和运行校园卡管理系统。

## 前置要求

### 必需软件

| 软件           | 最低版本   | 说明                           |
| -------------- | ---------- | ------------------------------ |
| **CMake**      | 3.24+      | 构建系统                       |
| **Qt6**        | 6.5+       | GUI 框架                       |
| **C++ 编译器** | C++20 支持 | MSVC 2019+、GCC 10+、Clang 10+ |
| **Git**        | 2.0+       | 版本控制（需支持子模块）       |

### 可选软件

- **Ninja** - 更快的构建工具（推荐）
- **Visual Studio 2022** - Windows 开发环境
- **MSYS2** - Windows 上的 MinGW 环境

## 获取源码

### 克隆仓库

```bash
git clone --recursive https://github.com/your-repo/software-design.git
cd software-design
```

!!! warning "重要"
必须使用 `--recursive` 参数以同时克隆 ElaWidgetTools 子模块。

### 更新子模块

如果已经克隆但缺少子模块：

```bash
git submodule update --init --recursive
```

## 构建项目

### 方式一：使用 CMake Presets（推荐）

项目提供了预配置的构建预设，简化构建流程。

#### 1. 配置环境变量

=== "MSVC"

    ```powershell
    $env:QT_MSVC_PATH = "C:/Qt/6.7.0/msvc2019_64"
    ```

=== "MinGW64"

    ```powershell
    $env:MSYS2_ROOT = "C:/msys64"
    $env:QT_MINGW_PATH = "C:/msys64/mingw64"
    ```

#### 2. 配置和构建

=== "MSVC Release"

    ```bash
    cmake --preset msvc-release
    cmake --build --preset msvc-release
    ```

=== "MinGW64 + Ninja"

    ```bash
    cmake --preset ninja-mingw-release
    cmake --build --preset ninja-mingw-release
    ```

=== "MinGW64 + Make"

    ```bash
    cmake --preset mingw-release
    cmake --build --preset mingw-release
    ```

#### 3. 自定义预设

复制示例文件并根据本地环境修改：

```bash
cp CMakeUserPresets.json.example CMakeUserPresets.json
# 编辑 CMakeUserPresets.json 修改路径
```

### 方式二：传统 CMake 构建

=== "Windows (Visual Studio)"

    ```bash
    mkdir build && cd build
    cmake .. -G "Visual Studio 17 2022" -DCMAKE_PREFIX_PATH="C:/Qt/6.7.0/msvc2019_64"
    cmake --build . --config Release
    ```

=== "Windows (MSYS2)"

    ```bash
    mkdir build && cd build
    cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Release
    ninja
    ```

=== "Linux"

    ```bash
    mkdir build && cd build
    cmake .. -DCMAKE_PREFIX_PATH="$HOME/Qt/6.7.0/gcc_64"
    make -j$(nproc)
    ```

=== "macOS"

    ```bash
    mkdir build && cd build
    cmake .. -DCMAKE_PREFIX_PATH="$HOME/Qt/6.7.0/macos"
    make -j$(sysctl -n hw.ncpu)
    ```

## 运行程序

构建完成后，可执行文件位于构建目录中：

```bash
# Windows
./build/Release/CampusCardSystem.exe

# Linux/macOS
./build/CampusCardSystem
```

!!! note "首次运行"
程序首次运行时会自动在可执行文件同级目录创建 `data/` 文件夹并生成示例数据。

## 常见问题

### Qt 找不到

确保 `CMAKE_PREFIX_PATH` 指向正确的 Qt 安装目录：

```bash
cmake .. -DCMAKE_PREFIX_PATH="/path/to/Qt/6.x.x/compiler"
```

### 子模块为空

重新初始化子模块：

```bash
git submodule update --init --recursive --force
```

### 编译错误：C++20 特性不支持

确保使用支持 C++20 的编译器：

- MSVC 2019 16.10+
- GCC 10+
- Clang 10+

## 下一步

- [使用教程](usage.md) - 了解如何使用系统
- [系统概述](../architecture/overview.md) - 深入了解系统架构
