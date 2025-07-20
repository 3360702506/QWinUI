#include "../../include/QWinUI/Controls/QWinUIProgressBar.h"
#include "../../include/QWinUI/QWinUITheme.h"
#include <QPainter>
#include <QFontMetrics>
#include <QPainterPath>
#include <QEasingCurve>
#include <cmath>

QT_BEGIN_NAMESPACE

// 静态成员初始化
const double QWinUIProgressBar::IndeterminateBlockMinWidth = 0.10; // 最小 10% 宽度
const double QWinUIProgressBar::IndeterminateBlockMaxWidth = 0.50; // 最大 50% 宽度

QWinUIProgressBar::QWinUIProgressBar(QWidget* parent)
    : QWinUIWidget(parent)
    , m_value(0.0)
    , m_minimum(0.0)
    , m_maximum(100.0)
    , m_isIndeterminate(false)
    , m_state(Normal)
    , m_showPercentage(false)
    , m_showValue(false)
    , m_autoToolTip(true)
    , m_animationDuration(DefaultAnimationDuration)
    , m_valueAnimation(nullptr)
    , m_indeterminateTimer(nullptr)
    , m_animatedValue(0.0)
    , m_indeterminatePosition(0.0)
    , m_isFirstCycle(true)
{
    initializeProgressBar();
}

QWinUIProgressBar::~QWinUIProgressBar()
{
    if (m_valueAnimation) {
        m_valueAnimation->stop();
    }
    if (m_indeterminateTimer) {
        m_indeterminateTimer->stop();
    }
}

void QWinUIProgressBar::initializeProgressBar()
{
    // 设置基本样式
    setCornerRadius(static_cast<int>(QWinUICornerRadius::Small)); // 2px圆角
    setShadowDepth(QWinUIShadowDepth::None);
    
    // 设置固定高度（总高度，容纳进度条突出部分）
    setFixedHeight(DefaultTotalHeight);

    // 设置初始工具提示
    updateAutoToolTip();
    
    // 设置动画
    setupAnimations();
    
    // 连接主题变化
    if (QWinUITheme* theme = QWinUITheme::getInstance()) {
        connect(theme, &QWinUITheme::themeChanged, this, &QWinUIProgressBar::onThemeChanged);
    }
}

void QWinUIProgressBar::setupAnimations()
{
    // 值变化动画
    m_valueAnimation = new QPropertyAnimation(this);
    m_valueAnimation->setDuration(m_animationDuration);
    m_valueAnimation->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_valueAnimation, &QPropertyAnimation::valueChanged, 
            this, &QWinUIProgressBar::onAnimationValueChanged);
    
    // 不确定状态动画定时器
    m_indeterminateTimer = new QTimer(this);
    m_indeterminateTimer->setInterval(16); // ~60 FPS
    connect(m_indeterminateTimer, &QTimer::timeout, 
            this, &QWinUIProgressBar::onIndeterminateAnimation);
}

double QWinUIProgressBar::value() const
{
    return m_value;
}

void QWinUIProgressBar::setValue(double value)
{
    double clampedValue = qBound(m_minimum, value, m_maximum);
    if (qFuzzyCompare(m_value, clampedValue)) {
        return;
    }
    
    // 如果当前是不确定状态，先停止不确定动画
    if (m_isIndeterminate) {
        setIndeterminate(false);
    }
    
    double oldValue = m_value;
    m_value = clampedValue;

    // 启动动画到新值
    startValueAnimation(m_value);

    // 更新自动 ToolTip
    if (m_autoToolTip) {
        updateAutoToolTip();
    }

    emit valueChanged(m_value);
}

double QWinUIProgressBar::minimum() const
{
    return m_minimum;
}

void QWinUIProgressBar::setMinimum(double minimum)
{
    if (qFuzzyCompare(m_minimum, minimum)) {
        return;
    }
    
    m_minimum = minimum;
    if (m_maximum < m_minimum) {
        m_maximum = m_minimum;
    }
    if (m_value < m_minimum) {
        setValue(m_minimum);
    }
    
    emit minimumChanged(m_minimum);
    update();
}

double QWinUIProgressBar::maximum() const
{
    return m_maximum;
}

void QWinUIProgressBar::setMaximum(double maximum)
{
    if (qFuzzyCompare(m_maximum, maximum)) {
        return;
    }
    
    m_maximum = maximum;
    if (m_minimum > m_maximum) {
        m_minimum = m_maximum;
    }
    if (m_value > m_maximum) {
        setValue(m_maximum);
    }
    
    emit maximumChanged(m_maximum);
    update();
}

