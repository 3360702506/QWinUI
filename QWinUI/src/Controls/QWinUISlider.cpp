#include "QWinUI/Controls/QWinUISlider.h"
#include "QWinUI/Controls/QWinUIToolTip.h"
#include "QWinUI/QWinUITheme.h"
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QFocusEvent>
#include <QResizeEvent>
#include <QApplication>
#include <QtMath>

QWinUISlider::QWinUISlider(QWidget* parent)
    : QWinUIWidget(parent)
    , m_orientation(Qt::Horizontal)
    , m_value(0.0)
    , m_minimum(0.0)
    , m_maximum(100.0)
    , m_stepSize(DEFAULT_STEP_SIZE)
    , m_showValueTooltip(false)
    , m_animationDuration(DEFAULT_ANIMATION_DURATION)
    , m_showTickMarks(false)
    , m_tickPlacement(TicksBelow)
    , m_tickMode(TicksByStepSize)
    , m_tickInterval(DEFAULT_STEP_SIZE)
    , m_isHovered(false)
    , m_isPressed(false)
    , m_isFocused(false)
    , m_isDragging(false)
    , m_thumbScale(1.0)
    , m_innerCircleScale(1.0)
    , m_trackHoverProgress(0.0)
    , m_animatedValue(0.0)
    , m_valueAnimation(nullptr)
    , m_thumbScaleAnimation(nullptr)
    , m_innerCircleAnimation(nullptr)
    , m_trackHoverAnimation(nullptr)
    , m_valueTooltip(nullptr)
    , m_tooltipTimer(nullptr)
{
    initializeSlider();
}

QWinUISlider::QWinUISlider(Qt::Orientation orientation, QWidget* parent)
    : QWinUIWidget(parent)
    , m_orientation(orientation)
    , m_value(0.0)
    , m_minimum(0.0)
    , m_maximum(100.0)
    , m_stepSize(DEFAULT_STEP_SIZE)
    , m_showValueTooltip(false)
    , m_animationDuration(DEFAULT_ANIMATION_DURATION)
    , m_showTickMarks(false)
    , m_tickPlacement(TicksBelow)
    , m_tickMode(TicksByStepSize)
    , m_tickInterval(DEFAULT_STEP_SIZE)
    , m_isHovered(false)
    , m_isPressed(false)
    , m_isFocused(false)
    , m_isDragging(false)
    , m_thumbScale(1.0)
    , m_innerCircleScale(1.0)
    , m_trackHoverProgress(0.0)
    , m_animatedValue(0.0)
    , m_valueAnimation(nullptr)
    , m_thumbScaleAnimation(nullptr)
    , m_innerCircleAnimation(nullptr)
    , m_trackHoverAnimation(nullptr)
    , m_valueTooltip(nullptr)
    , m_tooltipTimer(nullptr)
{
    initializeSlider();
}

QWinUISlider::~QWinUISlider()
{
    // 动画对象会被 Qt 的父子关系自动清理
}

void QWinUISlider::initializeSlider()
{
    setFocusPolicy(Qt::StrongFocus);
    setCursor(Qt::PointingHandCursor);

    // 设置最小尺寸
    if (m_orientation == Qt::Horizontal) {
        setMinimumSize(MINIMUM_LENGTH, THUMB_OUTER_SIZE + 10);
    } else {
        setMinimumSize(THUMB_OUTER_SIZE + 10, MINIMUM_LENGTH);
    }

    // 设置动画
    setupAnimations();

    // 创建自定义工具提示
    m_valueTooltip = new QWinUIToolTip(this);
    m_valueTooltip->setPlacement(QWinUIToolTip::Top);

    // 设置工具提示定时器
    m_tooltipTimer = new QTimer(this);
    m_tooltipTimer->setSingleShot(true);
    m_tooltipTimer->setInterval(500); // 500ms 延迟显示工具提示
    connect(m_tooltipTimer, &QTimer::timeout, this, &QWinUISlider::updateValueTooltip);

    // 连接主题变化信号
    QWinUITheme* theme = QWinUITheme::getInstance();
    if (theme) {
        connect(theme, &QWinUITheme::themeChanged, this, &QWinUISlider::onThemeChanged);
    }

    // 初始化动画值
    m_animatedValue = m_value;
}

