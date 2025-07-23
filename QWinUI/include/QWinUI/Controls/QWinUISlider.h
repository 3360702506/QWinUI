#ifndef QWINUISLIDER_H
#define QWINUISLIDER_H

#include "../QWinUIWidget.h"
#include <QWidget>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QTimer>

QT_BEGIN_NAMESPACE

class QWinUIToolTip;

class QWINUI_EXPORT QWinUISlider : public QWinUIWidget
{
    Q_OBJECT
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)
    Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(double minimum READ minimum WRITE setMinimum NOTIFY minimumChanged)
    Q_PROPERTY(double maximum READ maximum WRITE setMaximum NOTIFY maximumChanged)
    Q_PROPERTY(double stepSize READ stepSize WRITE setStepSize NOTIFY stepSizeChanged)
    Q_PROPERTY(bool showValueTooltip READ showValueTooltip WRITE setShowValueTooltip NOTIFY showValueTooltipChanged)
    Q_PROPERTY(bool showTickMarks READ showTickMarks WRITE setShowTickMarks NOTIFY showTickMarksChanged)
    Q_PROPERTY(TickPlacement tickPlacement READ tickPlacement WRITE setTickPlacement NOTIFY tickPlacementChanged)
    Q_PROPERTY(TickMode tickMode READ tickMode WRITE setTickMode NOTIFY tickModeChanged)
    Q_PROPERTY(double tickInterval READ tickInterval WRITE setTickInterval NOTIFY tickIntervalChanged)
    Q_PROPERTY(int animationDuration READ animationDuration WRITE setAnimationDuration NOTIFY animationDurationChanged)
    Q_PROPERTY(double thumbScale READ thumbScale WRITE setThumbScale)
    Q_PROPERTY(double innerCircleScale READ innerCircleScale WRITE setInnerCircleScale)
    Q_PROPERTY(double trackHoverProgress READ trackHoverProgress WRITE setTrackHoverProgress)
    Q_PROPERTY(double animatedValue READ animatedValue WRITE setAnimatedValue)

public:
    // 刻度位置枚举
    enum TickPlacement {
        NoTicks = 0,
        TicksAbove = 1,
        TicksBelow = 2,
        TicksBothSides = 3
    };
    Q_ENUM(TickPlacement)

    // 刻度模式枚举
    enum TickMode {
        TicksByStepSize = 0,    // 按步长显示刻度
        TicksByInterval = 1     // 按指定间隔显示刻度
    };
    Q_ENUM(TickMode)

    explicit QWinUISlider(QWidget* parent = nullptr);
    explicit QWinUISlider(Qt::Orientation orientation, QWidget* parent = nullptr);
    ~QWinUISlider() override;

    // 方向属性
    Qt::Orientation orientation() const;
    void setOrientation(Qt::Orientation orientation);

    // 值属性
    double value() const;
    void setValue(double value);

    // 范围属性
    double minimum() const;
    void setMinimum(double minimum);

    double maximum() const;
    void setMaximum(double maximum);

    void setRange(double minimum, double maximum);

    // 步长属性
    double stepSize() const;
    void setStepSize(double stepSize);

    // 工具提示属性
    bool showValueTooltip() const;
    void setShowValueTooltip(bool show);

    // 刻度相关属性
    bool showTickMarks() const;
    void setShowTickMarks(bool show);

    TickPlacement tickPlacement() const;
    void setTickPlacement(TickPlacement placement);

    TickMode tickMode() const;
    void setTickMode(TickMode mode);

    double tickInterval() const;
    void setTickInterval(double interval);

    // 设置自定义刻度值
    void setTickValues(const QList<double>& values);
    QList<double> tickValues() const;

    // 动画属性
    int animationDuration() const;
    void setAnimationDuration(int duration);

    // 动画进度属性
    double thumbScale() const { return m_thumbScale; }
    void setThumbScale(double scale) { m_thumbScale = scale; update(); }

    double innerCircleScale() const { return m_innerCircleScale; }
    void setInnerCircleScale(double scale) { m_innerCircleScale = scale; update(); }

    double trackHoverProgress() const { return m_trackHoverProgress; }
    void setTrackHoverProgress(double progress) { m_trackHoverProgress = progress; update(); }

    double animatedValue() const { return m_animatedValue; }
    void setAnimatedValue(double value) { m_animatedValue = value; update(); }

    // 尺寸相关
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public slots:
    void increaseValue();
    void decreaseValue();
    void pageUp();
    void pageDown();
    void toMinimum();
    void toMaximum();

