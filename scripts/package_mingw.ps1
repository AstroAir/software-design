# CampusCardSystem MinGW64 打包脚本
# 用于在 MSYS2 MinGW64 环境下收集所有依赖并创建完整的安装包

param(
    [string]$BuildDir = "build",
    [string]$OutputDir = "package",
    [string]$MSYS2Root = "D:\msys64",
    [switch]$Clean,
    [switch]$CreateInstaller
)

$ErrorActionPreference = "Stop"

# 项目根目录
$ProjectRoot = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
$BuildPath = Join-Path $ProjectRoot $BuildDir
$OutputPath = Join-Path $ProjectRoot $OutputDir
$MinGWBin = Join-Path $MSYS2Root "mingw64\bin"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "CampusCardSystem MinGW64 打包脚本" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "项目根目录: $ProjectRoot"
Write-Host "构建目录: $BuildPath"
Write-Host "输出目录: $OutputPath"
Write-Host "MinGW64 Bin: $MinGWBin"

# 检查构建目录是否存在
if (-not (Test-Path $BuildPath)) {
    Write-Error "构建目录不存在: $BuildPath"
    Write-Host "请先运行构建命令：cmake --build build --config Release" -ForegroundColor Yellow
    exit 1
}

# 检查可执行文件是否存在
$ExePath = Join-Path $BuildPath "CampusCardSystem.exe"
if (-not (Test-Path $ExePath)) {
    Write-Error "可执行文件不存在: $ExePath"
    Write-Host "请先构建项目" -ForegroundColor Yellow
    exit 1
}

# 检查 MSYS2 MinGW64 是否存在
if (-not (Test-Path $MinGWBin)) {
    Write-Error "MSYS2 MinGW64 目录不存在: $MinGWBin"
    Write-Host "请确保已安装 MSYS2 并设置正确的路径" -ForegroundColor Yellow
    exit 1
}

# 清理输出目录
if ($Clean -and (Test-Path $OutputPath)) {
    Write-Host "清理输出目录..." -ForegroundColor Yellow
    Remove-Item -Recurse -Force $OutputPath
}

# 创建输出目录
if (-not (Test-Path $OutputPath)) {
    New-Item -ItemType Directory -Path $OutputPath | Out-Null
}

Write-Host ""
Write-Host "步骤 1: 复制主程序和 ElaWidgetTools DLL" -ForegroundColor Green

# 复制主程序
Copy-Item $ExePath $OutputPath -Force
Write-Host "  已复制: CampusCardSystem.exe"

# 复制 ElaWidgetTools DLL
$ElaDll = Join-Path $BuildPath "ElaWidgetTools.dll"
if (Test-Path $ElaDll) {
    Copy-Item $ElaDll $OutputPath -Force
    Write-Host "  已复制: ElaWidgetTools.dll"
} else {
    Write-Warning "ElaWidgetTools.dll 未找到"
}

Write-Host ""
Write-Host "步骤 2: 使用 windeployqt6 收集 Qt 依赖" -ForegroundColor Green

$WinDeployQt = Join-Path $MinGWBin "windeployqt6.exe"
if (-not (Test-Path $WinDeployQt)) {
    Write-Error "windeployqt6.exe 未找到: $WinDeployQt"
    exit 1
}

# 设置环境变量确保 windeployqt 能找到正确的 Qt
$env:PATH = "$MinGWBin;$env:PATH"

# 运行 windeployqt6
$TargetExe = Join-Path $OutputPath "CampusCardSystem.exe"
& $WinDeployQt --release --no-translations --no-system-d3d-compiler --no-opengl-sw $TargetExe

if ($LASTEXITCODE -ne 0) {
    Write-Error "windeployqt6 执行失败"
    exit 1
}

Write-Host "  Qt 依赖收集完成"

Write-Host ""
Write-Host "步骤 3: 复制 MinGW 运行时 DLL" -ForegroundColor Green

# MinGW 运行时 DLL 列表
$MinGWDlls = @(
    "libgcc_s_seh-1.dll",
    "libstdc++-6.dll",
    "libwinpthread-1.dll",
    # ICU 库 (Qt 可能需要)
    "libicudt76.dll",
    "libicuin76.dll",
    "libicuuc76.dll",
    # zlib
    "zlib1.dll",
    # libpng (Qt GUI 可能需要)
    "libpng16-16.dll",
    # libjpeg
    "libjpeg-8.dll",
    # freetype (Qt 字体渲染)
    "libfreetype-6.dll",
    # harfbuzz (Qt 文本塑形)
    "libharfbuzz-0.dll",
    # bzip2
    "libbz2-1.dll",
    # PCRE2 (Qt 正则表达式)
    "libpcre2-16-0.dll",
    # double-conversion
    "libdouble-conversion.dll",
    # zstd
    "libzstd.dll",
    # brotli
    "libbrotlidec.dll",
    "libbrotlicommon.dll",
    # glib (某些 Qt 插件可能需要)
    "libglib-2.0-0.dll",
    "libintl-8.dll",
    "libiconv-2.dll",
    # graphite2 (harfbuzz 依赖)
    "libgraphite2.dll",
    # MD4C (Qt 可能需要)
    "libmd4c.dll"
)

