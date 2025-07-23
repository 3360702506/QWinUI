#include "QWinUI/Controls/QWinUIToggleButton.h"
#include "QWinUI/QWinUITheme.h"
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QEasingCurve>
#include <QApplication>

QWinUIToggleButton::QWinUIToggleButton(QWidget* parent)
    : QWinUIButton(parent)
    , m_isChecked(false)
    , m_checkedText("Checked")
    , m_uncheckedText("Unchecked")
    , m_toggleAnimation(nullptr)
    , m_backgroundColorAnimation(nullptr)
    , m_textColorAnimation(nullptr)
    , m_animationEnabled(true)
{
    initializeToggleButton();
}

QWinUIToggleButton::QWinUIToggleButton(const QString& text, QWidget* parent)
    : QWinUIButton(text, parent)
    , m_isChecked(false)
    , m_checkedText(text)
    , m_uncheckedText(text)
    , m_toggleAnimation(nullptr)
    , m_backgroundColorAnimation(nullptr)
    , m_textColorAnimation(nullptr)
    , m_animationEnabled(true)
{
    initializeToggleButton();
}

QWinUIToggleButton::QWinUIToggleButton(const QString& checkedText, const QString& uncheckedText, QWidget* parent)
    : QWinUIButton(uncheckedText, parent)
    , m_isChecked(false)
    , m_checkedText(checkedText)
    , m_uncheckedText(uncheckedText)
    , m_toggleAnimation(nullptr)
    , m_backgroundColorAnimation(nullptr)
    , m_textColorAnimation(nullptr)
    , m_animationEnabled(true)
{
    initializeToggleButton();
}

QWinUIToggleButton::~QWinUIToggleButton()
{
    if (m_toggleAnimation) {
        m_toggleAnimation->stop();
        delete m_toggleAnimation;
    }
}

void QWinUIToggleButton::initializeToggleButton()
{
    // 设置基本属性
    // QWinUIButton 没有 setCheckable 方法，我们自己管理选中状态

    // 初始化动画
    setupColorAnimations();
    
    // 初始化颜色
    updateColors();
    
    // 设置初始文本
    updateButtonText();
    
    // 连接主题变化信号
    QWinUITheme* theme = QWinUITheme::getInstance();
    if (theme) {
        connect(theme, &QWinUITheme::themeChanged, this, &QWinUIToggleButton::updateColors);
    }
}

void QWinUIToggleButton::setupColorAnimations()
{
    // 创建动画组
    m_toggleAnimation = new QParallelAnimationGroup(this);
    
    // 背景色动画
    m_backgroundColorAnimation = new QPropertyAnimation(this, "currentBackgroundColor", this);
    m_backgroundColorAnimation->setDuration(TOGGLE_ANIMATION_DURATION);
    m_backgroundColorAnimation->setEasingCurve(QEasingCurve::OutCubic);
    
    // 文本色动画
    m_textColorAnimation = new QPropertyAnimation(this, "currentTextColor", this);
    m_textColorAnimation->setDuration(TOGGLE_ANIMATION_DURATION);
    m_textColorAnimation->setEasingCurve(QEasingCurve::OutCubic);
    
    // 添加到动画组
    m_toggleAnimation->addAnimation(m_backgroundColorAnimation);
    m_toggleAnimation->addAnimation(m_textColorAnimation);
    
    // 连接动画完成信号
    connect(m_toggleAnimation, &QParallelAnimationGroup::finished,
            this, &QWinUIToggleButton::onToggleAnimationFinished);
    
    // 颜色动画现在由父类处理，不需要这些连接
}

void QWinUIToggleButton::updateColors()
{
    // 颜色现在由父类 QWinUIButton 管理
    // 我们只需要确保父类知道当前的 checked 状态
    setCheckedState(m_isChecked);
    update();
}

void QWinUIToggleButton::updateButtonText()
{
    setText(m_isChecked ? m_checkedText : m_uncheckedText);
}

// 属性访问器
bool QWinUIToggleButton::isChecked() const
{
    return m_isChecked;
}

void QWinUIToggleButton::setChecked(bool checked)
{
    setChecked(checked, m_animationEnabled);
}

void QWinUIToggleButton::setChecked(bool checked, bool animated)
{
    if (m_isChecked != checked) {
        updateToggleState(checked, animated);
    }
}

QString QWinUIToggleButton::checkedText() const
{
    return m_checkedText;
}

void QWinUIToggleButton::setCheckedText(const QString& text)
{
    if (m_checkedText != text) {
        m_checkedText = text;
        if (m_isChecked) {
            updateButtonText();
        }
        emit checkedTextChanged(text);
    }
}

QString QWinUIToggleButton::uncheckedText() const
{
    return m_uncheckedText;
}

void QWinUIToggleButton::setUncheckedText(const QString& text)
{
    if (m_uncheckedText != text) {
        m_uncheckedText = text;
        if (!m_isChecked) {
            updateButtonText();
        }
        emit uncheckedTextChanged(text);
    }
}

