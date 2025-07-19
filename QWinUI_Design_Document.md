# QWinUI - Qt WinUI3风格组件库设计文档

## 概述

QWinUI是一个基于Qt的自定义组件库，严格遵循Microsoft WinUI3和Fluent Design System的设计规范，为Qt应用程序提供现代化的Win11风格用户界面组件。

## 设计原则

- **Fluent Design System**: 遵循Microsoft的设计语言
- **Win11风格**: 现代化的圆角、阴影、透明效果
- **响应式设计**: 支持不同屏幕尺寸和DPI
- **主题支持**: 支持浅色/深色主题切换
- **无障碍访问**: 符合无障碍设计标准

## 核心组件架构

### 基础类层次结构

```mermaid
classDiagram
    class QWinUIWidget {
        <<abstract>>
        #QWinUITheme* theme
        #QWinUIStyle* style
        #bool isDarkMode
        #int cornerRadius
        #QColor accentColor
        +setTheme(QWinUITheme*)
        +setCornerRadius(int)
        +setAccentColor(QColor)
        +paintEvent(QPaintEvent*)
        #drawBackground(QPainter*)
        #drawBorder(QPainter*)
        #applyFluentEffects(QPainter*)
    }

    class QWinUIButton {
        -ButtonState state
        -QString text
        -QIcon icon
        -bool isDefault
        +setText(QString)
        +setIcon(QIcon)
        +setDefault(bool)
        +clicked()
        #paintEvent(QPaintEvent*)
        #mousePressEvent(QMouseEvent*)
        #mouseReleaseEvent(QMouseEvent*)
        #enterEvent(QEnterEvent*)
        #leaveEvent(QEvent*)
    }

    class QWinUITextBox {
        -QString placeholderText
        -bool isPassword
        -QValidator* validator
        -bool hasError
        +setPlaceholderText(QString)
        +setPassword(bool)
        +setValidator(QValidator*)
        +setError(bool)
        +textChanged(QString)
        #paintEvent(QPaintEvent*)
        #focusInEvent(QFocusEvent*)
        #focusOutEvent(QFocusEvent*)
    }

    class QWinUICheckBox {
        -bool isChecked
        -bool isIndeterminate
        -QString text
        +setChecked(bool)
        +setIndeterminate(bool)
        +setText(QString)
        +stateChanged(int)
        #paintEvent(QPaintEvent*)
        #mousePressEvent(QMouseEvent*)
    }

    QWinUIWidget <|-- QWinUIButton
    QWinUIWidget <|-- QWinUITextBox
    QWinUIWidget <|-- QWinUICheckBox
```

### 导航组件

```mermaid
classDiagram
    class QWinUINavigationView {
        -QList~QWinUINavigationItem*~ items
        -QWinUINavigationItem* selectedItem
        -bool isPaneOpen
        -NavigationViewDisplayMode displayMode
        +addItem(QWinUINavigationItem*)
        +removeItem(QWinUINavigationItem*)
        +setSelectedItem(QWinUINavigationItem*)
        +setPaneOpen(bool)
        +setDisplayMode(NavigationViewDisplayMode)
        +selectionChanged(QWinUINavigationItem*)
        #paintEvent(QPaintEvent*)
        #resizeEvent(QResizeEvent*)
    }

    class QWinUINavigationItem {
        -QString text
        -QIcon icon
        -QWidget* content
        -QList~QWinUINavigationItem*~ children
        -bool isExpanded
        +setText(QString)
        +setIcon(QIcon)
        +setContent(QWidget*)
        +addChild(QWinUINavigationItem*)
        +setExpanded(bool)
        +clicked()
    }

    class QWinUITabView {
        -QList~QWinUITabItem*~ tabs
        -QWinUITabItem* selectedTab
        -bool canCloseTabs
        -bool canReorderTabs
        +addTab(QWinUITabItem*)
        +removeTab(QWinUITabItem*)
        +setSelectedTab(QWinUITabItem*)
        +setCanCloseTabs(bool)
        +setCanReorderTabs(bool)
        +tabSelected(QWinUITabItem*)
        +tabClosed(QWinUITabItem*)
        #paintEvent(QPaintEvent*)
        #mousePressEvent(QMouseEvent*)
    }

    class QWinUITabItem {
        -QString header
        -QIcon icon
        -QWidget* content
        -bool canClose
        +setHeader(QString)
        +setIcon(QIcon)
        +setContent(QWidget*)
        +setCanClose(bool)
        +closeRequested()
    }

    QWinUIWidget <|-- QWinUINavigationView
    QWinUIWidget <|-- QWinUITabView
    QWinUINavigationView --> QWinUINavigationItem
    QWinUITabView --> QWinUITabItem
```

