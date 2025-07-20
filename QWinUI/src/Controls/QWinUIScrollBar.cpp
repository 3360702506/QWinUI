#include "QWinUI/Controls/QWinUIScrollBar.h"
#include "QWinUI/QWinUITheme.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QContextMenuEvent>
#include <QApplication>
#include <QStyle>
#include <QStyleOption>
#include <QtMath>

QT_BEGIN_NAMESPACE

QWinUIScrollBar::QWinUIScrollBar(Qt::Orientation orientation, QWidget* parent)
    : QWinUIWidget(parent)
    , m_orientation(orientation)
    , m_minimum(0)
    , m_maximum(99)
    , m_value(0)
    , m_pageStep(10)
    , m_singleStep(1)
    , m_tracking(true)
    , m_invertedAppearance(false)
    , m_thumbWidth(4)
    , m_trackWidth(6)
    , m_opacity(1.0)
    , m_autoHide(false)
    , m_autoHideTimer(nullptr)
    , m_fadeAnimation(nullptr)
    , m_sliderDown(false)
    , m_thumbHovered(false)
    , m_trackHovered(false)
    , m_pressValue(0)
    , m_geometryDirty(true)
{
    initializeScrollBar();
}

QWinUIScrollBar::~QWinUIScrollBar()
{
    if (m_fadeAnimation) {
        m_fadeAnimation->stop();
        delete m_fadeAnimation;
    }
    
    if (m_autoHideTimer) {
        delete m_autoHideTimer;
    }
}

void QWinUIScrollBar::initializeScrollBar()
{
    // 设置基本属性
    setAttribute(Qt::WA_Hover, true);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    
    // 设置大小策略
    if (m_orientation == Qt::Horizontal) {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        setFixedHeight(m_trackWidth);
    } else {
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        setFixedWidth(m_trackWidth);
    }
    
    // 创建自动隐藏定时器
    m_autoHideTimer = new QTimer(this);
    m_autoHideTimer->setSingleShot(true);
    m_autoHideTimer->setInterval(1500); // 1.5秒后自动隐藏
    connect(m_autoHideTimer, &QTimer::timeout, this, &QWinUIScrollBar::onAutoHideTimer);
    
    // 创建淡入淡出动画
    m_fadeAnimation = new QPropertyAnimation(this, "opacity");
    m_fadeAnimation->setDuration(150);
    m_fadeAnimation->setEasingCurve(QEasingCurve::OutQuart);
    connect(m_fadeAnimation, &QPropertyAnimation::finished, this, &QWinUIScrollBar::onFadeAnimationFinished);
    
    // 设置圆角
    setCornerRadius(static_cast<int>(QWinUICornerRadius::Small));
}

Qt::Orientation QWinUIScrollBar::orientation() const
{
    return m_orientation;
}

void QWinUIScrollBar::setOrientation(Qt::Orientation orientation)
{
    if (m_orientation != orientation) {
        m_orientation = orientation;
        m_geometryDirty = true;
        
        // 更新大小策略
        if (m_orientation == Qt::Horizontal) {
            setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            setFixedHeight(m_trackWidth);
            setMaximumWidth(QWIDGETSIZE_MAX);
        } else {
            setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
            setFixedWidth(m_trackWidth);
            setMaximumHeight(QWIDGETSIZE_MAX);
        }
        
        updateGeometry();
        update();
        emit orientationChanged(orientation);
    }
}

int QWinUIScrollBar::minimum() const
{
    return m_minimum;
}

void QWinUIScrollBar::setMinimum(int minimum)
{
    if (m_minimum != minimum) {
        m_minimum = minimum;
        if (m_value < minimum) {
            setValue(minimum);
        }
        m_geometryDirty = true;
        update();
        emit minimumChanged(minimum);
        emit rangeChanged(m_minimum, m_maximum);
    }
}

int QWinUIScrollBar::maximum() const
{
    return m_maximum;
}

void QWinUIScrollBar::setMaximum(int maximum)
{
    if (m_maximum != maximum) {
        m_maximum = maximum;
        if (m_value > maximum) {
            setValue(maximum);
        }
        m_geometryDirty = true;
        update();
        emit maximumChanged(maximum);
        emit rangeChanged(m_minimum, m_maximum);
    }
}

