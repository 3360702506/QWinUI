#include "QWinUI/Controls/QWinUIVariableSizedWrapGrid.h"
#include "QWinUI/QWinUITheme.h"
#include <QResizeEvent>
#include <QPainter>
#include <QPainterPath>
#include <QChildEvent>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QApplication>
#include <QDebug>

// 附加属性键定义
const char* QWinUIVariableSizedWrapGrid::ROWSPAN_PROPERTY = "QWinUIRowSpan";
const char* QWinUIVariableSizedWrapGrid::COLUMNSPAN_PROPERTY = "QWinUIColumnSpan";
const char* QWinUIVariableSizedWrapGrid::HORIZONTAL_ALIGNMENT_PROPERTY = "QWinUIHorizontalAlignment";
const char* QWinUIVariableSizedWrapGrid::VERTICAL_ALIGNMENT_PROPERTY = "QWinUIVerticalAlignment";

QWinUIVariableSizedWrapGrid::QWinUIVariableSizedWrapGrid(QWidget* parent)
    : QWinUIWidget(parent)
    , m_orientation(QWinUIOrientation::Horizontal)
    , m_itemWidth(DEFAULT_ITEM_WIDTH)
    , m_itemHeight(DEFAULT_ITEM_HEIGHT)
    , m_maximumRowsOrColumns(DEFAULT_MAX_ROWS_OR_COLUMNS)
    , m_horizontalChildrenAlignment(QWinUIHorizontalAlignment::Left)
    , m_verticalChildrenAlignment(QWinUIVerticalAlignment::Top)
    , m_itemSpacing(DEFAULT_ITEM_SPACING)
    , m_actualRows(0)
    , m_actualColumns(0)
    , m_layoutDirty(true)
    , m_layoutAnimation(nullptr)
    , m_animationEnabled(true)
{
    initializeComponent();
}

QWinUIVariableSizedWrapGrid::~QWinUIVariableSizedWrapGrid()
{
    if (m_layoutAnimation) {
        m_layoutAnimation->stop();
        delete m_layoutAnimation;
    }
}

void QWinUIVariableSizedWrapGrid::initializeComponent()
{
    // 设置基本属性
    setFocusPolicy(Qt::NoFocus);
    setAttribute(Qt::WA_Hover);
    
    // 初始化动画组
    m_layoutAnimation = new QParallelAnimationGroup(this);
    connect(m_layoutAnimation, &QParallelAnimationGroup::finished,
            this, &QWinUIVariableSizedWrapGrid::onItemAnimationFinished);
    
    // 初始化颜色
    updateColors();
    
    // 连接主题变化信号
    QWinUITheme* theme = QWinUITheme::getInstance();
    if (theme) {
        connect(theme, &QWinUITheme::themeChanged, this, &QWinUIVariableSizedWrapGrid::updateColors);
    }
    
    // 设置默认尺寸
    setMinimumSize(200, 200);
}

void QWinUIVariableSizedWrapGrid::updateColors()
{
    QWinUITheme* theme = QWinUITheme::getInstance();
    if (!theme) return;
    
    if (theme->isDarkMode()) {
        m_backgroundColor = QColor(32, 32, 32, 0); // 透明背景
        m_borderColor = QColor(80, 80, 80);
    } else {
        m_backgroundColor = QColor(255, 255, 255, 0); // 透明背景
        m_borderColor = QColor(200, 200, 200);
    }
    
    update();
}

// 属性访问器实现
QWinUIOrientation QWinUIVariableSizedWrapGrid::orientation() const
{
    return m_orientation;
}

void QWinUIVariableSizedWrapGrid::setOrientation(QWinUIOrientation orientation)
{
    if (m_orientation != orientation) {
        m_orientation = orientation;
        m_layoutDirty = true;
        updateLayout();
        emit orientationChanged(orientation);
    }
}

double QWinUIVariableSizedWrapGrid::itemWidth() const
{
    return m_itemWidth;
}

void QWinUIVariableSizedWrapGrid::setItemWidth(double width)
{
    if (width > 0 && !qFuzzyCompare(m_itemWidth, width)) {
        m_itemWidth = width;
        m_layoutDirty = true;
        updateLayout();
        emit itemWidthChanged(width);
    }
}

double QWinUIVariableSizedWrapGrid::itemHeight() const
{
    return m_itemHeight;
}

