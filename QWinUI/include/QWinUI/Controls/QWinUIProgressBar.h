#ifndef QWINUIPROGRESSBAR_H
#define QWINUIPROGRESSBAR_H

#include "../QWinUIWidget.h"
#include <QPropertyAnimation>
#include <QTimer>

QT_BEGIN_NAMESPACE

class QWINUI_EXPORT QWinUIProgressBar : public QWinUIWidget
{
    Q_OBJECT
    Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(double minimum READ minimum WRITE setMinimum NOTIFY minimumChanged)
    Q_PROPERTY(double maximum READ maximum WRITE setMaximum NOTIFY maximumChanged)
    Q_PROPERTY(bool isIndeterminate READ isIndeterminate WRITE setIndeterminate NOTIFY indeterminateChanged)
    Q_PROPERTY(QWinUIProgressBarState state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(bool showPercentage READ showPercentage WRITE setShowPercentage NOTIFY showPercentageChanged)
    Q_PROPERTY(bool showValue READ showValue WRITE setShowValue NOTIFY showValueChanged)
    Q_PROPERTY(bool autoToolTip READ autoToolTip WRITE setAutoToolTip NOTIFY autoToolTipChanged)
    Q_PROPERTY(int animationDuration READ animationDuration WRITE setAnimationDuration NOTIFY animationDurationChanged)

public:
    enum QWinUIProgressBarState {
        Normal,
        Error,
        Paused
    };
    Q_ENUM(QWinUIProgressBarState)

    explicit QWinUIProgressBar(QWidget* parent = nullptr);
    ~QWinUIProgressBar();

    // 属性访问器
    double value() const;
    void setValue(double value);

    double minimum() const;
    void setMinimum(double minimum);

    double maximum() const;
    void setMaximum(double maximum);

    bool isIndeterminate() const;
    void setIndeterminate(bool indeterminate);

    QWinUIProgressBarState state() const;
    void setState(QWinUIProgressBarState state);

    bool showPercentage() const;
    void setShowPercentage(bool show);

    bool showValue() const;
    void setShowValue(bool show);

    bool autoToolTip() const;
    void setAutoToolTip(bool enable);

    int animationDuration() const;
    void setAnimationDuration(int duration);

    // 便利方法
    void setRange(double minimum, double maximum);
    double percentage() const;
    void reset();

    // 尺寸相关
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void valueChanged(double value);
    void minimumChanged(double minimum);
    void maximumChanged(double maximum);
    void indeterminateChanged(bool indeterminate);
    void stateChanged(QWinUIProgressBarState state);
    void showPercentageChanged(bool show);
    void showValueChanged(bool show);
    void autoToolTipChanged(bool enable);
    void animationDurationChanged(int duration);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onAnimationValueChanged(const QVariant& value);
    void onIndeterminateAnimation();
    void onThemeChanged();

private:
    void initializeProgressBar();
    void setupAnimations();
    void startValueAnimation(double targetValue);
    void startIndeterminateAnimation();
    void stopIndeterminateAnimation();
    void updateAnimatedValue();
    
    // 绘制方法
    void drawBackground(QPainter* painter, const QRect& rect);
    void drawProgressFill(QPainter* painter, const QRect& rect);
    void drawIndeterminateProgress(QPainter* painter, const QRect& rect);
    void drawPercentageText(QPainter* painter, const QRect& rect);
    
    // 颜色获取
    QColor getBackgroundColor() const;
    QColor getProgressColor() const;
    QColor getTextColor() const;
    
    // 几何计算
    QRect getProgressRect() const;
    QRect getTextRect() const;
    double getProgressWidth() const;

    // ToolTip 更新
    void updateAutoToolTip();

private:
    double m_value;
    double m_minimum;
    double m_maximum;
    bool m_isIndeterminate;
    QWinUIProgressBarState m_state;
    bool m_showPercentage;
    bool m_showValue;
    bool m_autoToolTip;
    int m_animationDuration;
    
    // 动画相关
    QPropertyAnimation* m_valueAnimation;
    QTimer* m_indeterminateTimer;
    double m_animatedValue;
    double m_indeterminatePosition;
    bool m_isFirstCycle;
    
    // 样式常量
    static const int DefaultBackgroundHeight = 4;  // 背景高度（较小）
    static const int DefaultProgressHeight = 6;    // 进度条高度（较大）
    static const int DefaultTotalHeight = 8;       // 总高度
    static const int DefaultAnimationDuration = 250;
    static const int IndeterminateAnimationDuration = 2000;
    static const double IndeterminateBlockMinWidth;
    static const double IndeterminateBlockMaxWidth;
};

QT_END_NAMESPACE

#endif // QWINUIPROGRESSBAR_H
