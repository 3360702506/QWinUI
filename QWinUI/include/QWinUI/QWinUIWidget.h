#ifndef QWINUIWIDGET_H
#define QWINUIWIDGET_H

#include "QWinUIGlobal.h"
#include <QWidget>
#include <QColor>
#include <QPainter>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QTimer>
#include <QPoint>

QT_BEGIN_NAMESPACE

class QWinUITheme;
class QWinUIAnimation;
class QWinUIBlurEffect;
class QWinUIToolTip;

class QWINUI_EXPORT QWinUIWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int cornerRadius READ cornerRadius WRITE setCornerRadius NOTIFY cornerRadiusChanged)
    Q_PROPERTY(QColor accentColor READ accentColor WRITE setAccentColor NOTIFY accentColorChanged)
    Q_PROPERTY(QWinUIControlState controlState READ controlState WRITE setControlState NOTIFY controlStateChanged)
    Q_PROPERTY(bool useAcrylicEffect READ useAcrylicEffect WRITE setUseAcrylicEffect NOTIFY useAcrylicEffectChanged)
    Q_PROPERTY(QWinUIShadowDepth shadowDepth READ shadowDepth WRITE setShadowDepth NOTIFY shadowDepthChanged)
    Q_PROPERTY(QString toolTipText READ toolTipText WRITE setToolTipText NOTIFY toolTipTextChanged)
    Q_PROPERTY(bool toolTipEnabled READ isToolTipEnabled WRITE setToolTipEnabled NOTIFY toolTipEnabledChanged)

public:
    explicit QWinUIWidget(QWidget *parent = nullptr);
    virtual ~QWinUIWidget();

    // 主题相关
    QWinUITheme* theme() const;
    void setTheme(QWinUITheme* theme);

    // 圆角设置
    int cornerRadius() const;
    void setCornerRadius(int radius);
    void setCornerRadius(QWinUICornerRadius radius);

    // 强调色设置
    QColor accentColor() const;
    void setAccentColor(const QColor& color);

    // 控件状态
    QWinUIControlState controlState() const;
    void setControlState(QWinUIControlState state);

    // Acrylic效果
    bool useAcrylicEffect() const;
    void setUseAcrylicEffect(bool use);

    // Windows模糊效果
    void enableWindowsBlur(bool enable = true);
    void setWindowsBlurType(int blurType); // 0=None, 1=Blur, 2=Acrylic, 3=Mica

    // 阴影设置
    QWinUIShadowDepth shadowDepth() const;
    void setShadowDepth(QWinUIShadowDepth depth);

    // 动画相关
    void startAnimation(QWinUIAnimationType type, int duration = 200);
    void stopAnimation();

    // 主题检测
    bool isDarkMode() const;

    // 主题颜色获取（供子类使用）
    QColor themeBackgroundColor() const;
    QColor themeTextColor() const;
    QColor themeBorderColor() const;
    QColor themeAccentColor() const;

    // 主题切换动画模式
    enum QWinUITransitionMode {
        RippleTransition,    // 圆圈扩散动画
        FadeTransition       // 颜色过渡动画
    };
    Q_ENUM(QWinUITransitionMode)

    // 主题切换动画
    void startThemeTransition(const QPoint& clickPos = QPoint());
    void startThemeTransition(QWinUITransitionMode mode, const QPoint& clickPos = QPoint());
    bool isThemeTransitioning() const;

    // ToolTip 相关
    QString toolTipText() const;
    void setToolTipText(const QString& text);

    bool isToolTipEnabled() const;
    void setToolTipEnabled(bool enabled);

    void showToolTip();
    void hideToolTip();

signals:
    void cornerRadiusChanged(int radius);
    void accentColorChanged(const QColor& color);
    void controlStateChanged(QWinUIControlState state);
    void useAcrylicEffectChanged(bool use);
    void shadowDepthChanged(QWinUIShadowDepth depth);
    void themeChanged();
    void toolTipTextChanged(const QString& text);
    void toolTipEnabledChanged(bool enabled);

protected:
    // 重写的事件处理
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void changeEvent(QEvent* event) override;

    // 绘制相关的虚函数
    virtual void drawBackground(QPainter* painter, const QRect& rect);
    virtual void drawBorder(QPainter* painter, const QRect& rect);
    virtual void drawContent(QPainter* painter, const QRect& rect);
    virtual void drawForeground(QPainter* painter, const QRect& rect);

    // Fluent Design效果
    void applyFluentEffects(QPainter* painter, const QRect& rect);
    void drawAcrylicBackground(QPainter* painter, const QRect& rect);
    void drawRevealEffect(QPainter* painter, const QRect& rect, const QPoint& hotSpot);
    void applyShadowEffect();

    // 颜色获取辅助函数
    QColor getBackgroundColor() const;
    QColor getBorderColor() const;
    QColor getForegroundColor() const;
    QColor getAccentColor() const;

    // 状态更新
    virtual void updateControlState();
    virtual void onThemeChanged();

private slots:
    void onThemeChangedInternal();
    void onAnimationFinished();

private:
    void initializeWidget();
    void setupShadowEffect();
    void updateShadowEffect();

    // 主题相关辅助方法
    QColor getThemeBackgroundColor() const;
    QColor getThemeTextColor() const;
    QColor getThemeBorderColor() const;

    // 主题切换动画相关
    void updateThemeTransition();
    void drawThemeTransition(QPainter* painter);
    void drawRippleTransition(QPainter* painter, const QColor& oldColor, const QColor& newColor);
    void drawFadeTransition(QPainter* painter, const QColor& oldColor, const QColor& newColor);
    bool isCircleTouchingWidget(const QPoint& center, double radius) const;
    double getDistanceToWidget(const QPoint& center) const;
    double getDistanceToWidgetGlobal(const QPoint& center, const QRect& widgetRect) const;
    QColor getTransitionBackgroundColor(const QPoint& pos) const;
    double getDistanceFromTransitionCenter(const QPoint& pos) const;

private:
    QWinUITheme* m_theme;
    QWinUIAnimation* m_animation;
    
    int m_cornerRadius;
    QColor m_accentColor;
    QWinUIControlState m_controlState;
    bool m_useAcrylicEffect;
    QWinUIShadowDepth m_shadowDepth;
    
    QGraphicsDropShadowEffect* m_shadowEffect;
    QPropertyAnimation* m_stateAnimation;

    bool m_isHovered;
    bool m_isPressed;
    QPoint m_lastMousePos;

    // 主题切换动画相关
    bool m_isThemeTransitioning;
    QWinUITransitionMode m_transitionMode;
    QPoint m_transitionCenter;
    double m_transitionRadius;
    double m_maxTransitionRadius;
    QTimer* m_transitionTimer;
    QWinUIThemeMode m_oldThemeMode;
    QWinUIThemeMode m_newThemeMode;
    bool m_hasBeenTouched; // 标记此控件是否已被圆圈接触过

    // 颜色过渡动画相关
    double m_fadeProgress; // 0.0 到 1.0 的过渡进度
    QColor m_oldBackgroundColor;
    QColor m_newBackgroundColor;

    // ToolTip 相关
    QWinUIToolTip* m_toolTip;
    QString m_toolTipText;
    bool m_toolTipEnabled;
};

QT_END_NAMESPACE

#endif // QWINUIWIDGET_H
