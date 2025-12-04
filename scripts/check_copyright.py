#!/usr/bin/env python3
"""
检查 C++ 源文件是否包含版权头
用于 pre-commit 钩子
"""

import sys
import re
from pathlib import Path

# 版权头模式
COPYRIGHT_PATTERN = re.compile(
    r'/\*\*[\s\S]*?@file[\s\S]*?@brief[\s\S]*?\*/',
    re.MULTILINE
)

# 简化的版权头模式（至少包含文件注释）
SIMPLE_PATTERN = re.compile(
    r'^/\*\*',
    re.MULTILINE
)


def check_file(filepath: Path) -> bool:
    """检查单个文件是否包含版权头"""
    try:
        content = filepath.read_text(encoding='utf-8')
    except UnicodeDecodeError:
        content = filepath.read_text(encoding='gbk')
    
    # 检查是否有 Doxygen 风格的文件注释
    if SIMPLE_PATTERN.search(content[:500]):  # 只检查前 500 个字符
        return True
    
    return False


def main() -> int:
    """主函数"""
    failed_files = []
    
    for arg in sys.argv[1:]:
        filepath = Path(arg)
        if filepath.suffix in ['.h', '.hpp', '.cpp', '.cc', '.cxx']:
            if not check_file(filepath):
                failed_files.append(str(filepath))
    
    if failed_files:
        print("以下文件缺少版权头/文件注释：")
        for f in failed_files:
            print(f"  - {f}")
        print("\n请添加 Doxygen 风格的文件注释，例如：")
        print('/**')
        print(' * @file filename.h')
        print(' * @brief 文件简要描述')
        print(' */')
        return 1
    
    return 0


if __name__ == '__main__':
    sys.exit(main())