void QWinUISlider::setupAnimations()
{
    // 值变化动画
    m_valueAnimation = new QPropertyAnimation(this, "animatedValue", this);
    m_valueAnimation->setDuration(m_animationDuration);
    m_valueAnimation->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_valueAnimation, &QPropertyAnimation::finished, this, &QWinUISlider::onValueAnimationFinished);

    // 拇指缩放动画（外圆）
    m_thumbScaleAnimation = new QPropertyAnimation(this, "thumbScale", this);
    m_thumbScaleAnimation->setDuration(m_animationDuration);
    m_thumbScaleAnimation->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_thumbScaleAnimation, &QPropertyAnimation::finished, this, &QWinUISlider::onThumbScaleAnimationFinished);

    // 内圆缩放动画
    m_innerCircleAnimation = new QPropertyAnimation(this, "innerCircleScale", this);
    m_innerCircleAnimation->setDuration(m_animationDuration);
    m_innerCircleAnimation->setEasingCurve(QEasingCurve::OutCubic);

    // 轨道悬浮动画
    m_trackHoverAnimation = new QPropertyAnimation(this, "trackHoverProgress", this);
    m_trackHoverAnimation->setDuration(m_animationDuration);
    m_trackHoverAnimation->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_trackHoverAnimation, &QPropertyAnimation::finished, this, &QWinUISlider::onTrackHoverAnimationFinished);
}

// 属性访问器
Qt::Orientation QWinUISlider::orientation() const
{
    return m_orientation;
}

void QWinUISlider::setOrientation(Qt::Orientation orientation)
{
    if (m_orientation != orientation) {
        m_orientation = orientation;
        
        // 更新最小尺寸
        if (m_orientation == Qt::Horizontal) {
            setMinimumSize(MINIMUM_LENGTH, THUMB_OUTER_SIZE + 10);
        } else {
            setMinimumSize(THUMB_OUTER_SIZE + 10, MINIMUM_LENGTH);
        }
        
        updateGeometry();
        update();
        emit orientationChanged(orientation);
    }
}

double QWinUISlider::value() const
{
    return m_value;
}

void QWinUISlider::setValue(double value)
{
    double constrainedValue = constrainValue(value);
    if (qAbs(m_value - constrainedValue) > 1e-6) {
        m_value = constrainedValue;
        
        // 启动值变化动画
        if (!m_isDragging) {
            startValueAnimation(m_value);
        } else {
            m_animatedValue = m_value;
        }
        
        update();
        emit valueChanged(m_value);
    }
}

double QWinUISlider::minimum() const
{
    return m_minimum;
}

void QWinUISlider::setMinimum(double minimum)
{
    if (qAbs(m_minimum - minimum) > 1e-6) {
        m_minimum = minimum;
        if (m_maximum < m_minimum) {
            m_maximum = m_minimum;
        }
        setValue(constrainValue(m_value));
        update();
        emit minimumChanged(minimum);
    }
}

double QWinUISlider::maximum() const
{
    return m_maximum;
}

void QWinUISlider::setMaximum(double maximum)
{
    if (qAbs(m_maximum - maximum) > 1e-6) {
        m_maximum = maximum;
        if (m_minimum > m_maximum) {
            m_minimum = m_maximum;
        }
        setValue(constrainValue(m_value));
        update();
        emit maximumChanged(maximum);
    }
}

void QWinUISlider::setRange(double minimum, double maximum)
{
    m_minimum = minimum;
    m_maximum = qMax(minimum, maximum);
    setValue(constrainValue(m_value));
    update();
    emit minimumChanged(m_minimum);
    emit maximumChanged(m_maximum);
}

double QWinUISlider::stepSize() const
{
    return m_stepSize;
}

