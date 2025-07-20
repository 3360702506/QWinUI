#include "QWinUI/Controls/QWinUIScrollView.h"
#include "QWinUI/Controls/QWinUIScrollBar.h"
#include "QWinUI/QWinUITheme.h"
#include <QPainter>
#include <QPainterPath>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QScrollBar>
#include <QStyle>
#include <QtMath>

QT_BEGIN_NAMESPACE

QWinUIScrollView::QWinUIScrollView(QWidget* parent)
    : QWinUIWidget(parent)
    , m_scrollArea(nullptr)
    , m_contentWidget(nullptr)
    , m_horizontalScrollBar(nullptr)
    , m_verticalScrollBar(nullptr)
    , m_horizontalPolicy(Qt::ScrollBarAsNeeded)
    , m_verticalPolicy(Qt::ScrollBarAsNeeded)
    , m_smoothScrollingEnabled(true)
    , m_scrollAnimationDuration(300)
    , m_scrollEasingCurve(QEasingCurve::OutQuint)
    , m_horizontalScrollAnimation(nullptr)
    , m_verticalScrollAnimation(nullptr)
    , m_autoHideScrollBars(true)
    , m_scrollBarFadeDelay(800)
    , m_scrollBarFadeTimer(nullptr)
    , m_scrollBarsVisible(true)
    , m_isDragging(false)
    , m_dragDirection(Qt::Vertical)
    , m_dragDirectionLocked(false)
    , m_isScrolling(false)
{
    initializeScrollView();
}

QWinUIScrollView::~QWinUIScrollView()
{
    if (m_horizontalScrollAnimation) {
        m_horizontalScrollAnimation->stop();
        delete m_horizontalScrollAnimation;
    }
    
    if (m_verticalScrollAnimation) {
        m_verticalScrollAnimation->stop();
        delete m_verticalScrollAnimation;
    }
    
    if (m_scrollBarFadeTimer) {
        delete m_scrollBarFadeTimer;
    }
}

void QWinUIScrollView::initializeScrollView()
{
    // 设置基本属性
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_Hover, true);
    setMouseTracking(true);
    
    // 创建滚动区域
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setFrameStyle(QFrame::NoFrame);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setWidgetResizable(true);

    // 设置初始背景色
    QColor bgColor = themeBackgroundColor();
    m_scrollArea->viewport()->setStyleSheet(
        QString("QWidget { background-color: %1; }")
        .arg(bgColor.name(QColor::HexArgb))
    );

    // 安装事件过滤器来拦截滚轮事件
    m_scrollArea->installEventFilter(this);
    m_scrollArea->viewport()->installEventFilter(this);
    
    // 创建自定义滚动条
    m_horizontalScrollBar = new QWinUIScrollBar(Qt::Horizontal, this);
    m_verticalScrollBar = new QWinUIScrollBar(Qt::Vertical, this);
    
    // 设置滚动条属性
    m_horizontalScrollBar->setAutoHide(m_autoHideScrollBars);
    m_verticalScrollBar->setAutoHide(m_autoHideScrollBars);
    
    // 连接滚动条信号
    connect(m_horizontalScrollBar, &QWinUIScrollBar::valueChanged,
            this, &QWinUIScrollView::onHorizontalScrollValueChanged);
    connect(m_verticalScrollBar, &QWinUIScrollBar::valueChanged,
            this, &QWinUIScrollView::onVerticalScrollValueChanged);
    
    // 连接滚动区域的滚动条信号
    connect(m_scrollArea->horizontalScrollBar(), &QScrollBar::valueChanged,
            m_horizontalScrollBar, &QWinUIScrollBar::setValue);
    connect(m_scrollArea->verticalScrollBar(), &QScrollBar::valueChanged,
            m_verticalScrollBar, &QWinUIScrollBar::setValue);

    connect(m_scrollArea->horizontalScrollBar(), &QScrollBar::rangeChanged,
            this, [this](int min, int max) {
                m_horizontalScrollBar->setRange(min, max);
                m_horizontalScrollBar->setPageStep(m_scrollArea->horizontalScrollBar()->pageStep());
                updateScrollBarVisibility();
            });
    connect(m_scrollArea->verticalScrollBar(), &QScrollBar::rangeChanged,
            this, [this](int min, int max) {
                m_verticalScrollBar->setRange(min, max);
                m_verticalScrollBar->setPageStep(m_scrollArea->verticalScrollBar()->pageStep());
                updateScrollBarVisibility();
            });
    
    // 创建平滑滚动动画
    m_horizontalScrollAnimation = new QPropertyAnimation(m_scrollArea->horizontalScrollBar(), "value", this);
    m_horizontalScrollAnimation->setEasingCurve(QEasingCurve(m_scrollEasingCurve));
    m_horizontalScrollAnimation->setDuration(m_scrollAnimationDuration);
    connect(m_horizontalScrollAnimation, &QPropertyAnimation::finished,
            this, &QWinUIScrollView::onScrollAnimationFinished);
    connect(m_horizontalScrollAnimation, &QPropertyAnimation::valueChanged,
            this, [this]() { resetScrollBarFadeTimer(); });

    m_verticalScrollAnimation = new QPropertyAnimation(m_scrollArea->verticalScrollBar(), "value", this);
    m_verticalScrollAnimation->setEasingCurve(QEasingCurve(m_scrollEasingCurve));
    m_verticalScrollAnimation->setDuration(m_scrollAnimationDuration);
    connect(m_verticalScrollAnimation, &QPropertyAnimation::finished,
            this, &QWinUIScrollView::onScrollAnimationFinished);
    connect(m_verticalScrollAnimation, &QPropertyAnimation::valueChanged,
            this, [this]() { resetScrollBarFadeTimer(); });
    
    // 创建滚动条淡出定时器
    m_scrollBarFadeTimer = new QTimer(this);
    m_scrollBarFadeTimer->setSingleShot(true);
    m_scrollBarFadeTimer->setInterval(m_scrollBarFadeDelay);
    connect(m_scrollBarFadeTimer, &QTimer::timeout, this, &QWinUIScrollView::onScrollBarFadeTimer);
    
    // 设置布局
    setupScrollBars();
    
    // 设置圆角
    setCornerRadius(static_cast<int>(QWinUICornerRadius::Medium));
}

