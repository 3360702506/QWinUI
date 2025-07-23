#include "QWinUI/Controls/QWinUIRadioButton.h"
#include "QWinUI/QWinUITheme.h"
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QFontMetrics>
#include <QApplication>

QWinUIRadioButton::QWinUIRadioButton(QWidget* parent)
    : QWinUIWidget(parent)
    , m_text("")
    , m_checked(false)
    , m_groupName("")
    , m_animationDuration(DEFAULT_ANIMATION_DURATION)
    , m_isHovered(false)
    , m_isPressed(false)
    , m_isFocused(false)
    , m_hoverProgress(0.0)
    , m_pressProgress(0.0)
    , m_checkProgress(0.0)
    , m_hoverAnimation(nullptr)
    , m_pressAnimation(nullptr)
    , m_checkAnimation(nullptr)
{
    initializeRadioButton();
}

QWinUIRadioButton::QWinUIRadioButton(const QString& text, QWidget* parent)
    : QWinUIWidget(parent)
    , m_text(text)
    , m_checked(false)
    , m_groupName("")
    , m_animationDuration(DEFAULT_ANIMATION_DURATION)
    , m_isHovered(false)
    , m_isPressed(false)
    , m_isFocused(false)
    , m_hoverProgress(0.0)
    , m_pressProgress(0.0)
    , m_checkProgress(0.0)
    , m_hoverAnimation(nullptr)
    , m_pressAnimation(nullptr)
    , m_checkAnimation(nullptr)
{
    initializeRadioButton();
}

QWinUIRadioButton::~QWinUIRadioButton()
{
    // 动画对象会被 Qt 的父子关系自动清理
}

void QWinUIRadioButton::initializeRadioButton()
{
    setFocusPolicy(Qt::StrongFocus);
    setCursor(Qt::PointingHandCursor);
    
    // 设置最小尺寸
    setMinimumHeight(MINIMUM_HEIGHT);
    
    // 设置动画
    setupAnimations();
    
    // 连接主题变化信号
    QWinUITheme* theme = QWinUITheme::getInstance();
    if (theme) {
        connect(theme, &QWinUITheme::themeChanged, this, &QWinUIRadioButton::onThemeChanged);
    }
}

void QWinUIRadioButton::setupAnimations()
{
    // 悬停动画
    m_hoverAnimation = new QPropertyAnimation(this, "hoverProgress", this);
    m_hoverAnimation->setDuration(m_animationDuration);
    m_hoverAnimation->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_hoverAnimation, &QPropertyAnimation::finished, this, &QWinUIRadioButton::onHoverAnimationFinished);

    // 按压动画 - 更快的响应，符合 WinUI 3 的即时反馈
    m_pressAnimation = new QPropertyAnimation(this, "pressProgress", this);
    m_pressAnimation->setDuration(m_animationDuration / 3); // 按压动画更快，提供即时反馈
    m_pressAnimation->setEasingCurve(QEasingCurve::OutQuart); // 更快的缓动曲线
    connect(m_pressAnimation, &QPropertyAnimation::finished, this, &QWinUIRadioButton::onPressAnimationFinished);

    // 选中动画
    m_checkAnimation = new QPropertyAnimation(this, "checkProgress", this);
    m_checkAnimation->setDuration(m_animationDuration);
    m_checkAnimation->setEasingCurve(QEasingCurve::OutBack);
    connect(m_checkAnimation, &QPropertyAnimation::finished, this, &QWinUIRadioButton::onCheckAnimationFinished);
}

// 属性访问器
QString QWinUIRadioButton::text() const
{
    return m_text;
}

void QWinUIRadioButton::setText(const QString& text)
{
    if (m_text != text) {
        m_text = text;
        updateGeometry();
        update();
        emit textChanged(text);
    }
}

bool QWinUIRadioButton::isChecked() const
{
    return m_checked;
}

