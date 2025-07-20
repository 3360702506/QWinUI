#pragma once

#include "QWinUI/QWinUIWidget.h"
#include <QPropertyAnimation>
#include <QTimer>
#include <QPainter>
#include <QPainterPath>

QT_BEGIN_NAMESPACE

class QWINUI_EXPORT QWinUIProgressRing : public QWinUIWidget
{
    Q_OBJECT
    Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(double animatedValue READ animatedValue WRITE setAnimatedValue)
    Q_PROPERTY(double minimum READ minimum WRITE setMinimum NOTIFY minimumChanged)
    Q_PROPERTY(double maximum READ maximum WRITE setMaximum NOTIFY maximumChanged)
    Q_PROPERTY(bool isIndeterminate READ isIndeterminate WRITE setIsIndeterminate NOTIFY isIndeterminateChanged)
    Q_PROPERTY(bool isActive READ isActive WRITE setIsActive NOTIFY isActiveChanged)
    Q_PROPERTY(int ringThickness READ ringThickness WRITE setRingThickness NOTIFY ringThicknessChanged)
    Q_PROPERTY(QColor ringColor READ ringColor WRITE setRingColor NOTIFY ringColorChanged)
    Q_PROPERTY(QColor backgroundRingColor READ backgroundRingColor WRITE setBackgroundRingColor NOTIFY backgroundRingColorChanged)

public:
    explicit QWinUIProgressRing(QWidget* parent = nullptr);
    ~QWinUIProgressRing();

    // 值属性
    double value() const;
    void setValue(double value);

    // 动画值属性（内部使用）
    double animatedValue() const;
    void setAnimatedValue(double value);

    double minimum() const;
    void setMinimum(double minimum);

    double maximum() const;
    void setMaximum(double maximum);

    void setRange(double minimum, double maximum);

    // 状态属性
    bool isIndeterminate() const;
    void setIsIndeterminate(bool indeterminate);

    bool isActive() const;
    void setIsActive(bool active);

    // 外观属性
    int ringThickness() const;
    void setRingThickness(int thickness);

    QColor ringColor() const;
    void setRingColor(const QColor& color);

    QColor backgroundRingColor() const;
    void setBackgroundRingColor(const QColor& color);

    // 便捷方法
    double percentage() const;
    void setPercentage(double percentage);

    // 尺寸建议
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void valueChanged(double value);
    void minimumChanged(double minimum);
    void maximumChanged(double maximum);
    void isIndeterminateChanged(bool indeterminate);
    void isActiveChanged(bool active);
    void ringThicknessChanged(int thickness);
    void ringColorChanged(const QColor& color);
    void backgroundRingColorChanged(const QColor& color);

protected:
    // 事件处理
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;

    // 主题处理
    void onThemeChanged() override;

private slots:
    void updateIndeterminateAnimation();
    void onValueAnimationFinished();

private:
    void initializeComponent();
    void updateColors();
    void startIndeterminateAnimation();
    void stopIndeterminateAnimation();
    void startValueAnimation(double targetValue);
    
    // 绘制方法
    void drawProgressRing(QPainter& painter);
    void drawDeterminateRing(QPainter& painter, const QRectF& rect);
    void drawIndeterminateRing(QPainter& painter, const QRectF& rect);
    
    // 工具方法
    QRectF getRingRect() const;
    double normalizeValue(double value) const;
    QColor getThemeRingColor() const;
    QColor getThemeBackgroundRingColor() const;

    // 成员变量
    double m_value;
    double m_minimum;
    double m_maximum;
    bool m_isIndeterminate;
    bool m_isActive;
    int m_ringThickness;
    QColor m_ringColor;
    QColor m_backgroundRingColor;
    bool m_useThemeColors;

    // 动画相关
    QPropertyAnimation* m_valueAnimation;
    QTimer* m_indeterminateTimer;
    double m_animatedValue;  // 当前动画显示的值
    double m_targetValue;    // 目标值

    // 不确定动画状态
    double m_indeterminateAngle;
    double m_indeterminateSpan;
    bool m_indeterminateDirection;
    double m_indeterminatePhase;  // 动画相位
    qint64 m_lastUpdateTime;      // 上次更新时间
    bool m_animationPaused;       // 动画暂停状态

    // 常量
    static constexpr double DEFAULT_MINIMUM = 0.0;
    static constexpr double DEFAULT_MAXIMUM = 100.0;
    static constexpr double DEFAULT_VALUE = 0.0;
    static constexpr int DEFAULT_RING_THICKNESS = 4;
    static constexpr int DEFAULT_SIZE = 32;

    // 动画常量
    static constexpr int INDETERMINATE_TIMER_INTERVAL = 16; // 60 FPS
    static constexpr int VALUE_ANIMATION_DURATION = 280;    // 确定进度动画时长
    static constexpr double INDETERMINATE_ROTATION_SPEED = 8.0;  // 旋转速度（度/帧）- 加快
    static constexpr double INDETERMINATE_MIN_SPAN = 8.0;   // 最小弧长
    static constexpr double INDETERMINATE_MAX_SPAN = 80.0;  // 最大弧长
    static constexpr double INDETERMINATE_SPAN_SPEED = 2.5; // 弧长变化速度 - 加快
    static constexpr double ANIMATION_EPSILON = 0.01;      // 动画精度阈值
};

QT_END_NAMESPACE
