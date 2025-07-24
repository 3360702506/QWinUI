#include "QWinUI/Controls/QWinUISplitButton.h"
#include "QWinUI/QWinUITheme.h"
#include <QApplication>
#include <QDebug>
#include <QPainter>
#include <QFontDatabase>



// QWinUISplitButton 实现
QWinUISplitButton::QWinUISplitButton(QWidget* parent)
    : QWinUIWidget(parent)
    , m_mainButton(nullptr)
    , m_dropDownButton(nullptr)
    , m_layout(nullptr)
    , m_text("")
    , m_icon()
    , m_flyout(nullptr)
    , m_dropDownAnimation(nullptr)
    , m_dropDownOffset(0.0)
{
    initializeButton();
}

QWinUISplitButton::QWinUISplitButton(const QString& text, QWidget* parent)
    : QWinUISplitButton(parent)
{
    setText(text);
}

QWinUISplitButton::QWinUISplitButton(const QString& text, const QIcon& icon, QWidget* parent)
    : QWinUISplitButton(parent)
{
    setText(text);
    setIcon(icon);
}

QWinUISplitButton::~QWinUISplitButton()
{
    if (m_flyout) {
        m_flyout->hide();
    }
}

void QWinUISplitButton::initializeButton()
{
    setFocusPolicy(Qt::StrongFocus);

    // 设置默认尺寸 - 使用最小高度而不是固定高度
    setMinimumHeight(BUTTON_HEIGHT);
    setMaximumHeight(BUTTON_HEIGHT);
    setMinimumWidth(80);

    // 创建水平布局
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);

    // 创建主按钮
    m_mainButton = new QWinUIButton(this);
    m_mainButton->setButtonStyle(QWinUIButton::Standard);
    m_mainButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_mainButton->setMinimumHeight(BUTTON_HEIGHT);
    m_mainButton->setMaximumHeight(BUTTON_HEIGHT);
    m_mainButton->setMinimumWidth(40); // 设置最小宽度防止重叠

    // 创建下拉按钮
    m_dropDownButton = new QWinUIDropDownButton(this);
    m_dropDownButton->setButtonStyle(QWinUIButton::Standard);
    m_dropDownButton->setFixedSize(DROPDOWN_WIDTH, BUTTON_HEIGHT);

    // 设置按钮样式，使它们看起来像一个整体
    // 不使用样式表，让QWinUIButton自己处理样式
    // 只是确保它们紧密相邻

    // 添加到布局
    m_layout->addWidget(m_mainButton);
    m_layout->addWidget(m_dropDownButton);

    // 创建箭头动画
    m_dropDownAnimation = new QPropertyAnimation(m_dropDownButton, "arrowOffset", this);
    m_dropDownAnimation->setDuration(200); // 增加动画时长使其更明显
    m_dropDownAnimation->setEasingCurve(QEasingCurve::OutBounce); // 使用弹跳效果

    // 确保动画属性正确设置
    m_dropDownAnimation->setStartValue(0.0);
    m_dropDownAnimation->setEndValue(0.0);



    // 连接信号
    connect(m_mainButton, &QWinUIButton::clicked, this, &QWinUISplitButton::onMainButtonClicked);
    connect(m_dropDownButton, &QWinUIButton::clicked, this, &QWinUISplitButton::onDropDownButtonClicked);

    // 连接按下和释放信号以实现动画
    connect(m_dropDownButton, &QWinUIButton::pressed, this, &QWinUISplitButton::onDropDownButtonPressed);
    connect(m_dropDownButton, &QWinUIButton::released, this, &QWinUISplitButton::onDropDownButtonReleased);

    // 连接主题变化信号
    QWinUITheme* theme = QWinUITheme::getInstance();
    if (theme) {
        connect(theme, &QWinUITheme::themeChanged, this, [this]() {
            // 主题变化时更新按钮样式
            if (m_mainButton) {
                m_mainButton->update();
            }
            if (m_dropDownButton) {
                m_dropDownButton->update();
            }
            update(); // 更新自身
        });

        // 连接颜色变化信号
        connect(theme, &QWinUITheme::colorChanged, this, [this](const QString& colorName, const QColor& color) {
            Q_UNUSED(colorName)
            Q_UNUSED(color)
            // 颜色变化时更新按钮
            if (m_mainButton) {
                m_mainButton->update();
            }
            if (m_dropDownButton) {
                m_dropDownButton->update();
            }
        });
    }
}

void QWinUISplitButton::setText(const QString& text)
{
    if (m_text != text) {
        m_text = text;
        if (m_mainButton) {
            m_mainButton->setText(text);
        }
        updateLayout();
        emit textChanged(text);
    }
}

void QWinUISplitButton::setIcon(const QIcon& icon)
{
    if (m_icon.cacheKey() != icon.cacheKey()) {
        m_icon = icon;
        if (m_mainButton) {
            m_mainButton->setIcon(icon);
        }
        updateLayout();
        emit iconChanged(icon);
    }
}