void QWinUIScrollBar::setRange(int minimum, int maximum)
{
    if (m_minimum != minimum || m_maximum != maximum) {
        m_minimum = minimum;
        m_maximum = maximum;
        if (m_value < minimum) {
            setValue(minimum);
        } else if (m_value > maximum) {
            setValue(maximum);
        }
        m_geometryDirty = true;
        update();
        emit minimumChanged(minimum);
        emit maximumChanged(maximum);
        emit rangeChanged(minimum, maximum);
    }
}

int QWinUIScrollBar::value() const
{
    return m_value;
}

void QWinUIScrollBar::setValue(int value)
{
    int clampedValue = qBound(m_minimum, value, m_maximum);
    if (m_value != clampedValue) {
        m_value = clampedValue;
        m_geometryDirty = true;
        update();
        emit valueChanged(clampedValue);
    }
}

int QWinUIScrollBar::pageStep() const
{
    return m_pageStep;
}

void QWinUIScrollBar::setPageStep(int pageStep)
{
    if (m_pageStep != pageStep) {
        m_pageStep = qMax(0, pageStep);
        m_geometryDirty = true;
        update();
        emit pageStepChanged(m_pageStep);
    }
}

int QWinUIScrollBar::singleStep() const
{
    return m_singleStep;
}

void QWinUIScrollBar::setSingleStep(int singleStep)
{
    if (m_singleStep != singleStep) {
        m_singleStep = qMax(0, singleStep);
        emit singleStepChanged(m_singleStep);
    }
}

bool QWinUIScrollBar::hasTracking() const
{
    return m_tracking;
}

void QWinUIScrollBar::setTracking(bool enable)
{
    if (m_tracking != enable) {
        m_tracking = enable;
        emit trackingChanged(enable);
    }
}

bool QWinUIScrollBar::invertedAppearance() const
{
    return m_invertedAppearance;
}

void QWinUIScrollBar::setInvertedAppearance(bool invert)
{
    if (m_invertedAppearance != invert) {
        m_invertedAppearance = invert;
        m_geometryDirty = true;
        update();
        emit invertedAppearanceChanged(invert);
    }
}

int QWinUIScrollBar::thumbWidth() const
{
    return m_thumbWidth;
}

void QWinUIScrollBar::setThumbWidth(int width)
{
    if (m_thumbWidth != width) {
        m_thumbWidth = qBound(4, width, 20);
        m_geometryDirty = true;
        update();
        emit thumbWidthChanged(m_thumbWidth);
    }
}

int QWinUIScrollBar::trackWidth() const
{
    return m_trackWidth;
}

void QWinUIScrollBar::setTrackWidth(int width)
{
    if (m_trackWidth != width) {
        m_trackWidth = qBound(8, width, 32);
        
        // 更新控件大小
        if (m_orientation == Qt::Horizontal) {
            setFixedHeight(m_trackWidth);
        } else {
            setFixedWidth(m_trackWidth);
        }
        
        m_geometryDirty = true;
        updateGeometry();
        update();
        emit trackWidthChanged(m_trackWidth);
    }
}

bool QWinUIScrollBar::autoHide() const
{
    return m_autoHide;
}

void QWinUIScrollBar::setAutoHide(bool autoHide)
{
    if (m_autoHide != autoHide) {
        m_autoHide = autoHide;
        
        if (autoHide) {
            m_autoHideTimer->start();
        } else {
            m_autoHideTimer->stop();
            setOpacity(1.0);
        }
        
        emit autoHideChanged(autoHide);
    }
}

double QWinUIScrollBar::opacity() const
{
    return m_opacity;
}

void QWinUIScrollBar::setOpacity(double opacity)
{
    double clampedOpacity = qBound(0.0, opacity, 1.0);
    if (!qFuzzyCompare(m_opacity, clampedOpacity)) {
        m_opacity = clampedOpacity;
        update();
        emit opacityChanged(clampedOpacity);
    }
}

