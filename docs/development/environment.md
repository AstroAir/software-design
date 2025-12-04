# 开发环境

本文档介绍如何配置开发环境。

## 推荐配置

### 硬件要求

| 项目 | 最低配置     | 推荐配置   |
| ---- | ------------ | ---------- |
| CPU  | 双核 2GHz    | 四核 3GHz+ |
| 内存 | 4GB          | 8GB+       |
| 硬盘 | 2GB 可用空间 | SSD 5GB+   |

### 软件要求

| 软件       | 版本  | 说明           |
| ---------- | ----- | -------------- |
| CMake      | 3.24+ | 构建系统       |
| Qt6        | 6.5+  | GUI 框架       |
| C++ 编译器 | C++20 | MSVC/GCC/Clang |
| Git        | 2.0+  | 版本控制       |

## Windows 开发环境

### 方式一：Visual Studio + Qt

1. **安装 Visual Studio 2022**

    - 选择「使用 C++ 的桌面开发」工作负载
    - 确保包含 CMake 工具

2. **安装 Qt6**

    - 下载 [Qt Online Installer](https://www.qt.io/download)
    - 选择 Qt 6.5+ 版本
    - 选择 MSVC 2019 64-bit 组件

3. **配置环境变量**

    ```powershell
    $env:QT_MSVC_PATH = "C:/Qt/6.7.0/msvc2019_64"
    ```

4. **构建项目**

    ```bash
    cmake --preset msvc-release
    cmake --build --preset msvc-release
    ```

### 方式二：MSYS2 + MinGW

1. **安装 MSYS2**

    - 下载 [MSYS2](https://www.msys2.org/)
    - 安装到 `C:\msys64`

2. **安装依赖**

    ```bash
    pacman -S mingw-w64-x86_64-cmake
    pacman -S mingw-w64-x86_64-ninja
    pacman -S mingw-w64-x86_64-gcc
    pacman -S mingw-w64-x86_64-qt6-base
    ```

3. **配置环境变量**

    ```powershell
    $env:MSYS2_ROOT = "C:/msys64"
    $env:QT_MINGW_PATH = "C:/msys64/mingw64"
    ```

4. **构建项目**

    ```bash
    cmake --preset ninja-mingw-release
    cmake --build --preset ninja-mingw-release
    ```

## Linux 开发环境

### Ubuntu/Debian

1. **安装依赖**

    ```bash
    sudo apt update
    sudo apt install build-essential cmake ninja-build
    sudo apt install qt6-base-dev qt6-tools-dev
    ```

2. **构建项目**

    ```bash
    mkdir build && cd build
    cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
    ninja
    ```

### Fedora

1. **安装依赖**

    ```bash
    sudo dnf install cmake ninja-build gcc-c++
    sudo dnf install qt6-qtbase-devel qt6-qttools-devel
    ```

2. **构建项目**

    ```bash
    mkdir build && cd build
    cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
    ninja
    ```

## macOS 开发环境

1. **安装 Xcode Command Line Tools**

    ```bash
    xcode-select --install
    ```

2. **安装 Homebrew**

    ```bash
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    ```

3. **安装依赖**

    ```bash
    brew install cmake ninja qt@6
    ```

4. **构建项目**

    ```bash
    mkdir build && cd build
    cmake .. -G Ninja -DCMAKE_PREFIX_PATH=$(brew --prefix qt@6)
    ninja
    ```

## IDE 配置

### Visual Studio Code

推荐扩展：

- **C/C++** - Microsoft 官方 C++ 扩展
- **CMake Tools** - CMake 集成
- **Qt tools** - Qt 支持

`.vscode/settings.json` 示例：

```json
{
    "cmake.configureSettings": {
        "CMAKE_PREFIX_PATH": "C:/Qt/6.7.0/msvc2019_64"
    },
    "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools"
}
```

### CLion

1. 打开项目目录
2. CLion 自动检测 CMakeLists.txt
3. 配置 CMake 选项：
    - `CMAKE_PREFIX_PATH` = Qt 安装路径

### Qt Creator

1. 打开 CMakeLists.txt
2. 选择 Qt Kit
3. 配置构建目录
4. 点击构建

## 调试配置

### Visual Studio

1. 设置启动项目为 `CampusCardSystem`
2. 按 F5 启动调试

### VS Code

`.vscode/launch.json` 示例：

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug",
            "type": "cppvsdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/Release/CampusCardSystem.exe",
            "cwd": "${workspaceFolder}/build/Release"
        }
    ]
}
```

## 下一步

- [代码规范](code-style.md) - 了解编码规范
- [贡献指南](contributing.md) - 参与项目开发
