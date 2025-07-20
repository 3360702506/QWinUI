#include "QWinUI/Controls/QWinUIProgressRing.h"
#include "QWinUI/QWinUITheme.h"
#include <QPainter>
#include <QPainterPath>
#include <QResizeEvent>
#include <QDateTime>
#include <QtMath>
#include <QDebug>

QT_BEGIN_NAMESPACE

QWinUIProgressRing::QWinUIProgressRing(QWidget* parent)
    : QWinUIWidget(parent)
    , m_value(DEFAULT_VALUE)
    , m_minimum(DEFAULT_MINIMUM)
    , m_maximum(DEFAULT_MAXIMUM)
    , m_isIndeterminate(false)
    , m_isActive(true)
    , m_ringThickness(DEFAULT_RING_THICKNESS)
    , m_useThemeColors(true)
    , m_valueAnimation(nullptr)
    , m_indeterminateTimer(new QTimer(this))
    , m_animatedValue(DEFAULT_VALUE)
    , m_targetValue(DEFAULT_VALUE)
    , m_indeterminateAngle(0.0)
    , m_indeterminateSpan(INDETERMINATE_MIN_SPAN)
    , m_indeterminateDirection(true)
    , m_indeterminatePhase(0.0)
    , m_lastUpdateTime(0)
    , m_animationPaused(false)
{
    initializeComponent();
}

QWinUIProgressRing::~QWinUIProgressRing()
{
    if (m_valueAnimation) {
        m_valueAnimation->stop();
    }
    m_indeterminateTimer->stop();
}

void QWinUIProgressRing::initializeComponent()
{
    // 设置基本属性
    setFixedSize(DEFAULT_SIZE, DEFAULT_SIZE);
    setAttribute(Qt::WA_TranslucentBackground, false);
    
    // 初始化颜色
    updateColors();
    
    // 设置不确定动画定时器
    m_indeterminateTimer->setInterval(INDETERMINATE_TIMER_INTERVAL);
    connect(m_indeterminateTimer, &QTimer::timeout, this, &QWinUIProgressRing::updateIndeterminateAnimation);
    
    // 创建值动画 - 使用自定义属性以获得更好的控制
    m_valueAnimation = new QPropertyAnimation(this, "animatedValue", this);
    m_valueAnimation->setDuration(VALUE_ANIMATION_DURATION);
    m_valueAnimation->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_valueAnimation, &QPropertyAnimation::valueChanged, this, [this](const QVariant& value) {
        m_animatedValue = value.toDouble();
        update(); // 只在动画值变化时重绘
    });
    connect(m_valueAnimation, &QPropertyAnimation::finished, this, &QWinUIProgressRing::onValueAnimationFinished);
    
    // 设置工具提示
    setToolTipText("进度环");
}

void QWinUIProgressRing::updateColors()
{
    if (m_useThemeColors) {
        m_ringColor = getThemeRingColor();
        m_backgroundRingColor = getThemeBackgroundRingColor();
    }
}

double QWinUIProgressRing::value() const
{
    return m_value;
}

void QWinUIProgressRing::setValue(double value)
{
    double newValue = qBound(m_minimum, value, m_maximum);
    if (qFuzzyCompare(m_value, newValue)) {
        return;
    }

    m_value = newValue;
    m_targetValue = newValue;

    // 启动平滑动画到新值
    if (m_valueAnimation->state() == QPropertyAnimation::Running) {
        // 如果动画正在运行，从当前动画值开始新动画
        m_valueAnimation->stop();
    }

    m_valueAnimation->setStartValue(m_animatedValue);
    m_valueAnimation->setEndValue(m_targetValue);
    m_valueAnimation->start();

    emit valueChanged(m_value);
}

double QWinUIProgressRing::animatedValue() const
{
    return m_animatedValue;
}

void QWinUIProgressRing::setAnimatedValue(double value)
{
    if (qFuzzyCompare(m_animatedValue, value)) {
        return;
    }
    m_animatedValue = value;
    // 不发射信号，这是内部动画属性
}

double QWinUIProgressRing::minimum() const
{
    return m_minimum;
}

void QWinUIProgressRing::setMinimum(double minimum)
{
    if (qFuzzyCompare(m_minimum, minimum)) {
        return;
    }
    
    m_minimum = minimum;
    if (m_maximum < m_minimum) {
        m_maximum = m_minimum;
        emit maximumChanged(m_maximum);
    }
    if (m_value < m_minimum) {
        setValue(m_minimum);
    }
    emit minimumChanged(m_minimum);
    update();
}

double QWinUIProgressRing::maximum() const
{
    return m_maximum;
}

void QWinUIProgressRing::setMaximum(double maximum)
{
    if (qFuzzyCompare(m_maximum, maximum)) {
        return;
    }
    
    m_maximum = maximum;
    if (m_minimum > m_maximum) {
        m_minimum = m_maximum;
        emit minimumChanged(m_minimum);
    }
    if (m_value > m_maximum) {
        setValue(m_maximum);
    }
    emit maximumChanged(m_maximum);
    update();
}

