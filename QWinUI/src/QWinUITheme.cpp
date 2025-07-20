#include "QWinUI/QWinUITheme.h"
#include "QWinUI/QWinUIWidget.h"
#include <QApplication>
#include <QStyleHints>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QPalette>

#ifdef Q_OS_WIN
#include <Windows.h>
#ifndef __MINGW32__
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#endif
#endif

QT_BEGIN_NAMESPACE

// 静态成员初始化
QWinUITheme* QWinUITheme::s_instance = nullptr;

// 颜色名称常量定义
const QString QWinUITheme::Colors::SystemAccentColor = "SystemAccentColor";
const QString QWinUITheme::Colors::SystemAccentColorLight1 = "SystemAccentColorLight1";
const QString QWinUITheme::Colors::SystemAccentColorLight2 = "SystemAccentColorLight2";
const QString QWinUITheme::Colors::SystemAccentColorLight3 = "SystemAccentColorLight3";
const QString QWinUITheme::Colors::SystemAccentColorDark1 = "SystemAccentColorDark1";
const QString QWinUITheme::Colors::SystemAccentColorDark2 = "SystemAccentColorDark2";
const QString QWinUITheme::Colors::SystemAccentColorDark3 = "SystemAccentColorDark3";

const QString QWinUITheme::Colors::ControlFillColorDefault = "ControlFillColorDefault";
const QString QWinUITheme::Colors::ControlFillColorSecondary = "ControlFillColorSecondary";
const QString QWinUITheme::Colors::ControlFillColorTertiary = "ControlFillColorTertiary";
const QString QWinUITheme::Colors::ControlFillColorDisabled = "ControlFillColorDisabled";
const QString QWinUITheme::Colors::ControlFillColorTransparent = "ControlFillColorTransparent";
const QString QWinUITheme::Colors::ControlFillColorInputActive = "ControlFillColorInputActive";

const QString QWinUITheme::Colors::ControlStrokeColorDefault = "ControlStrokeColorDefault";
const QString QWinUITheme::Colors::ControlStrokeColorSecondary = "ControlStrokeColorSecondary";
const QString QWinUITheme::Colors::ControlStrokeColorDisabled = "ControlStrokeColorDisabled";

const QString QWinUITheme::Colors::TextFillColorPrimary = "TextFillColorPrimary";
const QString QWinUITheme::Colors::TextFillColorSecondary = "TextFillColorSecondary";
const QString QWinUITheme::Colors::TextFillColorTertiary = "TextFillColorTertiary";
const QString QWinUITheme::Colors::TextFillColorDisabled = "TextFillColorDisabled";

const QString QWinUITheme::Colors::ApplicationPageBackgroundThemeBrush = "ApplicationPageBackgroundThemeBrush";
const QString QWinUITheme::Colors::LayerFillColorDefault = "LayerFillColorDefault";

// 字体名称常量定义
const QString QWinUITheme::Fonts::CaptionTextBlockStyle = "CaptionTextBlockStyle";
const QString QWinUITheme::Fonts::BodyTextBlockStyle = "BodyTextBlockStyle";
const QString QWinUITheme::Fonts::BodyStrongTextBlockStyle = "BodyStrongTextBlockStyle";
const QString QWinUITheme::Fonts::SubtitleTextBlockStyle = "SubtitleTextBlockStyle";
const QString QWinUITheme::Fonts::TitleTextBlockStyle = "TitleTextBlockStyle";
const QString QWinUITheme::Fonts::TitleLargeTextBlockStyle = "TitleLargeTextBlockStyle";
const QString QWinUITheme::Fonts::DisplayTextBlockStyle = "DisplayTextBlockStyle";

// 间距名称常量定义
const QString QWinUITheme::Spacing::XXSmall = "XXSmall";
const QString QWinUITheme::Spacing::XSmall = "XSmall";
const QString QWinUITheme::Spacing::Small = "Small";
const QString QWinUITheme::Spacing::Medium = "Medium";
const QString QWinUITheme::Spacing::Large = "Large";
const QString QWinUITheme::Spacing::XLarge = "XLarge";
const QString QWinUITheme::Spacing::XXLarge = "XXLarge";

QWinUITheme* QWinUITheme::getInstance()
{
    if (!s_instance) {
        s_instance = new QWinUITheme();
    }
    return s_instance;
}