### 数据展示组件

```mermaid
classDiagram
    class QWinUIListView {
        -QAbstractItemModel* model
        -QWinUIItemDelegate* delegate
        -SelectionMode selectionMode
        -bool isVirtualized
        +setModel(QAbstractItemModel*)
        +setDelegate(QWinUIItemDelegate*)
        +setSelectionMode(SelectionMode)
        +setVirtualized(bool)
        +itemClicked(QModelIndex)
        +selectionChanged(QItemSelection)
        #paintEvent(QPaintEvent*)
        #mousePressEvent(QMouseEvent*)
        #wheelEvent(QWheelEvent*)
    }

    class QWinUIGridView {
        -int itemWidth
        -int itemHeight
        -int spacing
        -Qt::Orientation orientation
        +setItemSize(QSize)
        +setSpacing(int)
        +setOrientation(Qt::Orientation)
        #paintEvent(QPaintEvent*)
        #resizeEvent(QResizeEvent*)
    }

    class QWinUITreeView {
        -bool showRootLines
        -bool showBranches
        -int indentation
        +setShowRootLines(bool)
        +setShowBranches(bool)
        +setIndentation(int)
        +itemExpanded(QModelIndex)
        +itemCollapsed(QModelIndex)
        #paintEvent(QPaintEvent*)
        #drawBranches(QPainter*, QRect, QModelIndex)
    }

    class QWinUIItemDelegate {
        <<abstract>>
        +paint(QPainter*, QStyleOptionViewItem, QModelIndex)
        +sizeHint(QStyleOptionViewItem, QModelIndex)
        +createEditor(QWidget*, QStyleOptionViewItem, QModelIndex)
        #drawBackground(QPainter*, QStyleOptionViewItem)
        #drawText(QPainter*, QStyleOptionViewItem, QString)
        #drawIcon(QPainter*, QStyleOptionViewItem, QIcon)
    }

    QWinUIWidget <|-- QWinUIListView
    QWinUIListView <|-- QWinUIGridView
    QWinUIListView <|-- QWinUITreeView
    QWinUIListView --> QWinUIItemDelegate
```

### 输入控件组件

