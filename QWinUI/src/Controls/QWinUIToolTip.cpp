#include "../../include/QWinUI/Controls/QWinUIToolTip.h"
#include "../../include/QWinUI/QWinUITheme.h"
#include <QPainter>
#include <QApplication>
#include <QScreen>
#include <QFontMetrics>
#include <QMouseEvent>
#include <QHoverEvent>
#include <QShowEvent>
#include <QHideEvent>
#include <QPainterPath>
#include <QGraphicsDropShadowEffect>
#include <QTimer>

QT_BEGIN_NAMESPACE

// 静态成员初始化
QWinUIToolTip* QWinUIToolTip::s_instance = nullptr;
const QMargins QWinUIToolTip::ContentMargins = QMargins(12, 8, 12, 8);

QWinUIToolTip::QWinUIToolTip(QWidget* parent)
    : QWinUIWidget(parent)
    , m_target(nullptr)
    , m_showDelay(DefaultShowDelay)
    , m_hideDelay(DefaultHideDelay)
    , m_placement(Auto)
    , m_actualPlacement(Bottom)
    , m_showTimer(nullptr)
    , m_hideTimer(nullptr)
    , m_fadeAnimation(nullptr)
    , m_opacityEffect(nullptr)
    , m_currentOpacity(0.0)
    , m_isShowing(false)
    , m_isHiding(false)
    , m_isVisible(false)
{
    initializeToolTip();
}

QWinUIToolTip::QWinUIToolTip(const QString& text, QWidget* target, QWidget* parent)
    : QWinUIWidget(parent)
    , m_text(text)
    , m_target(target)
    , m_showDelay(DefaultShowDelay)
    , m_hideDelay(DefaultHideDelay)
    , m_placement(Auto)
    , m_actualPlacement(Bottom)
    , m_showTimer(nullptr)
    , m_hideTimer(nullptr)
    , m_fadeAnimation(nullptr)
    , m_opacityEffect(nullptr)
    , m_currentOpacity(0.0)
    , m_isShowing(false)
    , m_isHiding(false)
    , m_isVisible(false)
{
    initializeToolTip();
    if (m_target) {
        installTargetEventFilter();
    }
}

QWinUIToolTip::~QWinUIToolTip()
{
    if (m_target) {
        removeTargetEventFilter();
    }
    
    if (s_instance == this) {
        s_instance = nullptr;
    }
}

void QWinUIToolTip::initializeToolTip()
{
    // 设置窗口属性
    setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_ShowWithoutActivating, true);
    setAttribute(Qt::WA_DeleteOnClose, false);
    setAttribute(Qt::WA_NoSystemBackground, true);  // 禁用系统背景

    // 设置基本样式
    setCornerRadius(static_cast<int>(QWinUICornerRadius::Medium)); // 4px圆角
    setShadowDepth(QWinUIShadowDepth::Medium);
    
    // 创建定时器
    m_showTimer = new QTimer(this);
    m_showTimer->setSingleShot(true);
    connect(m_showTimer, &QTimer::timeout, this, &QWinUIToolTip::onShowTimer);
    
    m_hideTimer = new QTimer(this);
    m_hideTimer->setSingleShot(true);
    connect(m_hideTimer, &QTimer::timeout, this, &QWinUIToolTip::onHideTimer);
    
    // 设置动画
    setupAnimations();
    
    // 连接主题变化
    if (QWinUITheme* theme = QWinUITheme::getInstance()) {
        connect(theme, &QWinUITheme::themeChanged, this, &QWinUIToolTip::onThemeChanged);
    }

    // 初始隐藏
    hide();
}

void QWinUIToolTip::setupAnimations()
{
    // 创建透明度效果
    m_opacityEffect = new QGraphicsOpacityEffect(this);
    m_opacityEffect->setOpacity(0.0);
    setGraphicsEffect(m_opacityEffect);
    
    // 创建淡入淡出动画
    m_fadeAnimation = new QPropertyAnimation(m_opacityEffect, "opacity", this);
    m_fadeAnimation->setDuration(FadeAnimationDuration);
    m_fadeAnimation->setEasingCurve(QEasingCurve::OutCubic);
    
    connect(m_fadeAnimation, &QPropertyAnimation::finished, [this]() {
        if (m_fadeAnimation->endValue().toDouble() == 0.0) {
            onFadeOutFinished();
        } else {
            onFadeInFinished();
        }
    });
}