void QWinUITheme::destroyInstance()
{
    if (s_instance) {
        delete s_instance;
        s_instance = nullptr;
    }
}

QWinUITheme::QWinUITheme(QObject* parent)
    : QObject(parent)
    , m_themeMode(QWinUIThemeMode::Auto)
    , m_accentColor(QColor(0, 120, 215)) // Windows 11默认蓝色
    , m_followSystemTheme(true)
    , m_isDarkMode(false)
    , m_settings(nullptr)
    , m_themeTransitionEnabled(true)
    , m_themeTransitionMode(0) // 0 = RippleTransition
{
    initializeTheme();
}

QWinUITheme::~QWinUITheme()
{
    disconnectFromSystemTheme();
    if (m_settings) {
        delete m_settings;
    }
}

void QWinUITheme::initializeTheme()
{
    // 创建设置对象
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir().mkpath(configPath);
    m_settings = new QSettings(configPath + "/qwinui_theme.ini", QSettings::IniFormat);

    // 初始化字体
    QFont defaultFont("Segoe UI Variable", 14);
    m_fonts[Fonts::BodyTextBlockStyle] = defaultFont;
    
    QFont captionFont = defaultFont;
    captionFont.setPointSize(12);
    m_fonts[Fonts::CaptionTextBlockStyle] = captionFont;
    
    QFont strongFont = defaultFont;
    strongFont.setWeight(QFont::DemiBold);
    m_fonts[Fonts::BodyStrongTextBlockStyle] = strongFont;
    
    QFont subtitleFont = defaultFont;
    subtitleFont.setPointSize(20);
    m_fonts[Fonts::SubtitleTextBlockStyle] = subtitleFont;
    
    QFont titleFont = defaultFont;
    titleFont.setPointSize(28);
    titleFont.setWeight(QFont::DemiBold);
    m_fonts[Fonts::TitleTextBlockStyle] = titleFont;
    
    QFont titleLargeFont = defaultFont;
    titleLargeFont.setPointSize(40);
    titleLargeFont.setWeight(QFont::DemiBold);
    m_fonts[Fonts::TitleLargeTextBlockStyle] = titleLargeFont;
    
    QFont displayFont = defaultFont;
    displayFont.setPointSize(68);
    displayFont.setWeight(QFont::DemiBold);
    m_fonts[Fonts::DisplayTextBlockStyle] = displayFont;

    // 初始化间距
    m_spacing[Spacing::XXSmall] = 2;
    m_spacing[Spacing::XSmall] = 4;
    m_spacing[Spacing::Small] = 8;
    m_spacing[Spacing::Medium] = 12;
    m_spacing[Spacing::Large] = 16;
    m_spacing[Spacing::XLarge] = 24;
    m_spacing[Spacing::XXLarge] = 32;

    // 获取系统强调色
    m_accentColor = getSystemAccentColor();

    // 检测系统主题
    if (m_followSystemTheme) {
        connectToSystemTheme();
        onSystemThemeChanged();
    } else {
        // 根据当前主题模式加载颜色
        switch (m_themeMode) {
        case QWinUIThemeMode::Light:
            m_isDarkMode = false;
            loadLightTheme();
            break;
        case QWinUIThemeMode::Dark:
            m_isDarkMode = true;
            loadDarkTheme();
            break;
        case QWinUIThemeMode::Auto:
            onSystemThemeChanged();
            break;
        }
    }

    updateAccentColorVariants();
}