QRect QWinUIScrollBar::thumbRect() const
{
    if (m_geometryDirty) {
        const_cast<QWinUIScrollBar*>(this)->updateGeometry();
    }
    return m_thumbRect;
}

QRect QWinUIScrollBar::trackRect() const
{
    if (m_geometryDirty) {
        const_cast<QWinUIScrollBar*>(this)->updateGeometry();
    }
    return m_trackRect;
}

QRect QWinUIScrollBar::grooveRect() const
{
    if (m_geometryDirty) {
        const_cast<QWinUIScrollBar*>(this)->updateGeometry();
    }
    return m_grooveRect;
}

bool QWinUIScrollBar::isSliderDown() const
{
    return m_sliderDown;
}

bool QWinUIScrollBar::isThumbHovered() const
{
    return m_thumbHovered;
}

void QWinUIScrollBar::triggerAction(int action)
{
    // 实现滚动条动作
    emit actionTriggered(action);
}

void QWinUIScrollBar::fadeIn()
{
    startFadeAnimation(1.0);
}

void QWinUIScrollBar::fadeOut()
{
    startFadeAnimation(0.0);
}

void QWinUIScrollBar::updateGeometry()
{
    m_trackRect = calculateTrackRect();
    m_grooveRect = calculateGrooveRect();
    m_thumbRect = calculateThumbRect();
    m_geometryDirty = false;
}

void QWinUIScrollBar::startFadeAnimation(double targetOpacity)
{
    if (m_fadeAnimation->state() == QPropertyAnimation::Running) {
        m_fadeAnimation->stop();
    }
    
    m_fadeAnimation->setStartValue(m_opacity);
    m_fadeAnimation->setEndValue(targetOpacity);
    m_fadeAnimation->start();
}

void QWinUIScrollBar::onFadeAnimationFinished()
{
    // 动画完成处理
}

void QWinUIScrollBar::onAutoHideTimer()
{
    if (m_autoHide && !m_thumbHovered && !m_sliderDown) {
        fadeOut();
    }
}

void QWinUIScrollBar::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setOpacity(m_opacity);

    // 只绘制滑块，不绘制背景轨道
    if (m_maximum > m_minimum) {
        drawThumb(&painter);
    }
}

void QWinUIScrollBar::drawTrack(QPainter* painter)
{
    QRect track = trackRect();
    if (track.isEmpty()) {
        return;
    }

    QColor trackColor = getTrackColor();
    painter->fillRect(track, trackColor);
}

void QWinUIScrollBar::drawThumb(QPainter* painter)
{
    QRect thumb = thumbRect();
    if (thumb.isEmpty()) {
        return;
    }

    QColor thumbColor = getThumbColor();

    // 根据悬停状态调整滑块大小
    QRect drawRect = thumb;
    if (m_thumbHovered || m_sliderDown) {
        // 悬停或按下时，滑块稍微变粗
        if (m_orientation == Qt::Horizontal) {
            int newHeight = qMin(8, m_trackWidth + 2);
            int offset = (thumb.height() - newHeight) / 2;
            drawRect.adjust(0, offset, 0, -offset);
        } else {
            int newWidth = qMin(8, m_trackWidth + 2);
            int offset = (thumb.width() - newWidth) / 2;
            drawRect.adjust(offset, 0, -offset, 0);
        }
    }

    // 绘制圆角矩形滑块
    int radius = qMin(drawRect.width(), drawRect.height()) / 2;
    painter->setBrush(thumbColor);
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(drawRect, radius, radius);
}

QRect QWinUIScrollBar::calculateTrackRect() const
{
    QRect rect = this->rect();

    if (m_orientation == Qt::Horizontal) {
        int trackHeight = qMin(m_trackWidth, rect.height());
        int y = (rect.height() - trackHeight) / 2;
        return QRect(0, y, rect.width(), trackHeight);
    } else {
        int trackWidth = qMin(m_trackWidth, rect.width());
        int x = (rect.width() - trackWidth) / 2;
        return QRect(x, 0, trackWidth, rect.height());
    }
}

