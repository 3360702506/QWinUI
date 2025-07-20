# QWinUI - 现代化的 Qt WinUI3 风格控件库

## 项目概述

QWinUI 是一个基于 Qt 6 开发的现代化控件库，完全遵循 Microsoft WinUI3 设计规范，为 Qt 应用程序提供原生 Windows 11 风格的用户界面体验。

## 核心特性

### 🎨 设计系统
- **Fluent Design System**: 完整实现微软 Fluent Design 设计语言
- **主题系统**: 支持浅色/深色主题自动切换，响应系统主题变化
- **强调色**: 支持系统强调色自动获取和应用
- **圆角设计**: 统一的圆角设计规范，支持多种圆角半径
- **阴影效果**: 多层次阴影系统，营造深度感

### 🎭 视觉效果
- **亚克力效果**: 真实的亚克力材质背景效果
- **动画系统**: 流畅的过渡动画和状态变化动画
- **响应式设计**: 自适应不同屏幕尺寸和分辨率
- **高DPI支持**: 完美支持高分辨率显示器

### 🧩 控件体系

#### 基础控件
- **QWinUIWidget**: 所有控件的基类，提供主题支持和基础功能
- **QWinUIButton**: 现代化按钮控件，支持多种样式
- **QWinUIIconButton**: 图标按钮，支持 Fluent Icons 和自定义图标

#### 输入控件
- **QWinUITextBox**: 现代化文本输入框
- **QWinUIPasswordBox**: 密码输入框，支持显示/隐藏密码
- **QWinUIRichEditBox**: 富文本编辑器

#### 显示控件
- **QWinUIProgressBar**: 进度条控件，支持确定和不确定进度
- **QWinUIToolTip**: 现代化工具提示

#### 特效控件
- **QWinUIAcrylicBrush**: 亚克力背景效果

### 🎯 图标系统

#### Fluent Icons
- **完整的图标库**: 包含导航、操作、媒体、系统、状态等分类
- **矢量图标**: 基于字体的矢量图标，支持任意缩放
- **主题适配**: 图标颜色自动适配主题

#### 图标管理器
- **QWinUIIconManager**: 统一的图标资源管理
- **多格式支持**: 支持 SVG、PNG 等多种图标格式
- **分类管理**: 支持图标分类和标签系统

### 🎨 主题系统

#### QWinUITheme
- **单例模式**: 全局主题管理器
- **自动检测**: 自动检测系统主题变化
- **强调色**: 自动获取系统强调色
- **主题切换**: 支持手动和自动主题切换

#### 主题特性
- **浅色主题**: 明亮清爽的界面风格
- **深色主题**: 护眼的深色界面风格
- **系统同步**: 与 Windows 系统主题保持同步

## 技术架构

### 设计模式
- **继承体系**: 所有控件继承自 QWinUIWidget 基类
- **单例模式**: 主题管理器和图标管理器使用单例模式
- **观察者模式**: 主题变化通知机制

### 核心组件
```
QWinUIWidget (基类)
├── QWinUIButton
│   └── QWinUIIconButton
├── QWinUITextBox
├── QWinUIPasswordBox
├── QWinUIRichEditBox
├── QWinUIProgressBar
└── QWinUIToolTip
```

### 管理器系统
- **QWinUITheme**: 主题管理器
- **QWinUIIconManager**: 图标资源管理器

## 开发优势

### 🚀 易于使用
- **Qt 兼容**: 完全兼容 Qt 6 开发模式
- **简单集成**: 只需包含头文件即可使用
- **丰富示例**: 提供完整的使用示例

### 🎯 高度定制
- **样式定制**: 支持自定义颜色、圆角、阴影等
- **主题扩展**: 可扩展自定义主题
- **图标扩展**: 支持自定义图标库

### 📱 现代化
- **Windows 11 风格**: 原生 Windows 11 界面体验
- **触摸友好**: 支持触摸操作
- **无障碍访问**: 支持无障碍功能

## 应用场景

### 桌面应用
- **企业应用**: 现代化的企业级桌面应用
- **工具软件**: 专业的工具类软件
- **媒体应用**: 多媒体播放和编辑软件

### 系统集成
- **Windows 集成**: 与 Windows 系统深度集成
- **主题同步**: 自动适配系统主题变化
- **原生体验**: 提供原生 Windows 应用体验

## 版本信息

- **当前版本**: 1.0.0 (开发中)
- **Qt 版本要求**: Qt 6.4+
- **平台支持**: Windows 10/11
- **编译器**: MSVC 2019+

## 开源协议

QWinUI 采用 MIT 开源协议，允许商业和非商业使用。

## 技术规范

### 系统要求
- **操作系统**: Windows 10 版本 1903 或更高版本，Windows 11
- **Qt 版本**: Qt 6.4.0 或更高版本
- **编译器**:
  - MSVC 2019 (v142) 或更高版本
  - MinGW 11.0 或更高版本 (实验性支持)
