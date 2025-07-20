#ifndef QWINUIAPPBARSEPARATOR_H
#define QWINUIAPPBARSEPARATOR_H

#include "../QWinUIWidget.h"
#include <QColor>

QT_BEGIN_NAMESPACE

class QWINUI_EXPORT QWinUIAppBarSeparator : public QWinUIWidget
{
    Q_OBJECT
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)
    Q_PROPERTY(int separatorWidth READ separatorWidth WRITE setSeparatorWidth NOTIFY separatorWidthChanged)

public:
    explicit QWinUIAppBarSeparator(QWidget* parent = nullptr);
    explicit QWinUIAppBarSeparator(Qt::Orientation orientation, QWidget* parent = nullptr);
    ~QWinUIAppBarSeparator() override;

    // 方向属性
    Qt::Orientation orientation() const;
    void setOrientation(Qt::Orientation orientation);

    // 分隔线宽度
    int separatorWidth() const;
    void setSeparatorWidth(int width);

    // 尺寸相关
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void orientationChanged(Qt::Orientation orientation);
    void separatorWidthChanged(int width);

protected:
    void paintEvent(QPaintEvent* event) override;
    void showEvent(QShowEvent* event) override;

private slots:
    void onThemeChanged();

private:
    void initializeSeparator();
    void updateSeparatorAppearance();
    
    // 绘制方法
    void drawSeparator(QPainter* painter, const QRect& rect);

    // 颜色获取方法
    QColor getThemeSeparatorColor() const;

    // 几何计算
    QRect getSeparatorRect() const;

private:
    Qt::Orientation m_orientation;
    int m_separatorWidth;
    
    // WinUI3 默认值
    static const int DefaultVerticalWidth = 1;
    static const int DefaultHorizontalHeight = 1;
    static const int DefaultVerticalHeight = 20;
    static const int DefaultHorizontalWidth = 20;
};

QT_END_NAMESPACE

#endif // QWINUIAPPBARSEPARATOR_H