QString QWinUIToolTip::text() const
{
    return m_text;
}

void QWinUIToolTip::setText(const QString& text)
{
    if (m_text != text) {
        m_text = text;
        updateGeometry();
        update();
        emit textChanged(text);
    }
}

QWidget* QWinUIToolTip::target() const
{
    return m_target;
}

void QWinUIToolTip::setTarget(QWidget* target)
{
    if (m_target != target) {
        if (m_target) {
            removeTargetEventFilter();
        }
        
        m_target = target;
        
        if (m_target) {
            installTargetEventFilter();
        }
        
        emit targetChanged(target);
    }
}

int QWinUIToolTip::showDelay() const
{
    return m_showDelay;
}

void QWinUIToolTip::setShowDelay(int delay)
{
    if (m_showDelay != delay) {
        m_showDelay = delay;
        emit showDelayChanged(delay);
    }
}

int QWinUIToolTip::hideDelay() const
{
    return m_hideDelay;
}

void QWinUIToolTip::setHideDelay(int delay)
{
    if (m_hideDelay != delay) {
        m_hideDelay = delay;
        emit hideDelayChanged(delay);
    }
}

QWinUIToolTip::QWinUIToolTipPlacement QWinUIToolTip::placement() const
{
    return m_placement;
}

void QWinUIToolTip::setPlacement(QWinUIToolTipPlacement placement)
{
    if (m_placement != placement) {
        m_placement = placement;
        if (isVisible()) {
            updatePosition();
        }
        emit placementChanged(placement);
    }
}

double QWinUIToolTip::opacity() const
{
    return m_currentOpacity;
}

void QWinUIToolTip::setOpacity(double opacity)
{
    m_currentOpacity = qBound(0.0, opacity, 1.0);
    if (m_opacityEffect) {
        m_opacityEffect->setOpacity(m_currentOpacity);
    }
}

QSize QWinUIToolTip::sizeHint() const
{
    if (m_text.isEmpty()) {
        return QSize(0, 0);
    }

    QFontMetrics fm(font());
    QRect textRect = fm.boundingRect(QRect(0, 0, 300, 0),
                                   Qt::TextWordWrap | Qt::AlignLeft, m_text);

    QMargins margins = getContentMargins();
    return QSize(textRect.width() + margins.left() + margins.right(),
                textRect.height() + margins.top() + margins.bottom());
}

QSize QWinUIToolTip::minimumSizeHint() const
{
    return sizeHint();
}

void QWinUIToolTip::showToolTip()
{
    if (m_text.isEmpty() || m_isShowing) {
        return;
    }
    
    m_isShowing = true;
    m_isHiding = false;
    
    // 停止隐藏定时器
    if (m_hideTimer->isActive()) {
        m_hideTimer->stop();
    }
    
    emit aboutToShow();
    
    // 计算位置
    updatePosition();
    
    // 显示窗口
    show();
    
    // 开始淡入动画
    m_fadeAnimation->setStartValue(0.0);
    m_fadeAnimation->setEndValue(1.0);
    m_fadeAnimation->start();
    
    m_isVisible = true;
    emit visibilityChanged(true);
}

void QWinUIToolTip::hideToolTip()
{
    if (!m_isVisible || m_isHiding) {
        return;
    }
    
    m_isHiding = true;
    m_isShowing = false;
    
    // 停止显示定时器
    if (m_showTimer->isActive()) {
        m_showTimer->stop();
    }
    
    emit aboutToHide();
    
    // 开始淡出动画
    m_fadeAnimation->setStartValue(m_opacityEffect->opacity());
    m_fadeAnimation->setEndValue(0.0);
    m_fadeAnimation->start();
}

void QWinUIToolTip::showToolTip(const QPoint& position)
{
    move(position);
    showToolTip();
}

// 静态便利方法
void QWinUIToolTip::showText(const QPoint& pos, const QString& text, QWidget* parent)
{
    if (text.isEmpty()) {
        hideText();
        return;
    }

    if (!s_instance) {
        s_instance = new QWinUIToolTip(parent ? parent : qApp->activeWindow());
    }

    s_instance->setText(text);
    s_instance->showToolTip(pos);
}