void QWinUIVariableSizedWrapGrid::setItemHeight(double height)
{
    if (height > 0 && !qFuzzyCompare(m_itemHeight, height)) {
        m_itemHeight = height;
        m_layoutDirty = true;
        updateLayout();
        emit itemHeightChanged(height);
    }
}

int QWinUIVariableSizedWrapGrid::maximumRowsOrColumns() const
{
    return m_maximumRowsOrColumns;
}

void QWinUIVariableSizedWrapGrid::setMaximumRowsOrColumns(int max)
{
    if (m_maximumRowsOrColumns != max) {
        m_maximumRowsOrColumns = max;
        m_layoutDirty = true;
        updateLayout();
        emit maximumRowsOrColumnsChanged(max);
    }
}

QWinUIHorizontalAlignment QWinUIVariableSizedWrapGrid::horizontalChildrenAlignment() const
{
    return m_horizontalChildrenAlignment;
}

void QWinUIVariableSizedWrapGrid::setHorizontalChildrenAlignment(QWinUIHorizontalAlignment alignment)
{
    if (m_horizontalChildrenAlignment != alignment) {
        m_horizontalChildrenAlignment = alignment;
        m_layoutDirty = true;
        updateLayout();
        emit horizontalChildrenAlignmentChanged(alignment);
    }
}

QWinUIVerticalAlignment QWinUIVariableSizedWrapGrid::verticalChildrenAlignment() const
{
    return m_verticalChildrenAlignment;
}

void QWinUIVariableSizedWrapGrid::setVerticalChildrenAlignment(QWinUIVerticalAlignment alignment)
{
    if (m_verticalChildrenAlignment != alignment) {
        m_verticalChildrenAlignment = alignment;
        m_layoutDirty = true;
        updateLayout();
        emit verticalChildrenAlignmentChanged(alignment);
    }
}

int QWinUIVariableSizedWrapGrid::itemSpacing() const
{
    return m_itemSpacing;
}

void QWinUIVariableSizedWrapGrid::setItemSpacing(int spacing)
{
    if (spacing >= 0 && m_itemSpacing != spacing) {
        m_itemSpacing = spacing;
        m_layoutDirty = true;
        updateLayout();
        emit itemSpacingChanged(spacing);
    }
}

// 子项管理
void QWinUIVariableSizedWrapGrid::addItem(QWidget* widget)
{
    addItem(widget, 1, 1);
}

void QWinUIVariableSizedWrapGrid::addItem(QWidget* widget, int rowSpan, int columnSpan)
{
    if (!widget || findItemInfo(widget)) return;
    
    // 设置附加属性
    setRowSpan(widget, rowSpan);
    setColumnSpan(widget, columnSpan);
    
    // 设置父子关系
    widget->setParent(this);
    widget->installEventFilter(this);
    
    // 创建项目信息
    QWinUIGridItemInfo itemInfo;
    itemInfo.widget = widget;
    itemInfo.properties = getItemProperties(widget);
    itemInfo.row = -1;
    itemInfo.column = -1;
    itemInfo.positionAnimation = nullptr;
    itemInfo.sizeAnimation = nullptr;
    
    m_items.append(itemInfo);
    
    m_layoutDirty = true;
    updateLayout();
    
    emit itemAdded(widget);
}

void QWinUIVariableSizedWrapGrid::removeItem(QWidget* widget)
{
    if (!widget) return;
    
    removeItemInfo(widget);
    widget->removeEventFilter(this);
    widget->setParent(nullptr);
    
    m_layoutDirty = true;
    updateLayout();
    
    emit itemRemoved(widget);
}

void QWinUIVariableSizedWrapGrid::clearItems()
{
    while (!m_items.isEmpty()) {
        QWinUIGridItemInfo& itemInfo = m_items.first();
        removeItem(itemInfo.widget);
    }
}

// 附加属性设置
void QWinUIVariableSizedWrapGrid::setRowSpan(QWidget* widget, int rowSpan)
{
    if (widget && rowSpan > 0) {
        widget->setProperty(ROWSPAN_PROPERTY, rowSpan);
    }
}

int QWinUIVariableSizedWrapGrid::getRowSpan(QWidget* widget)
{
    if (widget) {
        return widget->property(ROWSPAN_PROPERTY).toInt();
    }
    return 1;
}