void QWinUIRadioButton::setChecked(bool checked)
{
    if (m_checked != checked) {
        m_checked = checked;
        
        if (checked) {
            // 取消同组其他按钮的选中状态
            updateGroupSelection();
        }
        
        // 启动选中动画
        startCheckAnimation(checked);
        
        emit checkedChanged(checked);
    }
}

QString QWinUIRadioButton::groupName() const
{
    return m_groupName;
}

void QWinUIRadioButton::setGroupName(const QString& groupName)
{
    if (m_groupName != groupName) {
        m_groupName = groupName;
        emit groupNameChanged(groupName);
    }
}

int QWinUIRadioButton::animationDuration() const
{
    return m_animationDuration;
}

void QWinUIRadioButton::setAnimationDuration(int duration)
{
    if (m_animationDuration != duration) {
        m_animationDuration = duration;
        
        // 更新动画持续时间
        if (m_hoverAnimation) {
            m_hoverAnimation->setDuration(duration);
        }
        if (m_pressAnimation) {
            m_pressAnimation->setDuration(duration / 3); // 保持快速的按压响应
        }
        if (m_checkAnimation) {
            m_checkAnimation->setDuration(duration);
        }
        
        emit animationDurationChanged(duration);
    }
}

QSize QWinUIRadioButton::sizeHint() const
{
    QFontMetrics fm(font());
    int textWidth = m_text.isEmpty() ? 0 : fm.horizontalAdvance(m_text);
    int width = INDICATOR_SIZE + (textWidth > 0 ? TEXT_SPACING + textWidth : 0);
    int height = qMax(INDICATOR_SIZE, fm.height());
    height = qMax(height, MINIMUM_HEIGHT);
    
    return QSize(width, height);
}

QSize QWinUIRadioButton::minimumSizeHint() const
{
    return QSize(INDICATOR_SIZE, MINIMUM_HEIGHT);
}

void QWinUIRadioButton::click()
{
    if (!m_checked) {
        setChecked(true);
        emit clicked();
    }
}

// 静态方法 - 分组管理
QList<QWinUIRadioButton*> QWinUIRadioButton::getGroupButtons(QWidget* parent, const QString& groupName)
{
    QList<QWinUIRadioButton*> buttons;
    if (!parent) return buttons;
    
    QList<QWinUIRadioButton*> allButtons = parent->findChildren<QWinUIRadioButton*>();
    for (QWinUIRadioButton* button : allButtons) {
        if (button->parentWidget() == parent) {
            if (groupName.isEmpty() || button->groupName() == groupName) {
                buttons.append(button);
            }
        }
    }
    
    return buttons;
}

QWinUIRadioButton* QWinUIRadioButton::getCheckedButton(QWidget* parent, const QString& groupName)
{
    QList<QWinUIRadioButton*> buttons = getGroupButtons(parent, groupName);
    for (QWinUIRadioButton* button : buttons) {
        if (button->isChecked()) {
            return button;
        }
    }
    return nullptr;
}

// 事件处理
void QWinUIRadioButton::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect = this->rect();

    // 计算指示器和文本区域
    QRect indicatorRect(0, (rect.height() - INDICATOR_SIZE) / 2, INDICATOR_SIZE, INDICATOR_SIZE);
    QRect textRect;

    if (!m_text.isEmpty()) {
        textRect = QRect(INDICATOR_SIZE + TEXT_SPACING, 0,
                        rect.width() - INDICATOR_SIZE - TEXT_SPACING, rect.height());
    }

    // WinUI 3 风格：不绘制焦点边框，焦点通过其他方式表示
    // if (m_isFocused) {
    //     // 焦点状态可以通过悬浮效果或其他方式表示
    // }

    // 绘制单选按钮指示器
    drawRadioIndicator(&painter, indicatorRect);

    // 绘制文本
    if (!m_text.isEmpty()) {
        drawText(&painter, textRect);
    }
}