void QWinUISlider::setStepSize(double stepSize)
{
    if (qAbs(m_stepSize - stepSize) > 1e-6 && stepSize > 0) {
        m_stepSize = stepSize;
        emit stepSizeChanged(stepSize);
    }
}

bool QWinUISlider::showValueTooltip() const
{
    return m_showValueTooltip;
}

void QWinUISlider::setShowValueTooltip(bool show)
{
    if (m_showValueTooltip != show) {
        m_showValueTooltip = show;
        if (!show) {
            hideValueTooltip();
        }
        emit showValueTooltipChanged(show);
    }
}

// 刻度相关方法实现
bool QWinUISlider::showTickMarks() const
{
    return m_showTickMarks;
}

void QWinUISlider::setShowTickMarks(bool show)
{
    if (m_showTickMarks != show) {
        m_showTickMarks = show;
        update();
        emit showTickMarksChanged(show);
    }
}

QWinUISlider::TickPlacement QWinUISlider::tickPlacement() const
{
    return m_tickPlacement;
}

void QWinUISlider::setTickPlacement(TickPlacement placement)
{
    if (m_tickPlacement != placement) {
        m_tickPlacement = placement;
        update();
        emit tickPlacementChanged(placement);
    }
}

QWinUISlider::TickMode QWinUISlider::tickMode() const
{
    return m_tickMode;
}

void QWinUISlider::setTickMode(TickMode mode)
{
    if (m_tickMode != mode) {
        m_tickMode = mode;
        update();
        emit tickModeChanged(mode);
    }
}

double QWinUISlider::tickInterval() const
{
    return m_tickInterval;
}

void QWinUISlider::setTickInterval(double interval)
{
    if (qAbs(m_tickInterval - interval) > 1e-6) {
        m_tickInterval = interval;
        update();
        emit tickIntervalChanged(interval);
    }
}

void QWinUISlider::setTickValues(const QList<double>& values)
{
    m_customTickValues = values;
    update();
}

QList<double> QWinUISlider::tickValues() const
{
    return m_customTickValues;
}

int QWinUISlider::animationDuration() const
{
    return m_animationDuration;
}

void QWinUISlider::setAnimationDuration(int duration)
{
    if (m_animationDuration != duration) {
        m_animationDuration = duration;
        
        // 更新动画持续时间
        if (m_valueAnimation) {
            m_valueAnimation->setDuration(duration);
        }
        if (m_thumbScaleAnimation) {
            m_thumbScaleAnimation->setDuration(duration);
        }
        if (m_trackHoverAnimation) {
            m_trackHoverAnimation->setDuration(duration);
        }
        
        emit animationDurationChanged(duration);
    }
}

QSize QWinUISlider::sizeHint() const
{
    if (m_orientation == Qt::Horizontal) {
        return QSize(200, THUMB_OUTER_SIZE + 10);
    } else {
        return QSize(THUMB_OUTER_SIZE + 10, 200);
    }
}

QSize QWinUISlider::minimumSizeHint() const
{
    if (m_orientation == Qt::Horizontal) {
        return QSize(MINIMUM_LENGTH, THUMB_OUTER_SIZE + 10);
    } else {
        return QSize(THUMB_OUTER_SIZE + 10, MINIMUM_LENGTH);
    }
}

// 槽函数
void QWinUISlider::increaseValue()
{
    setValue(m_value + m_stepSize);
}

void QWinUISlider::decreaseValue()
{
    setValue(m_value - m_stepSize);
}

void QWinUISlider::pageUp()
{
    setValue(m_value + m_stepSize * PAGE_STEP_MULTIPLIER);
}

void QWinUISlider::pageDown()
{
    setValue(m_value - m_stepSize * PAGE_STEP_MULTIPLIER);
}

void QWinUISlider::toMinimum()
{
    setValue(m_minimum);
}

void QWinUISlider::toMaximum()
{
    setValue(m_maximum);
}

