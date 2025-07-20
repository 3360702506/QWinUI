#include "QWinUI/Controls/QWinUIToggleSwitch.h"
#include "QWinUI/QWinUITheme.h"
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPropertyAnimation>
#include <QEasingCurve>

QWinUIToggleSwitch::QWinUIToggleSwitch(QWidget* parent)
    : QWinUIWidget(parent)
    , m_isOn(false)
    , m_onContent("On")
    , m_offContent("Off")
    , m_isHovered(false)
    , m_isPressed(false)
    , m_isDragging(false)
    , m_thumbPosition(0.0)
    , m_dragStartThumbPos(0.0)
    , m_hoverProgress(0.0)
    , m_pressProgress(0.0)
    , m_thumbAnimation(nullptr)
    , m_hoverAnimation(nullptr)
    , m_pressAnimation(nullptr)
{
    initializeToggleSwitch();
}

QWinUIToggleSwitch::QWinUIToggleSwitch(const QString& text, QWidget* parent)
    : QWinUIWidget(parent)
    , m_isOn(false)
    , m_onContent("On")
    , m_offContent(text.isEmpty() ? "Off" : text)
    , m_isHovered(false)
    , m_isPressed(false)
    , m_isDragging(false)
    , m_thumbPosition(0.0)
    , m_dragStartThumbPos(0.0)
    , m_hoverProgress(0.0)
    , m_pressProgress(0.0)
    , m_thumbAnimation(nullptr)
    , m_hoverAnimation(nullptr)
    , m_pressAnimation(nullptr)
{
    initializeToggleSwitch();
}

QWinUIToggleSwitch::~QWinUIToggleSwitch()
{
    if (m_thumbAnimation) {
        m_thumbAnimation->stop();
        delete m_thumbAnimation;
    }
    if (m_hoverAnimation) {
        m_hoverAnimation->stop();
        delete m_hoverAnimation;
    }
    if (m_pressAnimation) {
        m_pressAnimation->stop();
        delete m_pressAnimation;
    }
}

void QWinUIToggleSwitch::initializeToggleSwitch()
{
    // 设置基本属性
    setMinimumSize(TRACK_WIDTH + 60, TRACK_HEIGHT + 8); // 为文本留出空间
    setCursor(Qt::PointingHandCursor);
    setFocusPolicy(Qt::StrongFocus);
    
    // 创建动画
    m_thumbAnimation = new QPropertyAnimation(this, "thumbPosition");
    m_thumbAnimation->setDuration(150);
    m_thumbAnimation->setEasingCurve(QEasingCurve::OutCubic);
    
    m_hoverAnimation = new QPropertyAnimation(this, "hoverProgress");
    m_hoverAnimation->setDuration(100);
    m_hoverAnimation->setEasingCurve(QEasingCurve::OutQuad);
    
    m_pressAnimation = new QPropertyAnimation(this, "pressProgress");
    m_pressAnimation->setDuration(50);
    m_pressAnimation->setEasingCurve(QEasingCurve::OutQuad);
    
    // 连接主题变化信号
    connect(this, &QWinUIWidget::themeChanged, this, &QWinUIToggleSwitch::onThemeChanged);
    
    updateToggleSwitch();
}

void QWinUIToggleSwitch::setIsOn(bool on)
{
    if (m_isOn != on) {
        m_isOn = on;
        
        // 启动滑块动画
        if (m_thumbAnimation) {
            m_thumbAnimation->stop();
            m_thumbAnimation->setStartValue(m_thumbPosition);
            m_thumbAnimation->setEndValue(on ? 1.0 : 0.0);
            m_thumbAnimation->start();
        } else {
            m_thumbPosition = on ? 1.0 : 0.0;
            update();
        }
        
        emit toggled(on);
    }
}

void QWinUIToggleSwitch::setOnContent(const QString& content)
{
    if (m_onContent != content) {
        m_onContent = content;
        update();
        emit onContentChanged(content);
    }
}

void QWinUIToggleSwitch::setOffContent(const QString& content)
{
    if (m_offContent != content) {
        m_offContent = content;
        update();
        emit offContentChanged(content);
    }
}