void QWinUIProgressRing::setRange(double minimum, double maximum)
{
    setMinimum(minimum);
    setMaximum(maximum);
}

bool QWinUIProgressRing::isIndeterminate() const
{
    return m_isIndeterminate;
}

void QWinUIProgressRing::setIsIndeterminate(bool indeterminate)
{
    if (m_isIndeterminate == indeterminate) {
        return;
    }
    
    m_isIndeterminate = indeterminate;
    
    if (m_isIndeterminate && m_isActive) {
        startIndeterminateAnimation();
    } else {
        stopIndeterminateAnimation();
    }
    
    emit isIndeterminateChanged(m_isIndeterminate);
    update();
}

bool QWinUIProgressRing::isActive() const
{
    return m_isActive;
}

void QWinUIProgressRing::setIsActive(bool active)
{
    if (m_isActive == active) {
        return;
    }
    
    m_isActive = active;
    
    if (m_isActive && m_isIndeterminate) {
        startIndeterminateAnimation();
    } else {
        stopIndeterminateAnimation();
    }
    
    emit isActiveChanged(m_isActive);
    update();
}

int QWinUIProgressRing::ringThickness() const
{
    return m_ringThickness;
}

void QWinUIProgressRing::setRingThickness(int thickness)
{
    thickness = qMax(1, thickness);
    if (m_ringThickness == thickness) {
        return;
    }
    
    m_ringThickness = thickness;
    emit ringThicknessChanged(m_ringThickness);
    update();
}

QColor QWinUIProgressRing::ringColor() const
{
    return m_ringColor;
}

void QWinUIProgressRing::setRingColor(const QColor& color)
{
    if (m_ringColor == color) {
        return;
    }
    
    m_ringColor = color;
    m_useThemeColors = false;
    emit ringColorChanged(m_ringColor);
    update();
}

QColor QWinUIProgressRing::backgroundRingColor() const
{
    return m_backgroundRingColor;
}

void QWinUIProgressRing::setBackgroundRingColor(const QColor& color)
{
    if (m_backgroundRingColor == color) {
        return;
    }
    
    m_backgroundRingColor = color;
    m_useThemeColors = false;
    emit backgroundRingColorChanged(m_backgroundRingColor);
    update();
}

double QWinUIProgressRing::percentage() const
{
    if (qFuzzyCompare(m_maximum, m_minimum)) {
        return 0.0;
    }
    return (m_value - m_minimum) / (m_maximum - m_minimum) * 100.0;
}

void QWinUIProgressRing::setPercentage(double percentage)
{
    percentage = qBound(0.0, percentage, 100.0);
    double newValue = m_minimum + (m_maximum - m_minimum) * (percentage / 100.0);
    setValue(newValue);
}

QSize QWinUIProgressRing::sizeHint() const
{
    return QSize(DEFAULT_SIZE, DEFAULT_SIZE);
}

QSize QWinUIProgressRing::minimumSizeHint() const
{
    int minSize = m_ringThickness * 4;
    return QSize(minSize, minSize);
}

void QWinUIProgressRing::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    if (m_isActive) {
        drawProgressRing(painter);
    }
}

void QWinUIProgressRing::resizeEvent(QResizeEvent* event)
{
    QWinUIWidget::resizeEvent(event);
    update();
}

void QWinUIProgressRing::showEvent(QShowEvent* event)
{
    QWinUIWidget::showEvent(event);
    m_animationPaused = false;

    if (m_isActive && m_isIndeterminate) {
        startIndeterminateAnimation();
    }

    // 确保动画值与当前值同步
    if (qAbs(m_animatedValue - m_value) > ANIMATION_EPSILON) {
        m_animatedValue = m_value;
        update();
    }
}

void QWinUIProgressRing::hideEvent(QHideEvent* event)
{
    QWinUIWidget::hideEvent(event);
    stopIndeterminateAnimation();
    m_animationPaused = true;

    // 停止值动画以节省资源
    if (m_valueAnimation->state() == QPropertyAnimation::Running) {
        m_valueAnimation->pause();
    }
}

void QWinUIProgressRing::onThemeChanged()
{
    updateColors();
    update();
    QWinUIWidget::onThemeChanged();
}

void QWinUIProgressRing::updateIndeterminateAnimation()
{
    if (m_animationPaused || !isVisible()) {
        return; // 不可见时暂停动画以节省资源
    }

    // 计算时间差以确保帧率无关的动画
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    if (m_lastUpdateTime == 0) {
        m_lastUpdateTime = currentTime;
        return;
    }

    double deltaTime = (currentTime - m_lastUpdateTime) / 1000.0; // 转换为秒
    m_lastUpdateTime = currentTime;

    // 更新动画相位 - 加快呼吸效果
    m_indeterminatePhase += deltaTime * 4.0; // 4弧度/秒的基础速度 - 加快
    if (m_indeterminatePhase >= 2.0 * M_PI) {
        m_indeterminatePhase -= 2.0 * M_PI;
    }

    // 平滑的旋转动画 - 更快的旋转
    m_indeterminateAngle += INDETERMINATE_ROTATION_SPEED;
    if (m_indeterminateAngle >= 360.0) {
        m_indeterminateAngle -= 360.0;
    }

    // 更自然的呼吸效果 - 使用正弦波，更快的变化
    double breathingFactor = (qSin(m_indeterminatePhase * 1.5) + 1.0) / 2.0; // 0到1之间，频率更高
    m_indeterminateSpan = INDETERMINATE_MIN_SPAN +
                         (INDETERMINATE_MAX_SPAN - INDETERMINATE_MIN_SPAN) * breathingFactor;

    update();
}