void QWinUIScrollView::setupScrollBars()
{
    // 创建网格布局
    QGridLayout* layout = new QGridLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    
    // 添加滚动区域 (0,0)
    layout->addWidget(m_scrollArea, 0, 0);
    
    // 添加垂直滚动条 (0,1)
    layout->addWidget(m_verticalScrollBar, 0, 1);
    
    // 添加水平滚动条 (1,0)
    layout->addWidget(m_horizontalScrollBar, 1, 0);
    
    // 设置列和行的拉伸因子
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 0);
    layout->setRowStretch(0, 1);
    layout->setRowStretch(1, 0);
    
    setLayout(layout);
}

QWidget* QWinUIScrollView::widget() const
{
    return m_contentWidget;
}

void QWinUIScrollView::setWidget(QWidget* widget)
{
    if (m_contentWidget == widget) {
        return;
    }
    
    if (m_contentWidget) {
        m_contentWidget->removeEventFilter(this);
    }
    
    m_contentWidget = widget;
    m_scrollArea->setWidget(widget);

    if (m_contentWidget) {
        m_contentWidget->installEventFilter(this);
        connect(m_contentWidget, &QWidget::destroyed, this, [this]() {
            m_contentWidget = nullptr;
        });

        // 立即应用当前主题到新内容
        propagateThemeToChildren(m_contentWidget);
    }
    
    updateScrollBars();
}

QWidget* QWinUIScrollView::takeWidget()
{
    QWidget* widget = m_contentWidget;
    if (widget) {
        widget->removeEventFilter(this);
        m_scrollArea->takeWidget();
        m_contentWidget = nullptr;
        updateScrollBars();
    }
    return widget;
}

Qt::ScrollBarPolicy QWinUIScrollView::horizontalScrollBarPolicy() const
{
    return m_horizontalPolicy;
}

void QWinUIScrollView::setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy policy)
{
    if (m_horizontalPolicy != policy) {
        m_horizontalPolicy = policy;
        updateScrollBarVisibility();
        emit horizontalScrollBarPolicyChanged(policy);
    }
}

Qt::ScrollBarPolicy QWinUIScrollView::verticalScrollBarPolicy() const
{
    return m_verticalPolicy;
}

void QWinUIScrollView::setVerticalScrollBarPolicy(Qt::ScrollBarPolicy policy)
{
    if (m_verticalPolicy != policy) {
        m_verticalPolicy = policy;
        updateScrollBarVisibility();
        emit verticalScrollBarPolicyChanged(policy);
    }
}