void QWinUIVariableSizedWrapGrid::setColumnSpan(QWidget* widget, int columnSpan)
{
    if (widget && columnSpan > 0) {
        widget->setProperty(COLUMNSPAN_PROPERTY, columnSpan);
    }
}

int QWinUIVariableSizedWrapGrid::getColumnSpan(QWidget* widget)
{
    if (widget) {
        return widget->property(COLUMNSPAN_PROPERTY).toInt();
    }
    return 1;
}

void QWinUIVariableSizedWrapGrid::setHorizontalAlignment(QWidget* widget, QWinUIHorizontalAlignment alignment)
{
    if (widget) {
        widget->setProperty(HORIZONTAL_ALIGNMENT_PROPERTY, static_cast<int>(alignment));
    }
}

QWinUIHorizontalAlignment QWinUIVariableSizedWrapGrid::getHorizontalAlignment(QWidget* widget)
{
    if (widget) {
        return static_cast<QWinUIHorizontalAlignment>(widget->property(HORIZONTAL_ALIGNMENT_PROPERTY).toInt());
    }
    return QWinUIHorizontalAlignment::Stretch;
}

void QWinUIVariableSizedWrapGrid::setVerticalAlignment(QWidget* widget, QWinUIVerticalAlignment alignment)
{
    if (widget) {
        widget->setProperty(VERTICAL_ALIGNMENT_PROPERTY, static_cast<int>(alignment));
    }
}

QWinUIVerticalAlignment QWinUIVariableSizedWrapGrid::getVerticalAlignment(QWidget* widget)
{
    if (widget) {
        return static_cast<QWinUIVerticalAlignment>(widget->property(VERTICAL_ALIGNMENT_PROPERTY).toInt());
    }
    return QWinUIVerticalAlignment::Stretch;
}

// 尺寸提示
QSize QWinUIVariableSizedWrapGrid::sizeHint() const
{
    return calculateRequiredSize();
}

QSize QWinUIVariableSizedWrapGrid::minimumSizeHint() const
{
    if (m_items.isEmpty()) {
        return QSize(static_cast<int>(m_itemWidth), static_cast<int>(m_itemHeight));
    }

    // 计算最小尺寸（至少能容纳一个最大的项目）
    int maxRowSpan = 1;
    int maxColSpan = 1;

    for (const auto& itemInfo : m_items) {
        maxRowSpan = qMax(maxRowSpan, itemInfo.properties.rowSpan);
        maxColSpan = qMax(maxColSpan, itemInfo.properties.columnSpan);
    }

    int minWidth = static_cast<int>(m_itemWidth * maxColSpan + m_itemSpacing * (maxColSpan - 1));
    int minHeight = static_cast<int>(m_itemHeight * maxRowSpan + m_itemSpacing * (maxRowSpan - 1));

    return QSize(minWidth, minHeight);
}

// 事件处理
void QWinUIVariableSizedWrapGrid::resizeEvent(QResizeEvent* event)
{
    QWinUIWidget::resizeEvent(event);
    m_layoutDirty = true;
    updateLayout();
}

void QWinUIVariableSizedWrapGrid::paintEvent(QPaintEvent* event)
{
    // 调用父类的paintEvent来处理主题切换动画
    QWinUIWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 可选：绘制调试网格线（在调试模式下）
    #ifdef QT_DEBUG
    if (false) { // 可以通过属性控制是否显示网格线
        painter.setPen(QPen(m_borderColor, 1, Qt::DashLine));

        // 绘制垂直线
        for (int col = 0; col <= m_actualColumns; ++col) {
            int x = static_cast<int>(col * (m_itemWidth + m_itemSpacing));
            painter.drawLine(x, 0, x, height());
        }

        // 绘制水平线
        for (int row = 0; row <= m_actualRows; ++row) {
            int y = static_cast<int>(row * (m_itemHeight + m_itemSpacing));
            painter.drawLine(0, y, width(), y);
        }
    }
    #endif
}

void QWinUIVariableSizedWrapGrid::childEvent(QChildEvent* event)
{
    QWinUIWidget::childEvent(event);

    if (event->type() == QEvent::ChildAdded) {
        QWidget* widget = qobject_cast<QWidget*>(event->child());
        if (widget && !findItemInfo(widget)) {
            // 自动添加新的子控件
            addItem(widget);
        }
    } else if (event->type() == QEvent::ChildRemoved) {
        QWidget* widget = qobject_cast<QWidget*>(event->child());
        if (widget) {
            removeItemInfo(widget);
            m_layoutDirty = true;
            updateLayout();
        }
    }
}