void QWinUIToggleSwitch::setThumbPosition(double position)
{
    if (m_thumbPosition != position) {
        m_thumbPosition = position;
        update();
    }
}

void QWinUIToggleSwitch::setHoverProgress(double progress)
{
    if (m_hoverProgress != progress) {
        m_hoverProgress = progress;
        update();
    }
}

void QWinUIToggleSwitch::setPressProgress(double progress)
{
    if (m_pressProgress != progress) {
        m_pressProgress = progress;
        update();
    }
}

void QWinUIToggleSwitch::toggle()
{
    setIsOn(!m_isOn);
}

void QWinUIToggleSwitch::updateToggleSwitch()
{
    update();
}

void QWinUIToggleSwitch::onThemeChanged()
{
    update();
}

void QWinUIToggleSwitch::paintEvent(QPaintEvent* event)
{
    // 首先调用父类的paintEvent来处理主题切换动画
    QWinUIWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect switchRect(4, (height() - TRACK_HEIGHT) / 2, TRACK_WIDTH, TRACK_HEIGHT);

    // 绘制轨道
    drawTrack(&painter, switchRect);

    // 绘制滑块
    drawThumb(&painter, switchRect);

    // 绘制内容文本
    QRect contentRect(TRACK_WIDTH + CONTENT_SPACING + 4, 0, width() - TRACK_WIDTH - CONTENT_SPACING - 8, height());
    drawContent(&painter, contentRect);
}

void QWinUIToggleSwitch::drawTrack(QPainter* painter, const QRect& rect)
{
    painter->save();

    QColor trackColor = getTrackColor();

    // 绘制轨道背景
    painter->setBrush(trackColor);
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(rect, TRACK_HEIGHT / 2, TRACK_HEIGHT / 2);

    painter->restore();
}

void QWinUIToggleSwitch::drawThumb(QPainter* painter, const QRect& rect)
{
    painter->save();

    QColor thumbColor = getThumbColor();

    // 计算滑块位置 - 基于更小的尺寸计算
    int baseThumbSize = THUMB_SIZE - 4; // 默认状态比原来小4像素，让球更小
    int thumbTravel = rect.width() - THUMB_SIZE - 6; // 保持行程不变
    int thumbX = rect.x() + 3 + static_cast<int>(thumbTravel * m_thumbPosition);
    int thumbY = rect.y() + (rect.height() - baseThumbSize) / 2;

    // 根据状态调整滑块大小
    int thumbWidth = baseThumbSize;
    int thumbHeight = baseThumbSize;

    // 悬浮效果：恢复到稍大的尺寸
    if (m_hoverProgress > 0.0) {
        int hoverIncrease = static_cast<int>(2 * m_hoverProgress); // 最大增加2像素
        thumbWidth += hoverIncrease;
        thumbHeight += hoverIncrease;

        // 调整位置以保持居中
        thumbX -= static_cast<int>(1 * m_hoverProgress);
        thumbY -= static_cast<int>(1 * m_hoverProgress);
    }

    // 按下效果：变成更短的圆角矩形，高度保持和悬浮时一致
    if (m_pressProgress > 0.0) {
        // 高度保持不变（和悬浮时一样）
        // 变长：宽度增加3像素（更短的矩形）
        thumbWidth += static_cast<int>(3 * m_pressProgress);

        // 调整位置以保持居中
        thumbX -= static_cast<int>(1.5 * m_pressProgress); // 宽度增加的一半
        // Y位置保持居中，不需要额外调整
    }

    QRect thumbRect(thumbX, thumbY, thumbWidth, thumbHeight);

    // 绘制滑块
    painter->setBrush(thumbColor);
    painter->setPen(Qt::NoPen);

    if (m_pressProgress > 0.0) {
        // 按下时绘制圆角矩形
        int radius = thumbHeight / 2; // 圆角半径为高度的一半
        painter->drawRoundedRect(thumbRect, radius, radius);
    } else {
        // 正常状态绘制圆形
        painter->drawEllipse(thumbRect);
    }

    painter->restore();
}