void QWinUISplitButton::setEnabled(bool enabled)
{
    QWinUIWidget::setEnabled(enabled);

    // 同步子按钮的enabled状态
    if (m_mainButton) {
        m_mainButton->setEnabled(enabled);
    }
    if (m_dropDownButton) {
        m_dropDownButton->setEnabled(enabled);
    }

    emit enabledChanged(enabled);
}

void QWinUISplitButton::setFlyout(QWinUIMenuFlyout* flyout)
{
    if (m_flyout != flyout) {
        // 断开旧连接
        if (m_flyout) {
            disconnect(m_flyout, nullptr, this, nullptr);
        }
        
        m_flyout = flyout;
        
        // 建立新连接
        if (m_flyout) {
            connect(m_flyout, &QWinUIMenuFlyout::aboutToShow, this, &QWinUISplitButton::onFlyoutAboutToShow);
            connect(m_flyout, &QWinUIMenuFlyout::aboutToHide, this, &QWinUISplitButton::onFlyoutAboutToHide);
        }
    }
}

void QWinUISplitButton::setHoverProgress(double progress)
{
    // 这个方法现在由内部按钮处理，保留接口兼容性
    Q_UNUSED(progress)
}

void QWinUISplitButton::setPressProgress(double progress)
{
    // 这个方法现在由内部按钮处理，保留接口兼容性
    Q_UNUSED(progress)
}

void QWinUISplitButton::setArrowOffset(double offset)
{
    if (qAbs(m_dropDownOffset - offset) > 1e-6) {
        m_dropDownOffset = offset;
        // 偏移现在由下拉按钮内部处理
    }
}

QSize QWinUISplitButton::sizeHint() const
{
    if (m_mainButton && m_dropDownButton) {
        QSize mainSize = m_mainButton->sizeHint();
        // 确保高度一致，宽度足够避免重叠
        int totalWidth = mainSize.width() + DROPDOWN_WIDTH;
        return QSize(qMax(totalWidth, 100), BUTTON_HEIGHT); // 增加最小宽度
    }
    return QSize(100, BUTTON_HEIGHT);
}

QSize QWinUISplitButton::minimumSizeHint() const
{
    if (m_mainButton && m_dropDownButton) {
        QSize mainSize = m_mainButton->minimumSizeHint();
        // 确保最小宽度包含下拉按钮，防止重叠
        int totalWidth = qMax(mainSize.width(), 40) + DROPDOWN_WIDTH; // 增加主按钮最小宽度
        return QSize(qMax(totalWidth, 80), BUTTON_HEIGHT); // 设置绝对最小宽度
    }
    return QSize(80, BUTTON_HEIGHT);
}

void QWinUISplitButton::updateLayout()
{
    updateGeometry();
}

void QWinUISplitButton::startArrowAnimation(bool pressed)
{
    if (m_dropDownAnimation && m_dropDownButton) {
        // 停止当前动画
        m_dropDownAnimation->stop();

        // 设置动画起始值为当前值
        double currentOffset = m_dropDownButton->arrowOffset();
        m_dropDownAnimation->setStartValue(currentOffset);

        // 设置目标值 - 增加偏移量使动画更明显
        double targetOffset = pressed ? 4.0 : 0.0; // 增加偏移量
        m_dropDownAnimation->setEndValue(targetOffset);

        // 启动动画
        m_dropDownAnimation->start();
    }
}

// 槽函数实现
void QWinUISplitButton::onMainButtonClicked()
{
    emit clicked();
}

void QWinUISplitButton::onDropDownButtonClicked()
{
    emit dropDownClicked();
    showFlyout();
}

void QWinUISplitButton::onDropDownButtonPressed()
{
    startArrowAnimation(true);
}

void QWinUISplitButton::onDropDownButtonReleased()
{
    startArrowAnimation(false);
}

void QWinUISplitButton::showFlyout()
{
    if (m_flyout && m_dropDownButton) {
        // 计算下拉按钮的全局位置
        QPoint globalPos = m_dropDownButton->mapToGlobal(QPoint(0, m_dropDownButton->height()));
        m_flyout->showAt(globalPos);
    }
}

void QWinUISplitButton::hideFlyout()
{
    if (m_flyout) {
        m_flyout->hide();
    }
}

void QWinUISplitButton::onFlyoutAboutToShow()
{
    // 菜单即将显示
}

void QWinUISplitButton::onFlyoutAboutToHide()
{
    // 菜单即将隐藏
}

void QWinUISplitButton::showEvent(QShowEvent* event)
{
    QWinUIWidget::showEvent(event);

    // 显示时立即获取并应用当前主题
    QWinUITheme* theme = QWinUITheme::getInstance();
    if (theme) {
        if (m_mainButton) {
            m_mainButton->update();
        }
        if (m_dropDownButton) {
            m_dropDownButton->update();
        }
        update();
    }
}