bool QWinUIScrollView::isHorizontalScrollBarVisible() const
{
    return m_horizontalScrollBar->isVisible();
}

void QWinUIScrollView::setHorizontalScrollBarVisible(bool visible)
{
    m_horizontalScrollBar->setVisible(visible);
    emit horizontalScrollBarVisibleChanged(visible);
}

bool QWinUIScrollView::isVerticalScrollBarVisible() const
{
    return m_verticalScrollBar->isVisible();
}

void QWinUIScrollView::setVerticalScrollBarVisible(bool visible)
{
    m_verticalScrollBar->setVisible(visible);
    emit verticalScrollBarVisibleChanged(visible);
}

bool QWinUIScrollView::isSmoothScrollingEnabled() const
{
    return m_smoothScrollingEnabled;
}

void QWinUIScrollView::setSmoothScrollingEnabled(bool enabled)
{
    if (m_smoothScrollingEnabled != enabled) {
        m_smoothScrollingEnabled = enabled;
        emit smoothScrollingEnabledChanged(enabled);
    }
}

int QWinUIScrollView::scrollAnimationDuration() const
{
    return m_scrollAnimationDuration;
}

void QWinUIScrollView::setScrollAnimationDuration(int duration)
{
    if (m_scrollAnimationDuration != duration) {
        m_scrollAnimationDuration = qMax(0, duration);
        m_horizontalScrollAnimation->setDuration(m_scrollAnimationDuration);
        m_verticalScrollAnimation->setDuration(m_scrollAnimationDuration);
        emit scrollAnimationDurationChanged(m_scrollAnimationDuration);
    }
}

QEasingCurve::Type QWinUIScrollView::scrollEasingCurve() const
{
    return m_scrollEasingCurve;
}

void QWinUIScrollView::setScrollEasingCurve(QEasingCurve::Type curve)
{
    if (m_scrollEasingCurve != curve) {
        m_scrollEasingCurve = curve;
        m_horizontalScrollAnimation->setEasingCurve(QEasingCurve(curve));
        m_verticalScrollAnimation->setEasingCurve(QEasingCurve(curve));
        emit scrollEasingCurveChanged(curve);
    }
}

bool QWinUIScrollView::autoHideScrollBars() const
{
    return m_autoHideScrollBars;
}

void QWinUIScrollView::setAutoHideScrollBars(bool autoHide)
{
    if (m_autoHideScrollBars != autoHide) {
        m_autoHideScrollBars = autoHide;
        m_horizontalScrollBar->setAutoHide(autoHide);
        m_verticalScrollBar->setAutoHide(autoHide);
        emit autoHideScrollBarsChanged(autoHide);
    }
}

int QWinUIScrollView::scrollBarFadeDelay() const
{
    return m_scrollBarFadeDelay;
}

void QWinUIScrollView::setScrollBarFadeDelay(int delay)
{
    if (m_scrollBarFadeDelay != delay) {
        m_scrollBarFadeDelay = qMax(0, delay);
        m_scrollBarFadeTimer->setInterval(m_scrollBarFadeDelay);
        emit scrollBarFadeDelayChanged(m_scrollBarFadeDelay);
    }
}

// 滚动位置获取
int QWinUIScrollView::horizontalScrollValue() const
{
    return m_scrollArea->horizontalScrollBar()->value();
}

int QWinUIScrollView::verticalScrollValue() const
{
    return m_scrollArea->verticalScrollBar()->value();
}

int QWinUIScrollView::horizontalScrollMaximum() const
{
    return m_scrollArea->horizontalScrollBar()->maximum();
}

int QWinUIScrollView::verticalScrollMaximum() const
{
    return m_scrollArea->verticalScrollBar()->maximum();
}

int QWinUIScrollView::horizontalScrollMinimum() const
{
    return m_scrollArea->horizontalScrollBar()->minimum();
}

int QWinUIScrollView::verticalScrollMinimum() const
{
    return m_scrollArea->verticalScrollBar()->minimum();
}

QRect QWinUIScrollView::viewportRect() const
{
    return m_scrollArea->viewport()->rect();
}

QSize QWinUIScrollView::viewportSize() const
{
    return m_scrollArea->viewport()->size();
}