QRect QWinUIScrollBar::calculateGrooveRect() const
{
    return calculateTrackRect();
}

QRect QWinUIScrollBar::calculateThumbRect() const
{
    QRect track = calculateTrackRect();
    if (track.isEmpty() || m_maximum <= m_minimum) {
        return QRect();
    }

    int range = m_maximum - m_minimum;
    if (range <= 0) {
        return QRect();
    }

    if (m_orientation == Qt::Horizontal) {
        int thumbWidth = qMax(20, (m_pageStep * track.width()) / (range + m_pageStep));
        thumbWidth = qMin(thumbWidth, track.width());

        int availableWidth = track.width() - thumbWidth;
        int thumbX = track.x();

        if (availableWidth > 0) {
            int pos = ((m_value - m_minimum) * availableWidth) / range;
            if (m_invertedAppearance) {
                pos = availableWidth - pos;
            }
            thumbX += pos;
        }

        int thumbHeight = qMin(m_thumbWidth, track.height());
        int thumbY = track.y() + (track.height() - thumbHeight) / 2;

        return QRect(thumbX, thumbY, thumbWidth, thumbHeight);
    } else {
        int thumbHeight = qMax(20, (m_pageStep * track.height()) / (range + m_pageStep));
        thumbHeight = qMin(thumbHeight, track.height());

        int availableHeight = track.height() - thumbHeight;
        int thumbY = track.y();

        if (availableHeight > 0) {
            int pos = ((m_value - m_minimum) * availableHeight) / range;
            if (m_invertedAppearance) {
                pos = availableHeight - pos;
            }
            thumbY += pos;
        }

        int thumbWidth = qMin(m_thumbWidth, track.width());
        int thumbX = track.x() + (track.width() - thumbWidth) / 2;

        return QRect(thumbX, thumbY, thumbWidth, thumbHeight);
    }
}

QColor QWinUIScrollBar::getThumbColor() const
{
    QWinUITheme* theme = QWinUITheme::getInstance();
    if (!theme) {
        return QColor(128, 128, 128);
    }

    if (m_sliderDown) {
        return theme->isDarkMode() ? QColor(255, 255, 255, 180) : QColor(0, 0, 0, 180);
    } else if (m_thumbHovered) {
        return theme->isDarkMode() ? QColor(255, 255, 255, 140) : QColor(0, 0, 0, 140);
    } else {
        return theme->isDarkMode() ? QColor(255, 255, 255, 100) : QColor(0, 0, 0, 100);
    }
}

QColor QWinUIScrollBar::getTrackColor() const
{
    QWinUITheme* theme = QWinUITheme::getInstance();
    if (!theme) {
        return QColor(240, 240, 240);
    }

    return theme->isDarkMode() ? QColor(255, 255, 255, 20) : QColor(0, 0, 0, 20);
}

void QWinUIScrollBar::mousePressEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton) {
        QWinUIWidget::mousePressEvent(event);
        return;
    }

    QRect thumb = thumbRect();
    if (thumb.contains(event->pos())) {
        // 点击在滑块上
        m_sliderDown = true;
        m_clickOffset = event->pos() - thumb.topLeft();
        m_pressValue = m_value;
        emit sliderPressed();
        update();
    } else {
        // 点击在轨道上，页面滚动
        QRect track = trackRect();
        if (track.contains(event->pos())) {
            int newValue;
            if (m_orientation == Qt::Horizontal) {
                newValue = pixelPosToRangeValue(event->pos().x() - thumb.width() / 2);
            } else {
                newValue = pixelPosToRangeValue(event->pos().y() - thumb.height() / 2);
            }
            setValue(newValue);
        }
    }

    // 重置自动隐藏定时器
    if (m_autoHide) {
        m_autoHideTimer->start();
    }

    QWinUIWidget::mousePressEvent(event);
}

