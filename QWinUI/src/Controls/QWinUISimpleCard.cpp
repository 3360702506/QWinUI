#include "QWinUI/Controls/QWinUISimpleCard.h"
#include "QWinUI/QWinUITheme.h"
#include <QPainter>
#include <QApplication>
#include <QMouseEvent>
#include <QDebug>

QWinUISimpleCard::QWinUISimpleCard(QWidget* parent)
    : QWinUIWidget(parent)
    , m_layout(nullptr)
    , m_shadowEffect(nullptr)
    , m_cornerRadius(DEFAULT_CORNER_RADIUS)
    , m_contentMargins(DEFAULT_CONTENT_MARGIN, DEFAULT_CONTENT_MARGIN, 
                       DEFAULT_CONTENT_MARGIN, DEFAULT_CONTENT_MARGIN)
    , m_hoverEnabled(true)
    , m_isHovered(false)
    , m_hoverAnimation(nullptr)
    , m_hoverProgress(0.0)
{
    initializeCard();
}

QWinUISimpleCard::~QWinUISimpleCard()
{
    // Qt会自动清理子对象
}

void QWinUISimpleCard::initializeCard()
{
    // 设置基本属性
    setAttribute(Qt::WA_Hover, true);
    setFocusPolicy(Qt::NoFocus);
    
    // 创建布局
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(m_contentMargins);
    m_layout->setSpacing(8);
    
    // 创建阴影效果
    m_shadowEffect = new QGraphicsDropShadowEffect(this);
    m_shadowEffect->setBlurRadius(SHADOW_BLUR_RADIUS);
    m_shadowEffect->setOffset(0, SHADOW_OFFSET);
    m_shadowEffect->setColor(QColor(0, 0, 0, 60));
    setGraphicsEffect(m_shadowEffect);
    
    // 创建悬浮动画
    m_hoverAnimation = new QPropertyAnimation(this, "hoverProgress", this);
    m_hoverAnimation->setDuration(ANIMATION_DURATION);
    m_hoverAnimation->setEasingCurve(QEasingCurve::OutCubic);
    
    // 不需要手动连接主题信号，QWinUIWidget已经处理了
    
    // 初始化主题
    updateTheme();
}

void QWinUISimpleCard::addWidget(QWidget* widget)
{
    if (widget && m_layout) {
        m_layout->addWidget(widget);
    }
}

void QWinUISimpleCard::removeWidget(QWidget* widget)
{
    if (widget && m_layout) {
        m_layout->removeWidget(widget);
    }
}

void QWinUISimpleCard::clearWidgets()
{
    if (m_layout) {
        QLayoutItem* item;
        while ((item = m_layout->takeAt(0)) != nullptr) {
            if (item->widget()) {
                item->widget()->setParent(nullptr);
            }
            delete item;
        }
    }
}

void QWinUISimpleCard::setCornerRadius(int radius)
{
    if (m_cornerRadius != radius) {
        m_cornerRadius = qMax(0, radius);
        update();
        emit cornerRadiusChanged(m_cornerRadius);
    }
}

void QWinUISimpleCard::setContentMargins(const QMargins& margins)
{
    if (m_contentMargins != margins) {
        m_contentMargins = margins;
        if (m_layout) {
            m_layout->setContentsMargins(margins);
        }
        emit contentMarginsChanged(margins);
    }
}

void QWinUISimpleCard::setContentMargins(int left, int top, int right, int bottom)
{
    setContentMargins(QMargins(left, top, right, bottom));
}

void QWinUISimpleCard::setHoverEnabled(bool enabled)
{
    if (m_hoverEnabled != enabled) {
        m_hoverEnabled = enabled;
        if (!enabled) {
            m_isHovered = false;
            startHoverAnimation(false);
        }
        emit hoverEnabledChanged(enabled);
    }
}

void QWinUISimpleCard::setHoverProgress(double progress)
{
    if (qAbs(m_hoverProgress - progress) > 1e-6) {
        m_hoverProgress = qBound(0.0, progress, 1.0);
        update();
    }
}

QSize QWinUISimpleCard::sizeHint() const
{
    if (m_layout) {
        QSize layoutSize = m_layout->sizeHint();
        return layoutSize + QSize(m_contentMargins.left() + m_contentMargins.right(),
                                  m_contentMargins.top() + m_contentMargins.bottom());
    }
    return QSize(200, 150);
}

QSize QWinUISimpleCard::minimumSizeHint() const
{
    if (m_layout) {
        QSize layoutSize = m_layout->minimumSize();
        return layoutSize + QSize(m_contentMargins.left() + m_contentMargins.right(),
                                  m_contentMargins.top() + m_contentMargins.bottom());
    }
    return QSize(100, 80);
}

// 事件处理
void QWinUISimpleCard::paintEvent(QPaintEvent* event)
{
    // 首先调用父类的paintEvent来处理主题切换动画
    QWinUIWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect cardRect = rect();

    // 只有在没有主题切换动画时才绘制背景
    if (!isThemeTransitioning()) {
        // 绘制背景
        drawBackground(&painter, cardRect);

        // 绘制边框
        drawBorder(&painter, cardRect);
    }
}

void QWinUISimpleCard::showEvent(QShowEvent* event)
{
    QWinUIWidget::showEvent(event);

    // 显示时立即应用当前主题
    updateTheme();
}

void QWinUISimpleCard::enterEvent(QEnterEvent* event)
{
    QWinUIWidget::enterEvent(event);

    if (m_hoverEnabled) {
        m_isHovered = true;
        startHoverAnimation(true);
        emit cardHovered(true);
    }
}

