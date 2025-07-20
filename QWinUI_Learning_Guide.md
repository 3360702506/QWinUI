# QWinUI 学习指南

## 目录
1. [快速开始](#快速开始)
2. [基础概念](#基础概念)
3. [控件使用](#控件使用)
4. [主题系统](#主题系统)
5. [图标系统](#图标系统)
6. [高级特性](#高级特性)
7. [最佳实践](#最佳实践)
8. [常见问题](#常见问题)

## 快速开始

### 环境要求
- Qt 6.4 或更高版本
- Windows 10/11 操作系统
- MSVC 2019 或更高版本编译器

### 项目集成

1. **包含头文件**
```cpp
#include <QWinUI/QWinUIWidget.h>
#include <QWinUI/Controls/QWinUIButton.h>
#include <QWinUI/Controls/QWinUIIconButton.h>
#include <QWinUI/QWinUITheme.h>
```

2. **基本使用**
```cpp
// 创建主窗口
class MainWindow : public QWinUIWidget
{
public:
    MainWindow(QWidget* parent = nullptr) : QWinUIWidget(parent)
    {
        // 设置窗口属性
        setWindowTitle("QWinUI 应用");
        resize(800, 600);
        
        // 创建控件
        setupUI();
    }
    
private:
    void setupUI()
    {
        auto layout = new QVBoxLayout(this);
        
        // 创建按钮
        auto button = new QWinUIButton("点击我", this);
        layout->addWidget(button);
        
        // 创建图标按钮
        auto iconButton = new QWinUIIconButton(this);
        iconButton->setFluentIcon(QWinUIFluentIcons::Navigation::HOME);
        layout->addWidget(iconButton);
    }
};
```

## 基础概念

### QWinUIWidget 基类

所有 QWinUI 控件都继承自 `QWinUIWidget`，它提供了：

- **主题支持**: 自动响应主题变化
- **圆角设计**: 统一的圆角样式
- **阴影效果**: 可配置的阴影深度
- **工具提示**: 现代化的工具提示系统

```cpp
// 基本属性设置
widget->setCornerRadius(8);                    // 设置圆角
widget->setShadowDepth(QWinUIShadowDepth::Medium); // 设置阴影
widget->setToolTipText("这是一个工具提示");      // 设置工具提示
```

### 主题系统核心

QWinUI 的主题系统基于 `QWinUITheme` 单例：

```cpp
// 获取主题管理器
QWinUITheme* theme = QWinUITheme::getInstance();

// 检查当前主题
bool isDark = theme->isDarkMode();

// 获取强调色
QColor accentColor = theme->getAccentColor();

// 监听主题变化
connect(theme, &QWinUITheme::themeModeChanged, this, [this]() {
    // 主题变化时的处理逻辑
    updateUI();
});
```

## 控件使用

### 按钮控件

#### QWinUIButton - 基础按钮
```cpp
// 创建按钮
auto button = new QWinUIButton("按钮文本", this);

// 设置样式
button->setButtonStyle(QWinUIButton::Primary);  // 主要按钮样式
button->setButtonStyle(QWinUIButton::Secondary); // 次要按钮样式

// 设置大小
button->setFixedSize(120, 40);
```

#### QWinUIIconButton - 图标按钮
```cpp
// 使用 Fluent Icons
auto iconButton = new QWinUIIconButton(this);
iconButton->setFluentIcon(QWinUIFluentIcons::Navigation::HOME);

// 使用自定义图标
iconButton->setIconName("custom-icon-name");

// 设置图标颜色
iconButton->setColorizeIcon(true);
iconButton->setIconColor(QColor(255, 0, 0));

// 设置工具提示
iconButton->setToolTipText("主页");
```

### 输入控件

#### QWinUITextBox - 文本输入框
```cpp
auto textBox = new QWinUITextBox(this);

// 设置占位符文本
textBox->setPlaceholderText("请输入文本...");

// 设置输入验证
textBox->setValidator(new QIntValidator(0, 100, this));

// 监听文本变化
connect(textBox, &QWinUITextBox::textChanged, this, [](const QString& text) {
    qDebug() << "文本变化:" << text;
});
```

#### QWinUIPasswordBox - 密码输入框
```cpp
auto passwordBox = new QWinUIPasswordBox(this);

// 设置占位符
passwordBox->setPlaceholderText("请输入密码...");

// 设置密码可见性切换
passwordBox->setPasswordRevealEnabled(true);
```

### 显示控件

#### QWinUIProgressBar - 进度条
```cpp
auto progressBar = new QWinUIProgressBar(this);

// 设置进度值
progressBar->setValue(50);
progressBar->setRange(0, 100);

// 设置不确定进度
progressBar->setIndeterminate(true);

// 启用自动工具提示
progressBar->setAutoToolTip(true);
```

## 主题系统

### 主题模式

QWinUI 支持三种主题模式：

```cpp
// 设置主题模式
QWinUITheme* theme = QWinUITheme::getInstance();

theme->setThemeMode(QWinUITheme::Light);  // 浅色主题
theme->setThemeMode(QWinUITheme::Dark);   // 深色主题
theme->setThemeMode(QWinUITheme::System); // 跟随系统
```

### 自定义主题颜色

```cpp
// 在控件中使用主题颜色
QColor backgroundColor = theme->isDarkMode() ? 
    QColor(32, 32, 32) : QColor(255, 255, 255);

QColor textColor = theme->isDarkMode() ? 
    QColor(255, 255, 255) : QColor(0, 0, 0);
```

## 图标系统

### Fluent Icons 使用

QWinUI 提供了完整的 Fluent Icons 图标库：

```cpp
// 导航图标
QWinUIFluentIcons::Navigation::HOME
QWinUIFluentIcons::Navigation::BACK
QWinUIFluentIcons::Navigation::SEARCH

// 操作图标
QWinUIFluentIcons::Actions::ADD
QWinUIFluentIcons::Actions::DELETE_ICON
QWinUIFluentIcons::Actions::EDIT

// 媒体图标
QWinUIFluentIcons::Media::PLAY
QWinUIFluentIcons::Media::PAUSE
QWinUIFluentIcons::Media::STOP

// 系统图标
QWinUIFluentIcons::System::SETTINGS
QWinUIFluentIcons::System::INFO

// 状态图标
QWinUIFluentIcons::Status::WARNING
QWinUIFluentIcons::Status::ERROR
```

### 图标管理器

```cpp
// 获取图标管理器
QWinUIIconManager* iconManager = QWinUIIconManager::getInstance();

// 加载图标资源
iconManager->loadIconsFromDirectory(":/icons");

// 使用图标
auto iconButton = new QWinUIIconButton(this);
iconButton->setIconName("solid/heart");
```

## 高级特性

### 亚克力效果

```cpp
// 创建亚克力背景
auto acrylicBrush = new QWinUIAcrylicBrush(this);
acrylicBrush->setTintColor(QColor(255, 255, 255, 100));
acrylicBrush->setBlurRadius(20);

// 应用到控件
widget->setStyleSheet("background: transparent;");
// 在 paintEvent 中使用 acrylicBrush 绘制背景
```

### 自定义工具提示

```cpp
// 创建自定义工具提示
auto tooltip = new QWinUIToolTip("自定义提示内容", targetWidget, this);
tooltip->setPlacement(QWinUIToolTip::Top);
tooltip->setShowDelay(500);
tooltip->setHideDelay(100);
```

## 最佳实践

### 1. 主题适配
```cpp
// 在自定义控件中正确处理主题变化
class CustomWidget : public QWinUIWidget
{
protected:
    void paintEvent(QPaintEvent* event) override
    {
        QPainter painter(this);
        
        // 使用主题相关的颜色
        QWinUITheme* theme = QWinUITheme::getInstance();
        QColor bgColor = theme->isDarkMode() ? 
            QColor(45, 45, 45) : QColor(255, 255, 255);
            
        painter.fillRect(rect(), bgColor);
    }
};
```

### 2. 响应式设计
```cpp
// 根据窗口大小调整布局
void MainWindow::resizeEvent(QResizeEvent* event)
{
    QWinUIWidget::resizeEvent(event);
    
    // 根据窗口宽度调整布局
    if (width() < 600) {
        // 紧凑布局
        layout->setSpacing(5);
    } else {
        // 正常布局
        layout->setSpacing(10);
    }
}
```

### 3. 性能优化
```cpp
// 避免频繁的主题查询
class OptimizedWidget : public QWinUIWidget
{
private:
    QColor m_cachedBackgroundColor;
    
public:
    OptimizedWidget(QWidget* parent = nullptr) : QWinUIWidget(parent)
    {
        updateCachedColors();
        
        // 只在主题变化时更新缓存
        connect(QWinUITheme::getInstance(), &QWinUITheme::themeModeChanged,
                this, &OptimizedWidget::updateCachedColors);
    }
    
private:
    void updateCachedColors()
    {
        QWinUITheme* theme = QWinUITheme::getInstance();
        m_cachedBackgroundColor = theme->isDarkMode() ? 
            QColor(45, 45, 45) : QColor(255, 255, 255);
        update();
    }
};
```

## 常见问题

### Q: 如何确保图标在不同主题下显示正确？
A: 使用 `setColorizeIcon(false)` 让图标自动适配主题，或者监听主题变化事件手动更新图标颜色。

### Q: 工具提示不显示怎么办？
A: 确保使用 `setToolTipText()` 而不是 Qt 的 `setToolTip()`，QWinUI 有自己的工具提示系统。

### Q: 如何添加自定义图标？
A: 将图标文件放入资源目录，然后使用 `QWinUIIconManager::loadIconsFromDirectory()` 加载。

### Q: 主题切换后控件没有更新？
A: 确保自定义控件正确连接了主题变化信号，并在信号处理中调用 `update()`。

## 实战示例

### 完整的登录界面示例

```cpp
class LoginWindow : public QWinUIWidget
{
    Q_OBJECT

public:
    LoginWindow(QWidget* parent = nullptr) : QWinUIWidget(parent)
    {
        setupUI();
        connectSignals();
    }

private:
    void setupUI()
    {
        setWindowTitle("用户登录");
        setFixedSize(400, 300);

        auto mainLayout = new QVBoxLayout(this);
        mainLayout->setSpacing(20);
        mainLayout->setContentsMargins(40, 40, 40, 40);

        // 标题
        auto titleLabel = new QLabel("欢迎登录", this);
        titleLabel->setAlignment(Qt::AlignCenter);
        titleLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
        mainLayout->addWidget(titleLabel);

        // 用户名输入
        m_usernameBox = new QWinUITextBox(this);
        m_usernameBox->setPlaceholderText("请输入用户名");
        mainLayout->addWidget(m_usernameBox);

        // 密码输入
        m_passwordBox = new QWinUIPasswordBox(this);
        m_passwordBox->setPlaceholderText("请输入密码");
        mainLayout->addWidget(m_passwordBox);

        // 按钮布局
        auto buttonLayout = new QHBoxLayout();

        m_loginButton = new QWinUIButton("登录", this);
        m_loginButton->setButtonStyle(QWinUIButton::Primary);
        buttonLayout->addWidget(m_loginButton);

        m_cancelButton = new QWinUIButton("取消", this);
        buttonLayout->addWidget(m_cancelButton);

        mainLayout->addLayout(buttonLayout);

        // 进度条（初始隐藏）
        m_progressBar = new QWinUIProgressBar(this);
        m_progressBar->setIndeterminate(true);
        m_progressBar->hide();
        mainLayout->addWidget(m_progressBar);
    }

    void connectSignals()
    {
        connect(m_loginButton, &QWinUIButton::clicked, this, &LoginWindow::onLogin);
        connect(m_cancelButton, &QWinUIButton::clicked, this, &QWidget::close);
    }

private slots:
    void onLogin()
    {
        QString username = m_usernameBox->text();
        QString password = m_passwordBox->text();

        if (username.isEmpty() || password.isEmpty()) {
            QWinUIToolTip::showText(mapToGlobal(rect().center()),
                                   "请填写完整信息", this);
            return;
        }

        // 显示进度条
        m_progressBar->show();
        m_loginButton->setEnabled(false);

        // 模拟登录过程
        QTimer::singleShot(2000, this, [this]() {
            m_progressBar->hide();
            m_loginButton->setEnabled(true);
            accept();
        });
    }

private:
    QWinUITextBox* m_usernameBox;
    QWinUIPasswordBox* m_passwordBox;
    QWinUIButton* m_loginButton;
    QWinUIButton* m_cancelButton;
    QWinUIProgressBar* m_progressBar;
};
```

### 现代化工具栏示例

```cpp
class ModernToolBar : public QWinUIWidget
{
public:
    ModernToolBar(QWidget* parent = nullptr) : QWinUIWidget(parent)
    {
        setupToolBar();
    }

private:
    void setupToolBar()
    {
        auto layout = new QHBoxLayout(this);
        layout->setSpacing(5);
        layout->setContentsMargins(10, 5, 10, 5);

        // 文件操作
        addToolButton(QWinUIFluentIcons::Actions::OPEN, "打开", layout);
        addToolButton(QWinUIFluentIcons::Actions::SAVE, "保存", layout);

        // 分隔符
        layout->addWidget(createSeparator());

        // 编辑操作
        addToolButton(QWinUIFluentIcons::Actions::CUT, "剪切", layout);
        addToolButton(QWinUIFluentIcons::Actions::COPY, "复制", layout);
        addToolButton(QWinUIFluentIcons::Actions::PASTE, "粘贴", layout);

        // 分隔符
        layout->addWidget(createSeparator());

        // 撤销重做
        addToolButton(QWinUIFluentIcons::Actions::UNDO, "撤销", layout);
        addToolButton(QWinUIFluentIcons::Actions::REDO, "重做", layout);

        // 弹性空间
        layout->addStretch();

        // 设置按钮
        addToolButton(QWinUIFluentIcons::System::SETTINGS, "设置", layout);
    }

    void addToolButton(const QChar& icon, const QString& tooltip, QHBoxLayout* layout)
    {
        auto button = new QWinUIIconButton(this);
        button->setFluentIcon(icon);
        button->setToolTipText(tooltip);
        button->setFixedSize(32, 32);
        layout->addWidget(button);
    }

    QWidget* createSeparator()
    {
        auto separator = new QFrame(this);
        separator->setFrameShape(QFrame::VLine);
        separator->setFixedWidth(1);
        separator->setStyleSheet("color: #E0E0E0;");
        return separator;
    }
};
```

## 调试技巧

### 主题调试
```cpp
// 添加主题调试信息
void debugTheme()
{
    QWinUITheme* theme = QWinUITheme::getInstance();
    qDebug() << "当前主题模式:" << (theme->isDarkMode() ? "深色" : "浅色");
    qDebug() << "系统强调色:" << theme->getAccentColor().name();
    qDebug() << "主题模式设置:" << theme->getThemeMode();
}
```

### 图标调试
```cpp
// 检查图标加载状态
void debugIcons()
{
    QWinUIIconManager* manager = QWinUIIconManager::getInstance();
    if (manager) {
        qDebug() << "已加载图标数量:" << manager->getIconNames().size();
        qDebug() << "图标分类:" << manager->getCategories();
    }
}
```

## 性能优化建议

### 1. 减少不必要的重绘
```cpp
// 使用 update() 而不是 repaint()
void MyWidget::someDataChanged()
{
    // 好的做法
    update();  // 延迟重绘，可以合并多次调用

    // 避免
    // repaint();  // 立即重绘，性能较差
}
```

### 2. 缓存复杂计算
```cpp
class OptimizedWidget : public QWinUIWidget
{
private:
    mutable QPixmap m_cachedBackground;
    mutable bool m_backgroundDirty = true;

protected:
    void paintEvent(QPaintEvent* event) override
    {
        QPainter painter(this);

        if (m_backgroundDirty) {
            updateCachedBackground();
            m_backgroundDirty = false;
        }

        painter.drawPixmap(0, 0, m_cachedBackground);
    }

    void resizeEvent(QResizeEvent* event) override
    {
        QWinUIWidget::resizeEvent(event);
        m_backgroundDirty = true;
    }

private:
    void updateCachedBackground()
    {
        m_cachedBackground = QPixmap(size());
        m_cachedBackground.fill(Qt::transparent);

        QPainter painter(&m_cachedBackground);
        // 绘制复杂的背景...
    }
};
```

### 3. 合理使用动画
```cpp
// 避免同时运行过多动画
class AnimationManager
{
private:
    static int s_activeAnimations;
    static const int MAX_ANIMATIONS = 5;

public:
    static bool canStartAnimation()
    {
        return s_activeAnimations < MAX_ANIMATIONS;
    }

    static void animationStarted() { s_activeAnimations++; }
    static void animationFinished() { s_activeAnimations--; }
};
```

## 部署指南

### 1. 必需的 DLL 文件
确保以下文件包含在发布包中：
- QWinUI.dll
- Qt6Core.dll
- Qt6Gui.dll
- Qt6Widgets.dll
- Qt6Svg.dll (如果使用 SVG 图标)

### 2. 字体依赖
QWinUI 依赖以下字体：
- Segoe Fluent Icons (Windows 11 内置)
- Segoe MDL2 Assets (Windows 10 内置)

### 3. 资源文件
确保图标资源文件正确打包：
```qrc
<RCC>
    <qresource prefix="/icons">
        <file>icons/solid/heart.svg</file>
        <file>icons/outline/star.svg</file>
        <!-- 更多图标文件 -->
    </qresource>
</RCC>
```

---

*QWinUI 学习指南 - 让您快速掌握现代化 Qt 界面开发*