void QWinUIToggleSwitch::drawContent(QPainter* painter, const QRect& rect)
{
    painter->save();

    QColor contentColor = getContentColor();
    painter->setPen(contentColor);

    QString content = m_isOn ? m_onContent : m_offContent;
    painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, content);

    painter->restore();
}

QColor QWinUIToggleSwitch::getTrackColor() const
{
    bool isDark = isDarkMode();

    if (!isEnabled()) {
        return isDark ? QColor(255, 255, 255, 25) : QColor(0, 0, 0, 25);
    }

    if (m_isOn) {
        // 开启状态：蓝色背景
        QColor onColor = QColor(0, 120, 215);

        // 悬浮效果
        if (m_hoverProgress > 0.0) {
            QColor hoverColor = QColor(16, 137, 232);
            onColor = QColor::fromRgbF(
                onColor.redF() + (hoverColor.redF() - onColor.redF()) * m_hoverProgress,
                onColor.greenF() + (hoverColor.greenF() - onColor.greenF()) * m_hoverProgress,
                onColor.blueF() + (hoverColor.blueF() - onColor.blueF()) * m_hoverProgress
            );
        }

        return onColor;
    } else {
        // 关闭状态：灰色背景
        QColor offColor = isDark ? QColor(255, 255, 255, 40) : QColor(0, 0, 0, 40);

        // 悬浮效果
        if (m_hoverProgress > 0.0) {
            QColor hoverColor = isDark ? QColor(255, 255, 255, 60) : QColor(0, 0, 0, 60);
            offColor = QColor::fromRgbF(
                offColor.redF() + (hoverColor.redF() - offColor.redF()) * m_hoverProgress,
                offColor.greenF() + (hoverColor.greenF() - offColor.greenF()) * m_hoverProgress,
                offColor.blueF() + (hoverColor.blueF() - offColor.blueF()) * m_hoverProgress,
                offColor.alphaF() + (hoverColor.alphaF() - offColor.alphaF()) * m_hoverProgress
            );
        }

        return offColor;
    }
}

QColor QWinUIToggleSwitch::getThumbColor() const
{
    bool isDark = isDarkMode();

    if (!isEnabled()) {
        return isDark ? QColor(255, 255, 255, 87) : QColor(0, 0, 0, 87);
    }

    if (m_isOn) {
        // 开启状态：白色滑块
        return QColor(255, 255, 255);
    } else {
        // 关闭状态：深色滑块
        QColor thumbColor = isDark ? QColor(255, 255, 255, 200) : QColor(0, 0, 0, 200);

        // 悬浮效果
        if (m_hoverProgress > 0.0) {
            QColor hoverColor = isDark ? QColor(255, 255, 255, 230) : QColor(0, 0, 0, 230);
            thumbColor = QColor::fromRgbF(
                thumbColor.redF() + (hoverColor.redF() - thumbColor.redF()) * m_hoverProgress,
                thumbColor.greenF() + (hoverColor.greenF() - thumbColor.greenF()) * m_hoverProgress,
                thumbColor.blueF() + (hoverColor.blueF() - thumbColor.blueF()) * m_hoverProgress,
                thumbColor.alphaF() + (hoverColor.alphaF() - thumbColor.alphaF()) * m_hoverProgress
            );
        }

        return thumbColor;
    }
}

QColor QWinUIToggleSwitch::getContentColor() const
{
    bool isDark = isDarkMode();

    if (!isEnabled()) {
        return isDark ? QColor(255, 255, 255, 87) : QColor(0, 0, 0, 87);
    }

    return isDark ? QColor(255, 255, 255) : QColor(0, 0, 0);
}

void QWinUIToggleSwitch::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && isEnabled()) {
        m_isPressed = true;
        m_isDragging = false; // 初始不是拖动状态
        m_dragStartPos = event->pos();
        m_dragStartThumbPos = m_thumbPosition;

        if (m_pressAnimation) {
            m_pressAnimation->stop();
            m_pressAnimation->setStartValue(m_pressProgress);
            m_pressAnimation->setEndValue(1.0);
            m_pressAnimation->start();
        } else {
            m_pressProgress = 1.0;
            update();
        }
    }
    QWinUIWidget::mousePressEvent(event);
}