void QWinUITheme::loadLightTheme()
{
    // WinUI 3 浅色主题颜色
    m_colors[Colors::ControlFillColorDefault] = QColor(255, 255, 255, 179);  // rgba(255,255,255,0.7)
    m_colors[Colors::ControlFillColorSecondary] = QColor(249, 249, 249, 128); // rgba(249,249,249,0.5)
    m_colors[Colors::ControlFillColorTertiary] = QColor(249, 249, 249, 77);   // rgba(249,249,249,0.3)
    m_colors[Colors::ControlFillColorDisabled] = QColor(249, 249, 249, 77);   // rgba(249,249,249,0.3)
    m_colors[Colors::ControlFillColorTransparent] = QColor(255, 255, 255, 0);
    m_colors[Colors::ControlFillColorInputActive] = QColor(255, 255, 255);

    m_colors[Colors::ControlStrokeColorDefault] = QColor(117, 117, 117, 102); // rgba(117,117,117,0.4)
    m_colors[Colors::ControlStrokeColorSecondary] = QColor(117, 117, 117, 64); // rgba(117,117,117,0.25)
    m_colors[Colors::ControlStrokeColorDisabled] = QColor(117, 117, 117, 51);  // rgba(117,117,117,0.2)

    m_colors[Colors::TextFillColorPrimary] = QColor(14, 14, 14, 230);    // rgba(14,14,14,0.9)
    m_colors[Colors::TextFillColorSecondary] = QColor(96, 96, 96, 160);  // rgba(96,96,96,0.63)
    m_colors[Colors::TextFillColorTertiary] = QColor(96, 96, 96, 115);   // rgba(96,96,96,0.45)
    m_colors[Colors::TextFillColorDisabled] = QColor(96, 96, 96, 92);    // rgba(96,96,96,0.36)

    m_colors[Colors::ApplicationPageBackgroundThemeBrush] = QColor(243, 243, 243);
    m_colors[Colors::LayerFillColorDefault] = QColor(255, 255, 255, 128); // rgba(255,255,255,0.5)
}

void QWinUITheme::loadDarkTheme()
{
    // WinUI 3 深色主题颜色
    m_colors[Colors::ControlFillColorDefault] = QColor(255, 255, 255, 15);   // rgba(255,255,255,0.06)
    m_colors[Colors::ControlFillColorSecondary] = QColor(255, 255, 255, 23);  // rgba(255,255,255,0.09)
    m_colors[Colors::ControlFillColorTertiary] = QColor(255, 255, 255, 13);   // rgba(255,255,255,0.05)
    m_colors[Colors::ControlFillColorDisabled] = QColor(255, 255, 255, 10);   // rgba(255,255,255,0.04)
    m_colors[Colors::ControlFillColorTransparent] = QColor(255, 255, 255, 0);
    m_colors[Colors::ControlFillColorInputActive] = QColor(30, 30, 30);

    m_colors[Colors::ControlStrokeColorDefault] = QColor(255, 255, 255, 18);  // rgba(255,255,255,0.07)
    m_colors[Colors::ControlStrokeColorSecondary] = QColor(255, 255, 255, 26); // rgba(255,255,255,0.1)
    m_colors[Colors::ControlStrokeColorDisabled] = QColor(255, 255, 255, 13);  // rgba(255,255,255,0.05)

    m_colors[Colors::TextFillColorPrimary] = QColor(255, 255, 255);
    m_colors[Colors::TextFillColorSecondary] = QColor(255, 255, 255, 194);   // rgba(255,255,255,0.76)
    m_colors[Colors::TextFillColorTertiary] = QColor(255, 255, 255, 140);    // rgba(255,255,255,0.55)
    m_colors[Colors::TextFillColorDisabled] = QColor(255, 255, 255, 92);     // rgba(255,255,255,0.36)

    m_colors[Colors::ApplicationPageBackgroundThemeBrush] = QColor(32, 32, 32);
    m_colors[Colors::LayerFillColorDefault] = QColor(58, 58, 58, 77); // rgba(58,58,58,0.3)
}

QWinUIThemeMode QWinUITheme::themeMode() const
{
    return m_themeMode;
}

void QWinUITheme::setThemeMode(QWinUIThemeMode mode)
{
    if (m_themeMode != mode) {
        // 在改变主题之前，先启动动画
        if (m_themeTransitionEnabled) {
            startThemeTransitionForAllWidgets();
        }

        m_themeMode = mode;

        if (mode == QWinUIThemeMode::Auto) {
            if (!m_followSystemTheme) {
                m_followSystemTheme = true;
                connectToSystemTheme();
            }
            onSystemThemeChanged();
        } else {
            if (m_followSystemTheme) {
                m_followSystemTheme = false;
                disconnectFromSystemTheme();
            }

            bool newDarkMode = (mode == QWinUIThemeMode::Dark);
            if (m_isDarkMode != newDarkMode) {
                m_isDarkMode = newDarkMode;
                if (m_isDarkMode) {
                    loadDarkTheme();
                } else {
                    loadLightTheme();
                }
                updateAccentColorVariants();

                // 如果没有启用动画，直接发送主题改变信号
                if (!m_themeTransitionEnabled) {
                    emit themeChanged();
                }
                // 如果启用了动画，themeChanged 信号会在动画完成后发送
            }
        }

        emit themeModeChanged(mode);
    }
}

