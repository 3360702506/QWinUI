#include "QWinUI/Controls/QWinUIAppBarSeparator.h"
#include "QWinUI/QWinUITheme.h"
#include <QPainter>
#include <QPaintEvent>
#include <QShowEvent>
#include <QTimer>

QT_BEGIN_NAMESPACE

QWinUIAppBarSeparator::QWinUIAppBarSeparator(QWidget* parent)
    : QWinUIWidget(parent)
    , m_orientation(Qt::Vertical)
    , m_separatorWidth(DefaultVerticalWidth)
{
    initializeSeparator();
}

QWinUIAppBarSeparator::QWinUIAppBarSeparator(Qt::Orientation orientation, QWidget* parent)
    : QWinUIWidget(parent)
    , m_orientation(orientation)
    , m_separatorWidth(orientation == Qt::Vertical ? DefaultVerticalWidth : DefaultHorizontalHeight)
{
    initializeSeparator();
}

QWinUIAppBarSeparator::~QWinUIAppBarSeparator()
{
}

void QWinUIAppBarSeparator::initializeSeparator()
{
    // 设置基本属性
    setCornerRadius(0); // 分隔线不需要圆角
    setShadowDepth(QWinUIShadowDepth::None); // 分隔线不需要阴影
    setFocusPolicy(Qt::NoFocus); // 分隔线不需要焦点

    // 连接主题变化信号
    if (QWinUITheme* theme = QWinUITheme::getInstance()) {
        connect(theme, &QWinUITheme::themeChanged, this, &QWinUIAppBarSeparator::onThemeChanged);
    }

    updateSeparatorAppearance();

    // 确保在初始化时立即更新一次，以获取正确的主题颜色
    QTimer::singleShot(0, this, [this]() {
        update();
    });
}

Qt::Orientation QWinUIAppBarSeparator::orientation() const
{
    return m_orientation;
}

void QWinUIAppBarSeparator::setOrientation(Qt::Orientation orientation)
{
    if (m_orientation != orientation) {
        m_orientation = orientation;

        // 根据方向调整默认宽度
        m_separatorWidth = (orientation == Qt::Vertical) ? DefaultVerticalWidth : DefaultHorizontalHeight;

        updateSeparatorAppearance();
        updateGeometry();
        emit orientationChanged(orientation);
    }
}



int QWinUIAppBarSeparator::separatorWidth() const
{
    return m_separatorWidth;
}

void QWinUIAppBarSeparator::setSeparatorWidth(int width)
{
    if (m_separatorWidth != width && width > 0) {
        m_separatorWidth = width;
        updateSeparatorAppearance();
        updateGeometry();
        emit separatorWidthChanged(width);
    }
}

QSize QWinUIAppBarSeparator::sizeHint() const
{
    if (m_orientation == Qt::Vertical) {
        return QSize(m_separatorWidth, DefaultVerticalHeight);
    } else {
        return QSize(DefaultHorizontalWidth, m_separatorWidth);
    }
}

QSize QWinUIAppBarSeparator::minimumSizeHint() const
{
    if (m_orientation == Qt::Vertical) {
        return QSize(m_separatorWidth, 1);
    } else {
        return QSize(1, m_separatorWidth);
    }
}

void QWinUIAppBarSeparator::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false); // 分隔线不需要抗锯齿，保持锐利边缘

    drawSeparator(&painter, rect());
}

void QWinUIAppBarSeparator::showEvent(QShowEvent* event)
{
    QWinUIWidget::showEvent(event);

    // 控件显示时确保获取正确的主题颜色
    update();
}

void QWinUIAppBarSeparator::onThemeChanged()
{
    update(); // 主题变化时重新绘制
}

void QWinUIAppBarSeparator::updateSeparatorAppearance()
{
    // 根据方向设置合适的尺寸策略
    if (m_orientation == Qt::Vertical) {
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        setFixedWidth(m_separatorWidth);
    } else {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        setFixedHeight(m_separatorWidth);
    }
    
    update();
}

void QWinUIAppBarSeparator::drawSeparator(QPainter* painter, const QRect& rect)
{
    Q_UNUSED(rect) // 我们使用getSeparatorRect()而不是传入的rect

    QColor separatorColor = getThemeSeparatorColor();
    painter->setPen(QPen(separatorColor, m_separatorWidth, Qt::SolidLine, Qt::FlatCap));

    QRect separatorRect = getSeparatorRect();

    if (m_orientation == Qt::Vertical) {
        // 垂直分隔线：从上到下的线
        int x = separatorRect.center().x();
        painter->drawLine(x, separatorRect.top(), x, separatorRect.bottom());
    } else {
        // 水平分隔线：从左到右的线
        int y = separatorRect.center().y();
        painter->drawLine(separatorRect.left(), y, separatorRect.right(), y);
    }
}

QColor QWinUIAppBarSeparator::getThemeSeparatorColor() const
{
    QWinUITheme* theme = QWinUITheme::getInstance();
    if (!theme) {
        // 如果没有主题，使用默认颜色
        return QColor(200, 200, 200, 180); // 更明显的半透明灰色
    }

    // 根据主题模式返回更明显的分隔线颜色
    // 参考其他控件的实现，使用更高的透明度以确保可见性
    if (theme->isDarkMode()) {
        // 深色主题：使用半透明白色，参考ScrollBar的实现
        return QColor(255, 255, 255, 60); // 比原来的26更明显
    } else {
        // 浅色主题：使用半透明黑色，确保在白色背景上可见
        return QColor(0, 0, 0, 40); // 使用黑色而不是灰色，更明显
    }
}

QRect QWinUIAppBarSeparator::getSeparatorRect() const
{
    return rect();
}

QT_END_NAMESPACE