foreach ($dll in $MinGWDlls) {
    $srcPath = Join-Path $MinGWBin $dll
    if (Test-Path $srcPath) {
        Copy-Item $srcPath $OutputPath -Force
        Write-Host "  已复制: $dll"
    } else {
        Write-Host "  未找到 (可选): $dll" -ForegroundColor DarkGray
    }
}

Write-Host ""
Write-Host "步骤 4: 复制数据目录" -ForegroundColor Green

$DataSrc = Join-Path $ProjectRoot "data"
$DataDst = Join-Path $OutputPath "data"
if (Test-Path $DataSrc) {
    if (-not (Test-Path $DataDst)) {
        New-Item -ItemType Directory -Path $DataDst | Out-Null
    }
    Copy-Item -Path "$DataSrc\*" -Destination $DataDst -Recurse -Force
    Write-Host "  数据目录已复制"
} else {
    Write-Host "  数据目录不存在，跳过" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "步骤 5: 验证依赖完整性" -ForegroundColor Green

# 使用 objdump 检查依赖
$Objdump = Join-Path $MinGWBin "objdump.exe"
if (Test-Path $Objdump) {
    Write-Host "  检查 CampusCardSystem.exe 的 DLL 依赖..."
    
    $output = & $Objdump -p $TargetExe 2>&1
    $dllNames = $output | Select-String "DLL Name:" | ForEach-Object {
        $_ -replace ".*DLL Name:\s*", "" -replace "\s+.*", ""
    }
    
    $missingDlls = @()
    foreach ($dll in $dllNames) {
        $dll = $dll.Trim()
        if ($dll -eq "") { continue }
        
        # 跳过 Windows 系统 DLL
        $systemDlls = @(
            "KERNEL32.dll", "USER32.dll", "GDI32.dll", "ADVAPI32.dll",
            "SHELL32.dll", "ole32.dll", "OLEAUT32.dll", "msvcrt.dll",
            "COMDLG32.dll", "WS2_32.dll", "WINMM.dll", "IMM32.dll",
            "VERSION.dll", "SHLWAPI.dll", "USERENV.dll", "dwmapi.dll",
            "UxTheme.dll", "CRYPT32.dll", "bcrypt.dll", "netapi32.dll",
            "iphlpapi.dll", "dnsapi.dll", "Secur32.dll", "WTSAPI32.dll",
            "d3d11.dll", "dxgi.dll", "SETUPAPI.dll", "Cfgmgr32.dll",
            "ntdll.dll", "RPCRT4.dll", "MPR.dll", "api-ms-*"
        )
        
        $isSystem = $false
        foreach ($sysDll in $systemDlls) {
            if ($dll -like $sysDll) {
                $isSystem = $true
                break
            }
        }
        
        if (-not $isSystem) {
            $dllPath = Join-Path $OutputPath $dll
            if (-not (Test-Path $dllPath)) {
                $missingDlls += $dll
            }
        }
    }
    
    if ($missingDlls.Count -gt 0) {
        Write-Host "  警告: 以下 DLL 可能缺失:" -ForegroundColor Yellow
        foreach ($dll in $missingDlls) {
            Write-Host "    - $dll" -ForegroundColor Yellow
            # 尝试从 MinGW 复制
            $srcPath = Join-Path $MinGWBin $dll
            if (Test-Path $srcPath) {
                Copy-Item $srcPath $OutputPath -Force
                Write-Host "      已自动复制: $dll" -ForegroundColor Green
            }
        }
    } else {
        Write-Host "  所有直接依赖已满足" -ForegroundColor Green
    }
}

Write-Host ""
Write-Host "步骤 6: 递归检查所有 DLL 依赖" -ForegroundColor Green

function Get-DllDependencies {
    param([string]$DllPath, [string]$Objdump)
    
    $output = & $Objdump -p $DllPath 2>&1
    $dllNames = @()
    $output | Select-String "DLL Name:" | ForEach-Object {
        $name = $_ -replace ".*DLL Name:\s*", "" -replace "\s+.*", ""
        $name = $name.Trim()
        if ($name -ne "") {
            $dllNames += $name
        }
    }
    return $dllNames
}

$systemDllPatterns = @(
    "KERNEL32*", "USER32*", "GDI32*", "ADVAPI32*", "SHELL32*",
    "ole32*", "OLEAUT32*", "msvcrt*", "COMDLG32*", "WS2_32*",
    "WINMM*", "IMM32*", "VERSION*", "SHLWAPI*", "USERENV*",
    "dwmapi*", "UxTheme*", "CRYPT32*", "bcrypt*", "netapi32*",
    "iphlpapi*", "dnsapi*", "Secur32*", "WTSAPI32*", "d3d11*",
    "dxgi*", "SETUPAPI*", "Cfgmgr32*", "ntdll*", "RPCRT4*",
    "MPR*", "api-ms-*", "ext-ms-*", "MSVCP*", "VCRUNTIME*",
    "ucrtbase*", "COMCTL32*", "WINSPOOL*", "PSAPI*", "CRYPTBASE*",
    "SSPICLI*", "KERNELBASE*", "PROFAPI*", "powrprof*", "shcore*",
    "NSI*", "MSWSOCK*", "wintrust*", "imagehlp*", "dbghelp*",
    "d3d9*", "OPENGL32*", "GLU32*"
)

function Is-SystemDll {
    param([string]$DllName)
    foreach ($pattern in $systemDllPatterns) {
        if ($DllName -like $pattern) {
            return $true
        }
    }
    return $false
}

$checkedDlls = @{}
$allDlls = Get-ChildItem -Path $OutputPath -Filter "*.dll" | ForEach-Object { $_.FullName }
$allDlls += $TargetExe

$queue = New-Object System.Collections.Queue
foreach ($dll in $allDlls) {
    $queue.Enqueue($dll)
}

$additionalDlls = @()

while ($queue.Count -gt 0) {
    $currentDll = $queue.Dequeue()
    $dllName = Split-Path -Leaf $currentDll
    
    if ($checkedDlls.ContainsKey($dllName.ToLower())) {
        continue
    }
    $checkedDlls[$dllName.ToLower()] = $true
    
    if (-not (Test-Path $currentDll)) {
        continue
    }
    
    $deps = Get-DllDependencies -DllPath $currentDll -Objdump $Objdump
    foreach ($dep in $deps) {
        if (Is-SystemDll -DllName $dep) {
            continue
        }
        
        $depPath = Join-Path $OutputPath $dep
        if (-not (Test-Path $depPath)) {
            $srcPath = Join-Path $MinGWBin $dep
            if (Test-Path $srcPath) {
                Copy-Item $srcPath $OutputPath -Force
                $additionalDlls += $dep
                $queue.Enqueue((Join-Path $OutputPath $dep))
            }
        } else {
            if (-not $checkedDlls.ContainsKey($dep.ToLower())) {
                $queue.Enqueue($depPath)
            }
        }
    }
}

if ($additionalDlls.Count -gt 0) {
    Write-Host "  已补充复制以下 DLL:" -ForegroundColor Green
    foreach ($dll in $additionalDlls) {
        Write-Host "    - $dll"
    }
} else {
    Write-Host "  无需补充额外 DLL" -ForegroundColor Green
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "打包完成！" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan

# 统计文件
$exeFiles = Get-ChildItem -Path $OutputPath -Filter "*.exe"
$dllFiles = Get-ChildItem -Path $OutputPath -Filter "*.dll"
$totalSize = (Get-ChildItem -Path $OutputPath -Recurse | Measure-Object -Property Length -Sum).Sum

Write-Host ""
Write-Host "统计信息:" -ForegroundColor Cyan
Write-Host "  可执行文件: $($exeFiles.Count) 个"
Write-Host "  DLL 文件: $($dllFiles.Count) 个"
Write-Host "  总大小: $([math]::Round($totalSize / 1MB, 2)) MB"
Write-Host "  输出目录: $OutputPath"

# 创建 ZIP 压缩包
Write-Host ""
Write-Host "步骤 7: 创建 ZIP 压缩包" -ForegroundColor Green

$ZipPath = Join-Path $ProjectRoot "CampusCardSystem-Windows-MinGW64.zip"
if (Test-Path $ZipPath) {
    Remove-Item $ZipPath -Force
}

Compress-Archive -Path "$OutputPath\*" -DestinationPath $ZipPath -CompressionLevel Optimal
Write-Host "  已创建: $ZipPath"
Write-Host "  ZIP 大小: $([math]::Round((Get-Item $ZipPath).Length / 1MB, 2)) MB"

Write-Host ""
Write-Host "打包完成！可以测试运行: $TargetExe" -ForegroundColor Green