QColor QWinUITheme::accentColor() const
{
    return m_accentColor;
}

void QWinUITheme::setAccentColor(const QColor& color)
{
    if (m_accentColor != color) {
        m_accentColor = color;
        updateAccentColorVariants();
        emit accentColorChanged(color);
        emit themeChanged();
    }
}

bool QWinUITheme::followSystemTheme() const
{
    return m_followSystemTheme;
}

void QWinUITheme::setFollowSystemTheme(bool follow)
{
    if (m_followSystemTheme != follow) {
        m_followSystemTheme = follow;

        if (follow) {
            connectToSystemTheme();
            onSystemThemeChanged();
        } else {
            disconnectFromSystemTheme();
        }

        emit followSystemThemeChanged(follow);
    }
}

bool QWinUITheme::isThemeTransitionEnabled() const
{
    return m_themeTransitionEnabled;
}

void QWinUITheme::setThemeTransitionEnabled(bool enabled)
{
    m_themeTransitionEnabled = enabled;
}

int QWinUITheme::themeTransitionMode() const
{
    return m_themeTransitionMode;
}

void QWinUITheme::setThemeTransitionMode(int mode)
{
    m_themeTransitionMode = mode;
}

void QWinUITheme::setThemeModeWithTransition(QWinUIThemeMode mode, const QPoint& rippleCenter)
{
    if (m_themeMode == mode) {
        return; // 主题模式没有变化，无需切换
    }

    // 如果启用了动画，触发所有 QWinUIWidget 的主题切换动画
    if (m_themeTransitionEnabled) {
        // 获取所有顶级 QWinUIWidget
        QList<QWidget*> topLevelWidgets = QApplication::topLevelWidgets();
        for (QWidget* widget : topLevelWidgets) {
            QWinUIWidget* winuiWidget = qobject_cast<QWinUIWidget*>(widget);
            if (winuiWidget) {
                // 计算相对于每个窗口的涟漪中心
                QPoint localRippleCenter = rippleCenter;
                if (!rippleCenter.isNull()) {
                    // 如果提供了全局坐标，转换为窗口本地坐标
                    localRippleCenter = winuiWidget->mapFromGlobal(rippleCenter);
                    // 如果转换后的坐标在窗口外，使用窗口中心
                    if (!winuiWidget->rect().contains(localRippleCenter)) {
                        localRippleCenter = winuiWidget->rect().center();
                    }
                } else {
                    // 如果没有提供坐标，使用窗口中心
                    localRippleCenter = winuiWidget->rect().center();
                }

                // 启动主题切换动画
                winuiWidget->startThemeTransition(static_cast<QWinUIWidget::QWinUITransitionMode>(m_themeTransitionMode), localRippleCenter);
            }
        }
    }

    // 设置新的主题模式
    setThemeMode(mode);
}

void QWinUITheme::startThemeTransitionForAllWidgets()
{
    // 获取所有顶级 QWinUIWidget
    QList<QWidget*> topLevelWidgets = QApplication::topLevelWidgets();
    for (QWidget* widget : topLevelWidgets) {
        QWinUIWidget* winuiWidget = qobject_cast<QWinUIWidget*>(widget);
        if (winuiWidget) {
            // 获取智能的涟漪中心位置
            QPoint rippleCenter;

            // 尝试获取当前鼠标位置
            QPoint globalMousePos = QCursor::pos();
            QPoint localMousePos = winuiWidget->mapFromGlobal(globalMousePos);

            // 如果鼠标在当前控件内，使用鼠标位置
            if (winuiWidget->rect().contains(localMousePos)) {
                rippleCenter = localMousePos;
            } else {
                // 否则使用控件中心
                rippleCenter = winuiWidget->rect().center();
            }

            // 启动主题切换动画
            winuiWidget->startThemeTransition(static_cast<QWinUIWidget::QWinUITransitionMode>(m_themeTransitionMode), rippleCenter);
        }
    }
}

bool QWinUITheme::isDarkMode() const
{
    return m_isDarkMode;
}

QColor QWinUITheme::getColor(const QString& colorName) const
{
    // 如果颜色映射为空，先加载默认主题
    if (m_colors.isEmpty()) {
        const_cast<QWinUITheme*>(this)->loadLightTheme();
    }

    QColor color = m_colors.value(colorName);
    if (!color.isValid()) {
        // 如果找不到颜色，返回白色而不是洋红色
        return QColor(255, 255, 255);
    }
    return color;
}