void QWinUIToolTip::hideText()
{
    if (s_instance) {
        s_instance->hideToolTip();
    }
}

void QWinUIToolTip::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect = this->rect();

    // 绘制阴影
    drawShadow(&painter, rect);

    // 绘制背景
    drawBackground(&painter, rect);

    // 绘制文本
    drawText(&painter, rect);
}

void QWinUIToolTip::showEvent(QShowEvent* event)
{
    QWinUIWidget::showEvent(event);
    updatePosition();
}

void QWinUIToolTip::hideEvent(QHideEvent* event)
{
    QWinUIWidget::hideEvent(event);
    m_isVisible = false;
    m_isShowing = false;
    m_isHiding = false;
    emit visibilityChanged(false);
}

bool QWinUIToolTip::eventFilter(QObject* object, QEvent* event)
{
    if (object == m_target && m_target) {
        switch (event->type()) {
        case QEvent::Enter:
            // 鼠标进入目标控件，启动显示定时器
            if (!m_isVisible && !m_isShowing) {
                m_showTimer->start(m_showDelay);
            }
            break;

        case QEvent::MouseMove:
            // 记录鼠标位置
            if (QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event)) {
                m_mousePosition = m_target->mapToGlobal(mouseEvent->pos());
            }
            break;

        case QEvent::Leave:
            // 鼠标离开目标控件，启动隐藏定时器
            if (m_showTimer->isActive()) {
                m_showTimer->stop();
            }
            if (m_isVisible) {
                m_hideTimer->start(m_hideDelay);
            }
            break;

        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonDblClick:
        case QEvent::KeyPress:
            // 用户交互时立即隐藏
            hideToolTip();
            break;

        default:
            break;
        }
    }

    return QWinUIWidget::eventFilter(object, event);
}

void QWinUIToolTip::onShowTimer()
{
    if (m_target && !m_text.isEmpty()) {
        showToolTip();
    }
}

void QWinUIToolTip::onHideTimer()
{
    hideToolTip();
}

void QWinUIToolTip::onFadeInFinished()
{
    m_isShowing = false;
}

void QWinUIToolTip::onFadeOutFinished()
{
    m_isHiding = false;
    hide();
}

void QWinUIToolTip::onThemeChanged()
{
    update();
}

void QWinUIToolTip::updatePosition()
{
    if (!m_target) {
        return;
    }

    calculateOptimalPosition();
    QPoint pos = calculatePosition(m_actualPlacement);
    move(pos);
}

void QWinUIToolTip::calculateOptimalPosition()
{
    if (m_placement == Auto) {
        m_actualPlacement = getOptimalPlacement();
    } else {
        m_actualPlacement = m_placement;
    }
}

QPoint QWinUIToolTip::calculatePosition(QWinUIToolTipPlacement placement)
{
    if (!m_target) {
        return QPoint(0, 0);
    }

    // 使用鼠标位置作为基准点，如果没有记录则使用控件中心
    QPoint basePos = m_mousePosition.isNull() ?
                     m_target->mapToGlobal(m_target->rect().center()) :
                     m_mousePosition;

    QSize tooltipSize = sizeHint();
    QPoint pos;

    const int spacing = 12; // 与鼠标位置的间距

    switch (placement) {
    case Top:
        pos = QPoint(basePos.x() - tooltipSize.width() / 2,
                    basePos.y() - tooltipSize.height() - spacing);
        break;

    case Bottom:
        pos = QPoint(basePos.x() - tooltipSize.width() / 2,
                    basePos.y() + spacing);
        break;

    case Left:
        pos = QPoint(basePos.x() - tooltipSize.width() - spacing,
                    basePos.y() - tooltipSize.height() / 2);
        break;

    case Right:
        pos = QPoint(basePos.x() + spacing,
                    basePos.y() - tooltipSize.height() / 2);
        break;

    default:
        // 默认显示在鼠标右下方
        pos = QPoint(basePos.x() + spacing,
                    basePos.y() + spacing);
        break;
    }

    // 确保ToolTip在屏幕范围内
    QScreen* screen = QApplication::screenAt(basePos);
    if (screen) {
        QRect screenRect = screen->availableGeometry();

        // 水平调整
        if (pos.x() < screenRect.left()) {
            pos.setX(screenRect.left() + 4);
        } else if (pos.x() + tooltipSize.width() > screenRect.right()) {
            pos.setX(screenRect.right() - tooltipSize.width() - 4);
        }

        // 垂直调整
        if (pos.y() < screenRect.top()) {
            pos.setY(screenRect.top() + 4);
        } else if (pos.y() + tooltipSize.height() > screenRect.bottom()) {
            pos.setY(screenRect.bottom() - tooltipSize.height() - 4);
        }
    }

    return pos;
}

