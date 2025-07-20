#pragma once

#include "QWinUI/QWinUIWidget.h"
#include <QGraphicsEffect>
#include <QPropertyAnimation>
#include <QTimer>

QT_BEGIN_NAMESPACE

// 背景源类型枚举
enum class QWinUIAcrylicBackgroundSource {
    HostBackdrop,  // 使用主机背景（窗口后面的内容）
    Backdrop       // 使用应用内背景（控件后面的内容）
};

class QWINUI_EXPORT QWinUIAcrylicBrush : public QWinUIWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor tintColor READ tintColor WRITE setTintColor NOTIFY tintColorChanged)
    Q_PROPERTY(double tintOpacity READ tintOpacity WRITE setTintOpacity NOTIFY tintOpacityChanged)
    Q_PROPERTY(QColor fallbackColor READ fallbackColor WRITE setFallbackColor NOTIFY fallbackColorChanged)
    Q_PROPERTY(double tintLuminosityOpacity READ tintLuminosityOpacity WRITE setTintLuminosityOpacity NOTIFY tintLuminosityOpacityChanged)
    Q_PROPERTY(QWinUIAcrylicBackgroundSource backgroundSource READ backgroundSource WRITE setBackgroundSource NOTIFY backgroundSourceChanged)
    Q_PROPERTY(bool isEffectEnabled READ isEffectEnabled WRITE setEffectEnabled NOTIFY effectEnabledChanged)
    Q_PROPERTY(double blurRadius READ blurRadius WRITE setBlurRadius NOTIFY blurRadiusChanged)
    Q_PROPERTY(double noiseOpacity READ noiseOpacity WRITE setNoiseOpacity NOTIFY noiseOpacityChanged)

public:
    explicit QWinUIAcrylicBrush(QWidget* parent = nullptr);
    ~QWinUIAcrylicBrush();

    // 核心属性
    QColor tintColor() const;
    void setTintColor(const QColor& color);

    double tintOpacity() const;
    void setTintOpacity(double opacity);

    QColor fallbackColor() const;
    void setFallbackColor(const QColor& color);

    double tintLuminosityOpacity() const;
    void setTintLuminosityOpacity(double opacity);

    QWinUIAcrylicBackgroundSource backgroundSource() const;
    void setBackgroundSource(QWinUIAcrylicBackgroundSource source);

    // 效果控制
    bool isEffectEnabled() const;
    void setEffectEnabled(bool enabled);

    double blurRadius() const;
    void setBlurRadius(double radius);

    double noiseOpacity() const;
    void setNoiseOpacity(double opacity);

    // 预设样式
    void applyNavigationPanelStyle();
    void applyContentDialogStyle();
    void applySidebarStyle();
    void applyTooltipStyle();

    // 动画方法
    void animateTintOpacity(double targetOpacity, int duration = 300);
    void animateTintColor(const QColor& targetColor, int duration = 300);

    // 尺寸建议
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void tintColorChanged(const QColor& color);
    void tintOpacityChanged(double opacity);
    void fallbackColorChanged(const QColor& color);
    void tintLuminosityOpacityChanged(double opacity);
    void backgroundSourceChanged(QWinUIAcrylicBackgroundSource source);
    void effectEnabledChanged(bool enabled);
    void blurRadiusChanged(double radius);
    void noiseOpacityChanged(double opacity);

protected:
    // 事件处理
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;

    // 主题处理
    void onThemeChanged() override;

private slots:
    void updateBackgroundCapture();
    void onTintOpacityAnimationFinished();
    void onTintColorAnimationFinished();

private:
    void initializeComponent();
    void updateEffectState();
    void updateThemeColors();
    void captureBackground();
    void generateNoiseTexture();
    QPixmap applyAcrylicEffect(const QPixmap& background);
    QColor calculateLuminosityColor() const;
    bool shouldUseFallback() const;
    void setupAnimations();
    void applyAcrylicStyle();
    QString createAcrylicStyleSheet();
    void applyBlurEffect();

    // 核心属性
    QColor m_tintColor;
    double m_tintOpacity;
    QColor m_fallbackColor;
    double m_tintLuminosityOpacity;
    QWinUIAcrylicBackgroundSource m_backgroundSource;
    bool m_isEffectEnabled;
    double m_blurRadius;
    double m_noiseOpacity;

    // 效果相关
    QPixmap m_backgroundCapture;
    QPixmap m_noiseTexture;
    QPixmap m_cachedEffect;
    bool m_needsBackgroundUpdate;
    bool m_isCapturing;

    // 动画
    QPropertyAnimation* m_tintOpacityAnimation;
    QPropertyAnimation* m_tintColorAnimation;

    // 更新定时器
    QTimer* m_updateTimer;

    // 常量
    static constexpr double DEFAULT_TINT_OPACITY = 0.8;
    static constexpr double DEFAULT_LUMINOSITY_OPACITY = 0.85;
    static constexpr double DEFAULT_BLUR_RADIUS = 30.0;
    static constexpr double DEFAULT_NOISE_OPACITY = 0.02;
    static constexpr int UPDATE_INTERVAL_MS = 16; // ~60 FPS
};

QT_END_NAMESPACE
