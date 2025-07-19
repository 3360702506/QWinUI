# QWinUI - Qt WinUI3风格组件库

QWinUI是一个基于Qt的自定义组件库，严格遵循Microsoft WinUI3和Fluent Design System的设计规范，为Qt应用程序提供现代化的Win11风格用户界面组件。

## 特性

- 🎨 **Fluent Design**: 严格遵循Microsoft Fluent Design System
- 🌙 **主题支持**: 支持浅色/深色主题自动切换
- ✨ **现代效果**: Acrylic材质、Reveal高亮、流畅动画
- 📱 **响应式**: 支持不同屏幕尺寸和DPI缩放
- ♿ **无障碍**: 完整的键盘导航和屏幕阅读器支持
- 🌍 **国际化**: 支持RTL布局和多语言

## 组件列表

### 基础控件
- QWinUIButton - 按钮控件
- QWinUITextBox - 文本输入框
- QWinUICheckBox - 复选框
- QWinUIRadioButton - 单选按钮
- QWinUIToggleSwitch - 开关控件

### 导航控件
- QWinUINavigationView - 导航视图
- QWinUITabView - 标签页视图
- QWinUIBreadcrumbBar - 面包屑导航

### 数据展示
- QWinUIListView - 列表视图
- QWinUIGridView - 网格视图
- QWinUITreeView - 树形视图

### 更多组件...

## 快速开始

```cpp
#include <QApplication>
#include <QWinUI/QWinUIButton>
#include <QWinUI/QWinUITheme>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 设置主题
    QWinUITheme::getInstance()->setThemeMode(QWinUITheme::Auto);
    
    // 创建按钮
    QWinUIButton button("Hello QWinUI");
    button.show();
    
    return app.exec();
}
```

## 构建要求

- Qt 6.0+
- C++17
- CMake 3.16+

## 许可证

MIT License
