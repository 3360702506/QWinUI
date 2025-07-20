#include "QWinUI/QWinUIWidget.h"
#include "QWinUI/QWinUITheme.h"
#include "QWinUI/QWinUIAnimation.h"
#include "QWinUI/QWinUIBlurEffect.h"
#include "QWinUI/Controls/QWinUIToolTip.h"
#include <QApplication>
#include <QEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QTimer>
#include <cmath>

QT_BEGIN_NAMESPACE

QWinUIWidget::QWinUIWidget(QWidget *parent)
    : QWidget(parent)
    , m_theme(nullptr)
    , m_animation(nullptr)
    , m_cornerRadius(static_cast<int>(QWinUICornerRadius::Medium))
    , m_accentColor(QColor(0, 120, 215)) // Windows 11默认蓝色
    , m_controlState(QWinUIControlState::Normal)
    , m_useAcrylicEffect(false)
    , m_shadowDepth(QWinUIShadowDepth::None)
    , m_shadowEffect(nullptr)
    , m_stateAnimation(nullptr)
    , m_isHovered(false)
    , m_isPressed(false)
    , m_isThemeTransitioning(false)
    , m_transitionMode(RippleTransition)
    , m_transitionRadius(0.0)
    , m_maxTransitionRadius(0.0)
    , m_transitionTimer(nullptr)
    , m_oldThemeMode(QWinUIThemeMode::Light)
    , m_newThemeMode(QWinUIThemeMode::Light)
    , m_hasBeenTouched(false)
    , m_fadeProgress(0.0)
    , m_toolTip(nullptr)
    , m_toolTipEnabled(true)
    , m_clipPathValid(false)
{
    initializeWidget();
}

QWinUIWidget::~QWinUIWidget()
{
    if (m_animation) {
        delete m_animation;
    }

    if (m_toolTip) {
        delete m_toolTip;
    }
}

void QWinUIWidget::initializeWidget()
{
    // 设置基本属性
    setAttribute(Qt::WA_Hover, true);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setAttribute(Qt::WA_NoSystemBackground, true);  // 禁用系统背景
    setAttribute(Qt::WA_TranslucentBackground, true); // 启用透明背景
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true); // 启用鼠标跟踪，用于主题切换动画

    // 设置透明背景，让Windows模糊效果完全接管
    setStyleSheet("QWinUIWidget { border: none; background: transparent; }");
    
    // 获取全局主题
    m_theme = QWinUITheme::getInstance();
    if (m_theme) {
        connect(m_theme, &QWinUITheme::themeChanged, 
                this, &QWinUIWidget::onThemeChangedInternal);
    }
    
    // 创建动画对象
    m_animation = new QWinUIAnimation(this);
    
    // 设置状态动画
    m_stateAnimation = new QPropertyAnimation(this, "geometry");
    m_stateAnimation->setDuration(150);
    m_stateAnimation->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_stateAnimation, &QPropertyAnimation::finished,
            this, &QWinUIWidget::onAnimationFinished);
    
    // 阴影效果已禁用

    // 初始化主题切换动画定时器
    m_transitionTimer = new QTimer(this);
    m_transitionTimer->setInterval(33); // 约30FPS，减少CPU占用
    connect(m_transitionTimer, &QTimer::timeout, this, &QWinUIWidget::updateThemeTransition);
}

// Qt阴影效果已完全禁用，使用Windows原生模糊效果

QWinUITheme* QWinUIWidget::theme() const
{
    return m_theme;
}

void QWinUIWidget::setTheme(QWinUITheme* theme)
{
    if (m_theme != theme) {
        if (m_theme) {
            disconnect(m_theme, &QWinUITheme::themeChanged,
                      this, &QWinUIWidget::onThemeChangedInternal);
        }
        
        m_theme = theme;
        
        if (m_theme) {
            connect(m_theme, &QWinUITheme::themeChanged,
                   this, &QWinUIWidget::onThemeChangedInternal);
        }
        
        onThemeChanged();
        update();
    }
}

int QWinUIWidget::cornerRadius() const
{
    return m_cornerRadius;
}