```mermaid
classDiagram
    class QWinUISlider {
        -double minimum
        -double maximum
        -double value
        -double stepSize
        -Qt::Orientation orientation
        -bool showTicks
        -bool showLabels
        +setRange(double, double)
        +setValue(double)
        +setStepSize(double)
        +setOrientation(Qt::Orientation)
        +setShowTicks(bool)
        +setShowLabels(bool)
        +valueChanged(double)
        #paintEvent(QPaintEvent*)
        #mousePressEvent(QMouseEvent*)
        #mouseMoveEvent(QMouseEvent*)
        #drawTrack(QPainter*)
        #drawThumb(QPainter*)
        #drawTicks(QPainter*)
    }

    class QWinUIComboBox {
        -QStringList items
        -int currentIndex
        -QString currentText
        -bool isEditable
        -QCompleter* completer
        +addItem(QString)
        +addItems(QStringList)
        +setCurrentIndex(int)
        +setCurrentText(QString)
        +setEditable(bool)
        +setCompleter(QCompleter*)
        +currentIndexChanged(int)
        +currentTextChanged(QString)
        #paintEvent(QPaintEvent*)
        #mousePressEvent(QMouseEvent*)
        #showPopup()
        #hidePopup()
    }

    class QWinUIDatePicker {
        -QDate selectedDate
        -QDate minimumDate
        -QDate maximumDate
        -QString dateFormat
        +setSelectedDate(QDate)
        +setDateRange(QDate, QDate)
        +setDateFormat(QString)
        +dateChanged(QDate)
        #paintEvent(QPaintEvent*)
        #showCalendar()
    }

    class QWinUITimePicker {
        -QTime selectedTime
        -QString timeFormat
        -int minuteIncrement
        +setSelectedTime(QTime)
        +setTimeFormat(QString)
        +setMinuteIncrement(int)
        +timeChanged(QTime)
        #paintEvent(QPaintEvent*)
        #showTimePicker()
    }

    class QWinUIToggleSwitch {
        -bool isOn
        -QString onContent
        -QString offContent
        +setOn(bool)
        +setOnContent(QString)
        +setOffContent(QString)
        +toggled(bool)
        #paintEvent(QPaintEvent*)
        #mousePressEvent(QMouseEvent*)
        #animateToggle()
    }

    class QWinUIRadioButton {
        -bool isChecked
        -QString text
        -QButtonGroup* group
        +setChecked(bool)
        +setText(QString)
        +setGroup(QButtonGroup*)
        +toggled(bool)
        #paintEvent(QPaintEvent*)
        #mousePressEvent(QMouseEvent*)
    }

    QWinUIWidget <|-- QWinUISlider
    QWinUIWidget <|-- QWinUIComboBox
    QWinUIWidget <|-- QWinUIDatePicker
    QWinUIWidget <|-- QWinUITimePicker
    QWinUIWidget <|-- QWinUIToggleSwitch
    QWinUIWidget <|-- QWinUIRadioButton
```

### 进度和状态组件

```mermaid
classDiagram
    class QWinUIProgressBar {
        -double minimum
        -double maximum
        -double value
        -bool isIndeterminate
        -QString text
        -bool showText
        +setRange(double, double)
        +setValue(double)
        +setIndeterminate(bool)
        +setText(QString)
        +setShowText(bool)
        +valueChanged(double)
        #paintEvent(QPaintEvent*)
        #drawProgressBar(QPainter*)
        #drawIndeterminateAnimation(QPainter*)
    }

    class QWinUIProgressRing {
        -bool isActive
        -double strokeWidth
        -QColor ringColor
        +setActive(bool)
        +setStrokeWidth(double)
        +setRingColor(QColor)
        #paintEvent(QPaintEvent*)
        #drawRing(QPainter*)
        #updateAnimation()
    }

    class QWinUIInfoBar {
        -InfoBarSeverity severity
        -QString title
        -QString message
        -QIcon icon
        -bool isClosable
        -QList~QWinUIButton*~ actionButtons
        +setSeverity(InfoBarSeverity)
        +setTitle(QString)
        +setMessage(QString)
        +setIcon(QIcon)
        +setClosable(bool)
        +addActionButton(QWinUIButton*)
        +closed()
        #paintEvent(QPaintEvent*)
        #drawSeverityIcon(QPainter*)
    }

    class QWinUIRatingControl {
        -int maxRating
        -double currentRating
        -bool isReadOnly
        -QIcon filledIcon
        -QIcon emptyIcon
        +setMaxRating(int)
        +setCurrentRating(double)
        +setReadOnly(bool)
        +setIcons(QIcon, QIcon)
        +ratingChanged(double)
        #paintEvent(QPaintEvent*)
        #mousePressEvent(QMouseEvent*)
        #drawStars(QPainter*)
    }

    QWinUIWidget <|-- QWinUIProgressBar
    QWinUIWidget <|-- QWinUIProgressRing
    QWinUIWidget <|-- QWinUIInfoBar
    QWinUIWidget <|-- QWinUIRatingControl
```

