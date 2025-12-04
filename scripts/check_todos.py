#!/usr/bin/env python3
"""
检查 C++ 源文件中的 TODO/FIXME 注释
用于 pre-commit 钩子（仅警告，不阻止提交）
"""

import sys
import re
from pathlib import Path
from typing import List, Tuple

# TODO/FIXME 模式
TODO_PATTERN = re.compile(
    r'(//|/\*|\*)\s*(TODO|FIXME|XXX|HACK|BUG)[\s:]*(.*)$',
    re.IGNORECASE | re.MULTILINE
)


def find_todos(filepath: Path) -> List[Tuple[int, str, str]]:
    """查找文件中的 TODO/FIXME 注释"""
    try:
        content = filepath.read_text(encoding='utf-8')
    except UnicodeDecodeError:
        content = filepath.read_text(encoding='gbk')

    todos = []
    for i, line in enumerate(content.splitlines(), 1):
        match = TODO_PATTERN.search(line)
        if match:
            tag = match.group(2).upper()
            message = match.group(3).strip()
            todos.append((i, tag, message))

    return todos


def main() -> int:
    """主函数"""
    all_todos = {}

    for arg in sys.argv[1:]:
        filepath = Path(arg)
        if filepath.suffix in ['.h', '.hpp', '.cpp', '.cc', '.cxx']:
            todos = find_todos(filepath)
            if todos:
                all_todos[str(filepath)] = todos

    if all_todos:
        print("发现以下 TODO/FIXME 注释：")
        print("-" * 60)
        for filepath, todos in all_todos.items():
            print(f"\n{filepath}:")
            for line_num, tag, message in todos:
                print(f"  L{line_num}: [{tag}] {message}")
        print("-" * 60)
        print(f"共 {sum(len(t) for t in all_todos.values())} 个待办项")
        print("\n注意：这只是提醒，不会阻止提交。")

    # 始终返回 0，不阻止提交
    return 0


if __name__ == '__main__':
    sys.exit(main())