QWidget* QWinUIScrollView::viewport() const
{
    return m_scrollArea->viewport();
}

QWinUIScrollBar* QWinUIScrollView::horizontalScrollBar() const
{
    return m_horizontalScrollBar;
}

QWinUIScrollBar* QWinUIScrollView::verticalScrollBar() const
{
    return m_verticalScrollBar;
}

// 滚动控制槽函数
void QWinUIScrollView::scrollTo(int x, int y)
{
    // 使用动画滚动到指定位置
    if (m_smoothScrollingEnabled) {
        startScrollAnimation(x, y);
    } else {
        m_scrollArea->horizontalScrollBar()->setValue(x);
        m_scrollArea->verticalScrollBar()->setValue(y);
    }
}

void QWinUIScrollView::scrollToTop()
{
    // 使用动画滚动到顶部
    if (m_smoothScrollingEnabled) {
        startScrollAnimation(horizontalScrollValue(), verticalScrollMinimum());
    } else {
        scrollTo(horizontalScrollValue(), verticalScrollMinimum());
    }
}

void QWinUIScrollView::scrollToBottom()
{
    // 使用动画滚动到底部
    if (m_smoothScrollingEnabled) {
        startScrollAnimation(horizontalScrollValue(), verticalScrollMaximum());
    } else {
        scrollTo(horizontalScrollValue(), verticalScrollMaximum());
    }
}

void QWinUIScrollView::scrollToLeft()
{
    // 使用动画滚动到左侧
    if (m_smoothScrollingEnabled) {
        startScrollAnimation(horizontalScrollMinimum(), verticalScrollValue());
    } else {
        scrollTo(horizontalScrollMinimum(), verticalScrollValue());
    }
}

void QWinUIScrollView::scrollToRight()
{
    // 使用动画滚动到右侧
    if (m_smoothScrollingEnabled) {
        startScrollAnimation(horizontalScrollMaximum(), verticalScrollValue());
    } else {
        scrollTo(horizontalScrollMaximum(), verticalScrollValue());
    }
}

void QWinUIScrollView::scrollToCenter()
{
    // 使用动画滚动到中心
    int centerX = (horizontalScrollMinimum() + horizontalScrollMaximum()) / 2;
    int centerY = (verticalScrollMinimum() + verticalScrollMaximum()) / 2;
    if (m_smoothScrollingEnabled) {
        startScrollAnimation(centerX, centerY);
    } else {
        scrollTo(centerX, centerY);
    }
}

void QWinUIScrollView::smoothScrollTo(int x, int y)
{
    if (!m_smoothScrollingEnabled) {
        scrollTo(x, y);
        return;
    }

    startScrollAnimation(x, y);
}

void QWinUIScrollView::smoothScrollBy(int dx, int dy)
{
    int newX = horizontalScrollValue() + dx;
    int newY = verticalScrollValue() + dy;
    smoothScrollTo(newX, newY);
}

// 专门用于滚轮滚动的方法
void QWinUIScrollView::wheelScrollBy(int dx, int dy)
{
    if (m_smoothScrollingEnabled) {
        // 计算目标位置
        int currentX = horizontalScrollValue();
        int currentY = verticalScrollValue();
        int targetX = currentX + dx;
        int targetY = currentY + dy;

        // 约束到有效范围
        targetX = qBound(horizontalScrollMinimum(), targetX, horizontalScrollMaximum());
        targetY = qBound(verticalScrollMinimum(), targetY, verticalScrollMaximum());

        // 只有当目标位置与当前位置不同时才启动动画
        if (targetX != currentX || targetY != currentY) {
            startScrollAnimation(targetX, targetY);
        }
    } else {
        int newX = horizontalScrollValue() + dx;
        int newY = verticalScrollValue() + dy;
        setHorizontalScrollValue(newX);
        setVerticalScrollValue(newY);
    }
}

void QWinUIScrollView::setHorizontalScrollValue(int value)
{
    m_scrollArea->horizontalScrollBar()->setValue(value);
}

void QWinUIScrollView::setVerticalScrollValue(int value)
{
    m_scrollArea->verticalScrollBar()->setValue(value);
}