void QWinUIWidget::setCornerRadius(int radius)
{
    if (m_cornerRadius != radius) {
        m_cornerRadius = radius;
        m_clipPathValid = false; // 使缓存失效
        emit cornerRadiusChanged(radius);
        update();
    }
}

void QWinUIWidget::setCornerRadius(QWinUICornerRadius radius)
{
    setCornerRadius(static_cast<int>(radius));
}

QColor QWinUIWidget::accentColor() const
{
    return m_accentColor;
}

void QWinUIWidget::setAccentColor(const QColor& color)
{
    if (m_accentColor != color) {
        m_accentColor = color;
        emit accentColorChanged(color);
        update();
    }
}

QWinUIControlState QWinUIWidget::controlState() const
{
    return m_controlState;
}

void QWinUIWidget::setControlState(QWinUIControlState state)
{
    if (m_controlState != state) {
        m_controlState = state;
        emit controlStateChanged(state);
        updateControlState();
        update();
    }
}

bool QWinUIWidget::useAcrylicEffect() const
{
    return m_useAcrylicEffect;
}

void QWinUIWidget::setUseAcrylicEffect(bool use)
{
    if (m_useAcrylicEffect != use) {
        m_useAcrylicEffect = use;
        emit useAcrylicEffectChanged(use);
        update();
    }
}

QWinUIShadowDepth QWinUIWidget::shadowDepth() const
{
    return m_shadowDepth;
}

void QWinUIWidget::setShadowDepth(QWinUIShadowDepth depth)
{
    if (m_shadowDepth != depth) {
        m_shadowDepth = depth;
        emit shadowDepthChanged(depth);
        // Qt阴影效果已禁用，只保存状态
    }
}

bool QWinUIWidget::isDarkMode() const
{
    return m_theme ? m_theme->isDarkMode() : false;
}

// 主题颜色获取方法（供子类使用）
QColor QWinUIWidget::themeBackgroundColor() const
{
    return getThemeBackgroundColor();
}

QColor QWinUIWidget::themeTextColor() const
{
    return getThemeTextColor();
}

QColor QWinUIWidget::themeBorderColor() const
{
    return getThemeBorderColor();
}

QColor QWinUIWidget::themeAccentColor() const
{
    if (m_theme) {
        return m_theme->accentColor();
    }
    return m_accentColor; // 使用控件自己的强调色
}

void QWinUIWidget::startAnimation(QWinUIAnimationType type, int duration)
{
    if (m_animation) {
        m_animation->start(type, duration);
    }
}

void QWinUIWidget::stopAnimation()
{
    if (m_animation) {
        m_animation->stop();
    }
}

void QWinUIWidget::enableWindowsBlur(bool enable)
{
    if (enable) {
        // 启用模糊效果
        setAttribute(Qt::WA_TranslucentBackground, true);
        setAttribute(Qt::WA_NoSystemBackground, true);
        setStyleSheet("QWinUIWidget { border: none; background: transparent; }");
        QWinUIBlurEffect::enableBlurBehindWindow(this, QWinUIBlurEffect::Acrylic);
    } else {
        // 禁用模糊效果
        setAttribute(Qt::WA_TranslucentBackground, false);
        setAttribute(Qt::WA_NoSystemBackground, false);
        setStyleSheet("QWinUIWidget { border: none; }");
        QWinUIBlurEffect::disableBlurBehindWindow(this);
    }

    // 强制重绘
    update();
}

void QWinUIWidget::setWindowsBlurType(int blurType)
{
    QWinUIBlurEffect::BlurType type = static_cast<QWinUIBlurEffect::BlurType>(blurType);
    QWinUIBlurEffect::enableBlurBehindWindow(this, type);
}

void QWinUIWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)

    // 检查是否需要绘制背景（当没有模糊效果时）
    if (!testAttribute(Qt::WA_TranslucentBackground)) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        if (m_isThemeTransitioning) {
            // 主题切换动画期间的特殊绘制
            drawThemeTransition(&painter);
        } else {
            // 正常绘制
            QColor bgColor = getThemeBackgroundColor();

            if (m_cornerRadius > 0) {
                QPainterPath path;
                path.addRoundedRect(rect(), m_cornerRadius, m_cornerRadius);
                painter.fillPath(path, bgColor);
            } else {
                painter.fillRect(rect(), bgColor);
            }
        }
    }
}