void QWinUIToggleSwitch::mouseMoveEvent(QMouseEvent* event)
{
    if (m_isPressed && isEnabled()) {
        // 计算拖动距离
        int deltaX = event->pos().x() - m_dragStartPos.x();

        // 只有当拖动距离超过阈值时才开始拖动
        if (!m_isDragging && qAbs(deltaX) > 3) {
            m_isDragging = true;
        }

        if (m_isDragging) {
            // 计算轨道的实际可用宽度
            QRect switchRect(4, (height() - TRACK_HEIGHT) / 2, TRACK_WIDTH, TRACK_HEIGHT);
            int baseThumbSize = THUMB_SIZE - 4; // 与绘制时保持一致
            int trackWidth = switchRect.width() - baseThumbSize - 6; // 减去滑块大小和边距

            // 计算新的滑块位置
            double newPosition = m_dragStartThumbPos + (double)deltaX / trackWidth;
            newPosition = qBound(0.0, newPosition, 1.0); // 严格限制在0-1范围内

            // 停止滑块动画并直接设置位置
            if (m_thumbAnimation) {
                m_thumbAnimation->stop();
            }

            // 直接更新位置，不触发动画
            m_thumbPosition = newPosition;

            // 实时更新状态（但不发送信号，等到拖动结束再发送）
            bool currentState = newPosition > 0.5;
            if (currentState != m_isOn) {
                m_isOn = currentState;
                // 注意：这里不发送toggled信号，等到mouseRelease时再发送
            }

            update();
        }
    }
    QWinUIWidget::mouseMoveEvent(event);
}

void QWinUIToggleSwitch::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_isPressed) {
        m_isPressed = false;
        bool wasDragging = m_isDragging;
        m_isDragging = false;

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
            if (wasDragging) {
                // 拖动结束时，确保状态正确并发送信号
                bool finalState = m_thumbPosition > 0.5;

                // 确保滑块动画到正确的最终位置
                if (m_thumbAnimation) {
                    m_thumbAnimation->stop();
                    m_thumbAnimation->setStartValue(m_thumbPosition);
                    m_thumbAnimation->setEndValue(finalState ? 1.0 : 0.0);
                    m_thumbAnimation->start();
                } else {
                    m_thumbPosition = finalState ? 1.0 : 0.0;
                    update();
                }

                // 发送状态变化信号（如果状态确实改变了）
                if (finalState != m_isOn) {
                    m_isOn = finalState;
                    emit toggled(m_isOn);
                }
            } else {
                // 点击切换
                toggle();
            }
        }
    }
    QWinUIWidget::mouseReleaseEvent(event);
}

void QWinUIToggleSwitch::enterEvent(QEnterEvent* event)
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

void QWinUIToggleSwitch::leaveEvent(QEvent* event)
{
    m_isHovered = false;

    // 如果正在拖动，不要重置按下状态
    if (!m_isDragging) {
        m_isPressed = false;
    }

    if (m_hoverAnimation) {
        m_hoverAnimation->stop();
        m_hoverAnimation->setStartValue(m_hoverProgress);
        m_hoverAnimation->setEndValue(0.0);
        m_hoverAnimation->start();
    } else {
        m_hoverProgress = 0.0;
    }

    if (!m_isDragging && m_pressAnimation) {
        m_pressAnimation->stop();
        m_pressAnimation->setStartValue(m_pressProgress);
        m_pressAnimation->setEndValue(0.0);
        m_pressAnimation->start();
    } else if (!m_isDragging) {
        m_pressProgress = 0.0;
    }

    update();
    QWinUIWidget::leaveEvent(event);
}

void QWinUIToggleSwitch::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Space || event->key() == Qt::Key_Return) {
        if (isEnabled()) {
            toggle();
        }
        event->accept();
        return;
    }
    QWinUIWidget::keyPressEvent(event);
}

void QWinUIToggleSwitch::keyReleaseEvent(QKeyEvent* event)
{
    QWinUIWidget::keyReleaseEvent(event);
}