bool QWinUIVariableSizedWrapGrid::eventFilter(QObject* obj, QEvent* event)
{
    QWidget* widget = qobject_cast<QWidget*>(obj);
    if (widget && findItemInfo(widget)) {
        if (event->type() == QEvent::Resize || event->type() == QEvent::Show || event->type() == QEvent::Hide) {
            // 子控件尺寸或可见性变化时重新布局
            QTimer::singleShot(0, this, &QWinUIVariableSizedWrapGrid::updateLayout);
        }
    }

    return QWinUIWidget::eventFilter(obj, event);
}

void QWinUIVariableSizedWrapGrid::onThemeChanged()
{
    updateColors();
}

// 私有槽函数
void QWinUIVariableSizedWrapGrid::onItemAnimationFinished()
{
    // 动画完成后的清理工作
    for (auto& itemInfo : m_items) {
        if (itemInfo.positionAnimation) {
            itemInfo.positionAnimation->deleteLater();
            itemInfo.positionAnimation = nullptr;
        }
        if (itemInfo.sizeAnimation) {
            itemInfo.sizeAnimation->deleteLater();
            itemInfo.sizeAnimation = nullptr;
        }
    }
}

void QWinUIVariableSizedWrapGrid::updateLayout()
{
    if (!m_layoutDirty) return;

    calculateLayout();

    if (m_animationEnabled) {
        animateToNewPositions();
    } else {
        // 直接应用新位置
        for (auto& itemInfo : m_items) {
            if (itemInfo.widget && itemInfo.widget->isVisible()) {
                itemInfo.widget->setGeometry(itemInfo.targetRect);
                itemInfo.currentRect = itemInfo.targetRect;
            }
        }
    }

    m_layoutDirty = false;
}

// 布局计算
void QWinUIVariableSizedWrapGrid::calculateLayout()
{
    if (m_items.isEmpty()) {
        m_actualRows = 0;
        m_actualColumns = 0;
        return;
    }

    if (m_orientation == QWinUIOrientation::Horizontal) {
        calculateHorizontalLayout();
    } else {
        calculateVerticalLayout();
    }
}

void QWinUIVariableSizedWrapGrid::calculateHorizontalLayout()
{
    // 计算可用的列数
    int availableWidth = width();
    int maxColumns = m_maximumRowsOrColumns > 0 ? m_maximumRowsOrColumns :
                    qMax(1, static_cast<int>((availableWidth + m_itemSpacing) / (m_itemWidth + m_itemSpacing)));

    // 创建网格占用表
    QList<QList<bool>> grid;
    int currentMaxRow = 0;

    // 初始化网格
    for (int row = 0; row < 100; ++row) { // 预分配足够的行
        grid.append(QList<bool>(maxColumns, false));
    }

    // 放置每个项目
    for (auto& itemInfo : m_items) {
        if (!itemInfo.widget || !itemInfo.widget->isVisible()) continue;

        itemInfo.properties = getItemProperties(itemInfo.widget);

        // 查找可以放置项目的位置
        bool placed = false;
        for (int row = 0; row < grid.size() - itemInfo.properties.rowSpan; ++row) {
            for (int col = 0; col <= maxColumns - itemInfo.properties.columnSpan; ++col) {
                if (canPlaceItem(grid, row, col, itemInfo.properties.rowSpan, itemInfo.properties.columnSpan)) {
                    // 放置项目
                    placeItem(grid, row, col, itemInfo.properties.rowSpan, itemInfo.properties.columnSpan);
                    itemInfo.row = row;
                    itemInfo.column = col;
                    itemInfo.targetRect = calculateItemRect(row, col, itemInfo.properties.rowSpan, itemInfo.properties.columnSpan);

                    currentMaxRow = qMax(currentMaxRow, row + itemInfo.properties.rowSpan - 1);
                    placed = true;
                    break;
                }
            }
            if (placed) break;
        }
    }

    m_actualRows = currentMaxRow + 1;
    m_actualColumns = maxColumns;
}