// 虚函数的空实现（必须提供以避免链接错误）
void QWinUIWidget::drawBackground(QPainter* painter, const QRect& rect)
{
    Q_UNUSED(painter)
    Q_UNUSED(rect)
    // 已禁用Qt绘制，使用Windows原生效果
}

void QWinUIWidget::drawBorder(QPainter* painter, const QRect& rect)
{
    Q_UNUSED(painter)
    Q_UNUSED(rect)
    // 已禁用Qt绘制，使用Windows原生效果
}

void QWinUIWidget::drawContent(QPainter* painter, const QRect& rect)
{
    Q_UNUSED(painter)
    Q_UNUSED(rect)
    // 已禁用Qt绘制，使用Windows原生效果
}

void QWinUIWidget::drawForeground(QPainter* painter, const QRect& rect)
{
    Q_UNUSED(painter)
    Q_UNUSED(rect)
    // 已禁用Qt绘制，使用Windows原生效果
}

// 颜色获取方法的空实现（保持接口兼容性）
QColor QWinUIWidget::getBackgroundColor() const
{
    return QColor(255, 255, 255, 0); // 透明白色
}

QColor QWinUIWidget::getBorderColor() const
{
    return QColor(0, 0, 0, 0); // 透明黑色
}

QColor QWinUIWidget::getForegroundColor() const
{
    return QColor(0, 0, 0); // 黑色文本
}

QColor QWinUIWidget::getAccentColor() const
{
    return m_accentColor;
}

// 特效方法的空实现
void QWinUIWidget::applyFluentEffects(QPainter* painter, const QRect& rect)
{
    Q_UNUSED(painter)
    Q_UNUSED(rect)
    // 已禁用Qt特效，使用Windows原生API
}

void QWinUIWidget::drawAcrylicBackground(QPainter* painter, const QRect& rect)
{
    Q_UNUSED(painter)
    Q_UNUSED(rect)
    // 已禁用Qt Acrylic，使用Windows原生API
}

void QWinUIWidget::drawRevealEffect(QPainter* painter, const QRect& rect, const QPoint& hotSpot)
{
    Q_UNUSED(painter)
    Q_UNUSED(rect)
    Q_UNUSED(hotSpot)
    // 已禁用Qt Reveal效果，使用Windows原生API
}

void QWinUIWidget::applyShadowEffect()
{
    // 已禁用Qt阴影效果，使用Windows原生API
}

void QWinUIWidget::setupShadowEffect()
{
    // 已禁用Qt阴影效果
}

void QWinUIWidget::updateShadowEffect()
{
    // 已禁用Qt阴影效果
}

void QWinUIWidget::enterEvent(QEnterEvent* event)
{
    m_isHovered = true;
    // Qt 6.4兼容性修复
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    m_lastMousePos = event->position().toPoint();
#else
    m_lastMousePos = event->pos();
#endif
    // 禁用状态变化，防止触发重绘
    // setControlState(QWinUIControlState::Hovered);
    QWidget::enterEvent(event);
}

void QWinUIWidget::leaveEvent(QEvent* event)
{
    m_isHovered = false;
    m_lastMousePos = QPoint();
    // 禁用状态变化，防止触发重绘
    // if (m_controlState == QWinUIControlState::Hovered) {
    //     setControlState(QWinUIControlState::Normal);
    // }
    QWidget::leaveEvent(event);
}

void QWinUIWidget::mouseMoveEvent(QMouseEvent* event)
{
    // 更新鼠标位置，用于主题切换动画
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    m_lastMousePos = event->position().toPoint();
#else
    m_lastMousePos = event->pos();
#endif
    QWidget::mouseMoveEvent(event);
}

void QWinUIWidget::mousePressEvent(QMouseEvent* event)
{
    m_isPressed = true;
    // Qt 6.4兼容性修复
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    m_lastMousePos = event->position().toPoint();
#else
    m_lastMousePos = event->pos();
#endif
    // 禁用状态变化，防止触发重绘
    // setControlState(QWinUIControlState::Pressed);
    QWidget::mousePressEvent(event);
}

