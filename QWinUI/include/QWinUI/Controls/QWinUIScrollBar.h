#ifndef QWINUISCROLLBAR_H
#define QWINUISCROLLBAR_H

#include "../QWinUIGlobal.h"
#include "../QWinUIWidget.h"
#include <QScrollBar>
#include <QPropertyAnimation>
#include <QTimer>
#include <QEnterEvent>

QT_BEGIN_NAMESPACE

class QWINUI_EXPORT QWinUIScrollBar : public QWinUIWidget
{
    Q_OBJECT
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)
    Q_PROPERTY(int minimum READ minimum WRITE setMinimum NOTIFY minimumChanged)
    Q_PROPERTY(int maximum READ maximum WRITE setMaximum NOTIFY maximumChanged)
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(int pageStep READ pageStep WRITE setPageStep NOTIFY pageStepChanged)
    Q_PROPERTY(int singleStep READ singleStep WRITE setSingleStep NOTIFY singleStepChanged)
    Q_PROPERTY(bool tracking READ hasTracking WRITE setTracking NOTIFY trackingChanged)
    Q_PROPERTY(bool inverted READ invertedAppearance WRITE setInvertedAppearance NOTIFY invertedAppearanceChanged)
    Q_PROPERTY(int thumbWidth READ thumbWidth WRITE setThumbWidth NOTIFY thumbWidthChanged)
    Q_PROPERTY(int trackWidth READ trackWidth WRITE setTrackWidth NOTIFY trackWidthChanged)
    Q_PROPERTY(bool autoHide READ autoHide WRITE setAutoHide NOTIFY autoHideChanged)
    Q_PROPERTY(double opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)

public:
    explicit QWinUIScrollBar(Qt::Orientation orientation = Qt::Vertical, QWidget* parent = nullptr);
    ~QWinUIScrollBar();

    // 基本属性
    Qt::Orientation orientation() const;
    void setOrientation(Qt::Orientation orientation);

    int minimum() const;
    void setMinimum(int minimum);
    int maximum() const;
    void setMaximum(int maximum);
    void setRange(int minimum, int maximum);

    int value() const;
    void setValue(int value);

    int pageStep() const;
    void setPageStep(int pageStep);
    int singleStep() const;
    void setSingleStep(int singleStep);

    bool hasTracking() const;
    void setTracking(bool enable);

    bool invertedAppearance() const;
    void setInvertedAppearance(bool invert);

    // 外观属性
    int thumbWidth() const;
    void setThumbWidth(int width);
    int trackWidth() const;
    void setTrackWidth(int width);

    // 自动隐藏
    bool autoHide() const;
    void setAutoHide(bool autoHide);
    double opacity() const;
    void setOpacity(double opacity);

    // 几何计算
    QRect thumbRect() const;
    QRect trackRect() const;
    QRect grooveRect() const;

    // 状态查询
    bool isSliderDown() const;
    bool isThumbHovered() const;

public slots:
    void triggerAction(int action);
    void fadeIn();
    void fadeOut();

signals:
    void orientationChanged(Qt::Orientation orientation);
    void minimumChanged(int minimum);
    void maximumChanged(int maximum);
    void valueChanged(int value);
    void pageStepChanged(int pageStep);
    void singleStepChanged(int singleStep);
    void trackingChanged(bool tracking);
    void invertedAppearanceChanged(bool inverted);
    void thumbWidthChanged(int width);
    void trackWidthChanged(int width);
    void autoHideChanged(bool autoHide);
    void opacityChanged(double opacity);
    
    void sliderPressed();
    void sliderMoved(int value);
    void sliderReleased();
    void rangeChanged(int minimum, int maximum);
    void actionTriggered(int action);

protected:
    // QWidget 重写
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

    // 主题相关
    void onThemeChanged() override;

private slots:
    void onFadeAnimationFinished();
    void onAutoHideTimer();

private:
    void initializeScrollBar();
    void updateGeometry();
    void updateThumbPosition();
    void updateHoverState();
    
    // 绘制方法
    void drawTrack(QPainter* painter);
    void drawThumb(QPainter* painter);
    void drawGroove(QPainter* painter);
    
    // 几何计算
    QRect calculateThumbRect() const;
    QRect calculateTrackRect() const;
    QRect calculateGrooveRect() const;
    
    // 值计算
    int pixelPosToRangeValue(int pos) const;
    int rangeValueToPixelPos(int value) const;
    
    // 动作处理
    void processMousePress(const QPoint& pos);
    void processMouseMove(const QPoint& pos);
    void processMouseRelease();
    
    // 主题颜色
    QColor getThumbColor() const;
    QColor getTrackColor() const;
    QColor getGrooveColor() const;
    
    // 动画
    void startFadeAnimation(double targetOpacity);

private:
    Qt::Orientation m_orientation;
    int m_minimum;
    int m_maximum;
    int m_value;
    int m_pageStep;
    int m_singleStep;
    bool m_tracking;
    bool m_invertedAppearance;
    
    // 外观
    int m_thumbWidth;
    int m_trackWidth;
    double m_opacity;
    
    // 自动隐藏
    bool m_autoHide;
    QTimer* m_autoHideTimer;
    QPropertyAnimation* m_fadeAnimation;
    
    // 交互状态
    bool m_sliderDown;
    bool m_thumbHovered;
    bool m_trackHovered;
    QPoint m_clickOffset;
    int m_pressValue;
    
    // 几何缓存
    mutable QRect m_thumbRect;
    mutable QRect m_trackRect;
    mutable QRect m_grooveRect;
    mutable bool m_geometryDirty;
};

QT_END_NAMESPACE

#endif // QWINUISCROLLBAR_H