bool QWinUIProgressBar::isIndeterminate() const
{
    return m_isIndeterminate;
}

void QWinUIProgressBar::setIndeterminate(bool indeterminate)
{
    if (m_isIndeterminate == indeterminate) {
        return;
    }
    
    m_isIndeterminate = indeterminate;

    if (m_isIndeterminate) {
        startIndeterminateAnimation();
    } else {
        stopIndeterminateAnimation();
    }

    // 更新自动 ToolTip
    if (m_autoToolTip) {
        updateAutoToolTip();
    }

    emit indeterminateChanged(m_isIndeterminate);
    update();
}

QWinUIProgressBar::QWinUIProgressBarState QWinUIProgressBar::state() const
{
    return m_state;
}

void QWinUIProgressBar::setState(QWinUIProgressBarState state)
{
    if (m_state == state) {
        return;
    }
    
    m_state = state;
    emit stateChanged(m_state);
    update();
}

bool QWinUIProgressBar::showPercentage() const
{
    return m_showPercentage;
}

void QWinUIProgressBar::setShowPercentage(bool show)
{
    if (m_showPercentage == show) {
        return;
    }

    m_showPercentage = show;
    emit showPercentageChanged(m_showPercentage);
    update();
}

bool QWinUIProgressBar::showValue() const
{
    return m_showValue;
}

void QWinUIProgressBar::setShowValue(bool show)
{
    if (m_showValue == show) {
        return;
    }

    m_showValue = show;
    emit showValueChanged(m_showValue);
    update();
}

bool QWinUIProgressBar::autoToolTip() const
{
    return m_autoToolTip;
}

void QWinUIProgressBar::setAutoToolTip(bool enable)
{
    if (m_autoToolTip == enable) {
        return;
    }

    m_autoToolTip = enable;

    if (enable) {
        updateAutoToolTip();
    } else {
        setToolTipText("");  // 清空 ToolTip
    }

    emit autoToolTipChanged(m_autoToolTip);
}

int QWinUIProgressBar::animationDuration() const
{
    return m_animationDuration;
}

void QWinUIProgressBar::setAnimationDuration(int duration)
{
    if (m_animationDuration == duration) {
        return;
    }
    
    m_animationDuration = qMax(0, duration);
    if (m_valueAnimation) {
        m_valueAnimation->setDuration(m_animationDuration);
    }
    
    emit animationDurationChanged(m_animationDuration);
}

void QWinUIProgressBar::setRange(double minimum, double maximum)
{
    setMinimum(minimum);
    setMaximum(maximum);
}

double QWinUIProgressBar::percentage() const
{
    if (qFuzzyCompare(m_maximum, m_minimum)) {
        return 0.0;
    }
    return (m_value - m_minimum) / (m_maximum - m_minimum) * 100.0;
}

void QWinUIProgressBar::reset()
{
    setValue(m_minimum);
}

QSize QWinUIProgressBar::sizeHint() const
{
    return QSize(200, DefaultTotalHeight);
}

QSize QWinUIProgressBar::minimumSizeHint() const
{
    return QSize(50, DefaultTotalHeight);
}

void QWinUIProgressBar::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect = this->rect();

    // 绘制背景
    drawBackground(&painter, rect);

    // 绘制进度
    if (m_isIndeterminate) {
        drawIndeterminateProgress(&painter, rect);
    } else {
        drawProgressFill(&painter, rect);
    }

    // 绘制文本（如果启用）
    if ((m_showPercentage || m_showValue) && !m_isIndeterminate) {
        drawPercentageText(&painter, rect);
    }
}

void QWinUIProgressBar::resizeEvent(QResizeEvent* event)
{
    QWinUIWidget::resizeEvent(event);
    update();
}

void QWinUIProgressBar::onAnimationValueChanged(const QVariant& value)
{
    m_animatedValue = value.toDouble();
    update();
}

