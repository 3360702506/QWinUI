# QWinUIAppBarSeparator 使用指南

## 概述

QWinUIAppBarSeparator 是一个基于 QWinUIWidget 的分隔线控件，严格遵循 WinUI3 的 AppBarSeparator 设计规范。它提供了简洁的视觉分隔效果，自动适配当前主题的颜色。

## 特性

- **主题联动**: 自动响应主题变化，颜色与当前主题保持一致
- **双向支持**: 支持垂直和水平两种方向
- **WinUI3 规范**: 严格按照 Microsoft WinUI3 设计规范实现
- **轻量级**: 最小化的实现，专注于分隔线功能

## 基本使用

### 包含头文件

```cpp
#include <QWinUI/Controls/QWinUIAppBarSeparator.h>
```

### 创建分隔线

```cpp
// 创建垂直分隔线（默认）
QWinUIAppBarSeparator* verticalSeparator = new QWinUIAppBarSeparator(this);

// 创建水平分隔线
QWinUIAppBarSeparator* horizontalSeparator = new QWinUIAppBarSeparator(Qt::Horizontal, this);
```

### 在工具栏中使用

```cpp
// 创建工具栏布局
QHBoxLayout* toolbarLayout = new QHBoxLayout();

// 添加按钮
QWinUIIconButton* homeButton = new QWinUIIconButton(this);
homeButton->setFluentIcon(QWinUIFluentIcons::Navigation::HOME);
toolbarLayout->addWidget(homeButton);

QWinUIIconButton* searchButton = new QWinUIIconButton(this);
searchButton->setFluentIcon(QWinUIFluentIcons::Navigation::SEARCH);
toolbarLayout->addWidget(searchButton);

// 添加垂直分隔线
QWinUIAppBarSeparator* separator = new QWinUIAppBarSeparator(Qt::Vertical, this);
toolbarLayout->addWidget(separator);

// 添加更多按钮
QWinUIIconButton* settingsButton = new QWinUIIconButton(this);
settingsButton->setFluentIcon(QWinUIFluentIcons::System::SETTINGS);
toolbarLayout->addWidget(settingsButton);
```

### 在垂直布局中使用

```cpp
QVBoxLayout* layout = new QVBoxLayout();

// 添加内容
layout->addWidget(new QLabel("第一部分内容"));

// 添加水平分隔线
QWinUIAppBarSeparator* separator = new QWinUIAppBarSeparator(Qt::Horizontal, this);
layout->addWidget(separator);

// 添加更多内容
layout->addWidget(new QLabel("第二部分内容"));
```

## 属性设置

### 方向设置

```cpp
QWinUIAppBarSeparator* separator = new QWinUIAppBarSeparator(this);

// 设置为水平方向
separator->setOrientation(Qt::Horizontal);

// 获取当前方向
Qt::Orientation orientation = separator->orientation();
```

### 宽度设置

```cpp
// 设置分隔线宽度（像素）
separator->setSeparatorWidth(2); // 2像素宽度

// 获取当前宽度
int width = separator->separatorWidth();
```

## 默认值

- **垂直分隔线**: 宽度 1 像素，高度自适应
- **水平分隔线**: 高度 1 像素，宽度自适应
- **颜色**: 自动从主题获取 `ControlStrokeColorSecondary`

## 主题适配

QWinUIAppBarSeparator 会自动响应主题变化：

- **浅色主题**: 使用半透明的深色分隔线
- **深色主题**: 使用半透明的浅色分隔线
- **系统主题**: 跟随系统主题自动切换

```cpp
// 分隔线会自动响应主题变化，无需手动处理
QWinUITheme* theme = QWinUITheme::getInstance();
theme->setThemeMode(QWinUIThemeMode::Dark); // 分隔线颜色会自动更新
```

## 信号

```cpp
// 监听方向变化
connect(separator, &QWinUIAppBarSeparator::orientationChanged, 
        [](Qt::Orientation orientation) {
    qDebug() << "方向已更改为:" << (orientation == Qt::Vertical ? "垂直" : "水平");
});

// 监听宽度变化
connect(separator, &QWinUIAppBarSeparator::separatorWidthChanged,
        [](int width) {
    qDebug() << "宽度已更改为:" << width << "像素";
});
```

## 最佳实践

1. **工具栏分组**: 使用垂直分隔线将相关的工具栏按钮分组
2. **内容分区**: 使用水平分隔线分隔不同的内容区域
3. **保持简洁**: 避免过度使用分隔线，保持界面简洁
4. **主题一致性**: 依赖主题系统提供的颜色，不要自定义颜色

## 示例：完整的工具栏

```cpp
class ModernToolBar : public QWinUIWidget
{
public:
    ModernToolBar(QWidget* parent = nullptr) : QWinUIWidget(parent)
    {
        auto layout = new QHBoxLayout(this);
        layout->setSpacing(5);
        layout->setContentsMargins(10, 5, 10, 5);

        // 文件操作组
        addToolButton(QWinUIFluentIcons::Actions::OPEN, "打开", layout);
        addToolButton(QWinUIFluentIcons::Actions::SAVE, "保存", layout);
        
        // 分隔线
        layout->addWidget(new QWinUIAppBarSeparator(Qt::Vertical, this));

        // 编辑操作组
        addToolButton(QWinUIFluentIcons::Actions::CUT, "剪切", layout);
        addToolButton(QWinUIFluentIcons::Actions::COPY, "复制", layout);
        addToolButton(QWinUIFluentIcons::Actions::PASTE, "粘贴", layout);
        
        // 分隔线
        layout->addWidget(new QWinUIAppBarSeparator(Qt::Vertical, this));

        // 撤销重做组
        addToolButton(QWinUIFluentIcons::Actions::UNDO, "撤销", layout);
        addToolButton(QWinUIFluentIcons::Actions::REDO, "重做", layout);

        // 弹性空间
        layout->addStretch();

        // 设置按钮
        addToolButton(QWinUIFluentIcons::System::SETTINGS, "设置", layout);
    }

private:
    void addToolButton(const QChar& icon, const QString& tooltip, QHBoxLayout* layout)
    {
        auto button = new QWinUIIconButton(this);
        button->setFluentIcon(icon);
        button->setToolTipText(tooltip);
        button->setFixedSize(32, 32);
        layout->addWidget(button);
    }
};
```

## 注意事项

- 分隔线的颜色完全由主题系统控制，无法自定义
- 分隔线不接受焦点，不参与键盘导航
- 分隔线没有圆角和阴影效果，保持简洁的外观
- 建议在工具栏和菜单中使用，避免在普通内容区域过度使用

---

*QWinUIAppBarSeparator - 简洁而优雅的分隔线控件*