void QWinUIScrollView::ensureVisible(int x, int y, int xMargin, int yMargin)
{
    QRect viewport = viewportRect();
    int currentX = horizontalScrollValue();
    int currentY = verticalScrollValue();

    int newX = currentX;
    int newY = currentY;

    // 水平方向
    if (x - xMargin < currentX) {
        newX = x - xMargin;
    } else if (x + xMargin > currentX + viewport.width()) {
        newX = x + xMargin - viewport.width();
    }

    // 垂直方向
    if (y - yMargin < currentY) {
        newY = y - yMargin;
    } else if (y + yMargin > currentY + viewport.height()) {
        newY = y + yMargin - viewport.height();
    }

    if (newX != currentX || newY != currentY) {
        if (m_smoothScrollingEnabled) {
            startScrollAnimation(newX, newY);
        } else {
            scrollTo(newX, newY);
        }
    }
}

void QWinUIScrollView::ensureWidgetVisible(QWidget* childWidget, int xMargin, int yMargin)
{
    if (!childWidget || !m_contentWidget) {
        return;
    }

    QPoint childPos = childWidget->mapTo(m_contentWidget, QPoint(0, 0));
    QRect childRect(childPos, childWidget->size());

    ensureVisible(childRect.center().x(), childRect.center().y(),
                  childRect.width() / 2 + xMargin, childRect.height() / 2 + yMargin);
}

// 私有槽函数
void QWinUIScrollView::onHorizontalScrollValueChanged(int value)
{
    // 滚动条点击时直接设置值，不使用动画
    m_scrollArea->horizontalScrollBar()->setValue(value);
    resetScrollBarFadeTimer();
    emit horizontalScrollValueChanged(value);
}

void QWinUIScrollView::onVerticalScrollValueChanged(int value)
{
    // 滚动条点击时直接设置值，不使用动画
    m_scrollArea->verticalScrollBar()->setValue(value);
    resetScrollBarFadeTimer();
    emit verticalScrollValueChanged(value);
}

void QWinUIScrollView::onScrollAnimationFinished()
{
    m_isScrolling = false;
    emit scrollFinished();
}

void QWinUIScrollView::onScrollBarFadeTimer()
{
    if (m_autoHideScrollBars && !m_isDragging) {
        hideScrollBars();
    }
}

void QWinUIScrollView::onContentSizeChanged()
{
    updateScrollBars();
}

// 事件处理
void QWinUIScrollView::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制滚动视图背景
    QColor bgColor = themeBackgroundColor();

    if (cornerRadius() > 0) {
        QPainterPath path;
        path.addRoundedRect(rect(), cornerRadius(), cornerRadius());
        painter.fillPath(path, bgColor);
    } else {
        painter.fillRect(rect(), bgColor);
    }

    // 确保滚动区域也有正确的背景
    if (m_scrollArea && m_scrollArea->viewport()) {
        m_scrollArea->viewport()->setStyleSheet(
            QString("QWidget { background-color: %1; }")
            .arg(bgColor.name(QColor::HexArgb))
        );
    }
}

void QWinUIScrollView::resizeEvent(QResizeEvent* event)
{
    updateScrollBarGeometry();
    QWinUIWidget::resizeEvent(event);
}

void QWinUIScrollView::wheelEvent(QWheelEvent* event)
{
    // 处理滚轮事件
    QPoint angleDelta = event->angleDelta();

    if (angleDelta.isNull()) {
        QWinUIWidget::wheelEvent(event);
        return;
    }

    // 计算滚动距离 - 使用更精确的计算
    double deltaX = angleDelta.x() / 8.0; // Qt的角度单位是1/8度
    double deltaY = angleDelta.y() / 8.0;

    // 转换为像素 - 每15度约等于一个滚动步长
    double pixelX = -deltaX * 3.0; // 水平滚动敏感度 (增加偏移量)
    double pixelY = -deltaY * 3.0; // 垂直滚动敏感度 (增加偏移量)

    // 检查修饰键
    Qt::KeyboardModifiers modifiers = event->modifiers();
    if (modifiers & Qt::ShiftModifier) {
        // Shift+滚轮 = 水平滚动
        pixelX = pixelY;
        pixelY = 0;
    } else if (modifiers & Qt::ControlModifier) {
        // Ctrl+滚轮 = 快速滚动
        pixelX *= 2.5;
        pixelY *= 2.5;
    } else {
        // 正常情况下，只处理垂直滚动
        pixelX = 0;
    }

    // 应用滚动
    // 使用累积滚动，避免小数丢失
    static double accumulatedX = 0.0;
    static double accumulatedY = 0.0;

    accumulatedX += pixelX;
    accumulatedY += pixelY;

    int scrollX = static_cast<int>(accumulatedX);
    int scrollY = static_cast<int>(accumulatedY);

    if (qAbs(scrollX) >= 1 || qAbs(scrollY) >= 1) {
        wheelScrollBy(scrollX, scrollY);
        accumulatedX -= scrollX;
        accumulatedY -= scrollY;
    }

    // 显示滚动条
    showScrollBars();
    resetScrollBarFadeTimer();

    event->accept();
}

