#include "QWinUI/Controls/QWinUIButton.h"
#include "QWinUI/QWinUITheme.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QFontMetrics>
#include <QPropertyAnimation>
#include <QApplication>

QT_BEGIN_NAMESPACE

QWinUIButton::QWinUIButton(QWidget* parent)
    : QWinUIWidget(parent)
    , m_buttonStyle(Standard)
    , m_isDefault(false)
    , m_isPressed(false)
    , m_isHovered(false)
    , m_isChecked(false)
    , m_isFocused(false)
    , m_hoverAnimation(nullptr)
    , m_pressAnimation(nullptr)
    , m_hoverProgress(0.0)
    , m_pressProgress(0.0)
{
    initializeButton();
}

QWinUIButton::QWinUIButton(const QString& text, QWidget* parent)
    : QWinUIWidget(parent)
    , m_text(text)
    , m_buttonStyle(Standard)
    , m_isDefault(false)
    , m_isPressed(false)
    , m_isChecked(false)
    , m_isHovered(false)
    , m_isFocused(false)
    , m_hoverAnimation(nullptr)
    , m_pressAnimation(nullptr)
    , m_hoverProgress(0.0)
    , m_pressProgress(0.0)
{
    initializeButton();
}

QWinUIButton::~QWinUIButton()
{
    if (m_hoverAnimation) {
        delete m_hoverAnimation;
    }
    if (m_pressAnimation) {
        delete m_pressAnimation;
    }
}

void QWinUIButton::initializeButton()
{
    // 设置基本属性
    setMinimumSize(120, 32);
    setCornerRadius(3); // WinUI 3 圆角
    setCursor(Qt::PointingHandCursor);
    setFocusPolicy(Qt::StrongFocus);

    // 创建简单的动画
    m_hoverAnimation = new QPropertyAnimation(this, "hoverProgress");
    m_hoverAnimation->setDuration(100); // 更快的动画
    m_hoverAnimation->setEasingCurve(QEasingCurve::OutQuad);

    m_pressAnimation = new QPropertyAnimation(this, "pressProgress");
    m_pressAnimation->setDuration(50); // 很快的按下动画
    m_pressAnimation->setEasingCurve(QEasingCurve::OutQuad);

    updateButtonAppearance();
}

QString QWinUIButton::text() const
{
    return m_text;
}

void QWinUIButton::setText(const QString& text)
{
    if (m_text != text) {
        m_text = text;
        update();
        emit textChanged(text);
    }
}

QIcon QWinUIButton::icon() const
{
    return m_icon;
}

void QWinUIButton::setIcon(const QIcon& icon)
{
    if (m_icon.cacheKey() != icon.cacheKey()) {
        m_icon = icon;
        update();
        emit iconChanged(icon);
    }
}

QWinUIButton::QWinUIButtonStyle QWinUIButton::buttonStyle() const
{
    return m_buttonStyle;
}

void QWinUIButton::setButtonStyle(QWinUIButtonStyle style)
{
    if (m_buttonStyle != style) {
        m_buttonStyle = style;
        updateButtonAppearance();
        emit buttonStyleChanged(style);
    }
}

bool QWinUIButton::isDefault() const
{
    return m_isDefault;
}

void QWinUIButton::setDefault(bool isDefault)
{
    if (m_isDefault != isDefault) {
        m_isDefault = isDefault;
        updateButtonAppearance();
        emit defaultChanged(isDefault);
    }
}

QSize QWinUIButton::sizeHint() const
{
    QFontMetrics fm(font());
    int textWidth = m_text.isEmpty() ? 0 : fm.horizontalAdvance(m_text);
    int iconWidth = m_icon.isNull() ? 0 : 16;
    int spacing = (!m_text.isEmpty() && !m_icon.isNull()) ? 8 : 0;
    
    int width = textWidth + iconWidth + spacing + 24; // 24 for padding
    int height = qMax(32, fm.height() + 12); // minimum 32px height
    
    return QSize(qMax(width, 120), height);
}

QSize QWinUIButton::minimumSizeHint() const
{
    return QSize(120, 32);
}