void QWinUIVariableSizedWrapGrid::calculateVerticalLayout()
{
    // 计算可用的行数
    int availableHeight = height();
    int maxRows = m_maximumRowsOrColumns > 0 ? m_maximumRowsOrColumns :
                 qMax(1, static_cast<int>((availableHeight + m_itemSpacing) / (m_itemHeight + m_itemSpacing)));

    // 创建网格占用表
    QList<QList<bool>> grid;
    int currentMaxCol = 0;

    // 初始化网格
    for (int row = 0; row < maxRows; ++row) {
        grid.append(QList<bool>(100, false)); // 预分配足够的列
    }

    // 放置每个项目
    for (auto& itemInfo : m_items) {
        if (!itemInfo.widget || !itemInfo.widget->isVisible()) continue;

        itemInfo.properties = getItemProperties(itemInfo.widget);

        // 查找可以放置项目的位置
        bool placed = false;
        for (int col = 0; col < grid[0].size() - itemInfo.properties.columnSpan; ++col) {
            for (int row = 0; row <= maxRows - itemInfo.properties.rowSpan; ++row) {
                if (canPlaceItem(grid, row, col, itemInfo.properties.rowSpan, itemInfo.properties.columnSpan)) {
                    // 放置项目
                    placeItem(grid, row, col, itemInfo.properties.rowSpan, itemInfo.properties.columnSpan);
                    itemInfo.row = row;
                    itemInfo.column = col;
                    itemInfo.targetRect = calculateItemRect(row, col, itemInfo.properties.rowSpan, itemInfo.properties.columnSpan);

                    currentMaxCol = qMax(currentMaxCol, col + itemInfo.properties.columnSpan - 1);
                    placed = true;
                    break;
                }
            }
            if (placed) break;
        }
    }

    m_actualRows = maxRows;
    m_actualColumns = currentMaxCol + 1;
}

QSize QWinUIVariableSizedWrapGrid::calculateRequiredSize() const
{
    if (m_items.isEmpty()) {
        return QSize(static_cast<int>(m_itemWidth), static_cast<int>(m_itemHeight));
    }

    int requiredWidth = static_cast<int>(m_actualColumns * m_itemWidth + (m_actualColumns - 1) * m_itemSpacing);
    int requiredHeight = static_cast<int>(m_actualRows * m_itemHeight + (m_actualRows - 1) * m_itemSpacing);

    return QSize(requiredWidth, requiredHeight);
}

// 网格计算辅助方法
bool QWinUIVariableSizedWrapGrid::canPlaceItem(const QList<QList<bool>>& grid, int row, int col, int rowSpan, int colSpan) const
{
    if (row < 0 || col < 0 || row + rowSpan > grid.size() || col + colSpan > grid[0].size()) {
        return false;
    }

    for (int r = row; r < row + rowSpan; ++r) {
        for (int c = col; c < col + colSpan; ++c) {
            if (grid[r][c]) {
                return false;
            }
        }
    }

    return true;
}

void QWinUIVariableSizedWrapGrid::placeItem(QList<QList<bool>>& grid, int row, int col, int rowSpan, int colSpan)
{
    for (int r = row; r < row + rowSpan; ++r) {
        for (int c = col; c < col + colSpan; ++c) {
            grid[r][c] = true;
        }
    }
}

QRect QWinUIVariableSizedWrapGrid::calculateItemRect(int row, int col, int rowSpan, int colSpan) const
{
    int x = static_cast<int>(col * (m_itemWidth + m_itemSpacing));
    int y = static_cast<int>(row * (m_itemHeight + m_itemSpacing));
    int width = static_cast<int>(colSpan * m_itemWidth + (colSpan - 1) * m_itemSpacing);
    int height = static_cast<int>(rowSpan * m_itemHeight + (rowSpan - 1) * m_itemSpacing);

    return QRect(x, y, width, height);
}