void QWinUIWidget::mouseReleaseEvent(QMouseEvent* event)
{
    m_isPressed = false;
    // 禁用状态变化，防止触发重绘
    // if (m_isHovered) {
    //     setControlState(QWinUIControlState::Hovered);
    // } else {
    //     setControlState(QWinUIControlState::Normal);
    // }
    QWidget::mouseReleaseEvent(event);
}

void QWinUIWidget::focusInEvent(QFocusEvent* event)
{
    // 禁用状态变化，防止触发重绘
    // if (m_controlState != QWinUIControlState::Pressed) {
    //     setControlState(QWinUIControlState::Focused);
    // }
    QWidget::focusInEvent(event);
}

void QWinUIWidget::focusOutEvent(QFocusEvent* event)
{
    // 禁用状态变化，防止触发重绘
    // if (m_controlState == QWinUIControlState::Focused) {
    //     setControlState(m_isHovered ? QWinUIControlState::Hovered : QWinUIControlState::Normal);
    // }
    QWidget::focusOutEvent(event);
}

void QWinUIWidget::resizeEvent(QResizeEvent* event)
{
    // 尺寸变化时使缓存失效
    m_clipPathValid = false;
    QWidget::resizeEvent(event);
}

void QWinUIWidget::changeEvent(QEvent* event)
{
    // 禁用状态变化，防止触发重绘
    // if (event->type() == QEvent::EnabledChange) {
    //     setControlState(isEnabled() ? QWinUIControlState::Normal : QWinUIControlState::Disabled);
    // }
    QWidget::changeEvent(event);
}

void QWinUIWidget::updateControlState()
{
    // 子类可以重写此方法来响应状态变化
}

void QWinUIWidget::onThemeChanged()
{
    // 子类可以重写此方法来响应主题变化
}

void QWinUIWidget::onThemeChangedInternal()
{
    // 只有在没有进行主题切换动画时才立即更新
    if (!m_isThemeTransitioning) {
        onThemeChanged();
        emit themeChanged();
        update();
    }
    // 如果正在进行动画，onThemeChanged 会在动画完成后调用
}

void QWinUIWidget::onAnimationFinished()
{
    // 动画完成后的处理
}

// 主题相关辅助方法
QColor QWinUIWidget::getThemeBackgroundColor() const
{
    if (!m_theme) {
        // 如果没有主题实例，使用默认颜色
        return QColor(255, 255, 255); // 默认白色背景
    }

    switch (m_theme->themeMode()) {
    case QWinUIThemeMode::Dark:
        return QColor(32, 32, 32);  // 深色主题背景
    case QWinUIThemeMode::Light:
        return QColor(255, 255, 255);  // 浅色主题背景（纯白色）
    default:
        // 系统主题，根据系统设置自动判断
        return isDarkMode() ? QColor(32, 32, 32) : QColor(255, 255, 255);
    }
}

QColor QWinUIWidget::getThemeTextColor() const
{
    if (!m_theme) {
        return QColor(0, 0, 0); // 默认黑色文字
    }

    switch (m_theme->themeMode()) {
    case QWinUIThemeMode::Dark:
        return QColor(255, 255, 255);  // 深色主题白色文字
    case QWinUIThemeMode::Light:
        return QColor(0, 0, 0);  // 浅色主题黑色文字
    default:
        return isDarkMode() ? QColor(255, 255, 255) : QColor(0, 0, 0);
    }
}

QColor QWinUIWidget::getThemeBorderColor() const
{
    if (!m_theme) {
        return QColor(200, 200, 200); // 默认边框颜色
    }

    switch (m_theme->themeMode()) {
    case QWinUIThemeMode::Dark:
        return QColor(255, 255, 255, 20);  // 深色主题半透明白色边框
    case QWinUIThemeMode::Light:
        return QColor(0, 0, 0, 20);  // 浅色主题半透明黑色边框
    default:
        return isDarkMode() ? QColor(255, 255, 255, 20) : QColor(0, 0, 0, 20);
    }
}

// 主题切换动画实现
void QWinUIWidget::startThemeTransition(const QPoint& clickPos)
{
    // 默认使用圆圈扩散动画
    startThemeTransition(RippleTransition, clickPos);
}