void QWinUITheme::setColor(const QString& colorName, const QColor& color)
{
    if (m_colors.value(colorName) != color) {
        m_colors[colorName] = color;
        emit colorChanged(colorName, color);
        emit themeChanged();
    }
}

QFont QWinUITheme::getFont(const QString& fontName) const
{
    return m_fonts.value(fontName, QFont());
}

void QWinUITheme::setFont(const QString& fontName, const QFont& font)
{
    m_fonts[fontName] = font;
}

int QWinUITheme::getSpacing(const QString& spacingName) const
{
    return m_spacing.value(spacingName, 8);
}

void QWinUITheme::setSpacing(const QString& spacingName, int spacing)
{
    m_spacing[spacingName] = spacing;
}

QColor QWinUITheme::getSystemAccentColor() const
{
#ifdef Q_OS_WIN
#ifndef __MINGW32__
    // Windows 10/11 系统强调色获取（仅在MSVC下）
    DWORD accentColor = 0;
    BOOL opaque = FALSE;

    HRESULT hr = DwmGetColorizationColor(&accentColor, &opaque);
    if (SUCCEEDED(hr)) {
        int r = (accentColor >> 16) & 0xFF;
        int g = (accentColor >> 8) & 0xFF;
        int b = accentColor & 0xFF;
        return QColor(r, g, b);
    }
#endif
#endif

    // 默认返回Windows 11蓝色
    return QColor(0, 120, 215);
}

void QWinUITheme::updateAccentColorVariants()
{
    m_colors[Colors::SystemAccentColor] = m_accentColor;
    m_colors[Colors::SystemAccentColorLight1] = generateAccentVariant(m_accentColor, 1.2);
    m_colors[Colors::SystemAccentColorLight2] = generateAccentVariant(m_accentColor, 1.4);
    m_colors[Colors::SystemAccentColorLight3] = generateAccentVariant(m_accentColor, 1.6);
    m_colors[Colors::SystemAccentColorDark1] = generateAccentVariant(m_accentColor, 0.8);
    m_colors[Colors::SystemAccentColorDark2] = generateAccentVariant(m_accentColor, 0.6);
    m_colors[Colors::SystemAccentColorDark3] = generateAccentVariant(m_accentColor, 0.4);
}

QColor QWinUITheme::generateAccentVariant(const QColor& baseColor, double factor) const
{
    int r = qBound(0, static_cast<int>(baseColor.red() * factor), 255);
    int g = qBound(0, static_cast<int>(baseColor.green() * factor), 255);
    int b = qBound(0, static_cast<int>(baseColor.blue() * factor), 255);
    return QColor(r, g, b, baseColor.alpha());
}

void QWinUITheme::connectToSystemTheme()
{
    // Qt 6.4版本兼容性 - 简化实现
    // 在Qt 6.5+中才有colorSchemeChanged信号
    // 这里我们暂时不连接系统主题变化信号
}

void QWinUITheme::disconnectFromSystemTheme()
{
    // Qt 6.4版本兼容性 - 简化实现
}

void QWinUITheme::onSystemThemeChanged()
{
    // Qt 6.4版本兼容性 - 简化实现
    // 暂时使用简单的检测方法
    bool newDarkMode = false;

    // 简单的系统主题检测（基于调色板）
    if (QApplication::instance()) {
        QPalette palette = QApplication::palette();
        QColor windowColor = palette.color(QPalette::Window);
        // 如果窗口背景色比较暗，认为是深色主题
        newDarkMode = (windowColor.lightness() < 128);
    }

    if (m_isDarkMode != newDarkMode) {
        m_isDarkMode = newDarkMode;

        if (m_isDarkMode) {
            loadDarkTheme();
        } else {
            loadLightTheme();
        }

        updateAccentColorVariants();
        emit themeChanged();
    }
}

void QWinUITheme::saveTheme(const QString& filePath) const
{
    // 实现主题保存功能
    Q_UNUSED(filePath)
}

void QWinUITheme::loadTheme(const QString& filePath)
{
    // 实现主题加载功能
    Q_UNUSED(filePath)
}

void QWinUITheme::resetToDefault()
{
    m_accentColor = getSystemAccentColor();
    setThemeMode(QWinUIThemeMode::Auto);
    setFollowSystemTheme(true);
}

QT_END_NAMESPACE