// 事件处理
void QWinUISlider::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect = this->rect();

    // WinUI3 风格：不绘制外边框，保持简洁
    // 焦点状态通过其他方式表示（如拇指动画）

    // 绘制轨道
    drawTrack(&painter, rect);

    // 绘制进度
    drawProgress(&painter, rect);

    // 绘制刻度（在拇指之前绘制，避免被遮挡）
    if (m_showTickMarks) {
        drawTickMarks(&painter, rect);
    }

    // 绘制拇指
    drawThumb(&painter, rect);
}

void QWinUISlider::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && isEnabled()) {
        m_isPressed = true;
        m_isDragging = true;
        m_lastMousePos = event->pos();

        // WinUI3 按下效果：内圆变小
        startInnerCircleAnimation(0.75); // 内圆从12px缩小到9px (9/12 = 0.75)

        // 检查是否点击在拇指上
        QRect thumbRect = getThumbRect();
        if (thumbRect.contains(event->pos())) {
            // 点击在拇指上，开始拖拽
            setFocus();
        } else {
            // 点击在轨道上，跳转到该位置
            updateValueFromPosition(event->pos());
            setFocus();
        }

        emit sliderPressed();
        update();
    }
    QWinUIWidget::mousePressEvent(event);
}

void QWinUISlider::mouseMoveEvent(QMouseEvent* event)
{
    if (m_isDragging && isEnabled()) {
        updateValueFromPosition(event->pos());

        // 显示值工具提示
        if (m_showValueTooltip) {
            showValueTooltipAt(event->globalPosition().toPoint());
        }

        m_lastMousePos = event->pos();
    }
    QWinUIWidget::mouseMoveEvent(event);
}

void QWinUISlider::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_isPressed) {
        m_isPressed = false;
        m_isDragging = false;

        // WinUI3 释放效果：内圆恢复到悬浮大小或正常大小
        if (m_isHovered) {
            startInnerCircleAnimation(1.25); // 恢复到悬浮大小
        } else {
            startInnerCircleAnimation(1.0);  // 恢复到正常大小
        }

        // 隐藏工具提示
        if (m_showValueTooltip) {
            hideValueTooltip();
        }

        emit sliderReleased();
        update();
    }
    QWinUIWidget::mouseReleaseEvent(event);
}

void QWinUISlider::wheelEvent(QWheelEvent* event)
{
    if (isEnabled()) {
        double delta = event->angleDelta().y() / 120.0; // 标准滚轮步长
        setValue(m_value + delta * m_stepSize);
        event->accept();
    } else {
        QWinUIWidget::wheelEvent(event);
    }
}

void QWinUISlider::keyPressEvent(QKeyEvent* event)
{
    if (!isEnabled()) {
        QWinUIWidget::keyPressEvent(event);
        return;
    }

    switch (event->key()) {
    case Qt::Key_Left:
        if (m_orientation == Qt::Horizontal) {
            decreaseValue();
            event->accept();
            return;
        }
        break;
    case Qt::Key_Right:
        if (m_orientation == Qt::Horizontal) {
            increaseValue();
            event->accept();
            return;
        }
        break;
    case Qt::Key_Up:
        if (m_orientation == Qt::Vertical) {
            increaseValue();
            event->accept();
            return;
        }
        break;
    case Qt::Key_Down:
        if (m_orientation == Qt::Vertical) {
            decreaseValue();
            event->accept();
            return;
        }
        break;
    case Qt::Key_PageUp:
        pageUp();
        event->accept();
        return;
    case Qt::Key_PageDown:
        pageDown();
        event->accept();
        return;
    case Qt::Key_Home:
        toMinimum();
        event->accept();
        return;
    case Qt::Key_End:
        toMaximum();
        event->accept();
        return;
    }

    QWinUIWidget::keyPressEvent(event);
}

void QWinUISlider::enterEvent(QEnterEvent* event)
{
    if (isEnabled()) {
        m_isHovered = true;
        // WinUI3 悬浮效果：内圆适度变大
        startInnerCircleAnimation(1.25); // 内圆从12px放大到15px (15/12 = 1.25)
        startTrackHoverAnimation(1.0);

        // 启动工具提示定时器
        if (m_showValueTooltip) {
            m_tooltipTimer->start();
        }
    }
    QWinUIWidget::enterEvent(event);
}