void QWinUIButton::paintEvent(QPaintEvent* event)
{
    // 首先调用父类的paintEvent来处理主题切换动画
    QWinUIWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect buttonRect = rect();

    // 只有在没有主题切换动画时才绘制背景
    if (!isThemeTransitioning()) {
        // 获取基础颜色
        QColor bgColor = getBackgroundColor();
        QColor borderColor = getBorderColor();

        // 应用WinUI 3风格的悬浮和按下效果，使用动画进度
        if (m_hoverProgress > 0.0) {
            QColor hoverColor;
            if (m_buttonStyle == Accent) {
                hoverColor = QColor(32, 145, 240); // 更明显的亮蓝色
            } else if (m_buttonStyle == Standard) {
                bool isDark = isDarkMode();
                hoverColor = isDark ? QColor(255, 255, 255, 28) : QColor(245, 245, 245); // 加强悬浮效果
            } else if (m_buttonStyle == Subtle || m_buttonStyle == Hyperlink) {
                // Subtle和Hyperlink样式在悬浮时显示背景
                bool isDark = isDarkMode();
                hoverColor = isDark ? QColor(255, 255, 255, 15) : QColor(0, 0, 0, 10); // 微妙的背景色
            } else {
                hoverColor = bgColor.lighter(115); // 其他样式也加强悬浮效果
            }

            // 插值混合颜色
            bgColor = QColor::fromRgbF(
                bgColor.redF() + (hoverColor.redF() - bgColor.redF()) * m_hoverProgress,
                bgColor.greenF() + (hoverColor.greenF() - bgColor.greenF()) * m_hoverProgress,
                bgColor.blueF() + (hoverColor.blueF() - bgColor.blueF()) * m_hoverProgress,
                bgColor.alphaF() + (hoverColor.alphaF() - bgColor.alphaF()) * m_hoverProgress
            );
        }

        // 按下时背景色不变，只通过边框和文字变化来表示按下状态
        // WinUI 3 的按钮按下效果主要通过边框变淡和文字变淡来实现

        // 绘制背景
        painter.setBrush(bgColor);
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(buttonRect, cornerRadius(), cornerRadius());
    }

    // 获取文本颜色（无论是否在动画中都需要绘制文本）
    QColor textColor = getTextColor();

    // 只有在没有主题切换动画时才绘制边框
    if (!isThemeTransitioning()) {
        QColor borderColor = getBorderColor();

        // 绘制边框 - WinUI 3风格：按下时底部边框变淡
        if (borderColor.alpha() > 0) {
            painter.setBrush(Qt::NoBrush);

            if (m_isPressed || m_pressProgress > 0.0) {
                // 按下时：先绘制完整边框，然后用变淡的颜色重绘底部
                painter.setPen(QPen(borderColor, 0.5));
                painter.drawRoundedRect(buttonRect, cornerRadius(), cornerRadius());

                // 底部边框变淡效果 - 加强变淡程度
                QColor fadedBorderColor = borderColor;
                fadedBorderColor.setAlpha(static_cast<int>(borderColor.alpha() * (1.0 - 0.8 * m_pressProgress)));

                painter.setPen(QPen(fadedBorderColor, 0.5));
                // 重绘底部边框区域
                int radius = cornerRadius();
                QRect bottomRect = buttonRect.adjusted(radius/2, buttonRect.height()-2, -radius/2, 0);
                painter.drawLine(bottomRect.bottomLeft(), bottomRect.bottomRight());
            } else {
                // 正常状态：绘制完整边框
                painter.setPen(QPen(borderColor, 0.5));
                painter.drawRoundedRect(buttonRect, cornerRadius(), cornerRadius());
            }
        }
    }

    // 绘制文本 - 按下时稍微变暗而不是变透明，加强效果
    if (m_isPressed || m_pressProgress > 0.0) {
        // 使用darker()方法而不是修改alpha，避免发白效果，加强变暗程度
        QColor pressedTextColor = textColor.darker(static_cast<int>(100 + 30 * m_pressProgress));
        painter.setPen(pressedTextColor);
    } else {
        painter.setPen(textColor);
    }
    painter.drawText(buttonRect, Qt::AlignCenter, m_text);

    // 绘制焦点框
    if (m_isFocused) {
        drawFocusRect(&painter, buttonRect);
    }
}

void QWinUIButton::drawBackground(QPainter* painter, const QRect& rect) // 暂时不使用
{
    painter->save();

    QColor bgColor = getBackgroundColor();
    QColor borderColor = getBorderColor();

    // 简单的悬停和按下效果
    if (m_isHovered) {
        bgColor = bgColor.lighter(110);
    }
    if (m_isPressed) {
        bgColor = bgColor.darker(110);
    }

    // 绘制背景
    painter->setBrush(bgColor);
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(rect, cornerRadius(), cornerRadius());

    // 绘制边框
    if (borderColor.alpha() > 0) {
        painter->setBrush(Qt::NoBrush);
        painter->setPen(QPen(borderColor, 1));
        painter->drawRoundedRect(rect, cornerRadius(), cornerRadius());
    }

    painter->restore();
}