### 容器和布局组件

```mermaid
classDiagram
    class QWinUIExpander {
        -QString header
        -QWidget* content
        -bool isExpanded
        -ExpandDirection direction
        +setHeader(QString)
        +setContent(QWidget*)
        +setExpanded(bool)
        +setExpandDirection(ExpandDirection)
        +expanded()
        +collapsed()
        #paintEvent(QPaintEvent*)
        #mousePressEvent(QMouseEvent*)
        #animateExpansion()
    }

    class QWinUICard {
        -QWidget* content
        -QString title
        -QString subtitle
        -QIcon icon
        -int elevation
        +setContent(QWidget*)
        +setTitle(QString)
        +setSubtitle(QString)
        +setIcon(QIcon)
        +setElevation(int)
        #paintEvent(QPaintEvent*)
        #drawShadow(QPainter*)
        #drawCard(QPainter*)
    }

    class QWinUIScrollViewer {
        -QWidget* content
        -Qt::ScrollBarPolicy horizontalPolicy
        -Qt::ScrollBarPolicy verticalPolicy
        -bool smoothScrolling
        -double zoomFactor
        +setContent(QWidget*)
        +setScrollBarPolicy(Qt::Orientation, Qt::ScrollBarPolicy)
        +setSmoothScrolling(bool)
        +setZoomFactor(double)
        +scrollToTop()
        +scrollToBottom()
        #paintEvent(QPaintEvent*)
        #wheelEvent(QWheelEvent*)
        #drawScrollBars(QPainter*)
    }

    class QWinUISplitView {
        -QWidget* paneContent
        -QWidget* mainContent
        -double paneWidth
        -bool isPaneOpen
        -SplitViewDisplayMode displayMode
        +setPaneContent(QWidget*)
        +setMainContent(QWidget*)
        +setPaneWidth(double)
        +setPaneOpen(bool)
        +setDisplayMode(SplitViewDisplayMode)
        +paneOpened()
        +paneClosed()
        #paintEvent(QPaintEvent*)
        #resizeEvent(QResizeEvent*)
    }

    QWinUIWidget <|-- QWinUIExpander
    QWinUIWidget <|-- QWinUICard
    QWinUIWidget <|-- QWinUIScrollViewer
    QWinUIWidget <|-- QWinUISplitView
```

### 对话框和弹出组件

```mermaid
classDiagram
    class QWinUIContentDialog {
        -QString title
        -QWidget* content
        -QString primaryButtonText
        -QString secondaryButtonText
        -QString closeButtonText
        -DialogResult result
        +setTitle(QString)
        +setContent(QWidget*)
        +setPrimaryButtonText(QString)
        +setSecondaryButtonText(QString)
        +setCloseButtonText(QString)
        +showAsync()
        +primaryButtonClicked()
        +secondaryButtonClicked()
        +closeButtonClicked()
        #paintEvent(QPaintEvent*)
        #keyPressEvent(QKeyEvent*)
    }

    class QWinUIFlyout {
        -QWidget* content
        -QWidget* target
        -FlyoutPlacement placement
        -bool isLightDismissEnabled
        +setContent(QWidget*)
        +setTarget(QWidget*)
        +setPlacement(FlyoutPlacement)
        +setLightDismissEnabled(bool)
        +show()
        +hide()
        +opened()
        +closed()
        #paintEvent(QPaintEvent*)
        #calculatePosition()
    }

    class QWinUITeachingTip {
        -QString title
        -QString subtitle
        -QIcon icon
        -QWidget* target
        -TeachingTipPlacement placement
        -bool isOpen
        +setTitle(QString)
        +setSubtitle(QString)
        +setIcon(QIcon)
        +setTarget(QWidget*)
        +setPlacement(TeachingTipPlacement)
        +show()
        +hide()
        +closed()
        #paintEvent(QPaintEvent*)
        #drawTail(QPainter*)
    }

    class QWinUIToolTip {
        -QString text
        -QWidget* target
        -int showDelay
        -int hideDelay
        +setText(QString)
        +setTarget(QWidget*)
        +setShowDelay(int)
        +setHideDelay(int)
        +show()
        +hide()
        #paintEvent(QPaintEvent*)
        #positionNearTarget()
    }

    QWinUIWidget <|-- QWinUIContentDialog
    QWinUIWidget <|-- QWinUIFlyout
    QWinUIWidget <|-- QWinUITeachingTip
    QWinUIWidget <|-- QWinUIToolTip
```