void QWinUIProgressRing::onValueAnimationFinished()
{
    // 动画完成后的处理
}

void QWinUIProgressRing::startIndeterminateAnimation()
{
    if (!m_indeterminateTimer->isActive() && isVisible()) {
        m_indeterminateAngle = 0.0;
        m_indeterminateSpan = INDETERMINATE_MIN_SPAN;
        m_indeterminatePhase = 0.0;
        m_lastUpdateTime = 0;
        m_animationPaused = false;
        m_indeterminateTimer->start();
    }
}

void QWinUIProgressRing::stopIndeterminateAnimation()
{
    m_indeterminateTimer->stop();
    m_animationPaused = true;
    m_lastUpdateTime = 0;
}

void QWinUIProgressRing::startValueAnimation(double targetValue)
{
    if (m_valueAnimation->state() == QPropertyAnimation::Running) {
        m_valueAnimation->stop();
    }

    m_valueAnimation->setStartValue(m_value);
    m_valueAnimation->setEndValue(targetValue);
    m_valueAnimation->start();
}

void QWinUIProgressRing::drawProgressRing(QPainter& painter)
{
    QRectF ringRect = getRingRect();

    if (m_isIndeterminate) {
        drawIndeterminateRing(painter, ringRect);
    } else {
        drawDeterminateRing(painter, ringRect);
    }
}

void QWinUIProgressRing::drawDeterminateRing(QPainter& painter, const QRectF& rect)
{
    QPen backgroundPen(m_backgroundRingColor, m_ringThickness, Qt::SolidLine, Qt::RoundCap);
    QPen progressPen(m_ringColor, m_ringThickness, Qt::SolidLine, Qt::RoundCap);

    // 绘制背景环
    painter.setPen(backgroundPen);
    painter.drawEllipse(rect);

    // 绘制进度环 - 使用动画值
    if (m_animatedValue > m_minimum) {
        double progress = normalizeValue(m_animatedValue);
        double spanAngle = progress * 360.0;

        // 确保最小可见进度
        if (spanAngle > 0 && spanAngle < 3.0) {
            spanAngle = 3.0;
        }

        painter.setPen(progressPen);
        painter.drawArc(rect, 90 * 16, -spanAngle * 16); // 从顶部开始，顺时针
    }
}

void QWinUIProgressRing::drawIndeterminateRing(QPainter& painter, const QRectF& rect)
{
    QPen backgroundPen(m_backgroundRingColor, m_ringThickness, Qt::SolidLine, Qt::RoundCap);
    QPen progressPen(m_ringColor, m_ringThickness, Qt::SolidLine, Qt::RoundCap);

    // 绘制背景环（淡化）
    QColor fadedBackground = m_backgroundRingColor;
    fadedBackground.setAlpha(fadedBackground.alpha() * 0.3);
    backgroundPen.setColor(fadedBackground);
    painter.setPen(backgroundPen);
    painter.drawEllipse(rect);

    // 绘制不确定进度弧
    painter.setPen(progressPen);
    double startAngle = m_indeterminateAngle;
    double spanAngle = m_indeterminateSpan;

    painter.drawArc(rect, startAngle * 16, spanAngle * 16);
}

QRectF QWinUIProgressRing::getRingRect() const
{
    QRectF rect = this->rect();
    double margin = m_ringThickness / 2.0;
    return rect.adjusted(margin, margin, -margin, -margin);
}

double QWinUIProgressRing::normalizeValue(double value) const
{
    if (qFuzzyCompare(m_maximum, m_minimum)) {
        return 0.0;
    }
    return qBound(0.0, (value - m_minimum) / (m_maximum - m_minimum), 1.0);
}

QColor QWinUIProgressRing::getThemeRingColor() const
{
    if (theme()) {
        return theme()->accentColor();
    }
    return QColor(0, 120, 215); // 默认蓝色
}

QColor QWinUIProgressRing::getThemeBackgroundRingColor() const
{
    if (theme()) {
        if (theme()->themeMode() == QWinUIThemeMode::Dark) {
            return QColor(255, 255, 255, 25); // 深色模式下的浅色背景
        } else {
            return QColor(0, 0, 0, 25); // 浅色模式下的深色背景
        }
    }
    return QColor(0, 0, 0, 25);
}

QT_END_NAMESPACE