void QWinUISlider::leaveEvent(QEvent* event)
{
    m_isHovered = false;
    // WinUI3 离开效果：内圆恢复原始大小
    if (!m_isPressed) { // 如果没有按下，才恢复正常大小
        startInnerCircleAnimation(1.0); // 内圆恢复到12px
    }
    startTrackHoverAnimation(0.0);

    // 停止工具提示定时器并隐藏工具提示
    m_tooltipTimer->stop();
    if (m_showValueTooltip) {
        hideValueTooltip();
    }

    QWinUIWidget::leaveEvent(event);
}

void QWinUISlider::focusInEvent(QFocusEvent* event)
{
    m_isFocused = true;
    update();
    QWinUIWidget::focusInEvent(event);
}

void QWinUISlider::focusOutEvent(QFocusEvent* event)
{
    m_isFocused = false;
    update();
    QWinUIWidget::focusOutEvent(event);
}

void QWinUISlider::resizeEvent(QResizeEvent* event)
{
    QWinUIWidget::resizeEvent(event);
    update();
}

void QWinUISlider::onThemeChanged()
{
    update();
}

// 动画槽函数
void QWinUISlider::onValueAnimationFinished()
{
    // 值动画完成
}

void QWinUISlider::onThumbScaleAnimationFinished()
{
    // 拇指缩放动画完成
}

void QWinUISlider::onTrackHoverAnimationFinished()
{
    // 轨道悬浮动画完成
}

void QWinUISlider::updateValueTooltip()
{
    if (m_showValueTooltip && m_isHovered && m_valueTooltip) {
        QPoint thumbCenter = getThumbCenter();
        QPoint globalPos = mapToGlobal(thumbCenter);

        // 更新工具提示文本
        QString text = QString::number(m_value, 'f', (m_stepSize < 1.0) ? 1 : 0);
        m_valueTooltip->setText(text);

        // 显示工具提示
        m_valueTooltip->showToolTip(globalPos);
    }
}

// 绘制方法 - WinUI3 风格
void QWinUISlider::drawTrack(QPainter* painter, const QRect& rect)
{
    QWinUITheme* theme = QWinUITheme::getInstance();
    QRect trackRect = getTrackRect();

    // WinUI3 轨道颜色规范
    QColor trackColor;
    if (theme) {
        if (theme->isDarkMode()) {
            // 深色主题：使用半透明白色
            trackColor = QColor(255, 255, 255, 26); // rgba(255,255,255,0.1)
        } else {
            // 浅色主题：使用半透明黑色
            trackColor = QColor(0, 0, 0, 26); // rgba(0,0,0,0.1)
        }
    } else {
        trackColor = QColor(200, 200, 200);
    }

    if (!isEnabled()) {
        trackColor.setAlpha(trackColor.alpha() / 2);
    }

    // WinUI3 悬浮效果：轨道在悬浮时不变色，只有拇指变化
    // 这里保持轨道颜色不变，符合WinUI3设计

    // 绘制轨道 - 使用完全圆角
    painter->setPen(Qt::NoPen);
    painter->setBrush(trackColor);

    int radius = (m_orientation == Qt::Horizontal) ? TRACK_HEIGHT / 2 : TRACK_WIDTH / 2;
    painter->drawRoundedRect(trackRect, radius, radius);
}

void QWinUISlider::drawProgress(QPainter* painter, const QRect& rect)
{
    QWinUITheme* theme = QWinUITheme::getInstance();
    QRect trackRect = getTrackRect();

    // 计算进度矩形
    QRect progressRect = trackRect;
    double progress = (m_animatedValue - m_minimum) / (m_maximum - m_minimum);

    if (m_orientation == Qt::Horizontal) {
        int progressWidth = trackRect.width() * progress;
        progressRect.setWidth(progressWidth);
    } else {
        int progressHeight = trackRect.height() * progress;
        progressRect.setTop(trackRect.bottom() - progressHeight);
    }

    // WinUI3 进度颜色：使用系统强调色
    QColor progressColor = theme ? theme->accentColor() : QColor(0, 120, 215);

    if (!isEnabled()) {
        // 禁用状态：降低不透明度
        progressColor.setAlpha(progressColor.alpha() / 3);
    }

    // 绘制进度条 - 使用完全圆角
    painter->setPen(Qt::NoPen);
    painter->setBrush(progressColor);

    int radius = (m_orientation == Qt::Horizontal) ? TRACK_HEIGHT / 2 : TRACK_WIDTH / 2;
    painter->drawRoundedRect(progressRect, radius, radius);
}