### 主题和样式系统

```mermaid
classDiagram
    class QWinUITheme {
        <<singleton>>
        -ThemeMode currentMode
        -QColor accentColor
        -QMap~QString, QColor~ colorPalette
        -QMap~QString, QFont~ fontFamily
        -QMap~QString, int~ spacing
        +getInstance() QWinUITheme*
        +setThemeMode(ThemeMode)
        +setAccentColor(QColor)
        +getColor(QString) QColor
        +getFont(QString) QFont
        +getSpacing(QString) int
        +themeChanged()
        -loadLightTheme()
        -loadDarkTheme()
        -loadHighContrastTheme()
    }

    class QWinUIStyle {
        -QWinUITheme* theme
        -int cornerRadius
        -double shadowBlur
        -QColor shadowColor
        -double opacity
        +setTheme(QWinUITheme*)
        +setCornerRadius(int)
        +setShadow(double, QColor)
        +setOpacity(double)
        +drawPrimitive(PrimitiveElement, QStyleOption*, QPainter*, QWidget*)
        +drawControl(ControlElement, QStyleOption*, QPainter*, QWidget*)
        +drawComplexControl(ComplexControl, QStyleOptionComplex*, QPainter*, QWidget*)
        #drawFluentBackground(QPainter*, QRect)
        #drawFluentBorder(QPainter*, QRect)
        #applyAcrylicEffect(QPainter*, QRect)
    }

    class QWinUIAnimation {
        -QPropertyAnimation* animation
        -QEasingCurve easingCurve
        -int duration
        -bool isRunning
        +setDuration(int)
        +setEasingCurve(QEasingCurve)
        +start()
        +stop()
        +pause()
        +resume()
        +finished()
        #createFadeAnimation(QWidget*)
        #createSlideAnimation(QWidget*)
        #createScaleAnimation(QWidget*)
    }

    class QWinUIColorPalette {
        +static QColor SystemAccentColor
        +static QColor SystemAccentColorLight1
        +static QColor SystemAccentColorLight2
        +static QColor SystemAccentColorLight3
        +static QColor SystemAccentColorDark1
        +static QColor SystemAccentColorDark2
        +static QColor SystemAccentColorDark3
        +static QColor NeutralPrimary
        +static QColor NeutralSecondary
        +static QColor NeutralTertiary
        +static QColor NeutralQuaternary
        +static QColor NeutralLight
        +static QColor NeutralLighter
        +static QColor NeutralLightest
        +static QColor White
        +static QColor Black
        +getSystemAccentColor() QColor
        +updateFromSystem()
    }

    QWinUITheme --> QWinUIColorPalette
    QWinUIStyle --> QWinUITheme
    QWinUIWidget --> QWinUIStyle
    QWinUIWidget --> QWinUIAnimation
```

### 特殊效果组件