void QWinUIRadioButton::drawRadioIndicator(QPainter* painter, const QRect& rect)
{
    QWinUITheme* theme = QWinUITheme::getInstance();

    // 确保圆形完美居中，使用更细的边框
    int borderWidth = 1; // 更细的边框
    int adjustment = borderWidth;
    QRect circleRect = rect.adjusted(adjustment, adjustment, -adjustment, -adjustment);

    // 确保圆形是正圆（宽高相等）
    int size = qMin(circleRect.width(), circleRect.height());
    circleRect.setSize(QSize(size, size));
    circleRect.moveCenter(rect.center());

    if (m_checked) {
        // 选中状态：绘制同心圆
        QColor outerColor = theme ? theme->accentColor() : QColor(0, 120, 215);
        QColor innerColor = QColor(255, 255, 255); // 内圆始终是白色

        if (!isEnabled()) {
            outerColor.setAlpha(outerColor.alpha() / 2);
            innerColor.setAlpha(innerColor.alpha() / 2);
        }

        // 按压状态下外圆颜色稍微变深
        if (m_pressProgress > 0.0) {
            if (theme && theme->isDarkMode()) {
                // 深色模式下按压时颜色稍微变亮
                outerColor = outerColor.lighter(110 + 10 * m_pressProgress);
            } else {
                // 浅色模式下按压时颜色稍微变深
                outerColor = outerColor.darker(110 + 10 * m_pressProgress);
            }
        }

        // 绘制外圆（强调色）
        painter->setPen(Qt::NoPen);
        painter->setBrush(outerColor);
        painter->drawEllipse(circleRect);

        // 绘制内圆（白色），确保完美居中
        if (m_checkProgress > 0.0) {
            // 基础内圆大小（外圆的40%）
            double baseInnerRatio = 0.4;

            // 悬浮时内圆变大（增加25%）
            double hoverMultiplier = 1.0 + (0.25 * m_hoverProgress);

            // 按压时内圆明显缩小（减少30%），提供清晰的视觉反馈
            double pressMultiplier = 1.0 - (0.3 * m_pressProgress);

            // 计算最终的内圆大小
            double finalRatio = baseInnerRatio * hoverMultiplier * pressMultiplier;
            double innerDiameter = circleRect.width() * finalRatio * m_checkProgress;

            // 直接使用外圆的圆心作为内圆圆心
            int innerSize = qRound(innerDiameter);
            QRect innerRect = circleRect; // 从外圆开始
            innerRect.setSize(QSize(innerSize, innerSize));
            innerRect.moveCenter(circleRect.center()); // 确保圆心一致

            painter->setPen(Qt::NoPen);
            painter->setBrush(innerColor);
            painter->drawEllipse(innerRect);
        }
    } else {
        // 未选中状态：绘制空心圆
        QColor borderColor;
        QColor fillColor = Qt::transparent;

        // 获取边框颜色
        if (theme) {
            if (theme->isDarkMode()) {
                borderColor = QColor(255, 255, 255, 160);
            } else {
                borderColor = QColor(0, 0, 0, 160);
            }
        } else {
            borderColor = QColor(0, 0, 0, 160);
        }

        if (!isEnabled()) {
            borderColor.setAlpha(borderColor.alpha() / 2);
        }

        // 悬停效果：微妙的背景填充
        if (m_hoverProgress > 0.0) {
            QColor hoverFill;
            if (theme && theme->isDarkMode()) {
                hoverFill = QColor(255, 255, 255, 12); // 深色模式下更微妙
            } else {
                hoverFill = QColor(0, 0, 0, 8); // 浅色模式下更微妙
            }
            hoverFill.setAlpha(hoverFill.alpha() * m_hoverProgress);
            fillColor = hoverFill;
        }

        // 按压效果：显示灰色外圈和白色内圈，模拟选中状态但使用灰色
        if (m_pressProgress > 0.0) {
            // 绘制灰色外圈（类似选中状态的蓝色外圈）
            QColor pressOuterColor;
            if (theme && theme->isDarkMode()) {
                pressOuterColor = QColor(160, 160, 160); // 深色模式下的灰色
            } else {
                pressOuterColor = QColor(120, 120, 120); // 浅色模式下的灰色
            }
            pressOuterColor.setAlpha(255 * m_pressProgress);

            // 绘制灰色外圈
            painter->setPen(Qt::NoPen);
            painter->setBrush(pressOuterColor);
            painter->drawEllipse(circleRect);

            // 绘制白色内圈
            QColor innerColor = QColor(255, 255, 255);
            double innerRatio = 0.4; // 与选中状态相同的比例
            int innerSize = circleRect.width() * innerRatio * m_pressProgress;

            QRect innerRect = circleRect;
            innerRect.setSize(QSize(innerSize, innerSize));
            innerRect.moveCenter(circleRect.center());

            painter->setBrush(innerColor);
            painter->drawEllipse(innerRect);
        } else {
            // 正常状态：绘制边框圆圈
            if (m_hoverProgress > 0.0) {
                QColor hoverFill;
                if (theme && theme->isDarkMode()) {
                    hoverFill = QColor(255, 255, 255, 12);
                } else {
                    hoverFill = QColor(0, 0, 0, 8);
                }
                hoverFill.setAlpha(hoverFill.alpha() * m_hoverProgress);
                fillColor = hoverFill;
            }

            // 绘制外圆边框（更细的边框）
            painter->setPen(QPen(borderColor, borderWidth));
            painter->setBrush(fillColor);
            painter->drawEllipse(circleRect);
        }
    }
}

