# 打包指南

本指南介绍如何将校园卡管理系统打包为可分发的安装包。

## 概述

打包过程会收集所有必要的依赖项（DLL、Qt 插件、数据文件），创建一个可以在没有开发环境的机器上运行的完整安装包。

## Windows 打包

### 方式一：使用打包脚本（推荐）

项目提供了自动化打包脚本 `scripts/package_mingw.ps1`，适用于 MSYS2 MinGW64 环境。

#### 前置条件

- 已完成项目构建（Release 模式）
- MSYS2 MinGW64 环境已安装

#### 运行打包脚本

```powershell
# 基本用法
powershell -ExecutionPolicy Bypass -File scripts\package_mingw.ps1

# 清理并重新打包
powershell -ExecutionPolicy Bypass -File scripts\package_mingw.ps1 -Clean

# 自定义 MSYS2 路径
powershell -ExecutionPolicy Bypass -File scripts\package_mingw.ps1 -MSYS2Root "D:\msys64"
```

#### 脚本参数

| 参数 | 默认值 | 说明 |
|------|--------|------|
| `-BuildDir` | `build` | 构建目录路径 |
| `-OutputDir` | `package` | 输出目录路径 |
| `-MSYS2Root` | `D:\msys64` | MSYS2 安装根目录 |
| `-Clean` | - | 清理输出目录后重新打包 |

#### 打包内容

脚本会自动收集以下内容：

| 类别 | 内容 |
|------|------|
| **主程序** | `CampusCardSystem.exe` |
| **ElaWidgetTools** | `ElaWidgetTools.dll` |
| **Qt6 库** | Core, Gui, Widgets, Network, Svg 等 |
| **Qt6 插件** | platforms, imageformats, styles, iconengines 等 |
| **MinGW 运行时** | libgcc, libstdc++, libwinpthread 等 |
| **ICU 库** | libicudt, libicuin, libicuuc |
| **其他依赖** | zlib, libpng, libjpeg, freetype, harfbuzz 等 |
| **数据目录** | `data/` 目录 |

#### 输出结果

- `package/` - 包含所有文件的目录
- `CampusCardSystem-Windows-MinGW64.zip` - 压缩包（约 35MB）

### 方式二：手动打包

#### 1. 复制主程序

```powershell
mkdir package
cp build/CampusCardSystem.exe package/
cp build/ElaWidgetTools.dll package/
```

#### 2. 使用 windeployqt 收集 Qt 依赖

=== "MinGW64"

    ```powershell
    D:\msys64\mingw64\bin\windeployqt6.exe --release --no-translations package\CampusCardSystem.exe
    ```

=== "MSVC"

    ```powershell
    C:\Qt\6.7.0\msvc2019_64\bin\windeployqt.exe --release --no-translations package\CampusCardSystem.exe
    ```

#### 3. 复制 MinGW 运行时 DLL

MinGW 构建需要额外复制以下 DLL（从 `mingw64/bin/` 目录）：

```powershell
$dlls = @(
    "libgcc_s_seh-1.dll",
    "libstdc++-6.dll",
    "libwinpthread-1.dll",
    "zlib1.dll",
    "libpng16-16.dll",
    "libjpeg-8.dll",
    "libfreetype-6.dll",
    "libharfbuzz-0.dll",
    "libbz2-1.dll",
    "libpcre2-16-0.dll",
    "libdouble-conversion.dll",
    "libzstd.dll",
    "libbrotlidec.dll",
    "libbrotlicommon.dll",
    "libglib-2.0-0.dll",
    "libintl-8.dll",
    "libiconv-2.dll",
    "libgraphite2.dll",
    "libmd4c.dll",
    "libb2-1.dll",
    "libpcre2-8-0.dll"
)

foreach ($dll in $dlls) {
    Copy-Item "D:\msys64\mingw64\bin\$dll" package\ -ErrorAction SilentlyContinue
}

# ICU 库（版本号可能不同）
Copy-Item "D:\msys64\mingw64\bin\libicu*.dll" package\
```

#### 4. 复制数据目录

```powershell
Copy-Item -Recurse data package\
```

### 验证打包完整性

在打包目录中运行程序，确保没有缺少 DLL 的错误：

```powershell
# 清除 PATH 中的 MinGW 路径，模拟干净环境
$env:PATH = "C:\Windows\System32;C:\Windows"
.\package\CampusCardSystem.exe
```

如果程序正常启动，说明打包完整。

### DLL 依赖检查

使用 `objdump` 检查 DLL 依赖：

```powershell
D:\msys64\mingw64\bin\objdump.exe -p package\CampusCardSystem.exe | Select-String "DLL Name"
```

## GitHub Actions 自动打包

项目的 GitHub Actions 工作流已配置自动打包功能。

### 工作流配置

`.github/workflows/build.yml` 包含两个 Windows 构建任务：

| 任务名称 | 工具链 | 输出 |
|----------|--------|------|
| `build-windows-msvc` | Visual Studio 2022 | `CampusCardSystem-Windows-MSVC` |
| `build-windows-mingw` | MSYS2 MinGW64 | `CampusCardSystem-Windows-MinGW64` |

### 下载构建产物

1. 进入 GitHub 仓库的 **Actions** 页面
2. 选择最新的构建任务
3. 在 **Artifacts** 区域下载对应的压缩包

## 打包文件结构

```text
package/
├── CampusCardSystem.exe          # 主程序
├── ElaWidgetTools.dll            # UI 组件库
├── Qt6Core.dll                   # Qt 核心库
├── Qt6Gui.dll                    # Qt GUI 库
├── Qt6Widgets.dll                # Qt Widgets 库
├── Qt6Network.dll                # Qt 网络库
├── Qt6Svg.dll                    # Qt SVG 库
├── libgcc_s_seh-1.dll            # GCC 运行时
├── libstdc++-6.dll               # C++ 标准库
├── libwinpthread-1.dll           # 线程库
├── libicu*.dll                   # ICU 国际化库
├── ... (其他依赖 DLL)
├── platforms/                    # Qt 平台插件
│   └── qwindows.dll
├── imageformats/                 # 图像格式插件
│   ├── qgif.dll
│   ├── qico.dll
│   ├── qjpeg.dll
│   └── ...
├── styles/                       # 样式插件
│   └── qmodernwindowsstyle.dll
├── iconengines/                  # 图标引擎插件
│   └── qsvgicon.dll
└── data/                         # 数据目录
    ├── cards.json
    ├── admin.json
    └── records/
```

## 常见问题

### 程序启动报错：找不到 DLL

1. 检查是否遗漏了某些 DLL
2. 使用打包脚本的递归依赖检查功能
3. 使用 Dependency Walker 或 Dependencies 工具分析依赖

### 程序启动后立即退出

1. 检查 Qt 平台插件是否正确复制到 `platforms/` 目录
2. 确保 `qwindows.dll` 存在

### 图标或图片无法显示

1. 检查 `imageformats/` 目录是否包含必要的插件
2. 确保 `iconengines/qsvgicon.dll` 存在

### ICU 库版本不匹配

ICU 库版本号会随 MSYS2 更新变化。脚本会自动复制所有 `libicu*.dll` 文件。

## 下一步

- [使用教程](usage.md) - 了解如何使用打包后的程序
- [系统概述](../architecture/overview.md) - 深入了解系统架构