```mermaid
classDiagram
    class QWinUIAcrylicBrush {
        -double tintOpacity
        -QColor tintColor
        -double noiseOpacity
        -QString noiseTexture
        -AcrylicBackgroundSource backgroundSource
        +setTintOpacity(double)
        +setTintColor(QColor)
        +setNoiseOpacity(double)
        +setNoiseTexture(QString)
        +setBackgroundSource(AcrylicBackgroundSource)
        +paint(QPainter*, QRect)
        #generateNoiseTexture()
        #blurBackground(QPixmap)
    }

    class QWinUIRevealBrush {
        -QColor revealColor
        -double revealRadius
        -QPoint hotSpot
        -bool isPressed
        +setRevealColor(QColor)
        +setRevealRadius(double)
        +setHotSpot(QPoint)
        +setPressed(bool)
        +paint(QPainter*, QRect)
        #createRevealMask(QRect)
    }

    class QWinUIDropShadow {
        -double blurRadius
        -QColor shadowColor
        -QPointF offset
        -double opacity
        +setBlurRadius(double)
        +setShadowColor(QColor)
        +setOffset(QPointF)
        +setOpacity(double)
        +drawShadow(QPainter*, QRect)
        #createShadowPixmap(QSize)
    }

    class QWinUIParallaxView {
        -QWidget* source
        -double horizontalRatio
        -double verticalRatio
        -bool isHorizontalShiftClamped
        -bool isVerticalShiftClamped
        +setSource(QWidget*)
        +setHorizontalRatio(double)
        +setVerticalRatio(double)
        +setHorizontalShiftClamped(bool)
        +setVerticalShiftClamped(bool)
        #paintEvent(QPaintEvent*)
        #updateParallax()
    }

    QWinUIWidget --> QWinUIAcrylicBrush
    QWinUIWidget --> QWinUIRevealBrush
    QWinUIWidget --> QWinUIDropShadow
    QWinUIWidget <|-- QWinUIParallaxView
```

## 组件功能特性

### 基础控件特性

| 组件 | 主要特性 | WinUI3对应 |
|------|----------|------------|
| QWinUIButton | 圆角、悬停效果、点击动画、图标支持 | Button |
| QWinUITextBox | 浮动标签、错误状态、密码模式、验证器 | TextBox |
| QWinUICheckBox | 三态支持、动画切换、自定义样式 | CheckBox |
| QWinUIRadioButton | 分组管理、动画效果、自定义外观 | RadioButton |
| QWinUIToggleSwitch | 滑动动画、开关状态、内容标签 | ToggleSwitch |
| QWinUISlider | 平滑滑动、刻度显示、范围限制 | Slider |
| QWinUIComboBox | 下拉动画、搜索过滤、自动完成 | ComboBox |

### 导航控件特性

| 组件 | 主要特性 | WinUI3对应 |
|------|----------|------------|
| QWinUINavigationView | 自适应布局、面板切换、层级导航 | NavigationView |
| QWinUITabView | 标签页管理、拖拽重排、关闭按钮 | TabView |
| QWinUIBreadcrumbBar | 路径导航、点击跳转、溢出处理 | BreadcrumbBar |

### 数据展示特性

| 组件 | 主要特性 | WinUI3对应 |
|------|----------|------------|
| QWinUIListView | 虚拟化、选择模式、自定义项模板 | ListView |
| QWinUIGridView | 网格布局、响应式设计、项目大小调整 | GridView |
| QWinUITreeView | 展开折叠、层级显示、延迟加载 | TreeView |

### 容器和布局特性

| 组件 | 主要特性 | WinUI3对应 |
|------|----------|------------|
| QWinUIExpander | 展开动画、方向控制、内容容器 | Expander |
| QWinUICard | 阴影效果、圆角边框、内容承载 | 自定义Card |
| QWinUIScrollViewer | 平滑滚动、缩放支持、滚动条样式 | ScrollViewer |
| QWinUISplitView | 面板切换、响应式布局、显示模式 | SplitView |

### 反馈和状态特性

