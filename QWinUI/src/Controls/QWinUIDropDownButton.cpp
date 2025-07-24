#include "QWinUI/Controls/QWinUIDropDownButton.h"
#include "QWinUI/QWinUITheme.h"
#include <QApplication>
#include <QDebug>
#include <QPainter>
#include <QFontDatabase>

// QWinUIDropDownButton 实现
QWinUIDropDownButton::QWinUIDropDownButton(QWidget* parent)
    : QWinUIButton(parent)
    , m_arrowOffset(0.0)
    , m_flyout(nullptr)
{
    // 设置Fluent Icons字体
    QStringList fontFamilies = {"Segoe Fluent Icons", "Segoe UI Symbol", "Segoe UI", "Arial"};

    for (const QString& family : fontFamilies) {
        if (QFontDatabase::families().contains(family)) {
            m_fluentFont = QFont(family);
            break;
        }
    }

    m_fluentFont.setPixelSize(10);
    m_fluentFont.setWeight(QFont::Normal);

    // 创建默认菜单飞出控件
    m_flyout = new QWinUIMenuFlyout(this);

    // 连接按钮点击信号到显示菜单
    connect(this, &QWinUIButton::clicked, this, &QWinUIDropDownButton::showFlyout);

    // 连接主题变化信号
    QWinUITheme* theme = QWinUITheme::getInstance();
    if (theme) {
        connect(theme, &QWinUITheme::themeChanged, this, [this]() {
            update(); // 主题变化时重新绘制
        });

        // 连接颜色变化信号
        connect(theme, &QWinUITheme::colorChanged, this, [this](const QString& colorName, const QColor& color) {
            Q_UNUSED(colorName)
            Q_UNUSED(color)
            update(); // 颜色变化时重新绘制
        });

    }
}

void QWinUIDropDownButton::setArrowOffset(double offset)
{
    if (qAbs(m_arrowOffset - offset) > 1e-6) {
        m_arrowOffset = offset;
        update(); // 触发重绘
    }
}

void QWinUIDropDownButton::paintEvent(QPaintEvent* event)
{
    // 先调用父类的绘制
    QWinUIButton::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 获取主题
    QWinUITheme* theme = QWinUITheme::getInstance();
    QColor arrowColor;
    
    if (theme) {
        if (theme->isDarkMode()) {
            arrowColor = isEnabled() ? QColor(255, 255, 255, 200) : QColor(255, 255, 255, 100);
        } else {
            arrowColor = isEnabled() ? QColor(0, 0, 0, 200) : QColor(0, 0, 0, 100);
        }
    } else {
        arrowColor = isEnabled() ? QColor(0, 0, 0, 200) : QColor(0, 0, 0, 100);
    }

    // 绘制下拉箭头
    painter.setFont(m_fluentFont);
    painter.setPen(arrowColor);

    // 计算箭头区域 - 在按钮右侧
    QRect arrowRect = rect();
    arrowRect.setLeft(arrowRect.right() - 20); // 箭头区域宽度为20像素
    arrowRect.moveTop(arrowRect.top() + static_cast<int>(m_arrowOffset));

    // 使用Fluent Icons的下拉箭头字符
    QString arrowChar = QString(QWinUIFluentIcons::Arrows::CHEVRON_DOWN);
    painter.drawText(arrowRect, Qt::AlignCenter, arrowChar);
}

void QWinUIDropDownButton::showEvent(QShowEvent* event)
{
    QWinUIButton::showEvent(event);
    
    // 确保字体正确设置
    if (m_fluentFont.family().isEmpty()) {
        QStringList fontFamilies = {"Segoe Fluent Icons", "Segoe UI Symbol", "Segoe UI", "Arial"};
        
        for (const QString& family : fontFamilies) {
            if (QFontDatabase::families().contains(family)) {
                m_fluentFont = QFont(family);
                break;
            }
        }
        
        m_fluentFont.setPixelSize(10);
        m_fluentFont.setWeight(QFont::Normal);
    }
}

void QWinUIDropDownButton::setFlyout(QWinUIMenuFlyout* flyout)
{
    if (m_flyout && m_flyout->parent() == this) {
        m_flyout->deleteLater();
    }
    m_flyout = flyout;
    if (m_flyout) {
        m_flyout->setParent(this);
    }
}

QWinUIMenuFlyoutItem* QWinUIDropDownButton::addItem(const QString& text)
{
    if (!m_flyout) {
        m_flyout = new QWinUIMenuFlyout(this);
    }
    return m_flyout->addItem(text);
}

QWinUIMenuFlyoutItem* QWinUIDropDownButton::addItem(const QIcon& icon, const QString& text)
{
    if (!m_flyout) {
        m_flyout = new QWinUIMenuFlyout(this);
    }
    return m_flyout->addItem(text, icon);
}

void QWinUIDropDownButton::addSeparator()
{
    if (!m_flyout) {
        m_flyout = new QWinUIMenuFlyout(this);
    }
    m_flyout->addSeparator();
}

void QWinUIDropDownButton::showFlyout()
{
    if (!m_flyout || m_flyout->itemCount() == 0) {
        return;
    }

    emit flyoutAboutToShow();

    // 计算显示位置
    QPoint position(0, height());
    m_flyout->showAt(this, position);

    // 连接菜单的关闭信号
    connect(m_flyout, &QWinUIMenuFlyout::aboutToHide, this, &QWinUIDropDownButton::flyoutAboutToHide, Qt::UniqueConnection);
}

void QWinUIDropDownButton::mousePressEvent(QMouseEvent* event)
{
    // 调用父类处理
    QWinUIButton::mousePressEvent(event);
}