void QWinUISimpleCard::leaveEvent(QEvent* event)
{
    QWinUIWidget::leaveEvent(event);

    if (m_hoverEnabled) {
        m_isHovered = false;
        startHoverAnimation(false);
        emit cardHovered(false);
    }
}



void QWinUISimpleCard::resizeEvent(QResizeEvent* event)
{
    QWinUIWidget::resizeEvent(event);
    updateShadowEffect();
}

// 重写QWinUIWidget的主题变化处理
void QWinUISimpleCard::onThemeChanged()
{
    updateTheme();
}

// 私有方法
void QWinUISimpleCard::updateTheme()
{
    // 更新阴影颜色
    updateShadowEffect();

    // 重新绘制
    update();
}

void QWinUISimpleCard::updateShadowEffect()
{
    if (!m_shadowEffect) return;

    QWinUITheme* theme = QWinUITheme::getInstance();
    if (!theme) return;

    // 根据主题调整阴影，与WinUI 3卡片阴影保持一致
    if (theme->isDarkMode()) {
        // 深色主题：更强的阴影以增强层次感
        m_shadowEffect->setColor(QColor(0, 0, 0, 120));
        m_shadowEffect->setBlurRadius(SHADOW_BLUR_RADIUS + 2);
        m_shadowEffect->setOffset(0, SHADOW_OFFSET + 1);
    } else {
        // 浅色主题：柔和的阴影
        m_shadowEffect->setColor(QColor(0, 0, 0, 50));
        m_shadowEffect->setBlurRadius(SHADOW_BLUR_RADIUS);
        m_shadowEffect->setOffset(0, SHADOW_OFFSET);
    }
}

void QWinUISimpleCard::startHoverAnimation(bool hovered)
{
    if (!m_hoverAnimation) return;

    m_hoverAnimation->stop();
    m_hoverAnimation->setStartValue(m_hoverProgress);
    m_hoverAnimation->setEndValue(hovered ? 1.0 : 0.0);
    m_hoverAnimation->start();
}

// 颜色获取方法
QColor QWinUISimpleCard::getBackgroundColor() const
{
    bool isDark = isDarkMode();

    if (isDark) {
        // 深色主题 - 使用与WinUI 3卡片一致的背景色
        return QColor(58, 58, 58, 77); // LayerFillColorDefault
    } else {
        // 浅色主题 - 使用纯白色背景
        return QColor(255, 255, 255, 255);
    }
}

QColor QWinUISimpleCard::getBorderColor() const
{
    QWinUITheme* theme = QWinUITheme::getInstance();
    if (theme) {
        return theme->controlStrokeColorDefault();
    }

    bool isDark = isDarkMode();
    return isDark ? QColor(255, 255, 255, 18) : QColor(117, 117, 117, 102);
}

QColor QWinUISimpleCard::getHoverBackgroundColor() const
{
    QWinUITheme* theme = QWinUITheme::getInstance();
    if (theme) {
        return theme->getColor(QWinUITheme::Colors::ControlFillColorSecondary);
    }

    bool isDark = isDarkMode();
    return isDark ? QColor(255, 255, 255, 23) : QColor(249, 249, 249, 128);
}

// 绘制方法
void QWinUISimpleCard::drawBackground(QPainter* painter, const QRect& rect)
{
    // 获取当前背景颜色
    QColor bgColor = getBackgroundColor();

    if (m_hoverEnabled && m_hoverProgress > 0.0) {
        // 使用与QWinUIButton相同的颜色混合方式
        bool isDark = isDarkMode();
        if (isDark) {
            // 深色主题：在原背景上叠加半透明白色
            QColor hoverOverlay = QColor(255, 255, 255, static_cast<int>(15 * m_hoverProgress));
            bgColor = QColor(
                qMin(255, bgColor.red() + hoverOverlay.red() * hoverOverlay.alpha() / 255),
                qMin(255, bgColor.green() + hoverOverlay.green() * hoverOverlay.alpha() / 255),
                qMin(255, bgColor.blue() + hoverOverlay.blue() * hoverOverlay.alpha() / 255),
                bgColor.alpha()
            );
        } else {
            // 浅色主题：简单的颜色插值
            QColor hoverColor = getHoverBackgroundColor();
            int r = bgColor.red() + (hoverColor.red() - bgColor.red()) * m_hoverProgress;
            int g = bgColor.green() + (hoverColor.green() - bgColor.green()) * m_hoverProgress;
            int b = bgColor.blue() + (hoverColor.blue() - bgColor.blue()) * m_hoverProgress;
            int a = bgColor.alpha() + (hoverColor.alpha() - bgColor.alpha()) * m_hoverProgress;
            bgColor = QColor(r, g, b, a);
        }
    }



    // 绘制圆角矩形背景
    painter->setBrush(bgColor);
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(rect, m_cornerRadius, m_cornerRadius);
}

void QWinUISimpleCard::drawBorder(QPainter* painter, const QRect& rect)
{
    // 获取当前边框颜色
    QColor borderColor = getBorderColor();

    // 绘制边框
    QPen borderPen(borderColor, 1);
    painter->setPen(borderPen);
    painter->setBrush(Qt::NoBrush);

    // 调整矩形以确保边框完全可见
    QRect borderRect = rect.adjusted(0, 0, -1, -1);
    painter->drawRoundedRect(borderRect, m_cornerRadius, m_cornerRadius);
}
