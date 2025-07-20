#!/usr/bin/env python3
"""
自动生成SVG图标的QRC资源文件
遍历Icon目录下的所有SVG文件并生成对应的QRC文件
"""

import os
import sys
from pathlib import Path

def generate_qrc_file(icon_dir, output_file):
    """
    生成QRC文件
    
    Args:
        icon_dir: 图标目录路径
        output_file: 输出的QRC文件路径
    """
    
    # 检查图标目录是否存在
    if not os.path.exists(icon_dir):
        print(f"错误: 图标目录 {icon_dir} 不存在")
        return False
    
    # 收集所有SVG文件
    svg_files = []
    for root, dirs, files in os.walk(icon_dir):
        for file in files:
            if file.endswith('.svg'):
                # 获取相对于icon_dir的路径
                rel_path = os.path.relpath(os.path.join(root, file), icon_dir)
                # 转换为Unix风格的路径分隔符
                rel_path = rel_path.replace('\\', '/')
                svg_files.append(rel_path)
    
    # 按路径排序
    svg_files.sort()
    
    print(f"找到 {len(svg_files)} 个SVG图标文件")
    
    # 生成QRC文件内容
    qrc_content = ['<?xml version="1.0" encoding="UTF-8"?>']
    qrc_content.append('<RCC>')
    qrc_content.append('    <qresource prefix="/icons">')
    
    for svg_file in svg_files:
        # 生成别名（移除.svg扩展名）
        alias = svg_file[:-4]  # 移除.svg
        
        # 生成相对于QRC文件的路径
        file_path = f"../Icon/{svg_file}"
        
        qrc_content.append(f'        <file alias="{alias}">{file_path}</file>')
    
    qrc_content.append('    </qresource>')
    qrc_content.append('</RCC>')
    
    # 写入QRC文件
    try:
        os.makedirs(os.path.dirname(output_file), exist_ok=True)
        with open(output_file, 'w', encoding='utf-8') as f:
            f.write('\n'.join(qrc_content))
        print(f"成功生成QRC文件: {output_file}")
        return True
    except Exception as e:
        print(f"错误: 无法写入QRC文件 {output_file}: {e}")
        return False

def generate_icon_constants(svg_files, output_file):
    """
    生成图标路径常量头文件
    
    Args:
        svg_files: SVG文件列表
        output_file: 输出的头文件路径
    """
    
    # 生成头文件内容
    header_content = [
        '#pragma once',
        '',
        '// 自动生成的图标路径常量',
        '// 请勿手动修改此文件',
        '',
        '#include <QString>',
        '',
        'namespace QWinUIIcons {',
        ''
    ]
    
    # 按分类组织图标
    categories = {}
    for svg_file in svg_files:
        parts = svg_file.split('/')
        if len(parts) >= 2:
            category = parts[0]
            icon_name = parts[1][:-4]  # 移除.svg扩展名
        else:
            category = "general"
            icon_name = parts[0][:-4]
        
        if category not in categories:
            categories[category] = []
        categories[category].append((icon_name, svg_file[:-4]))
    
    # 生成常量定义
    for category, icons in sorted(categories.items()):
        header_content.append(f'    // {category.title()} Icons')
        header_content.append(f'    namespace {category.title()} {{')
        
        for icon_name, alias in sorted(icons):
            # 转换图标名称为有效的C++标识符
            const_name = icon_name.replace('-', '_').replace(' ', '_').replace('.', '_').upper()

            # 确保标识符不以数字开头
            if const_name and const_name[0].isdigit():
                const_name = 'ICON_' + const_name

            # 移除或替换其他无效字符
            import re
            const_name = re.sub(r'[^A-Z0-9_]', '_', const_name)

            # 确保不为空且不重复下划线
            const_name = re.sub(r'_+', '_', const_name).strip('_')

            if const_name:  # 只有有效的标识符才添加
                header_content.append(f'        inline const QString {const_name} = ":/icons/{alias}";')
        
        header_content.append('    }')
        header_content.append('')
    
    header_content.append('}')
    
    # 写入头文件
    try:
        os.makedirs(os.path.dirname(output_file), exist_ok=True)
        with open(output_file, 'w', encoding='utf-8') as f:
            f.write('\n'.join(header_content))
        print(f"成功生成图标常量头文件: {output_file}")
        return True
    except Exception as e:
        print(f"错误: 无法写入头文件 {output_file}: {e}")
        return False

def main():
    """主函数"""
    
    # 获取脚本所在目录
    script_dir = Path(__file__).parent
    project_root = script_dir.parent
    
    # 设置路径
    icon_dir = project_root / "Icon"
    qrc_file = project_root / "resources" / "icons.qrc"
    header_file = project_root / "include" / "QWinUI" / "QWinUIIconPaths.h"
    
    print("QWinUI 图标资源生成器")
    print("=" * 40)
    print(f"图标目录: {icon_dir}")
    print(f"QRC文件: {qrc_file}")
    print(f"头文件: {header_file}")
    print()
    
    # 收集SVG文件
    svg_files = []
    if icon_dir.exists():
        for svg_path in icon_dir.rglob("*.svg"):
            rel_path = svg_path.relative_to(icon_dir)
            # 转换为Unix风格的路径
            rel_path_str = str(rel_path).replace('\\', '/')
            svg_files.append(rel_path_str)
    else:
        print(f"错误: 图标目录 {icon_dir} 不存在")
        return 1
    
    svg_files.sort()
    print(f"找到 {len(svg_files)} 个SVG图标文件")
    
    # 生成QRC文件
    success = generate_qrc_file(str(icon_dir), str(qrc_file))
    if not success:
        return 1
    
    # 生成图标常量头文件
    success = generate_icon_constants(svg_files, str(header_file))
    if not success:
        return 1
    
    print()
    print("生成完成！")
    print()
    print("使用方法:")
    print("1. 在CMakeLists.txt中添加QRC文件")
    print("2. 包含头文件: #include \"QWinUI/QWinUIIconPaths.h\"")
    print("3. 使用图标: QWinUIIcons::Solid::HOME")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