QWinUIToolTip::QWinUIToolTipPlacement QWinUIToolTip::getOptimalPlacement()
{
    if (!m_target) {
        return Bottom;
    }

    // 使用鼠标位置作为基准点
    QPoint basePos = m_mousePosition.isNull() ?
                     m_target->mapToGlobal(m_target->rect().center()) :
                     m_mousePosition;

    QSize tooltipSize = sizeHint();
    const int spacing = 12;

    QScreen* screen = QApplication::screenAt(basePos);
    if (!screen) {
        return Bottom;
    }

    QRect screenRect = screen->availableGeometry();

    // 检查各个方向的可用空间（基于鼠标位置）
    int spaceTop = basePos.y() - screenRect.top();
    int spaceBottom = screenRect.bottom() - basePos.y();
    int spaceLeft = basePos.x() - screenRect.left();
    int spaceRight = screenRect.right() - basePos.x();

    // 优先选择右下方（最常用的位置）
    if (spaceRight >= tooltipSize.width() + spacing && spaceBottom >= tooltipSize.height() + spacing) {
        return Right;
    } else if (spaceBottom >= tooltipSize.height() + spacing) {
        return Bottom;
    } else if (spaceTop >= tooltipSize.height() + spacing) {
        return Top;
    } else if (spaceLeft >= tooltipSize.width() + spacing) {
        return Left;
    }

    // 如果都放不下，选择空间最大的方向
    if (spaceRight >= spaceBottom && spaceRight >= spaceTop && spaceRight >= spaceLeft) {
        return Right;
    } else if (spaceBottom >= spaceTop && spaceBottom >= spaceLeft) {
        return Bottom;
    } else if (spaceTop >= spaceLeft) {
        return Top;
    } else {
        return Left;
    }
}

void QWinUIToolTip::drawBackground(QPainter* painter, const QRect& rect)
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    // 获取背景颜色和边框颜色
    QColor bgColor = getBackgroundColor();
    QColor borderColor = getBorderColor();

    // 创建圆角矩形路径，留出阴影空间
    QRect bgRect = rect.adjusted(2, 2, -2, -2);
    QPainterPath path;
    path.addRoundedRect(bgRect, cornerRadius(), cornerRadius());

    // 绘制半透明背景，营造模糊效果
    painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter->fillPath(path, bgColor);

    // 绘制内部高光效果（模拟玻璃效果）
    QWinUITheme* theme = QWinUITheme::getInstance();
    if (theme) {
        QColor highlightColor;
        if (theme->isDarkMode()) {
            highlightColor = QColor(255, 255, 255, 15);  // 深色主题的微弱白色高光
        } else {
            highlightColor = QColor(255, 255, 255, 40);  // 浅色主题的白色高光
        }

        // 绘制顶部高光
        QRect highlightRect = bgRect.adjusted(1, 1, -1, -bgRect.height()/2);
        QPainterPath highlightPath;
        highlightPath.addRoundedRect(highlightRect, cornerRadius()-1, cornerRadius()-1);
        painter->fillPath(highlightPath, highlightColor);
    }

    // 绘制边框
    if (borderColor.isValid() && borderColor.alpha() > 0) {
        painter->setPen(QPen(borderColor, 1));
        painter->drawPath(path);
    }

    painter->restore();
}