void QWinUIScrollBar::mouseMoveEvent(QMouseEvent* event)
{
    if (m_sliderDown) {
        // 拖拽滑块
        QRect track = trackRect();
        QRect thumb = thumbRect();

        int newValue;
        if (m_orientation == Qt::Horizontal) {
            int newX = event->pos().x() - m_clickOffset.x();
            newValue = pixelPosToRangeValue(newX);
        } else {
            int newY = event->pos().y() - m_clickOffset.y();
            newValue = pixelPosToRangeValue(newY);
        }

        if (m_tracking) {
            setValue(newValue);
            emit sliderMoved(m_value);
        }
    } else {
        // 更新悬停状态
        updateHoverState();
    }

    QWinUIWidget::mouseMoveEvent(event);
}

void QWinUIScrollBar::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_sliderDown) {
        m_sliderDown = false;
        emit sliderReleased();
        update();

        if (!m_tracking) {
            // 如果没有启用跟踪，在释放时设置最终值
            QRect track = trackRect();
            int newValue;
            if (m_orientation == Qt::Horizontal) {
                int newX = event->pos().x() - m_clickOffset.x();
                newValue = pixelPosToRangeValue(newX);
            } else {
                int newY = event->pos().y() - m_clickOffset.y();
                newValue = pixelPosToRangeValue(newY);
            }
            setValue(newValue);
        }
    }

    QWinUIWidget::mouseReleaseEvent(event);
}

void QWinUIScrollBar::wheelEvent(QWheelEvent* event)
{
    int delta = event->angleDelta().y();
    int steps = delta / 120; // 标准滚轮步长

    int newValue = m_value - steps * m_singleStep;
    setValue(newValue);

    // 重置自动隐藏定时器
    if (m_autoHide) {
        m_autoHideTimer->start();
    }

    event->accept();
}

void QWinUIScrollBar::enterEvent(QEnterEvent* event)
{
    m_trackHovered = true;
    updateHoverState();

    // 如果启用自动隐藏，显示滚动条
    if (m_autoHide) {
        m_autoHideTimer->stop();
        fadeIn();
    }

    QWinUIWidget::enterEvent(event);
}

void QWinUIScrollBar::leaveEvent(QEvent* event)
{
    m_trackHovered = false;
    m_thumbHovered = false;
    update();

    // 如果启用自动隐藏，开始隐藏定时器
    if (m_autoHide && !m_sliderDown) {
        m_autoHideTimer->start();
    }

    QWinUIWidget::leaveEvent(event);
}

void QWinUIScrollBar::updateHoverState()
{
    QPoint mousePos = mapFromGlobal(QCursor::pos());
    QRect thumb = thumbRect();

    bool wasThumbHovered = m_thumbHovered;
    m_thumbHovered = thumb.contains(mousePos);

    if (wasThumbHovered != m_thumbHovered) {
        update();
    }
}

int QWinUIScrollBar::pixelPosToRangeValue(int pos) const
{
    QRect track = trackRect();
    QRect thumb = thumbRect();

    if (m_orientation == Qt::Horizontal) {
        int availableWidth = track.width() - thumb.width();
        if (availableWidth <= 0) {
            return m_minimum;
        }

        int pixelPos = pos - track.x();
        if (m_invertedAppearance) {
            pixelPos = availableWidth - pixelPos;
        }

        int range = m_maximum - m_minimum;
        int value = m_minimum + (pixelPos * range) / availableWidth;
        return qBound(m_minimum, value, m_maximum);
    } else {
        int availableHeight = track.height() - thumb.height();
        if (availableHeight <= 0) {
            return m_minimum;
        }

        int pixelPos = pos - track.y();
        if (m_invertedAppearance) {
            pixelPos = availableHeight - pixelPos;
        }

        int range = m_maximum - m_minimum;
        int value = m_minimum + (pixelPos * range) / availableHeight;
        return qBound(m_minimum, value, m_maximum);
    }
}

void QWinUIScrollBar::resizeEvent(QResizeEvent* event)
{
    m_geometryDirty = true;
    QWinUIWidget::resizeEvent(event);
}

void QWinUIScrollBar::contextMenuEvent(QContextMenuEvent* event)
{
    // 默认不显示上下文菜单
    event->ignore();
}

void QWinUIScrollBar::onThemeChanged()
{
    update();
    QWinUIWidget::onThemeChanged();
}

QT_END_NAMESPACE