QRect QWinUIVariableSizedWrapGrid::applyAlignment(const QRect& cellRect, const QSize& itemSize,
                                                 QWinUIHorizontalAlignment hAlign, QWinUIVerticalAlignment vAlign) const
{
    QRect result = cellRect;

    // 水平对齐
    switch (hAlign) {
    case QWinUIHorizontalAlignment::Left:
        result.setWidth(qMin(itemSize.width(), cellRect.width()));
        break;
    case QWinUIHorizontalAlignment::Center:
        result.setWidth(qMin(itemSize.width(), cellRect.width()));
        result.moveLeft(cellRect.left() + (cellRect.width() - result.width()) / 2);
        break;
    case QWinUIHorizontalAlignment::Right:
        result.setWidth(qMin(itemSize.width(), cellRect.width()));
        result.moveRight(cellRect.right());
        break;
    case QWinUIHorizontalAlignment::Stretch:
        // 保持原始宽度
        break;
    }

    // 垂直对齐
    switch (vAlign) {
    case QWinUIVerticalAlignment::Top:
        result.setHeight(qMin(itemSize.height(), cellRect.height()));
        break;
    case QWinUIVerticalAlignment::Center:
        result.setHeight(qMin(itemSize.height(), cellRect.height()));
        result.moveTop(cellRect.top() + (cellRect.height() - result.height()) / 2);
        break;
    case QWinUIVerticalAlignment::Bottom:
        result.setHeight(qMin(itemSize.height(), cellRect.height()));
        result.moveBottom(cellRect.bottom());
        break;
    case QWinUIVerticalAlignment::Stretch:
        // 保持原始高度
        break;
    }

    return result;
}

// 动画
void QWinUIVariableSizedWrapGrid::animateToNewPositions()
{
    if (m_layoutAnimation->state() == QAbstractAnimation::Running) {
        m_layoutAnimation->stop();
    }

    m_layoutAnimation->clear();

    for (auto& itemInfo : m_items) {
        if (itemInfo.widget && itemInfo.widget->isVisible()) {
            startItemAnimation(itemInfo);
        }
    }

    if (m_layoutAnimation->animationCount() > 0) {
        m_layoutAnimation->start();
    }
}

void QWinUIVariableSizedWrapGrid::startItemAnimation(QWinUIGridItemInfo& itemInfo)
{
    QWidget* widget = itemInfo.widget;
    if (!widget) return;

    QRect currentGeometry = widget->geometry();
    QRect targetGeometry = itemInfo.targetRect;

    // 如果位置没有变化，不需要动画
    if (currentGeometry == targetGeometry) {
        return;
    }

    // 位置动画
    if (currentGeometry.topLeft() != targetGeometry.topLeft()) {
        itemInfo.positionAnimation = new QPropertyAnimation(widget, "pos");
        itemInfo.positionAnimation->setDuration(ANIMATION_DURATION);
        itemInfo.positionAnimation->setEasingCurve(QEasingCurve::OutCubic);
        itemInfo.positionAnimation->setStartValue(currentGeometry.topLeft());
        itemInfo.positionAnimation->setEndValue(targetGeometry.topLeft());
        m_layoutAnimation->addAnimation(itemInfo.positionAnimation);
    }

    // 尺寸动画
    if (currentGeometry.size() != targetGeometry.size()) {
        itemInfo.sizeAnimation = new QPropertyAnimation(widget, "size");
        itemInfo.sizeAnimation->setDuration(ANIMATION_DURATION);
        itemInfo.sizeAnimation->setEasingCurve(QEasingCurve::OutCubic);
        itemInfo.sizeAnimation->setStartValue(currentGeometry.size());
        itemInfo.sizeAnimation->setEndValue(targetGeometry.size());
        m_layoutAnimation->addAnimation(itemInfo.sizeAnimation);
    }
}

// 项目管理辅助方法
QWinUIGridItemInfo* QWinUIVariableSizedWrapGrid::findItemInfo(QWidget* widget)
{
    for (auto& itemInfo : m_items) {
        if (itemInfo.widget == widget) {
            return &itemInfo;
        }
    }
    return nullptr;
}

void QWinUIVariableSizedWrapGrid::removeItemInfo(QWidget* widget)
{
    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items[i].widget == widget) {
            // 清理动画
            if (m_items[i].positionAnimation) {
                m_items[i].positionAnimation->stop();
                m_items[i].positionAnimation->deleteLater();
            }
            if (m_items[i].sizeAnimation) {
                m_items[i].sizeAnimation->stop();
                m_items[i].sizeAnimation->deleteLater();
            }

            m_items.removeAt(i);
            break;
        }
    }
}

QWinUIGridItemProperties QWinUIVariableSizedWrapGrid::getItemProperties(QWidget* widget) const
{
    QWinUIGridItemProperties properties;

    if (widget) {
        properties.rowSpan = qMax(1, getRowSpan(widget));
        properties.columnSpan = qMax(1, getColumnSpan(widget));
        properties.horizontalAlignment = getHorizontalAlignment(widget);
        properties.verticalAlignment = getVerticalAlignment(widget);
    }

    return properties;
}
