#ifndef QWINUITHEME_H
#define QWINUITHEME_H

#include "QWinUIGlobal.h"
#include <QObject>
#include <QColor>
#include <QFont>
#include <QMap>
#include <QSettings>
#include <QPoint>

QT_BEGIN_NAMESPACE

// 前向声明
class QWinUIWidget;

class QWINUI_EXPORT QWinUITheme : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QWinUIThemeMode themeMode READ themeMode WRITE setThemeMode NOTIFY themeModeChanged)
    Q_PROPERTY(QColor accentColor READ accentColor WRITE setAccentColor NOTIFY accentColorChanged)
    Q_PROPERTY(bool followSystemTheme READ followSystemTheme WRITE setFollowSystemTheme NOTIFY followSystemThemeChanged)

public:
    // 单例模式
    static QWinUITheme* getInstance();
    static void destroyInstance();

    // 主题模式
    QWinUIThemeMode themeMode() const;
    void setThemeMode(QWinUIThemeMode mode);

    // 强调色
    QColor accentColor() const;
    void setAccentColor(const QColor& color);

    // 系统主题跟随
    bool followSystemTheme() const;
    void setFollowSystemTheme(bool follow);

    // 当前是否为深色模式
    bool isDarkMode() const;

    // 主题切换动画控制
    bool isThemeTransitionEnabled() const;
    void setThemeTransitionEnabled(bool enabled);

    int themeTransitionMode() const;
    void setThemeTransitionMode(int mode);

    // 带动画的主题切换
    void setThemeModeWithTransition(QWinUIThemeMode mode, const QPoint& rippleCenter = QPoint());

    // 颜色获取
    QColor getColor(const QString& colorName) const;
    void setColor(const QString& colorName, const QColor& color);

    // 字体获取
    QFont getFont(const QString& fontName) const;
    void setFont(const QString& fontName, const QFont& font);

    // 间距获取
    int getSpacing(const QString& spacingName) const;
    void setSpacing(const QString& spacingName, int spacing);

    // 预定义颜色
    struct Colors {
        // 系统强调色
        static const QString SystemAccentColor;
        static const QString SystemAccentColorLight1;
        static const QString SystemAccentColorLight2;
        static const QString SystemAccentColorLight3;
        static const QString SystemAccentColorDark1;
        static const QString SystemAccentColorDark2;
        static const QString SystemAccentColorDark3;

        // 控件填充色
        static const QString ControlFillColorDefault;
        static const QString ControlFillColorSecondary;
        static const QString ControlFillColorTertiary;
        static const QString ControlFillColorDisabled;
        static const QString ControlFillColorTransparent;
        static const QString ControlFillColorInputActive;

        // 控件边框色
        static const QString ControlStrokeColorDefault;
        static const QString ControlStrokeColorSecondary;
        static const QString ControlStrokeColorOnAccentDefault;
        static const QString ControlStrokeColorOnAccentSecondary;
        static const QString ControlStrokeColorOnAccentTertiary;
        static const QString ControlStrokeColorOnAccentDisabled;
        static const QString ControlStrokeColorForStrongFillWhenOnImage;
        static const QString ControlStrokeColorDisabled;

        // 文本颜色
        static const QString TextFillColorPrimary;
        static const QString TextFillColorSecondary;
        static const QString TextFillColorTertiary;
        static const QString TextFillColorDisabled;
        static const QString TextFillColorInverse;

        // 背景色
        static const QString ApplicationPageBackgroundThemeBrush;
        static const QString LayerFillColorDefault;
        static const QString LayerFillColorAlt;
        static const QString LayerOnAcrylicFillColorDefault;
        static const QString LayerOnAccentAcrylicFillColorDefault;

        // 中性色
        static const QString NeutralPrimary;
        static const QString NeutralSecondary;
        static const QString NeutralTertiary;
        static const QString NeutralQuaternary;
        static const QString NeutralLight;
        static const QString NeutralLighter;
        static const QString NeutralLightest;

        // 语义色
        static const QString SystemFillColorSuccess;
        static const QString SystemFillColorCaution;
        static const QString SystemFillColorCritical;
        static const QString SystemFillColorNeutral;
        static const QString SystemFillColorSolidNeutral;
        static const QString SystemFillColorAttentionBackground;
        static const QString SystemFillColorSuccessBackground;
        static const QString SystemFillColorCautionBackground;
        static const QString SystemFillColorCriticalBackground;
    };

    // 预定义字体
    struct Fonts {
        static const QString CaptionTextBlockStyle;
        static const QString BodyTextBlockStyle;
        static const QString BodyStrongTextBlockStyle;
        static const QString SubtitleTextBlockStyle;
        static const QString TitleTextBlockStyle;
        static const QString TitleLargeTextBlockStyle;
        static const QString DisplayTextBlockStyle;
    };

    // 预定义间距
    struct Spacing {
        static const QString XXSmall;
        static const QString XSmall;
        static const QString Small;
        static const QString Medium;
        static const QString Large;
        static const QString XLarge;
        static const QString XXLarge;
    };

    // 主题保存和加载
    void saveTheme(const QString& filePath = QString()) const;
    void loadTheme(const QString& filePath = QString());

    // 重置为默认主题
    void resetToDefault();

    // 从系统获取强调色
    QColor getSystemAccentColor() const;

    // 便利方法 - 直接获取常用颜色
    QColor controlFillColorDefault() const { return getColor(Colors::ControlFillColorDefault); }
    QColor controlFillColorDisabled() const { return getColor(Colors::ControlFillColorDisabled); }
    QColor controlStrokeColorDefault() const { return getColor(Colors::ControlStrokeColorDefault); }
    QColor controlStrokeColorDisabled() const { return getColor(Colors::ControlStrokeColorDisabled); }
    QColor textFillColorPrimary() const { return getColor(Colors::TextFillColorPrimary); }
    QColor textFillColorDisabled() const { return getColor(Colors::TextFillColorDisabled); }

signals:
    void themeModeChanged(QWinUIThemeMode mode);
    void accentColorChanged(const QColor& color);
    void followSystemThemeChanged(bool follow);
    void themeChanged();
    void colorChanged(const QString& colorName, const QColor& color);

private slots:
    void onSystemThemeChanged();

private:
    explicit QWinUITheme(QObject* parent = nullptr);
    ~QWinUITheme();

    void initializeTheme();
    void loadLightTheme();
    void loadDarkTheme();
    void loadHighContrastTheme();
    void updateAccentColorVariants();
    void connectToSystemTheme();
    void disconnectFromSystemTheme();

    QColor adjustColorForTheme(const QColor& baseColor, bool isDark) const;
    QColor generateAccentVariant(const QColor& baseColor, double factor) const;

    // 主题切换动画相关
    void startThemeTransitionForAllWidgets();

private:
    static QWinUITheme* s_instance;

    QWinUIThemeMode m_themeMode;
    QColor m_accentColor;
    bool m_followSystemTheme;
    bool m_isDarkMode;

    QMap<QString, QColor> m_colors;
    QMap<QString, QFont> m_fonts;
    QMap<QString, int> m_spacing;

    QSettings* m_settings;

    // 主题切换动画控制
    bool m_themeTransitionEnabled;
    int m_themeTransitionMode;

    // 禁用拷贝构造和赋值
    Q_DISABLE_COPY(QWinUITheme)
};

QT_END_NAMESPACE

#endif // QWINUITHEME_H