void QWinUIRadioButton::drawText(QPainter* painter, const QRect& rect)
{
    QWinUITheme* theme = QWinUITheme::getInstance();
    QColor textColor;

    if (theme) {
        textColor = theme->isDarkMode() ? QColor(255, 255, 255) : QColor(0, 0, 0);
    } else {
        textColor = QColor(0, 0, 0);
    }

    if (!isEnabled()) {
        textColor.setAlpha(128);
    }

    painter->setPen(textColor);
    painter->setFont(font());
    painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, m_text);
}

void QWinUIRadioButton::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && isEnabled()) {
        m_isPressed = true;
        startPressAnimation(true);
        setFocus();

        // 立即更新显示，提供即时的视觉反馈
        update();
    }
    QWinUIWidget::mousePressEvent(event);
}

void QWinUIRadioButton::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_isPressed) {
        m_isPressed = false;
        startPressAnimation(false);

        if (rect().contains(event->pos()) && isEnabled()) {
            click();
        }

        // 立即更新显示
        update();
    }
    QWinUIWidget::mouseReleaseEvent(event);
}

void QWinUIRadioButton::enterEvent(QEnterEvent* event)
{
    if (isEnabled()) {
        m_isHovered = true;
        startHoverAnimation(true);
    }
    QWinUIWidget::enterEvent(event);
}

void QWinUIRadioButton::leaveEvent(QEvent* event)
{
    m_isHovered = false;
    m_isPressed = false;
    startHoverAnimation(false);
    startPressAnimation(false);
    QWinUIWidget::leaveEvent(event);
}

void QWinUIRadioButton::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
    case Qt::Key_Space:
    case Qt::Key_Return:
    case Qt::Key_Enter:
        if (isEnabled()) {
            click();
        }
        event->accept();
        break;
    case Qt::Key_Up:
    case Qt::Key_Left:
        navigateToPreviousButton();
        event->accept();
        break;
    case Qt::Key_Down:
    case Qt::Key_Right:
        navigateToNextButton();
        event->accept();
        break;
    default:
        QWinUIWidget::keyPressEvent(event);
        break;
    }
}

void QWinUIRadioButton::focusInEvent(QFocusEvent* event)
{
    m_isFocused = true;
    // 获得焦点时启动轻微的悬浮效果
    if (!m_isHovered) {
        startHoverAnimation(true);
    }
    QWinUIWidget::focusInEvent(event);
}