// 颜色属性
QColor QWinUIToggleButton::checkedBackgroundColor() const
{
    return m_checkedBackgroundColor;
}

void QWinUIToggleButton::setCheckedBackgroundColor(const QColor& color)
{
    if (m_checkedBackgroundColor != color) {
        m_checkedBackgroundColor = color;
        if (m_isChecked && m_toggleAnimation->state() != QAbstractAnimation::Running) {
            m_currentBackgroundColor = color;
            update();
        }
    }
}

QColor QWinUIToggleButton::uncheckedBackgroundColor() const
{
    return m_uncheckedBackgroundColor;
}

void QWinUIToggleButton::setUncheckedBackgroundColor(const QColor& color)
{
    if (m_uncheckedBackgroundColor != color) {
        m_uncheckedBackgroundColor = color;
        if (!m_isChecked && m_toggleAnimation->state() != QAbstractAnimation::Running) {
            m_currentBackgroundColor = color;
            update();
        }
    }
}

QColor QWinUIToggleButton::checkedTextColor() const
{
    return m_checkedTextColor;
}

void QWinUIToggleButton::setCheckedTextColor(const QColor& color)
{
    if (m_checkedTextColor != color) {
        m_checkedTextColor = color;
        if (m_isChecked && m_toggleAnimation->state() != QAbstractAnimation::Running) {
            m_currentTextColor = color;
            update();
        }
    }
}

QColor QWinUIToggleButton::uncheckedTextColor() const
{
    return m_uncheckedTextColor;
}

void QWinUIToggleButton::setUncheckedTextColor(const QColor& color)
{
    if (m_uncheckedTextColor != color) {
        m_uncheckedTextColor = color;
        if (!m_isChecked && m_toggleAnimation->state() != QAbstractAnimation::Running) {
            m_currentTextColor = color;
            update();
        }
    }
}

// 当前动画颜色
QColor QWinUIToggleButton::currentBackgroundColor() const
{
    return m_currentBackgroundColor;
}

void QWinUIToggleButton::setCurrentBackgroundColor(const QColor& color)
{
    if (m_currentBackgroundColor != color) {
        m_currentBackgroundColor = color;
        update();
    }
}

QColor QWinUIToggleButton::currentTextColor() const
{
    return m_currentTextColor;
}

void QWinUIToggleButton::setCurrentTextColor(const QColor& color)
{
    if (m_currentTextColor != color) {
        m_currentTextColor = color;
        update();
    }
}

void QWinUIToggleButton::toggle()
{
    setChecked(!m_isChecked);
}

// 事件处理
void QWinUIToggleButton::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        // 切换状态
        toggle();

        // 调用父类处理其他鼠标事件（如视觉反馈）
        QWinUIButton::mousePressEvent(event);
    } else {
        QWinUIButton::mousePressEvent(event);
    }
}

void QWinUIToggleButton::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Space || event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        // 键盘激活切换
        toggle();
        event->accept();
    } else {
        QWinUIButton::keyPressEvent(event);
    }
}

void QWinUIToggleButton::paintEvent(QPaintEvent* event)
{
    // 使用父类的绘制系统，它会自动处理 checked 状态
    QWinUIButton::paintEvent(event);
}

void QWinUIToggleButton::onThemeChanged()
{
    updateColors();
    // 调用父类的主题变化处理（如果是公有方法）
    // QWinUIButton::onThemeChanged();
}

// 私有槽函数
void QWinUIToggleButton::onToggleAnimationFinished()
{
    // 动画完成后确保颜色正确
    updateCurrentColors();
}

void QWinUIToggleButton::updateCurrentColors()
{
    update();
}

// 私有方法
void QWinUIToggleButton::updateToggleState(bool checked, bool animated)
{
    bool oldChecked = m_isChecked;
    m_isChecked = checked;

    // 更新父类的 checked 状态，这会自动触发样式更新
    setCheckedState(m_isChecked);

    // 更新文本
    updateButtonText();

    // 如果启用动画，可以在这里添加文本切换动画
    // 但颜色动画由父类的样式系统处理
    if (!animated || !m_animationEnabled) {
        update();
    }

    // 发射信号
    if (oldChecked != m_isChecked) {
        emit checkedChanged(m_isChecked);
        emit toggled(m_isChecked);
    }
}

void QWinUIToggleButton::startToggleAnimation()
{
    if (m_toggleAnimation->state() == QAbstractAnimation::Running) {
        m_toggleAnimation->stop();
    }

    // 设置动画起始和结束值
    QColor targetBackgroundColor = m_isChecked ? m_checkedBackgroundColor : m_uncheckedBackgroundColor;
    QColor targetTextColor = m_isChecked ? m_checkedTextColor : m_uncheckedTextColor;

    m_backgroundColorAnimation->setStartValue(m_currentBackgroundColor);
    m_backgroundColorAnimation->setEndValue(targetBackgroundColor);

    m_textColorAnimation->setStartValue(m_currentTextColor);
    m_textColorAnimation->setEndValue(targetTextColor);

    // 启动动画
    m_toggleAnimation->start();
}