void QWinUIScrollView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = true;
        m_lastMousePos = event->pos();
        m_dragStartPos = event->pos();
        m_dragDirectionLocked = false; // 重置方向锁定
        showScrollBars();
    }

    QWinUIWidget::mousePressEvent(event);
}

void QWinUIScrollView::mouseMoveEvent(QMouseEvent* event)
{
    if (m_isDragging && (event->buttons() & Qt::LeftButton)) {
        QPoint delta = m_lastMousePos - event->pos();
        QPoint totalDelta = m_dragStartPos - event->pos();

        // 如果还没有锁定方向，检测主要拖拽方向
        if (!m_dragDirectionLocked) {
            int threshold = 10; // 像素阈值

            if (qAbs(totalDelta.x()) > threshold || qAbs(totalDelta.y()) > threshold) {
                // 确定主要拖拽方向
                if (qAbs(totalDelta.x()) > qAbs(totalDelta.y())) {
                    m_dragDirection = Qt::Horizontal;
                } else {
                    m_dragDirection = Qt::Vertical;
                }
                m_dragDirectionLocked = true;
            }
        }

        // 根据锁定的方向进行滚动
        if (m_dragDirectionLocked) {
            if (m_dragDirection == Qt::Horizontal) {
                // 只允许水平滚动
                int newX = horizontalScrollValue() + delta.x();
                setHorizontalScrollValue(newX);
            } else {
                // 只允许垂直滚动
                int newY = verticalScrollValue() + delta.y();
                setVerticalScrollValue(newY);
            }
        }

        m_lastMousePos = event->pos();
    }

    QWinUIWidget::mouseMoveEvent(event);
}

void QWinUIScrollView::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
        m_dragDirectionLocked = false; // 重置方向锁定
        resetScrollBarFadeTimer();
    }

    QWinUIWidget::mouseReleaseEvent(event);
}

void QWinUIScrollView::keyPressEvent(QKeyEvent* event)
{
    // 处理键盘滚动
    switch (event->key()) {
    case Qt::Key_Up:
        wheelScrollBy(0, -20);
        event->accept();
        break;
    case Qt::Key_Down:
        wheelScrollBy(0, 20);
        event->accept();
        break;
    case Qt::Key_Left:
        wheelScrollBy(-20, 0);
        event->accept();
        break;
    case Qt::Key_Right:
        wheelScrollBy(20, 0);
        event->accept();
        break;
    case Qt::Key_PageUp:
        wheelScrollBy(0, -viewportSize().height());
        event->accept();
        break;
    case Qt::Key_PageDown:
        wheelScrollBy(0, viewportSize().height());
        event->accept();
        break;
    case Qt::Key_Home:
        scrollToTop();
        event->accept();
        break;
    case Qt::Key_End:
        scrollToBottom();
        event->accept();
        break;
    default:
        QWinUIWidget::keyPressEvent(event);
        break;
    }

    showScrollBars();
    resetScrollBarFadeTimer();
}

void QWinUIScrollView::enterEvent(QEnterEvent* event)
{
    showScrollBars();
    QWinUIWidget::enterEvent(event);
}

void QWinUIScrollView::leaveEvent(QEvent* event)
{
    if (!m_isDragging) {
        resetScrollBarFadeTimer();
    }
    QWinUIWidget::leaveEvent(event);
}

void QWinUIScrollView::onThemeChanged()
{
    updateScrollBarTheme();

    // 更新滚动区域背景
    if (m_scrollArea && m_scrollArea->viewport()) {
        QColor bgColor = themeBackgroundColor();
        m_scrollArea->viewport()->setStyleSheet(
            QString("QWidget { background-color: %1; }")
            .arg(bgColor.name(QColor::HexArgb))
        );
    }

    // 传播主题变化到内容控件
    if (m_contentWidget) {
        propagateThemeToChildren(m_contentWidget);

        // 强制重新布局和重绘滚动区域
        m_scrollArea->update();
        m_scrollArea->viewport()->update();
    }

    update();
    QWinUIWidget::onThemeChanged();
}