| 组件 | 主要特性 | WinUI3对应 |
|------|----------|------------|
| QWinUIProgressBar | 进度显示、不确定状态、文本标签 | ProgressBar |
| QWinUIProgressRing | 环形进度、动画效果、颜色自定义 | ProgressRing |
| QWinUIInfoBar | 严重级别、操作按钮、自动关闭 | InfoBar |
| QWinUIRatingControl | 星级评分、只读模式、自定义图标 | RatingControl |

### 对话框和弹出特性

| 组件 | 主要特性 | WinUI3对应 |
|------|----------|------------|
| QWinUIContentDialog | 模态对话框、按钮自定义、异步显示 | ContentDialog |
| QWinUIFlyout | 轻量弹出、位置控制、轻触关闭 | Flyout |
| QWinUITeachingTip | 教学提示、箭头指向、自动定位 | TeachingTip |
| QWinUIToolTip | 悬停提示、延迟显示、智能定位 | ToolTip |

## 设计规范

### 颜色系统

- **主色调**: 系统强调色（可自定义）
- **中性色**: 8级灰度色阶
- **语义色**: 成功、警告、错误、信息
- **透明度**: 支持Acrylic材质效果

### 字体系统

- **主字体**: Segoe UI Variable
- **备用字体**: 微软雅黑、苹方、Roboto
- **字号**: 12px-28px，支持缩放
- **字重**: Light、Regular、SemiBold、Bold

### 间距系统

- **基础单位**: 4px
- **常用间距**: 4px、8px、12px、16px、20px、24px、32px、40px
- **组件内边距**: 8px-16px
- **组件外边距**: 4px-24px

### 圆角系统

- **小圆角**: 2px（小按钮、输入框）
- **中圆角**: 4px（按钮、卡片）
- **大圆角**: 8px（面板、对话框）
- **超大圆角**: 16px（特殊容器）

### 阴影系统

- **浅阴影**: 0 1px 3px rgba(0,0,0,0.12)
- **中阴影**: 0 4px 12px rgba(0,0,0,0.15)
- **深阴影**: 0 8px 24px rgba(0,0,0,0.18)
- **超深阴影**: 0 16px 48px rgba(0,0,0,0.24)

## 实现要点

### 性能优化

1. **虚拟化**: 大数据列表使用虚拟化技术
2. **缓存**: 样式和绘制结果缓存
3. **异步**: 动画和复杂计算异步处理
4. **内存管理**: 智能资源释放和重用

### 无障碍支持

1. **键盘导航**: 完整的Tab键导航支持
2. **屏幕阅读器**: ARIA标签和语义化
3. **高对比度**: 高对比度主题支持
4. **缩放**: 支持系统DPI缩放

### 国际化支持

1. **RTL布局**: 从右到左语言支持
2. **字体回退**: 多语言字体支持
3. **文本方向**: 自动文本方向检测
4. **日期时间**: 本地化日期时间格式

### 主题切换

1. **实时切换**: 运行时主题切换
2. **系统同步**: 跟随系统主题变化
3. **自定义主题**: 支持用户自定义主题
4. **动画过渡**: 主题切换动画效果

## 开发计划

### 第一阶段：基础框架
- [ ] QWinUIWidget基类实现
- [ ] QWinUITheme主题系统
- [ ] QWinUIStyle样式引擎
- [ ] 基础动画框架

### 第二阶段：核心控件
- [ ] 按钮类控件（Button、ToggleButton等）
- [ ] 输入类控件（TextBox、ComboBox等）
- [ ] 选择类控件（CheckBox、RadioButton等）

### 第三阶段：高级控件
- [ ] 导航控件（NavigationView、TabView）
- [ ] 数据展示控件（ListView、GridView）
- [ ] 容器控件（Expander、Card等）

### 第四阶段：特效和优化
- [ ] Acrylic材质效果
- [ ] Reveal高亮效果
- [ ] 性能优化和测试
- [ ] 文档和示例完善

这个设计文档提供了完整的QWinUI组件库架构，严格遵循WinUI3的设计规范，包含了所有主要组件的UML类图和详细的实现规划。