void QWinUIWidget::startThemeTransition(QWinUITransitionMode mode, const QPoint& clickPos)
{
    if (m_isThemeTransitioning) {
        // 如果已经在切换中，忽略新的切换请求
        return;
    }

    // 设置动画模式
    m_transitionMode = mode;

    // 保存当前主题状态
    m_oldThemeMode = m_theme ? m_theme->themeMode() : QWinUIThemeMode::Light;

    // 设置切换参数
    m_isThemeTransitioning = true;

    if (m_transitionMode == RippleTransition) {
        // 圆圈扩散动画初始化
        m_hasBeenTouched = false; // 重置接触状态

        // 如果没有提供点击位置，使用最后的鼠标位置
        if (clickPos.isNull()) {
            m_transitionCenter = m_lastMousePos;
        } else {
            m_transitionCenter = clickPos;
        }

        m_transitionRadius = 0.0;

        // 计算最大半径（到控件最远角的距离）
        QRect widgetRect = rect();
        double maxDistance = 0.0;

        // 检查到四个角的距离，取最大值
        QPoint corners[4] = {
            widgetRect.topLeft(),
            widgetRect.topRight(),
            widgetRect.bottomLeft(),
            widgetRect.bottomRight()
        };

        for (int i = 0; i < 4; i++) {
            double distance = std::sqrt(std::pow(corners[i].x() - m_transitionCenter.x(), 2) +
                                       std::pow(corners[i].y() - m_transitionCenter.y(), 2));
            maxDistance = std::max(maxDistance, distance);
        }

        m_maxTransitionRadius = maxDistance;
    } else if (m_transitionMode == FadeTransition) {
        // 颜色过渡动画初始化
        m_fadeProgress = 0.0;

        // 保存当前背景颜色
        if (m_oldThemeMode == QWinUIThemeMode::Dark) {
            m_oldBackgroundColor = QColor(32, 32, 32);
        } else {
            m_oldBackgroundColor = QColor(243, 243, 243);
        }

        // 获取目标背景颜色
        QWinUIThemeMode currentMode = m_theme ? m_theme->themeMode() : QWinUIThemeMode::Light;
        if (currentMode == QWinUIThemeMode::Dark) {
            m_newBackgroundColor = QColor(32, 32, 32);
        } else {
            m_newBackgroundColor = QColor(243, 243, 243);
        }
    }

    // 启动动画定时器
    m_transitionTimer->start();

    // 启动所有子控件的动画
    for (QObject* child : children()) {
        QWinUIWidget* childWidget = qobject_cast<QWinUIWidget*>(child);
        if (childWidget && childWidget->isVisible()) {
            childWidget->m_isThemeTransitioning = true;
            childWidget->m_hasBeenTouched = false;
            childWidget->m_oldThemeMode = m_oldThemeMode;
            childWidget->m_newThemeMode = m_newThemeMode;
        }
    }
}



bool QWinUIWidget::isThemeTransitioning() const
{
    return m_isThemeTransitioning;
}