void QWinUIScrollView::propagateThemeToChildren(QWidget* widget)
{
    if (!widget) {
        return;
    }

    // 如果是QWinUIWidget，确保它使用相同的主题
    if (QWinUIWidget* qwinuiWidget = qobject_cast<QWinUIWidget*>(widget)) {
        // 设置主题
        if (theme()) {
            qwinuiWidget->setTheme(theme());
        }
        // 强制重绘
        qwinuiWidget->update();
    }

    // 递归处理所有子控件
    const QObjectList& children = widget->children();
    for (QObject* child : children) {
        if (QWidget* childWidget = qobject_cast<QWidget*>(child)) {
            propagateThemeToChildren(childWidget);
        }
    }
}

// 私有辅助方法
void QWinUIScrollView::updateScrollBars()
{
    if (!m_scrollArea) {
        return;
    }

    // 同步滚动条范围
    QScrollBar* hBar = m_scrollArea->horizontalScrollBar();
    QScrollBar* vBar = m_scrollArea->verticalScrollBar();

    m_horizontalScrollBar->setRange(hBar->minimum(), hBar->maximum());
    m_horizontalScrollBar->setPageStep(hBar->pageStep());
    m_horizontalScrollBar->setValue(hBar->value());

    m_verticalScrollBar->setRange(vBar->minimum(), vBar->maximum());
    m_verticalScrollBar->setPageStep(vBar->pageStep());
    m_verticalScrollBar->setValue(vBar->value());

    updateScrollBarVisibility();
}

void QWinUIScrollView::updateScrollBarVisibility()
{
    // 水平滚动条可见性
    bool hVisible = false;
    switch (m_horizontalPolicy) {
    case Qt::ScrollBarAlwaysOn:
        hVisible = true;
        break;
    case Qt::ScrollBarAlwaysOff:
        hVisible = false;
        break;
    case Qt::ScrollBarAsNeeded:
        hVisible = isScrollBarNeeded(Qt::Horizontal);
        break;
    }

    // 垂直滚动条可见性
    bool vVisible = false;
    switch (m_verticalPolicy) {
    case Qt::ScrollBarAlwaysOn:
        vVisible = true;
        break;
    case Qt::ScrollBarAlwaysOff:
        vVisible = false;
        break;
    case Qt::ScrollBarAsNeeded:
        vVisible = isScrollBarNeeded(Qt::Vertical);
        break;
    }

    m_horizontalScrollBar->setVisible(hVisible);
    m_verticalScrollBar->setVisible(vVisible);
}

void QWinUIScrollView::updateScrollBarGeometry()
{
    // 布局会自动处理几何形状
}

void QWinUIScrollView::updateContentGeometry()
{
    // 内容几何形状由滚动区域管理
}

void QWinUIScrollView::startScrollAnimation(int targetX, int targetY)
{
    if (!m_smoothScrollingEnabled) {
        setHorizontalScrollValue(targetX);
        setVerticalScrollValue(targetY);
        return;
    }

    // 约束目标值到有效范围
    targetX = qBound(horizontalScrollMinimum(), targetX, horizontalScrollMaximum());
    targetY = qBound(verticalScrollMinimum(), targetY, verticalScrollMaximum());

    bool needHorizontalAnimation = (targetX != horizontalScrollValue());
    bool needVerticalAnimation = (targetY != verticalScrollValue());

    if (!needHorizontalAnimation && !needVerticalAnimation) {
        return; // 无需动画
    }

    if (!m_isScrolling) {
        m_isScrolling = true;
        emit scrollStarted();
    }

    // 计算滚动距离，调整动画时长
    int distanceX = qAbs(targetX - horizontalScrollValue());
    int distanceY = qAbs(targetY - verticalScrollValue());
    int maxDistance = qMax(distanceX, distanceY);

    // 根据距离调整动画时长，距离越远动画越长
    int animationDuration = m_scrollAnimationDuration;
    if (maxDistance > 500) {
        animationDuration = qMin(800, m_scrollAnimationDuration + maxDistance / 3);
    }

    // 设置水平滚动动画
    if (needHorizontalAnimation) {
        if (m_horizontalScrollAnimation->state() == QPropertyAnimation::Running) {
            // 如果动画正在运行，平滑过渡到新目标值
            int currentValue = m_horizontalScrollAnimation->currentValue().toInt();
            m_horizontalScrollAnimation->stop();
            m_horizontalScrollAnimation->setStartValue(currentValue);
        } else {
            m_horizontalScrollAnimation->setStartValue(horizontalScrollValue());
        }
        m_horizontalScrollAnimation->setDuration(animationDuration);
        m_horizontalScrollAnimation->setEndValue(targetX);
        m_horizontalScrollAnimation->start();
    }

    // 设置垂直滚动动画
    if (needVerticalAnimation) {
        if (m_verticalScrollAnimation->state() == QPropertyAnimation::Running) {
            // 如果动画正在运行，平滑过渡到新目标值
            int currentValue = m_verticalScrollAnimation->currentValue().toInt();
            m_verticalScrollAnimation->stop();
            m_verticalScrollAnimation->setStartValue(currentValue);
        } else {
            m_verticalScrollAnimation->setStartValue(verticalScrollValue());
        }
        m_verticalScrollAnimation->setDuration(animationDuration);
        m_verticalScrollAnimation->setEndValue(targetY);
        m_verticalScrollAnimation->start();
    }
}