void QWinUIButton::drawContent(QPainter* painter, const QRect& rect)
{
    painter->save();
    
    QColor textColor = getTextColor();
    painter->setPen(textColor);
    painter->setFont(font());
    
    QRect contentRect = rect.adjusted(12, 6, -12, -6);
    
    if (!m_icon.isNull() && !m_text.isEmpty()) {
        // 图标 + 文本
        QSize iconSize(16, 16);
        QRect iconRect(contentRect.left(), 
                      contentRect.top() + (contentRect.height() - iconSize.height()) / 2,
                      iconSize.width(), iconSize.height());
        
        painter->drawPixmap(iconRect, m_icon.pixmap(iconSize));
        
        QRect textRect = contentRect.adjusted(iconSize.width() + 8, 0, 0, 0);
        painter->drawText(textRect, Qt::AlignCenter, m_text);
    } else if (!m_icon.isNull()) {
        // 仅图标
        QSize iconSize(16, 16);
        QRect iconRect((contentRect.width() - iconSize.width()) / 2 + contentRect.left(),
                      (contentRect.height() - iconSize.height()) / 2 + contentRect.top(),
                      iconSize.width(), iconSize.height());
        painter->drawPixmap(iconRect, m_icon.pixmap(iconSize));
    } else if (!m_text.isEmpty()) {
        // 仅文本
        painter->drawText(contentRect, Qt::AlignCenter, m_text);
    }
    
    painter->restore();
}

void QWinUIButton::drawFocusRect(QPainter* painter, const QRect& rect)
{
    painter->save();

    QColor focusColor = QColor(0, 120, 215); // WinUI 3 焦点颜色
    painter->setPen(QPen(focusColor, 2));
    painter->setBrush(Qt::NoBrush);

    // 焦点框稍微向外扩展
    QRect focusRect = rect.adjusted(-2, -2, 2, 2);
    painter->drawRoundedRect(focusRect, cornerRadius() + 1, cornerRadius() + 1);

    painter->restore();
}

QColor QWinUIButton::getBackgroundColor() const
{
    bool isDark = isDarkMode();

    if (!isEnabled()) {
        return isDark ? QColor(255, 255, 255, 10) : QColor(249, 249, 249, 77);
    }

    // 如果是选中状态，使用强调色
    if (m_isChecked) {
        QWinUITheme* theme = QWinUITheme::getInstance();
        if (theme) {
            return theme->accentColor();
        }
        return QColor(0, 120, 215); // 默认强调色
    }

    switch (m_buttonStyle) {
    case Accent:
        return QColor(0, 120, 215); // WinUI 3 Accent Blue
    case Subtle:
        return QColor(0, 0, 0, 0); // 透明
    case Hyperlink:
        return QColor(0, 0, 0, 0); // 透明
    case Standard:
    default:
        // WinUI 3 标准按钮背景色
        return isDark ? QColor(255, 255, 255, 15) : QColor(255, 255, 255, 179);
    }
}

QColor QWinUIButton::getBorderColor() const
{
    bool isDark = isDarkMode();

    if (!isEnabled()) {
        return isDark ? QColor(255, 255, 255, 8) : QColor(200, 200, 200, 51);
    }

    switch (m_buttonStyle) {
    case Accent:
        return QColor(0, 120, 215); // 强调按钮边框与背景同色
    case Subtle:
    case Hyperlink:
        return QColor(0, 0, 0, 0); // 无边框
    case Standard:
    default:
        // WinUI 3 标准按钮边框色
        return isDark ? QColor(255, 255, 255, 18) : QColor(117, 117, 117, 102);
    }
}

QColor QWinUIButton::getTextColor() const
{
    bool isDark = isDarkMode();

    if (!isEnabled()) {
        return isDark ? QColor(255, 255, 255, 87) : QColor(161, 161, 161);
    }

    // 如果是选中状态，使用白色文本（在强调色背景上）
    if (m_isChecked) {
        return QColor(255, 255, 255);
    }

    switch (m_buttonStyle) {
    case Accent:
        return QColor(255, 255, 255); // 白色文本在蓝色背景上
    case Hyperlink:
        return QColor(0, 120, 215); // 蓝色超链接文本
    case Subtle:
    case Standard:
    default:
        // WinUI 3 文本颜色
        return isDark ? QColor(255, 255, 255) : QColor(14, 14, 14, 230);
    }
}