- **C++ 标准**: C++17 或更高版本

### 依赖库
- **Qt 模块**:
  - Qt6Core
  - Qt6Gui
  - Qt6Widgets
  - Qt6Svg (可选，用于 SVG 图标支持)
  - Qt6SvgWidgets (可选)

### 性能特性
- **内存占用**: 基础库约 2-5MB
- **启动时间**: 典型应用启动时间 < 500ms
- **渲染性能**: 支持硬件加速，60fps 流畅动画
- **主题切换**: < 100ms 响应时间

## API 概览

### 核心类层次结构
```
QWinUIWidget
├── QWinUIButton
│   └── QWinUIIconButton
├── QWinUITextBox
├── QWinUIPasswordBox
├── QWinUIRichEditBox
├── QWinUIProgressBar
├── QWinUIToolTip
└── QWinUIAcrylicBrush

QObject
├── QWinUITheme (单例)
└── QWinUIIconManager (单例)
```

### 主要命名空间
- **QWinUIFluentIcons**: Fluent 图标常量定义
  - `Navigation`: 导航相关图标
  - `Actions`: 操作相关图标
  - `Media`: 媒体控制图标
  - `System`: 系统功能图标
  - `Status`: 状态指示图标

- **QWinUIIcons**: 自定义图标常量
  - `Solid`: 实心图标
  - `Outline`: 轮廓图标

### 枚举类型
```cpp
// 主题模式
enum QWinUIThemeMode {
    Light,   // 浅色主题
    Dark,    // 深色主题
    System   // 跟随系统
};

// 圆角半径
enum QWinUICornerRadius {
    None = 0,     // 无圆角
    Small = 2,    // 小圆角
    Medium = 4,   // 中等圆角
    Large = 8     // 大圆角
};

// 阴影深度
enum QWinUIShadowDepth {
    None,     // 无阴影
    Light,    // 浅阴影
    Medium,   // 中等阴影
    Deep      // 深阴影
};
```

## 设计原则

### Fluent Design 核心要素

1. **Light (光感)**
   - 智能高光效果
   - 动态光源模拟
   - 材质质感表现

2. **Depth (深度)**
   - 多层次阴影系统
   - Z轴空间感知
   - 层级关系清晰

3. **Motion (动效)**
   - 流畅的过渡动画
   - 有意义的运动轨迹
   - 符合物理规律的缓动

4. **Material (材质)**
   - 亚克力透明效果
   - 真实材质模拟
   - 环境感知适应

5. **Scale (缩放)**
   - 响应式设计
   - 多设备适配
   - 灵活的布局系统

### 可访问性支持

- **键盘导航**: 完整的键盘操作支持
- **屏幕阅读器**: 兼容 NVDA、JAWS 等
- **高对比度**: 自动适配高对比度主题
- **缩放支持**: 支持 100%-500% 界面缩放
- **色彩无障碍**: 符合 WCAG 2.1 AA 标准

## 扩展性

### 自定义主题
```cpp
// 创建自定义主题
class CustomTheme : public QWinUITheme
{
public:
    QColor getCustomColor() const override
    {
        return isDarkMode() ? QColor(64, 64, 64) : QColor(240, 240, 240);
    }
};
```

### 自定义控件
```cpp
// 继承基类创建自定义控件
class CustomWidget : public QWinUIWidget
{
    Q_OBJECT

public:
    CustomWidget(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    void updateTheme();
};
```

### 插件系统
- **图标插件**: 支持加载第三方图标库
- **主题插件**: 支持自定义主题包
- **控件插件**: 支持扩展控件库

## 国际化支持

- **多语言**: 支持 UTF-8 编码的所有语言
- **RTL 布局**: 支持从右到左的文字布局
- **本地化**: 内置字符串支持多语言切换
- **字体回退**: 自动选择合适的字体

## 开发工具

### 设计工具集成
- **Qt Designer**: 提供自定义控件插件
- **Qt Creator**: 完整的代码补全和调试支持
- **Visual Studio**: 通过 Qt VS Tools 集成

### 调试工具
- **主题检查器**: 实时查看主题状态
- **性能分析器**: 监控渲染性能
- **布局调试器**: 可视化布局结构

## 社区与支持

### 开源社区
- **GitHub**: 源代码托管和问题跟踪
- **文档**: 完整的 API 文档和教程
- **示例**: 丰富的示例代码和最佳实践

### 商业支持
- **技术咨询**: 专业的技术支持服务
- **定制开发**: 企业级定制解决方案
- **培训服务**: 开发团队培训课程

---

*QWinUI - 让 Qt 应用拥有现代化的 Windows 11 界面体验*

**版权所有 © 2024 QWinUI 开发团队**