void QWinUIProgressBar::onIndeterminateAnimation()
{
    // 使用平滑的速度变化，避免卡顿
    static double animationTime = 0.0;
    animationTime += 0.016; // 约 60 FPS

    // 基础速度
    double baseSpeed;
    double speedMultiplier;

    if (m_isFirstCycle) {
        // 第一次循环：较慢的速度，更温和的启动
        baseSpeed = 0.010; // 第一次较慢
        speedMultiplier = 0.8 + 0.4 * sin(animationTime * 1.5); // 0.4 到 1.2 倍速（较温和）
        speedMultiplier = qMax(0.4, speedMultiplier);
    } else {
        // 后续循环：正常速度
        baseSpeed = 0.018; // 正常速度
        speedMultiplier = 1.0 + 1.2 * sin(animationTime * 2.0); // 0.0 到 2.2 倍速
        speedMultiplier = qMax(0.3, speedMultiplier);
    }

    double currentSpeed = baseSpeed * speedMultiplier;

    // 更新不确定状态的位置
    m_indeterminatePosition += currentSpeed;

    // 重置位置时保持动画连续性
    if (m_indeterminatePosition > 1.0 + IndeterminateBlockMaxWidth) {
        m_indeterminatePosition = -IndeterminateBlockMaxWidth;
        m_isFirstCycle = false; // 第一次循环结束
        // 保持 animationTime 连续，避免动画重置
    }

    update();
}

void QWinUIProgressBar::onThemeChanged()
{
    update();
}

void QWinUIProgressBar::startValueAnimation(double targetValue)
{
    if (!m_valueAnimation) {
        return;
    }

    if (m_valueAnimation->state() == QPropertyAnimation::Running) {
        m_valueAnimation->stop();
    }

    double startValue = m_animatedValue;
    double endValue = (targetValue - m_minimum) / (m_maximum - m_minimum);

    m_valueAnimation->setStartValue(startValue);
    m_valueAnimation->setEndValue(endValue);
    m_valueAnimation->start();
}

void QWinUIProgressBar::startIndeterminateAnimation()
{
    if (m_indeterminateTimer) {
        m_indeterminatePosition = -IndeterminateBlockMaxWidth;

        // 重置第一次循环标志，确保每次启动都有温和的开始
        m_isFirstCycle = true;

        m_indeterminateTimer->start();
    }
}

void QWinUIProgressBar::stopIndeterminateAnimation()
{
    if (m_indeterminateTimer) {
        m_indeterminateTimer->stop();
    }
    m_indeterminatePosition = 0.0;
}

void QWinUIProgressBar::drawBackground(QPainter* painter, const QRect& rect)
{
    painter->save();

    QColor bgColor = getBackgroundColor();

    // 计算背景矩形（垂直居中，高度较小）
    int backgroundHeight = DefaultBackgroundHeight;
    int yOffset = (rect.height() - backgroundHeight) / 2;

    QRect backgroundRect = rect;
    backgroundRect.setTop(rect.top() + yOffset);
    backgroundRect.setHeight(backgroundHeight);

    // 创建圆角矩形路径
    QPainterPath path;
    path.addRoundedRect(backgroundRect, cornerRadius(), cornerRadius());

    // 绘制背景
    painter->fillPath(path, bgColor);

    painter->restore();
}

void QWinUIProgressBar::drawProgressFill(QPainter* painter, const QRect& rect)
{
    painter->save();

    QColor progressColor = getProgressColor();
    double progressWidth = getProgressWidth();

    if (progressWidth > 0) {
        // 计算进度矩形（垂直居中，高度比背景大）
        int progressHeight = DefaultProgressHeight;
        int yOffset = (rect.height() - progressHeight) / 2;

        QRect progressRect = rect;
        progressRect.setTop(rect.top() + yOffset);
        progressRect.setHeight(progressHeight);
        progressRect.setWidth(static_cast<int>(progressWidth));

        // 创建圆角矩形路径
        QPainterPath path;
        path.addRoundedRect(progressRect, cornerRadius(), cornerRadius());

        // 绘制进度
        painter->fillPath(path, progressColor);
    }

    painter->restore();
}