void QWinUIWidget::updateThemeTransition()
{
    if (!m_isThemeTransitioning) {
        return;
    }

    bool animationComplete = false;

    if (m_transitionMode == RippleTransition) {
        // 圆圈扩散动画更新 - 优化：减少动画步数，提高每步增量
        double radiusIncrement = m_maxTransitionRadius / 20.0; // 约0.67秒完成动画，但步数减半
        m_transitionRadius += radiusIncrement;

        // 检测圆圈是否接触到当前控件（使用全局坐标）
        QPoint globalCenter = mapToGlobal(m_transitionCenter);
        if (!m_hasBeenTouched && isCircleTouchingWidget(globalCenter, m_transitionRadius)) {
            m_hasBeenTouched = true; // 标记为已接触
            // 优化：只在状态改变时才更新
        }

        // 传播动画到所有子控件
        for (QObject* child : children()) {
            QWinUIWidget* childWidget = qobject_cast<QWinUIWidget*>(child);
            if (childWidget && childWidget->isVisible() && childWidget->m_isThemeTransitioning) {
                // 子控件使用全局坐标系的动画中心
                QPoint localCenter = childWidget->mapFromGlobal(globalCenter);

                childWidget->m_transitionCenter = localCenter;
                childWidget->m_transitionRadius = m_transitionRadius;
                childWidget->m_maxTransitionRadius = m_maxTransitionRadius;

                // 使用全局坐标检测子控件是否被接触
                if (!childWidget->m_hasBeenTouched && childWidget->isCircleTouchingWidget(globalCenter, m_transitionRadius)) {
                    childWidget->m_hasBeenTouched = true;
                }

                childWidget->update();
            }
        }

        // 检查圆圈动画是否完成
        if (m_transitionRadius >= m_maxTransitionRadius) {
            animationComplete = true;
            m_hasBeenTouched = true;
        }

    } else if (m_transitionMode == FadeTransition) {
        // 颜色过渡动画更新 - 优化：减少更新频率
        double fadeIncrement = 1.0 / 15.0; // 约0.5秒完成动画，但步数减半
        m_fadeProgress += fadeIncrement;

        // 传播动画到所有子控件
        for (QObject* child : children()) {
            QWinUIWidget* childWidget = qobject_cast<QWinUIWidget*>(child);
            if (childWidget && childWidget->isVisible() && childWidget->m_isThemeTransitioning) {
                childWidget->m_fadeProgress = m_fadeProgress;
                childWidget->m_oldBackgroundColor = m_oldBackgroundColor;
                childWidget->m_newBackgroundColor = m_newBackgroundColor;
                childWidget->update();
            }
        }

        // 检查颜色过渡动画是否完成
        if (m_fadeProgress >= 1.0) {
            animationComplete = true;
            m_fadeProgress = 1.0;
        }
    }

    // 检查动画是否完成
    if (animationComplete) {
        m_isThemeTransitioning = false;
        m_transitionTimer->stop();

        // 动画完成后，更新新主题状态
        m_newThemeMode = m_theme ? m_theme->themeMode() : QWinUIThemeMode::Light;

        // 结束所有子控件的动画
        for (QObject* child : children()) {
            QWinUIWidget* childWidget = qobject_cast<QWinUIWidget*>(child);
            if (childWidget) {
                childWidget->m_isThemeTransitioning = false;
                if (m_transitionMode == RippleTransition) {
                    childWidget->m_hasBeenTouched = true;
                } else if (m_transitionMode == FadeTransition) {
                    childWidget->m_fadeProgress = 1.0;
                }
                childWidget->update();
            }
        }

        // 动画完成后，调用主题改变处理
        onThemeChanged();
        emit themeChanged();

        // 发送全局主题改变信号（只在顶级窗口发送，避免重复）
        if (!parent() || !qobject_cast<QWinUIWidget*>(parent())) {
            QWinUITheme* theme = QWinUITheme::getInstance();
            if (theme) {
                emit theme->themeChanged();
            }
        }
    }

    // 触发重绘 - 恢复原有逻辑确保动画流畅
    update();
}

void QWinUIWidget::drawThemeTransition(QPainter* painter)
{
    if (!m_isThemeTransitioning) {
        return;
    }

    // 获取旧主题和新主题的背景色
    QWinUIThemeMode currentMode = m_theme ? m_theme->themeMode() : QWinUIThemeMode::Light;

    QColor oldBgColor;
    if (m_oldThemeMode == QWinUIThemeMode::Dark) {
        oldBgColor = QColor(32, 32, 32);
    } else {
        oldBgColor = QColor(243, 243, 243); // 使用正确的浅色主题背景色
    }

    QColor newBgColor;
    if (currentMode == QWinUIThemeMode::Dark) {
        newBgColor = QColor(32, 32, 32);
    } else {
        newBgColor = QColor(243, 243, 243); // 使用正确的浅色主题背景色
    }

    if (m_transitionMode == RippleTransition) {
        // 圆圈扩散动画绘制
        drawRippleTransition(painter, oldBgColor, newBgColor);
    } else if (m_transitionMode == FadeTransition) {
        // 颜色过渡动画绘制
        drawFadeTransition(painter, oldBgColor, newBgColor);
    }
}