bool QWinUIButton::isHovered() const
{
    return m_isHovered;
}

bool QWinUIButton::isPressed() const
{
    return m_isPressed;
}

bool QWinUIButton::isFocused() const
{
    return m_isFocused;
}

void QWinUIButton::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && isEnabled()) {
        m_isPressed = true;

        if (m_pressAnimation) {
            m_pressAnimation->stop();
            m_pressAnimation->setStartValue(m_pressProgress);
            m_pressAnimation->setEndValue(1.0);
            m_pressAnimation->start();
        } else {
            m_pressProgress = 1.0;
            update();
        }

        emit pressed();
    }
    QWinUIWidget::mousePressEvent(event);
}

void QWinUIButton::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_isPressed) {
        m_isPressed = false;

        if (m_pressAnimation) {
            m_pressAnimation->stop();
            m_pressAnimation->setStartValue(m_pressProgress);
            m_pressAnimation->setEndValue(0.0);
            m_pressAnimation->start();
        } else {
            m_pressProgress = 0.0;
            update();
        }

        if (rect().contains(event->pos()) && isEnabled()) {
            emit clicked();
        }

        emit released();
    }
    QWinUIWidget::mouseReleaseEvent(event);
}

void QWinUIButton::mouseMoveEvent(QMouseEvent* event)
{
    // 处理鼠标移动时的状态变化
    bool wasHovered = m_isHovered;
    m_isHovered = rect().contains(event->pos());

    if (wasHovered != m_isHovered) {
        m_hoverProgress = m_isHovered ? 1.0 : 0.0; // 直接设置，不使用动画
        update();
    }

    QWinUIWidget::mouseMoveEvent(event);
}

void QWinUIButton::enterEvent(QEnterEvent* event)
{
    m_isHovered = true;

    if (m_hoverAnimation) {
        m_hoverAnimation->stop();
        m_hoverAnimation->setStartValue(m_hoverProgress);
        m_hoverAnimation->setEndValue(1.0);
        m_hoverAnimation->start();
    } else {
        m_hoverProgress = 1.0;
        update();
    }

    QWinUIWidget::enterEvent(event);
}

void QWinUIButton::leaveEvent(QEvent* event)
{
    m_isHovered = false;
    m_isPressed = false;

    if (m_hoverAnimation) {
        m_hoverAnimation->stop();
        m_hoverAnimation->setStartValue(m_hoverProgress);
        m_hoverAnimation->setEndValue(0.0);
        m_hoverAnimation->start();
    } else {
        m_hoverProgress = 0.0;
    }

    if (m_pressAnimation) {
        m_pressAnimation->stop();
        m_pressAnimation->setStartValue(m_pressProgress);
        m_pressAnimation->setEndValue(0.0);
        m_pressAnimation->start();
    } else {
        m_pressProgress = 0.0;
    }

    update();
    QWinUIWidget::leaveEvent(event);
}

void QWinUIButton::keyPressEvent(QKeyEvent* event)
{
    if ((event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter || event->key() == Qt::Key_Space) && isEnabled()) {
        m_isPressed = true;
        m_pressProgress = 1.0; // 直接设置，不使用动画
        update();
        emit pressed();
    }
    QWidget::keyPressEvent(event);
}

void QWinUIButton::keyReleaseEvent(QKeyEvent* event)
{
    if ((event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter || event->key() == Qt::Key_Space) && m_isPressed) {
        m_isPressed = false;
        m_pressProgress = 0.0; // 直接设置，不使用动画

        if (isEnabled()) {
            emit clicked();
        }

        update();
        emit released();
    }
    QWinUIWidget::keyReleaseEvent(event);
}

void QWinUIButton::focusInEvent(QFocusEvent* event)
{
    m_isFocused = true;
    update();
    QWinUIWidget::focusInEvent(event);
}

void QWinUIButton::focusOutEvent(QFocusEvent* event)
{
    m_isFocused = false;
    update();
    QWinUIWidget::focusOutEvent(event);
}

void QWinUIButton::onThemeChanged()
{
    updateButtonAppearance();
}

void QWinUIButton::updateButtonAppearance()
{
    update();
}

// 为子类提供的状态设置方法
void QWinUIButton::setCheckedState(bool checked)
{
    if (m_isChecked != checked) {
        m_isChecked = checked;
        updateButtonAppearance();
    }
}

bool QWinUIButton::isCheckedState() const
{
    return m_isChecked;
}

QT_END_NAMESPACE
