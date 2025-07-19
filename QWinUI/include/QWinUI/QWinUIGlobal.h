#ifndef QWINUIGLOBAL_H
#define QWINUIGLOBAL_H

#include <QtCore/qglobal.h>

QT_BEGIN_NAMESPACE

#ifndef QT_STATIC
#  if defined(QT_BUILD_QWINUI_LIB)
#    define QWINUI_EXPORT Q_DECL_EXPORT
#  else
#    define QWINUI_EXPORT Q_DECL_IMPORT
#  endif
#else
#  define QWINUI_EXPORT
#endif

// QWinUI版本信息
#define QWINUI_VERSION_MAJOR 1
#define QWINUI_VERSION_MINOR 0
#define QWINUI_VERSION_PATCH 0
#define QWINUI_VERSION_STR "1.0.0"

// 主题模式枚举
enum class QWinUIThemeMode {
    Light,      // 浅色主题
    Dark,       // 深色主题
    Auto        // 跟随系统
};

// 控件状态枚举
enum class QWinUIControlState {
    Normal,     // 正常状态
    Hovered,    // 悬停状态
    Pressed,    // 按下状态
    Disabled,   // 禁用状态
    Focused     // 焦点状态
};

// 动画类型枚举
enum class QWinUIAnimationType {
    FadeIn,     // 淡入
    FadeOut,    // 淡出
    SlideIn,    // 滑入
    SlideOut,   // 滑出
    ScaleIn,    // 缩放进入
    ScaleOut    // 缩放退出
};

// 圆角大小枚举
enum class QWinUICornerRadius {
    None = 0,   // 无圆角
    Small = 2,  // 小圆角
    Medium = 4, // 中圆角
    Large = 8,  // 大圆角
    XLarge = 16 // 超大圆角
};

// 阴影深度枚举
enum class QWinUIShadowDepth {
    None = 0,   // 无阴影
    Shallow,    // 浅阴影
    Medium,     // 中阴影
    Deep,       // 深阴影
    VeryDeep    // 超深阴影
};

// 导航视图显示模式
enum class QWinUINavigationViewDisplayMode {
    Minimal,    // 最小模式
    Compact,    // 紧凑模式
    Expanded    // 展开模式
};

// 信息栏严重级别
enum class QWinUIInfoBarSeverity {
    Informational,  // 信息
    Success,        // 成功
    Warning,        // 警告
    Error          // 错误
};

QT_END_NAMESPACE

#endif // QWINUIGLOBAL_H