void QWinUIRadioButton::focusOutEvent(QFocusEvent* event)
{
    m_isFocused = false;
    // 失去焦点时，如果没有悬浮则停止悬浮效果
    if (!m_isHovered) {
        startHoverAnimation(false);
    }
    QWinUIWidget::focusOutEvent(event);
}

void QWinUIRadioButton::onThemeChanged()
{
    update();
}

// 动画槽函数
void QWinUIRadioButton::onHoverAnimationFinished()
{
    // 悬停动画完成
}

void QWinUIRadioButton::onPressAnimationFinished()
{
    // 按压动画完成
}

void QWinUIRadioButton::onCheckAnimationFinished()
{
    // 选中动画完成
}

// 私有方法
void QWinUIRadioButton::startHoverAnimation(bool hovered)
{
    if (m_hoverAnimation->state() == QAbstractAnimation::Running) {
        m_hoverAnimation->stop();
    }

    m_hoverAnimation->setStartValue(m_hoverProgress);
    m_hoverAnimation->setEndValue(hovered ? 1.0 : 0.0);
    m_hoverAnimation->start();
}

void QWinUIRadioButton::startPressAnimation(bool pressed)
{
    if (m_pressAnimation->state() == QAbstractAnimation::Running) {
        m_pressAnimation->stop();
    }

    m_pressAnimation->setStartValue(m_pressProgress);
    m_pressAnimation->setEndValue(pressed ? 1.0 : 0.0);
    m_pressAnimation->start();
}

void QWinUIRadioButton::startCheckAnimation(bool checked)
{
    if (m_checkAnimation->state() == QAbstractAnimation::Running) {
        m_checkAnimation->stop();
    }

    m_checkAnimation->setStartValue(m_checkProgress);
    m_checkAnimation->setEndValue(checked ? 1.0 : 0.0);
    m_checkAnimation->start();
}

void QWinUIRadioButton::updateGroupSelection()
{
    if (!m_checked) return;

    // 获取同组的所有单选按钮
    QList<QWinUIRadioButton*> siblings = getSiblingButtons();

    // 取消其他按钮的选中状态
    for (QWinUIRadioButton* sibling : siblings) {
        if (sibling != this && sibling->isChecked()) {
            sibling->m_checked = false;
            sibling->startCheckAnimation(false);
            emit sibling->checkedChanged(false);
        }
    }
}

QList<QWinUIRadioButton*> QWinUIRadioButton::getSiblingButtons() const
{
    QWidget* parent = parentWidget();
    if (!parent) return QList<QWinUIRadioButton*>();

    return getGroupButtons(parent, m_groupName);
}

void QWinUIRadioButton::navigateToNextButton()
{
    QList<QWinUIRadioButton*> siblings = getSiblingButtons();
    if (siblings.size() <= 1) return;

    int currentIndex = siblings.indexOf(const_cast<QWinUIRadioButton*>(this));
    if (currentIndex >= 0) {
        int nextIndex = (currentIndex + 1) % siblings.size();
        QWinUIRadioButton* nextButton = siblings[nextIndex];
        nextButton->setFocus();
        nextButton->click();
    }
}

void QWinUIRadioButton::navigateToPreviousButton()
{
    QList<QWinUIRadioButton*> siblings = getSiblingButtons();
    if (siblings.size() <= 1) return;

    int currentIndex = siblings.indexOf(const_cast<QWinUIRadioButton*>(this));
    if (currentIndex >= 0) {
        int prevIndex = (currentIndex - 1 + siblings.size()) % siblings.size();
        QWinUIRadioButton* prevButton = siblings[prevIndex];
        prevButton->setFocus();
        prevButton->click();
    }
}

// 状态检查方法
bool QWinUIRadioButton::isHovered() const
{
    return m_isHovered;
}

bool QWinUIRadioButton::isPressed() const
{
    return m_isPressed;
}

bool QWinUIRadioButton::isFocused() const
{
    return m_isFocused;
}