void QWinUISlider::drawThumb(QPainter* painter, const QRect& rect)
{
    QWinUITheme* theme = QWinUITheme::getInstance();
    QPoint thumbCenter = getThumbCenter();

    // WinUI3 双圆设计：外圆（白色背景）+ 内圆（主题色，有动画）

    // 外圆（白色背景圆）- 固定大小，不参与动画
    int outerSize = THUMB_OUTER_SIZE;
    QRect outerRect(thumbCenter.x() - outerSize/2, thumbCenter.y() - outerSize/2, outerSize, outerSize);

    // 内圆（主题色圆）- 参与动画
    int innerSize = THUMB_INNER_SIZE;
    if (m_innerCircleScale != 1.0) {
        innerSize = THUMB_INNER_SIZE * m_innerCircleScale;
    }
    QRect innerRect(thumbCenter.x() - innerSize/2, thumbCenter.y() - innerSize/2, innerSize, innerSize);

    // WinUI3 颜色规范
    QColor outerColor = QColor(255, 255, 255); // 外圆始终是白色
    QColor innerColor = theme ? theme->accentColor() : QColor(0, 120, 215); // 内圆是主题色

    if (!isEnabled()) {
        outerColor.setAlpha(outerColor.alpha() / 2);
        innerColor.setAlpha(innerColor.alpha() / 2);
    }

    // WinUI3 阴影效果：柔和的阴影
    if (isEnabled()) {
        QRect shadowRect = outerRect.adjusted(0, 1, 2, 3);
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(0, 0, 0, 12)); // 柔和的阴影
        painter->drawEllipse(shadowRect);
    }

    // 绘制外圆（白色，无边框）
    painter->setPen(Qt::NoPen);
    painter->setBrush(outerColor);
    painter->drawEllipse(outerRect);

    // 绘制内圆（主题色，无边框）
    painter->setPen(Qt::NoPen);
    painter->setBrush(innerColor);
    painter->drawEllipse(innerRect);
}

void QWinUISlider::drawFocusRect(QPainter* painter, const QRect& rect)
{
    QWinUITheme* theme = QWinUITheme::getInstance();
    QColor focusColor = theme ? theme->accentColor() : QColor(0, 120, 215);

    painter->setPen(QPen(focusColor, FOCUS_BORDER_WIDTH));
    painter->setBrush(Qt::NoBrush);
    painter->drawRoundedRect(rect.adjusted(1, 1, -1, -1), 4, 4);
}

// 几何计算方法
QRect QWinUISlider::getTrackRect() const
{
    QRect rect = this->rect();

    if (m_orientation == Qt::Horizontal) {
        int trackY = rect.center().y() - TRACK_HEIGHT / 2;
        int margin = THUMB_OUTER_SIZE / 2;
        return QRect(margin, trackY, rect.width() - 2 * margin, TRACK_HEIGHT);
    } else {
        int trackX = rect.center().x() - TRACK_WIDTH / 2;
        int margin = THUMB_OUTER_SIZE / 2;
        return QRect(trackX, margin, TRACK_WIDTH, rect.height() - 2 * margin);
    }
}

QRect QWinUISlider::getThumbRect() const
{
    QPoint center = getThumbCenter();
    return QRect(center.x() - THUMB_OUTER_SIZE/2, center.y() - THUMB_OUTER_SIZE/2, THUMB_OUTER_SIZE, THUMB_OUTER_SIZE);
}