void QWinUIWidget::drawRippleTransition(QPainter* painter, const QColor& oldColor, const QColor& newColor)
{
    // 首先绘制旧主题背景
    if (m_cornerRadius > 0) {
        QPainterPath path;
        path.addRoundedRect(rect(), m_cornerRadius, m_cornerRadius);
        painter->fillPath(path, oldColor);
    } else {
        painter->fillRect(rect(), oldColor);
    }

    // 如果控件已被接触，绘制新主题颜色的扩散动画
    if (m_hasBeenTouched && m_transitionRadius > 0) {
        painter->save();

        // 优化：简化绘制逻辑，减少路径计算
        QRectF circleRect(m_transitionCenter.x() - m_transitionRadius,
                         m_transitionCenter.y() - m_transitionRadius,
                         m_transitionRadius * 2,
                         m_transitionRadius * 2);

        // 优化：使用缓存的裁剪路径
        if (m_cornerRadius > 0) {
            // 检查缓存是否有效
            if (!m_clipPathValid || m_lastRect != rect()) {
                m_cachedClipPath = QPainterPath();
                m_cachedClipPath.addRoundedRect(rect(), m_cornerRadius, m_cornerRadius);
                m_lastRect = rect();
                m_clipPathValid = true;
            }
            painter->setClipPath(m_cachedClipPath);
        } else {
            painter->setClipRect(rect());
        }

        // 优化：使用简单的径向渐变，减少复杂的路径交集计算
        QRadialGradient gradient(m_transitionCenter, m_transitionRadius * 0.8);
        gradient.setColorAt(0.0, newColor);
        gradient.setColorAt(1.0, newColor);

        painter->setBrush(QBrush(gradient));
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(circleRect);

        painter->restore();
    }
}

void QWinUIWidget::drawFadeTransition(QPainter* painter, const QColor& oldColor, const QColor& newColor)
{
    // 计算当前的过渡颜色
    QColor currentColor;
    if (m_fadeProgress <= 0.0) {
        currentColor = oldColor;
    } else if (m_fadeProgress >= 1.0) {
        currentColor = newColor;
    } else {
        // 线性插值计算过渡颜色
        int r = oldColor.red() + (newColor.red() - oldColor.red()) * m_fadeProgress;
        int g = oldColor.green() + (newColor.green() - oldColor.green()) * m_fadeProgress;
        int b = oldColor.blue() + (newColor.blue() - oldColor.blue()) * m_fadeProgress;
        int a = oldColor.alpha() + (newColor.alpha() - oldColor.alpha()) * m_fadeProgress;
        currentColor = QColor(r, g, b, a);
    }

    // 绘制过渡颜色
    if (m_cornerRadius > 0) {
        QPainterPath path;
        path.addRoundedRect(rect(), m_cornerRadius, m_cornerRadius);
        painter->fillPath(path, currentColor);
    } else {
        painter->fillRect(rect(), currentColor);
    }
}

double QWinUIWidget::getDistanceFromTransitionCenter(const QPoint& pos) const
{
    return std::sqrt(std::pow(pos.x() - m_transitionCenter.x(), 2) +
                    std::pow(pos.y() - m_transitionCenter.y(), 2));
}

QColor QWinUIWidget::getTransitionBackgroundColor(const QPoint& pos) const
{
    if (!m_isThemeTransitioning) {
        return getThemeBackgroundColor();
    }

    double distance = getDistanceFromTransitionCenter(pos);

    if (distance <= m_transitionRadius) {
        // 在过渡圆形内，使用新主题颜色
        QWinUIThemeMode currentMode = m_theme ? m_theme->themeMode() : QWinUIThemeMode::Light;
        if (currentMode == QWinUIThemeMode::Dark) {
            return QColor(32, 32, 32);
        } else {
            return QColor(243, 243, 243);
        }
    } else {
        // 在过渡圆形外，使用旧主题颜色
        if (m_oldThemeMode == QWinUIThemeMode::Dark) {
            return QColor(32, 32, 32);
        } else {
            return QColor(243, 243, 243);
        }
    }
}

