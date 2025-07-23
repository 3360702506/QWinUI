#ifndef QWINUIVARIABLESIZEDWRAPGRID_H
#define QWINUIVARIABLESIZEDWRAPGRID_H

#include "../QWinUIWidget.h"
#include <QWidget>
#include <QList>
#include <QRect>
#include <QSize>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

QT_BEGIN_NAMESPACE

// 枚举定义
enum class QWinUIOrientation {
    Horizontal,
    Vertical
};

enum class QWinUIHorizontalAlignment {
    Left,
    Center,
    Right,
    Stretch
};

enum class QWinUIVerticalAlignment {
    Top,
    Center,
    Bottom,
    Stretch
};

// 附加属性结构
struct QWinUIGridItemProperties {
    int rowSpan = 1;
    int columnSpan = 1;
    QWinUIHorizontalAlignment horizontalAlignment = QWinUIHorizontalAlignment::Stretch;
    QWinUIVerticalAlignment verticalAlignment = QWinUIVerticalAlignment::Stretch;
};

// 网格项信息
struct QWinUIGridItemInfo {
    QWidget* widget;
    QWinUIGridItemProperties properties;
    QRect currentRect;
    QRect targetRect;
    int row;
    int column;
    QPropertyAnimation* positionAnimation;
    QPropertyAnimation* sizeAnimation;
};

class QWINUI_EXPORT QWinUIVariableSizedWrapGrid : public QWinUIWidget
{
    Q_OBJECT
    Q_PROPERTY(QWinUIOrientation orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)
    Q_PROPERTY(double itemWidth READ itemWidth WRITE setItemWidth NOTIFY itemWidthChanged)
    Q_PROPERTY(double itemHeight READ itemHeight WRITE setItemHeight NOTIFY itemHeightChanged)
    Q_PROPERTY(int maximumRowsOrColumns READ maximumRowsOrColumns WRITE setMaximumRowsOrColumns NOTIFY maximumRowsOrColumnsChanged)
    Q_PROPERTY(QWinUIHorizontalAlignment horizontalChildrenAlignment READ horizontalChildrenAlignment WRITE setHorizontalChildrenAlignment NOTIFY horizontalChildrenAlignmentChanged)
    Q_PROPERTY(QWinUIVerticalAlignment verticalChildrenAlignment READ verticalChildrenAlignment WRITE setVerticalChildrenAlignment NOTIFY verticalChildrenAlignmentChanged)
    Q_PROPERTY(int itemSpacing READ itemSpacing WRITE setItemSpacing NOTIFY itemSpacingChanged)

public:
    explicit QWinUIVariableSizedWrapGrid(QWidget* parent = nullptr);
    ~QWinUIVariableSizedWrapGrid() override;

    // 属性访问器
    QWinUIOrientation orientation() const;
    void setOrientation(QWinUIOrientation orientation);

    double itemWidth() const;
    void setItemWidth(double width);

    double itemHeight() const;
    void setItemHeight(double height);

    int maximumRowsOrColumns() const;
    void setMaximumRowsOrColumns(int max);

    QWinUIHorizontalAlignment horizontalChildrenAlignment() const;
    void setHorizontalChildrenAlignment(QWinUIHorizontalAlignment alignment);

    QWinUIVerticalAlignment verticalChildrenAlignment() const;
    void setVerticalChildrenAlignment(QWinUIVerticalAlignment alignment);

    int itemSpacing() const;
    void setItemSpacing(int spacing);

    // 子项管理
    void addItem(QWidget* widget);
    void addItem(QWidget* widget, int rowSpan, int columnSpan);
    void removeItem(QWidget* widget);
    void clearItems();

    // 附加属性设置
    static void setRowSpan(QWidget* widget, int rowSpan);
    static int getRowSpan(QWidget* widget);
    static void setColumnSpan(QWidget* widget, int columnSpan);
    static int getColumnSpan(QWidget* widget);
    static void setHorizontalAlignment(QWidget* widget, QWinUIHorizontalAlignment alignment);
    static QWinUIHorizontalAlignment getHorizontalAlignment(QWidget* widget);
    static void setVerticalAlignment(QWidget* widget, QWinUIVerticalAlignment alignment);
    static QWinUIVerticalAlignment getVerticalAlignment(QWidget* widget);

    // 尺寸提示
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void orientationChanged(QWinUIOrientation orientation);
    void itemWidthChanged(double width);
    void itemHeightChanged(double height);
    void maximumRowsOrColumnsChanged(int max);
    void horizontalChildrenAlignmentChanged(QWinUIHorizontalAlignment alignment);
    void verticalChildrenAlignmentChanged(QWinUIVerticalAlignment alignment);
    void itemSpacingChanged(int spacing);
    void itemAdded(QWidget* widget);
    void itemRemoved(QWidget* widget);

protected:
    // 事件处理
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void childEvent(QChildEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

    // 主题变化处理
    void onThemeChanged() override;

private slots:
    void onItemAnimationFinished();
    void updateLayout();

private:
    // 初始化
    void initializeComponent();
    void updateColors();

    // 布局计算
    void calculateLayout();
    void calculateHorizontalLayout();
    void calculateVerticalLayout();
    QSize calculateRequiredSize() const;
    
    // 网格计算辅助方法
    bool canPlaceItem(const QList<QList<bool>>& grid, int row, int col, int rowSpan, int colSpan) const;
    void placeItem(QList<QList<bool>>& grid, int row, int col, int rowSpan, int colSpan);
    QRect calculateItemRect(int row, int col, int rowSpan, int colSpan) const;
    QRect applyAlignment(const QRect& cellRect, const QSize& itemSize, 
                        QWinUIHorizontalAlignment hAlign, QWinUIVerticalAlignment vAlign) const;

    // 动画
    void animateToNewPositions();
    void startItemAnimation(QWinUIGridItemInfo& itemInfo);

    // 项目管理
    QWinUIGridItemInfo* findItemInfo(QWidget* widget);
    void removeItemInfo(QWidget* widget);
    QWinUIGridItemProperties getItemProperties(QWidget* widget) const;

private:
    // 布局属性
    QWinUIOrientation m_orientation;
    double m_itemWidth;
    double m_itemHeight;
    int m_maximumRowsOrColumns;
    QWinUIHorizontalAlignment m_horizontalChildrenAlignment;
    QWinUIVerticalAlignment m_verticalChildrenAlignment;
    int m_itemSpacing;

    // 网格状态
    QList<QWinUIGridItemInfo> m_items;
    int m_actualRows;
    int m_actualColumns;
    bool m_layoutDirty;

    // 动画
    QParallelAnimationGroup* m_layoutAnimation;
    bool m_animationEnabled;

    // 颜色和样式
    QColor m_backgroundColor;
    QColor m_borderColor;

    // 常量
    static constexpr double DEFAULT_ITEM_WIDTH = 100.0;
    static constexpr double DEFAULT_ITEM_HEIGHT = 100.0;
    static constexpr int DEFAULT_MAX_ROWS_OR_COLUMNS = -1; // 无限制
    static constexpr int DEFAULT_ITEM_SPACING = 8;
    static constexpr int ANIMATION_DURATION = 250;
    static constexpr int DEFAULT_CORNER_RADIUS = 4;

    // 附加属性键
    static const char* ROWSPAN_PROPERTY;
    static const char* COLUMNSPAN_PROPERTY;
    static const char* HORIZONTAL_ALIGNMENT_PROPERTY;
    static const char* VERTICAL_ALIGNMENT_PROPERTY;
};

QT_END_NAMESPACE

#endif // QWINUIVARIABLESIZEDWRAPGRID_H