QPoint QWinUISlider::getThumbCenter() const
{
    QRect trackRect = getTrackRect();
    double progress = (m_animatedValue - m_minimum) / (m_maximum - m_minimum);

    if (m_orientation == Qt::Horizontal) {
        int x = trackRect.left() + trackRect.width() * progress;
        return QPoint(x, trackRect.center().y());
    } else {
        int y = trackRect.bottom() - trackRect.height() * progress;
        return QPoint(trackRect.center().x(), y);
    }
}

double QWinUISlider::positionToValue(const QPoint& pos) const
{
    QRect trackRect = getTrackRect();
    double progress;

    if (m_orientation == Qt::Horizontal) {
        progress = double(pos.x() - trackRect.left()) / trackRect.width();
    } else {
        progress = double(trackRect.bottom() - pos.y()) / trackRect.height();
    }

    progress = qBound(0.0, progress, 1.0);
    return m_minimum + progress * (m_maximum - m_minimum);
}

QPoint QWinUISlider::valueToPosition(double value) const
{
    QRect trackRect = getTrackRect();
    double progress = (value - m_minimum) / (m_maximum - m_minimum);

    if (m_orientation == Qt::Horizontal) {
        int x = trackRect.left() + trackRect.width() * progress;
        return QPoint(x, trackRect.center().y());
    } else {
        int y = trackRect.bottom() - trackRect.height() * progress;
        return QPoint(trackRect.center().x(), y);
    }
}

// 动画控制方法
void QWinUISlider::startValueAnimation(double targetValue)
{
    if (m_valueAnimation->state() == QAbstractAnimation::Running) {
        m_valueAnimation->stop();
    }

    m_valueAnimation->setStartValue(m_animatedValue);
    m_valueAnimation->setEndValue(targetValue);
    m_valueAnimation->start();
}

void QWinUISlider::startThumbScaleAnimation(double targetScale)
{
    if (m_thumbScaleAnimation->state() == QAbstractAnimation::Running) {
        m_thumbScaleAnimation->stop();
    }

    m_thumbScaleAnimation->setStartValue(m_thumbScale);
    m_thumbScaleAnimation->setEndValue(targetScale);
    m_thumbScaleAnimation->start();
}

void QWinUISlider::startInnerCircleAnimation(double targetScale)
{
    if (m_innerCircleAnimation->state() == QAbstractAnimation::Running) {
        m_innerCircleAnimation->stop();
    }

    m_innerCircleAnimation->setStartValue(m_innerCircleScale);
    m_innerCircleAnimation->setEndValue(targetScale);
    m_innerCircleAnimation->start();
}

void QWinUISlider::startTrackHoverAnimation(double targetProgress)
{
    if (m_trackHoverAnimation->state() == QAbstractAnimation::Running) {
        m_trackHoverAnimation->stop();
    }

    m_trackHoverAnimation->setStartValue(m_trackHoverProgress);
    m_trackHoverAnimation->setEndValue(targetProgress);
    m_trackHoverAnimation->start();
}

// 值处理方法
double QWinUISlider::constrainValue(double value) const
{
    return qBound(m_minimum, value, m_maximum);
}

double QWinUISlider::snapToStep(double value) const
{
    if (m_stepSize <= 0) return value;

    double steps = qRound((value - m_minimum) / m_stepSize);
    return m_minimum + steps * m_stepSize;
}

void QWinUISlider::updateValueFromPosition(const QPoint& pos)
{
    double newValue = positionToValue(pos);
    newValue = snapToStep(newValue);
    setValue(newValue);
}

// 工具提示方法 - 使用自定义QWinUIToolTip
void QWinUISlider::showValueTooltipAt(const QPoint& pos)
{
    if (m_valueTooltip) {
        QString text = QString::number(m_value, 'f', (m_stepSize < 1.0) ? 1 : 0);
        m_valueTooltip->setText(text);
        m_valueTooltip->showToolTip(pos);
    }
}

void QWinUISlider::hideValueTooltip()
{
    if (m_valueTooltip) {
        m_valueTooltip->hideToolTip();
    }
}

