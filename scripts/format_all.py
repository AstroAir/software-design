#!/usr/bin/env python3
"""
格式化所有源代码文件
使用 clang-format 格式化 C++ 文件
使用 cmake-format 格式化 CMake 文件
"""

import subprocess
import sys
from pathlib import Path
from typing import List

# 项目根目录
PROJECT_ROOT = Path(__file__).parent.parent

# 排除目录
EXCLUDE_DIRS = {'build', 'third_party', '.git', '__pycache__'}

# C++ 文件扩展名
CPP_EXTENSIONS = {'.h', '.hpp', '.cpp', '.cc', '.cxx', '.hxx'}

# CMake 文件
CMAKE_FILES = {'CMakeLists.txt'}
CMAKE_EXTENSIONS = {'.cmake'}


def find_cpp_files() -> List[Path]:
    """查找所有 C++ 文件"""
    files = []
    for path in PROJECT_ROOT.rglob('*'):
        if any(excluded in path.parts for excluded in EXCLUDE_DIRS):
            continue
        if path.suffix in CPP_EXTENSIONS:
            files.append(path)
    return files


def find_cmake_files() -> List[Path]:
    """查找所有 CMake 文件"""
    files = []
    for path in PROJECT_ROOT.rglob('*'):
        if any(excluded in path.parts for excluded in EXCLUDE_DIRS):
            continue
        if path.name in CMAKE_FILES or path.suffix in CMAKE_EXTENSIONS:
            files.append(path)
    return files


def format_cpp_files(files: List[Path], check_only: bool = False) -> bool:
    """格式化 C++ 文件"""
    if not files:
        print("没有找到 C++ 文件")
        return True

    print(f"格式化 {len(files)} 个 C++ 文件...")

    args = ['clang-format', '-style=file']
    if check_only:
        args.append('--dry-run')
        args.append('--Werror')
    else:
        args.append('-i')

    args.extend(str(f) for f in files)

    try:
        result = subprocess.run(args, capture_output=True, text=True)
        if result.returncode != 0:
            print("格式化失败：")
            print(result.stderr)
            return False
        print("C++ 文件格式化完成")
        return True
    except FileNotFoundError:
        print("错误：未找到 clang-format，请确保已安装")
        return False


def format_cmake_files(files: List[Path], check_only: bool = False) -> bool:
    """格式化 CMake 文件"""
    if not files:
        print("没有找到 CMake 文件")
        return True

    print(f"格式化 {len(files)} 个 CMake 文件...")

    success = True
    for f in files:
        args = ['cmake-format']
        if check_only:
            args.append('--check')
        else:
            args.append('-i')
        args.append(str(f))

        try:
            result = subprocess.run(args, capture_output=True, text=True)
            if result.returncode != 0:
                print(f"格式化失败：{f}")
                print(result.stderr)
                success = False
        except FileNotFoundError:
            print("警告：未找到 cmake-format，跳过 CMake 文件格式化")
            return True

    if success:
        print("CMake 文件格式化完成")
    return success


def main() -> int:
    """主函数"""
    check_only = '--check' in sys.argv

    if check_only:
        print("检查模式：只检查格式，不修改文件")
    else:
        print("格式化模式：将修改文件")

    print()

    cpp_files = find_cpp_files()
    cmake_files = find_cmake_files()

    cpp_ok = format_cpp_files(cpp_files, check_only)
    cmake_ok = format_cmake_files(cmake_files, check_only)

    print()
    if cpp_ok and cmake_ok:
        print("✓ 所有文件格式正确")
        return 0
    else:
        print("✗ 存在格式问题")
        return 1


if __name__ == '__main__':
    sys.exit(main())