void QWinUIScrollView::stopScrollAnimation()
{
    if (m_horizontalScrollAnimation->state() == QPropertyAnimation::Running) {
        m_horizontalScrollAnimation->stop();
    }
    if (m_verticalScrollAnimation->state() == QPropertyAnimation::Running) {
        m_verticalScrollAnimation->stop();
    }
}

void QWinUIScrollView::showScrollBars()
{
    if (m_autoHideScrollBars && !m_scrollBarsVisible) {
        m_horizontalScrollBar->fadeIn();
        m_verticalScrollBar->fadeIn();
        m_scrollBarsVisible = true;
    }
}

void QWinUIScrollView::hideScrollBars()
{
    if (m_autoHideScrollBars && m_scrollBarsVisible) {
        m_horizontalScrollBar->fadeOut();
        m_verticalScrollBar->fadeOut();
        m_scrollBarsVisible = false;
    }
}

void QWinUIScrollView::resetScrollBarFadeTimer()
{
    if (m_autoHideScrollBars) {
        m_scrollBarFadeTimer->start();
    }
}

void QWinUIScrollView::updateScrollBarTheme()
{
    // 滚动条会自动响应主题变化
}

bool QWinUIScrollView::isScrollBarNeeded(Qt::Orientation orientation) const
{
    if (!m_scrollArea || !m_contentWidget) {
        return false;
    }

    QSize viewportSize = m_scrollArea->viewport()->size();

    if (orientation == Qt::Horizontal) {
        // 检查内容的实际需要宽度，而不是当前大小
        int contentWidth = m_contentWidget->sizeHint().width();
        if (contentWidth <= 0) {
            contentWidth = m_contentWidget->minimumSizeHint().width();
        }
        if (contentWidth <= 0) {
            contentWidth = m_contentWidget->size().width();
        }
        return contentWidth > viewportSize.width();
    } else {
        // 检查内容的实际需要高度
        int contentHeight = m_contentWidget->sizeHint().height();
        if (contentHeight <= 0) {
            contentHeight = m_contentWidget->minimumSizeHint().height();
        }
        if (contentHeight <= 0) {
            contentHeight = m_contentWidget->size().height();
        }
        return contentHeight > viewportSize.height();
    }
}

bool QWinUIScrollView::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::Wheel) {
        if (obj == m_scrollArea || obj == m_scrollArea->viewport()) {
            // 拦截滚轮事件并自己处理
            QWheelEvent* wheelEventPtr = static_cast<QWheelEvent*>(event);
            wheelEvent(wheelEventPtr);
            return true; // 阻止事件继续传播
        }
    }

    return QWinUIWidget::eventFilter(obj, event);
}

QPoint QWinUIScrollView::constrainScrollPosition(const QPoint& position) const
{
    int x = qBound(horizontalScrollMinimum(), position.x(), horizontalScrollMaximum());
    int y = qBound(verticalScrollMinimum(), position.y(), verticalScrollMaximum());
    return QPoint(x, y);
}

QT_END_NAMESPACE