// 状态检查方法
bool QWinUISlider::isHovered() const
{
    return m_isHovered;
}

bool QWinUISlider::isPressed() const
{
    return m_isPressed;
}

bool QWinUISlider::isFocused() const
{
    return m_isFocused;
}

// 刻度相关方法实现
QList<double> QWinUISlider::calculateTickValues() const
{
    QList<double> tickValues;

    if (m_tickMode == TicksByStepSize) {
        // 按步长计算刻度
        double current = m_minimum;
        while (current <= m_maximum + 1e-6) { // 添加小的容差
            tickValues.append(current);
            current += m_stepSize;
        }
    } else if (m_tickMode == TicksByInterval) {
        // 按指定间隔计算刻度
        if (m_tickInterval > 0) {
            double current = m_minimum;
            while (current <= m_maximum + 1e-6) { // 添加小的容差
                tickValues.append(current);
                current += m_tickInterval;
            }
        }
    }

    // 如果有自定义刻度值，使用自定义值
    if (!m_customTickValues.isEmpty()) {
        tickValues.clear();
        for (double value : m_customTickValues) {
            if (value >= m_minimum && value <= m_maximum) {
                tickValues.append(value);
            }
        }
    }

    return tickValues;
}

QRect QWinUISlider::getTickRect() const
{
    QRect rect = this->rect();
    QRect trackRect = getTrackRect();

    if (m_orientation == Qt::Horizontal) {
        // 水平滑块的刻度区域
        int tickY = trackRect.bottom() + TICK_SPACING;
        return QRect(trackRect.left(), tickY, trackRect.width(), TICK_LENGTH);
    } else {
        // 垂直滑块的刻度区域
        int tickX = trackRect.right() + TICK_SPACING;
        return QRect(tickX, trackRect.top(), TICK_LENGTH, trackRect.height());
    }
}

void QWinUISlider::drawTickMarks(QPainter* painter, const QRect& rect)
{
    if (!m_showTickMarks || m_tickPlacement == NoTicks) {
        return;
    }

    QWinUITheme* theme = QWinUITheme::getInstance();
    QColor tickColor;

    if (theme) {
        if (theme->isDarkMode()) {
            tickColor = QColor(255, 255, 255, 128); // 半透明白色
        } else {
            tickColor = QColor(0, 0, 0, 128); // 半透明黑色
        }
    } else {
        tickColor = QColor(128, 128, 128);
    }

    if (!isEnabled()) {
        tickColor.setAlpha(tickColor.alpha() / 2);
    }

    painter->setPen(QPen(tickColor, TICK_WIDTH));

    QList<double> tickValues = calculateTickValues();
    QRect trackRect = getTrackRect();

    for (double value : tickValues) {
        QPoint pos = valueToPosition(value);

        if (m_orientation == Qt::Horizontal) {
            int x = pos.x();

            // 绘制下方刻度
            if (m_tickPlacement == TicksBelow || m_tickPlacement == TicksBothSides) {
                int startY = trackRect.bottom() + TICK_SPACING;
                int endY = startY + TICK_LENGTH;
                painter->drawLine(x, startY, x, endY);
            }

            // 绘制上方刻度
            if (m_tickPlacement == TicksAbove || m_tickPlacement == TicksBothSides) {
                int endY = trackRect.top() - TICK_SPACING;
                int startY = endY - TICK_LENGTH;
                painter->drawLine(x, startY, x, endY);
            }
        } else {
            int y = pos.y();

            // 绘制右侧刻度
            if (m_tickPlacement == TicksBelow || m_tickPlacement == TicksBothSides) {
                int startX = trackRect.right() + TICK_SPACING;
                int endX = startX + TICK_LENGTH;
                painter->drawLine(startX, y, endX, y);
            }

            // 绘制左侧刻度
            if (m_tickPlacement == TicksAbove || m_tickPlacement == TicksBothSides) {
                int endX = trackRect.left() - TICK_SPACING;
                int startX = endX - TICK_LENGTH;
                painter->drawLine(startX, y, endX, y);
            }
        }
    }
}