void QWinUIProgressBar::drawIndeterminateProgress(QPainter* painter, const QRect& rect)
{
    painter->save();

    QColor progressColor = getProgressColor();

    // 计算动态长度（使用平滑的余弦变化）
    // 使用余弦函数创建平滑的长度变化，避免突变
    double lengthCycle = m_indeterminatePosition * 3.5; // 增加变化频率
    double lengthFactor = (cos(lengthCycle) + 1.0) / 2.0; // 0.0 到 1.0 的平滑变化

    // 使用更大的变化范围
    lengthFactor = 0.2 + 0.8 * lengthFactor; // 0.2 到 1.0（更大变化范围）

    double currentBlockWidth = IndeterminateBlockMinWidth +
                              (IndeterminateBlockMaxWidth - IndeterminateBlockMinWidth) * lengthFactor;

    // 计算不确定进度块的位置和大小
    double blockWidth = rect.width() * currentBlockWidth;
    double blockStart = rect.width() * m_indeterminatePosition;

    // 计算进度矩形（垂直居中，高度比背景大）
    int progressHeight = DefaultProgressHeight;
    int yOffset = (rect.height() - progressHeight) / 2;

    QRect blockRect = rect;
    blockRect.setTop(rect.top() + yOffset);
    blockRect.setHeight(progressHeight);
    blockRect.setLeft(static_cast<int>(blockStart));
    blockRect.setWidth(static_cast<int>(blockWidth));

    // 确保块不超出边界
    if (blockRect.left() < rect.left()) {
        blockRect.setLeft(rect.left());
    }
    if (blockRect.right() > rect.right()) {
        blockRect.setRight(rect.right());
    }

    if (blockRect.width() > 0) {
        // 创建圆角矩形路径
        QPainterPath path;
        path.addRoundedRect(blockRect, cornerRadius(), cornerRadius());

        // 绘制不确定进度块
        painter->fillPath(path, progressColor);
    }

    painter->restore();
}

void QWinUIProgressBar::drawPercentageText(QPainter* painter, const QRect& rect)
{
    painter->save();

    QColor textColor = getTextColor();
    painter->setPen(textColor);
    painter->setFont(font());

    QString text;
    if (m_showPercentage) {
        // 显示百分比（带%符号）
        text = QString("%1%").arg(static_cast<int>(percentage()));
    } else if (m_showValue) {
        // 只显示数值（不带%符号）
        text = QString::number(static_cast<int>(m_value));
    }

    // 绘制文本
    painter->drawText(rect, Qt::AlignCenter, text);

    painter->restore();
}

QColor QWinUIProgressBar::getBackgroundColor() const
{
    QWinUITheme* theme = QWinUITheme::getInstance();
    if (!theme) {
        return isDarkMode() ? QColor(45, 45, 45) : QColor(230, 230, 230);
    }

    // 使用主题系统的控件背景色
    if (theme->isDarkMode()) {
        return QColor(45, 45, 45);  // 深色主题背景
    } else {
        return QColor(230, 230, 230);  // 浅色主题背景
    }
}

QColor QWinUIProgressBar::getProgressColor() const
{
    QWinUITheme* theme = QWinUITheme::getInstance();
    if (!theme) {
        // 默认颜色
        switch (m_state) {
        case Error:
            return QColor(196, 43, 28);  // 红色
        case Paused:
            return QColor(255, 185, 0);  // 黄色
        default:
            return QColor(0, 120, 215);  // 蓝色
        }
    }

    // 根据状态返回不同颜色
    switch (m_state) {
    case Error:
        return theme->isDarkMode() ? QColor(255, 99, 71) : QColor(196, 43, 28);
    case Paused:
        return theme->isDarkMode() ? QColor(255, 193, 7) : QColor(255, 185, 0);
    default:
        // 使用主题强调色
        return theme->accentColor();
    }
}

QColor QWinUIProgressBar::getTextColor() const
{
    QWinUITheme* theme = QWinUITheme::getInstance();
    if (!theme) {
        return isDarkMode() ? QColor(255, 255, 255) : QColor(0, 0, 0);
    }

    if (theme->isDarkMode()) {
        return QColor(255, 255, 255);  // 深色主题：白色文字
    } else {
        return QColor(0, 0, 0);        // 浅色主题：黑色文字
    }
}

QRect QWinUIProgressBar::getProgressRect() const
{
    return rect();
}

QRect QWinUIProgressBar::getTextRect() const
{
    return rect();
}

double QWinUIProgressBar::getProgressWidth() const
{
    if (qFuzzyCompare(m_maximum, m_minimum)) {
        return 0.0;
    }

    double progress = m_animatedValue;
    return rect().width() * progress;
}

void QWinUIProgressBar::updateAutoToolTip()
{
    if (!m_autoToolTip) {
        return;
    }

    if (m_isIndeterminate) {
        setToolTipText("进度未知");
    } else {
        // 只显示数值，不显示百分号
        QString tooltipText = QString::number(static_cast<int>(m_value));
        setToolTipText(tooltipText);
    }
}

QT_END_NAMESPACE