bool QWinUIWidget::isCircleTouchingWidget(const QPoint& center, double radius) const
{
    // 将控件的本地坐标转换为全局坐标
    QRect globalWidgetRect = QRect(mapToGlobal(rect().topLeft()), rect().size());

    // 计算圆心到控件的最短距离（使用全局坐标）
    double distanceToWidget = getDistanceToWidgetGlobal(center, globalWidgetRect);

    // 如果圆的半径大于等于到控件的距离，说明圆圈接触到了控件
    return radius >= distanceToWidget;
}

double QWinUIWidget::getDistanceToWidget(const QPoint& center) const
{
    // 这个方法现在使用本地坐标
    QRect widgetRect = rect();

    // 如果圆心在控件内部，距离为0
    if (widgetRect.contains(center)) {
        return 0.0;
    }

    // 计算圆心到控件边界的最短距离
    double dx = 0.0;
    double dy = 0.0;

    if (center.x() < widgetRect.left()) {
        dx = widgetRect.left() - center.x();
    } else if (center.x() > widgetRect.right()) {
        dx = center.x() - widgetRect.right();
    }

    if (center.y() < widgetRect.top()) {
        dy = widgetRect.top() - center.y();
    } else if (center.y() > widgetRect.bottom()) {
        dy = center.y() - widgetRect.bottom();
    }

    return std::sqrt(dx * dx + dy * dy);
}

double QWinUIWidget::getDistanceToWidgetGlobal(const QPoint& center, const QRect& widgetRect) const
{
    // 如果圆心在控件内部，距离为0
    if (widgetRect.contains(center)) {
        return 0.0;
    }

    // 计算圆心到控件边界的最短距离
    double dx = 0.0;
    double dy = 0.0;

    if (center.x() < widgetRect.left()) {
        dx = widgetRect.left() - center.x();
    } else if (center.x() > widgetRect.right()) {
        dx = center.x() - widgetRect.right();
    }

    if (center.y() < widgetRect.top()) {
        dy = widgetRect.top() - center.y();
    } else if (center.y() > widgetRect.bottom()) {
        dy = center.y() - widgetRect.bottom();
    }

    return std::sqrt(dx * dx + dy * dy);
}

// ToolTip 相关方法实现
QString QWinUIWidget::toolTipText() const
{
    return m_toolTipText;
}

void QWinUIWidget::setToolTipText(const QString& text)
{
    if (m_toolTipText != text) {
        m_toolTipText = text;

        // 如果文本不为空且启用了 ToolTip，创建或更新 ToolTip
        if (!text.isEmpty() && m_toolTipEnabled) {
            if (!m_toolTip) {
                m_toolTip = new QWinUIToolTip(text, this, this);
                m_toolTip->setPlacement(QWinUIToolTip::Auto);
            } else {
                m_toolTip->setText(text);
            }
        } else if (m_toolTip) {
            // 如果文本为空或禁用了 ToolTip，删除现有的 ToolTip
            delete m_toolTip;
            m_toolTip = nullptr;
        }

        emit toolTipTextChanged(text);
    }
}

bool QWinUIWidget::isToolTipEnabled() const
{
    return m_toolTipEnabled;
}

void QWinUIWidget::setToolTipEnabled(bool enabled)
{
    if (m_toolTipEnabled != enabled) {
        m_toolTipEnabled = enabled;

        if (enabled && !m_toolTipText.isEmpty()) {
            // 启用 ToolTip 且有文本，创建 ToolTip
            if (!m_toolTip) {
                m_toolTip = new QWinUIToolTip(m_toolTipText, this, this);
                m_toolTip->setPlacement(QWinUIToolTip::Auto);
            }
        } else if (!enabled && m_toolTip) {
            // 禁用 ToolTip，删除现有的 ToolTip
            delete m_toolTip;
            m_toolTip = nullptr;
        }

        emit toolTipEnabledChanged(enabled);
    }
}

void QWinUIWidget::showToolTip()
{
    if (m_toolTip && m_toolTipEnabled && !m_toolTipText.isEmpty()) {
        m_toolTip->showToolTip();
    }
}

void QWinUIWidget::hideToolTip()
{
    if (m_toolTip) {
        m_toolTip->hideToolTip();
    }
}

QT_END_NAMESPACE