signals:
    void valueChanged(double value);
    void sliderPressed();
    void sliderReleased();
    void orientationChanged(Qt::Orientation orientation);
    void minimumChanged(double minimum);
    void maximumChanged(double maximum);
    void stepSizeChanged(double stepSize);
    void showValueTooltipChanged(bool show);
    void showTickMarksChanged(bool show);
    void tickPlacementChanged(TickPlacement placement);
    void tickModeChanged(TickMode mode);
    void tickIntervalChanged(double interval);
    void animationDurationChanged(int duration);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

    // 主题变化处理
    void onThemeChanged() override;

private slots:
    void onValueAnimationFinished();
    void onThumbScaleAnimationFinished();
    void onTrackHoverAnimationFinished();
    void updateValueTooltip();

private:
    // 初始化
    void initializeSlider();
    void setupAnimations();

    // 绘制方法
    void drawTrack(QPainter* painter, const QRect& rect);
    void drawProgress(QPainter* painter, const QRect& rect);
    void drawThumb(QPainter* painter, const QRect& rect);
    void drawTickMarks(QPainter* painter, const QRect& rect);
    void drawFocusRect(QPainter* painter, const QRect& rect);

    // 几何计算
    QRect getTrackRect() const;
    QRect getThumbRect() const;
    QPoint getThumbCenter() const;
    double positionToValue(const QPoint& pos) const;
    QPoint valueToPosition(double value) const;

    // 刻度计算
    QList<double> calculateTickValues() const;
    QRect getTickRect() const;

    // 动画控制
    void startValueAnimation(double targetValue);
    void startThumbScaleAnimation(double targetScale);
    void startInnerCircleAnimation(double targetScale);
    void startTrackHoverAnimation(double targetProgress);

    // 值处理
    double constrainValue(double value) const;
    double snapToStep(double value) const;
    void updateValueFromPosition(const QPoint& pos);

    // 工具提示
    void showValueTooltipAt(const QPoint& pos);
    void hideValueTooltip();

    // 状态检查
    bool isHovered() const;
    bool isPressed() const;
    bool isFocused() const;

private:
    // 基本属性
    Qt::Orientation m_orientation;
    double m_value;
    double m_minimum;
    double m_maximum;
    double m_stepSize;
    bool m_showValueTooltip;
    int m_animationDuration;

    // 刻度相关属性
    bool m_showTickMarks;
    TickPlacement m_tickPlacement;
    TickMode m_tickMode;
    double m_tickInterval;
    QList<double> m_customTickValues;

    // 状态
    bool m_isHovered;
    bool m_isPressed;
    bool m_isFocused;
    bool m_isDragging;
    QPoint m_lastMousePos;

    // 动画进度
    double m_thumbScale;
    double m_innerCircleScale;      // 内圆缩放
    double m_trackHoverProgress;
    double m_animatedValue; // 用于动画的值

    // 动画对象
    QPropertyAnimation* m_valueAnimation;
    QPropertyAnimation* m_thumbScaleAnimation;
    QPropertyAnimation* m_innerCircleAnimation;
    QPropertyAnimation* m_trackHoverAnimation;

    // 工具提示
    QWinUIToolTip* m_valueTooltip;
    QTimer* m_tooltipTimer;

    // WinUI3 设计规范常量
    static constexpr int TRACK_HEIGHT = 4;           // 轨道高度
    static constexpr int TRACK_WIDTH = 4;            // 轨道宽度（垂直）
    static constexpr int THUMB_OUTER_SIZE = 20;      // 拇指外圆大小
    static constexpr int THUMB_INNER_SIZE = 12;      // 拇指内圆大小
    static constexpr int THUMB_INNER_HOVER_SIZE = 16; // 悬浮时内圆大小
    static constexpr int MINIMUM_LENGTH = 100;       // 最小长度
    static constexpr int DEFAULT_ANIMATION_DURATION = 150;
    static constexpr double DEFAULT_STEP_SIZE = 1.0;
    static constexpr double PAGE_STEP_MULTIPLIER = 10.0;
    static constexpr int FOCUS_BORDER_WIDTH = 2;

    // 刻度相关常量
    static constexpr int TICK_LENGTH = 6;                // 刻度线长度
    static constexpr int TICK_SPACING = 4;               // 刻度线与轨道的间距
    static constexpr int TICK_WIDTH = 1;                 // 刻度线宽度
};

QT_END_NAMESPACE

#endif // QWINUISLIDER_H