void QWinUIToolTip::drawShadow(QPainter* painter, const QRect& rect)
{
    painter->save();

    QColor shadowColor = getShadowColor();
    if (!shadowColor.isValid()) {
        painter->restore();
        return;
    }

    // 创建阴影效果
    QPainterPath shadowPath;
    QRect shadowRect = rect.adjusted(ShadowOffset, ShadowOffset, ShadowOffset, ShadowOffset);
    shadowPath.addRoundedRect(shadowRect, cornerRadius(), cornerRadius());

    // 绘制模糊阴影
    for (int i = 0; i < ShadowBlurRadius; ++i) {
        QColor blurColor = shadowColor;
        blurColor.setAlpha(shadowColor.alpha() * (ShadowBlurRadius - i) / ShadowBlurRadius / 3);

        painter->setPen(QPen(blurColor, 1));
        painter->drawPath(shadowPath);

        shadowPath = QPainterPath();
        shadowRect = shadowRect.adjusted(-1, -1, 1, 1);
        shadowPath.addRoundedRect(shadowRect, cornerRadius() + i, cornerRadius() + i);
    }

    painter->restore();
}

void QWinUIToolTip::drawText(QPainter* painter, const QRect& rect)
{
    if (m_text.isEmpty()) {
        return;
    }

    painter->save();

    // 设置文本颜色和字体
    painter->setPen(getTextColor());
    painter->setFont(font());

    // 计算文本区域
    QRect textRect = getTextRect();

    // 绘制文本
    painter->drawText(textRect, Qt::TextWordWrap | Qt::AlignLeft | Qt::AlignVCenter, m_text);

    painter->restore();
}

QColor QWinUIToolTip::getBackgroundColor() const
{
    QWinUITheme* theme = QWinUITheme::getInstance();
    if (!theme) {
        return isDarkMode() ? QColor(45, 45, 45, 200) : QColor(255, 255, 255, 200);
    }

    // 使用半透明背景，营造模糊效果
    if (theme->isDarkMode()) {
        // 深色主题：半透明深色背景
        return QColor(32, 32, 32, 200);
    } else {
        // 浅色主题：半透明白色背景
        return QColor(255, 255, 255, 220);
    }
}

QColor QWinUIToolTip::getBorderColor() const
{
    QWinUITheme* theme = QWinUITheme::getInstance();
    if (!theme) {
        return isDarkMode() ? QColor(255, 255, 255, 30) : QColor(0, 0, 0, 30);
    }

    // 使用微妙的边框颜色，增强玻璃效果
    if (theme->isDarkMode()) {
        return QColor(255, 255, 255, 40);  // 深色主题：微弱白色边框
    } else {
        return QColor(0, 0, 0, 25);        // 浅色主题：微弱黑色边框
    }
}

QColor QWinUIToolTip::getTextColor() const
{
    QWinUITheme* theme = QWinUITheme::getInstance();
    if (!theme) {
        return isDarkMode() ? QColor(255, 255, 255) : QColor(0, 0, 0);
    }

    // 简单直接的文字颜色：深色主题用白色，浅色主题用黑色
    if (theme->isDarkMode()) {
        return QColor(255, 255, 255);  // 深色主题：白色文字
    } else {
        return QColor(0, 0, 0);        // 浅色主题：黑色文字
    }
}

QColor QWinUIToolTip::getShadowColor() const
{
    QWinUITheme* theme = QWinUITheme::getInstance();
    if (!theme) {
        return QColor(0, 0, 0, 25);
    }

    // 根据主题模式调整阴影颜色，使其更加柔和
    if (theme->isDarkMode()) {
        // 深色主题：使用柔和的阴影
        return QColor(0, 0, 0, 50);
    } else {
        // 浅色主题：使用更柔和的阴影
        return QColor(0, 0, 0, 20);
    }
}

QRect QWinUIToolTip::getTextRect() const
{
    QMargins margins = getContentMargins();
    return rect().adjusted(margins.left(), margins.top(), -margins.right(), -margins.bottom());
}

QMargins QWinUIToolTip::getContentMargins() const
{
    return ContentMargins;
}

void QWinUIToolTip::installTargetEventFilter()
{
    if (m_target) {
        m_target->installEventFilter(this);
    }
}

void QWinUIToolTip::removeTargetEventFilter()
{
    if (m_target) {
        m_target->removeEventFilter(this);
    }
}

QT_END_NAMESPACE
