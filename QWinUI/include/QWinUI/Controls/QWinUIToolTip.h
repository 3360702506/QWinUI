#ifndef QWINUITOOLTIP_H
#define QWINUITOOLTIP_H

#include "../QWinUIWidget.h"
#include <QTimer>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QMargins>

QT_BEGIN_NAMESPACE

class QWINUI_EXPORT QWinUIToolTip : public QWinUIWidget
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QWidget* target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(int showDelay READ showDelay WRITE setShowDelay NOTIFY showDelayChanged)
    Q_PROPERTY(int hideDelay READ hideDelay WRITE setHideDelay NOTIFY hideDelayChanged)
    Q_PROPERTY(QWinUIToolTipPlacement placement READ placement WRITE setPlacement NOTIFY placementChanged)
    Q_PROPERTY(bool isVisible READ isVisible WRITE setVisible NOTIFY visibilityChanged)
    Q_PROPERTY(double opacity READ opacity WRITE setOpacity)

public:
    enum QWinUIToolTipPlacement {
        Top,
        Bottom,
        Left,
        Right,
        Auto  // 自动选择最佳位置
    };
    Q_ENUM(QWinUIToolTipPlacement)

    explicit QWinUIToolTip(QWidget* parent = nullptr);
    explicit QWinUIToolTip(const QString& text, QWidget* target = nullptr, QWidget* parent = nullptr);
    ~QWinUIToolTip();

    // 属性访问器
    QString text() const;
    void setText(const QString& text);

    QWidget* target() const;
    void setTarget(QWidget* target);

    int showDelay() const;
    void setShowDelay(int delay);

    int hideDelay() const;
    void setHideDelay(int delay);

    QWinUIToolTipPlacement placement() const;
    void setPlacement(QWinUIToolTipPlacement placement);

    // 透明度控制（用于动画）
    double opacity() const;
    void setOpacity(double opacity);

    // 显示和隐藏
    void showToolTip();
    void hideToolTip();
    void showToolTip(const QPoint& position);

    // 静态便利方法
    static void showText(const QPoint& pos, const QString& text, QWidget* parent = nullptr);
    static void hideText();

    // 尺寸相关
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void textChanged(const QString& text);
    void targetChanged(QWidget* target);
    void showDelayChanged(int delay);
    void hideDelayChanged(int delay);
    void placementChanged(QWinUIToolTipPlacement placement);
    void visibilityChanged(bool visible);
    void aboutToShow();
    void aboutToHide();

protected:
    void paintEvent(QPaintEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;
    bool eventFilter(QObject* object, QEvent* event) override;

private slots:
    void onShowTimer();
    void onHideTimer();
    void onFadeInFinished();
    void onFadeOutFinished();
    void onThemeChanged();

private:
    void initializeToolTip();
    void setupAnimations();
    void updatePosition();
    void calculateOptimalPosition();
    QPoint calculatePosition(QWinUIToolTipPlacement placement);
    QWinUIToolTipPlacement getOptimalPlacement();
    
    // 绘制方法
    void drawBackground(QPainter* painter, const QRect& rect);
    void drawShadow(QPainter* painter, const QRect& rect);
    void drawText(QPainter* painter, const QRect& rect);
    
    // 颜色获取
    QColor getBackgroundColor() const;
    QColor getBorderColor() const;
    QColor getTextColor() const;
    QColor getShadowColor() const;
    
    // 几何计算
    QRect getTextRect() const;
    QMargins getContentMargins() const;
    
    // 目标控件事件处理
    void installTargetEventFilter();
    void removeTargetEventFilter();

private:
    QString m_text;
    QWidget* m_target;
    int m_showDelay;
    int m_hideDelay;
    QWinUIToolTipPlacement m_placement;
    QWinUIToolTipPlacement m_actualPlacement; // 实际使用的位置
    
    // 定时器
    QTimer* m_showTimer;
    QTimer* m_hideTimer;
    
    // 动画
    QPropertyAnimation* m_fadeAnimation;
    QGraphicsOpacityEffect* m_opacityEffect;
    double m_currentOpacity;
    
    // 状态
    bool m_isShowing;
    bool m_isHiding;
    bool m_isVisible;
    QPoint m_mousePosition;  // 记录鼠标位置
    
    // 静态实例（用于静态方法）
    static QWinUIToolTip* s_instance;
    
    // 样式常量
    static const int DefaultShowDelay = 500;  // 500ms
    static const int DefaultHideDelay = 100;  // 100ms
    static const int FadeAnimationDuration = 150; // 150ms
    static const int ShadowBlurRadius = 8;
    static const int ShadowOffset = 2;
    static const QMargins ContentMargins;
};

QT_END_NAMESPACE

#endif // QWINUITOOLTIP_H
