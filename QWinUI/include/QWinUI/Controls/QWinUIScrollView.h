#ifndef QWINUISCROLLVIEW_H
#define QWINUISCROLLVIEW_H

#include "../QWinUIGlobal.h"
#include "../QWinUIWidget.h"
#include <QScrollArea>
#include <QScrollBar>
#include <QPropertyAnimation>
#include <QTimer>
#include <QEasingCurve>
#include <QEnterEvent>

QT_BEGIN_NAMESPACE

class QWinUIScrollBar;

class QWINUI_EXPORT QWinUIScrollView : public QWinUIWidget
{
    Q_OBJECT
    Q_PROPERTY(bool horizontalScrollBarVisible READ isHorizontalScrollBarVisible WRITE setHorizontalScrollBarVisible NOTIFY horizontalScrollBarVisibleChanged)
    Q_PROPERTY(bool verticalScrollBarVisible READ isVerticalScrollBarVisible WRITE setVerticalScrollBarVisible NOTIFY verticalScrollBarVisibleChanged)
    Q_PROPERTY(Qt::ScrollBarPolicy horizontalScrollBarPolicy READ horizontalScrollBarPolicy WRITE setHorizontalScrollBarPolicy NOTIFY horizontalScrollBarPolicyChanged)
    Q_PROPERTY(Qt::ScrollBarPolicy verticalScrollBarPolicy READ verticalScrollBarPolicy WRITE setVerticalScrollBarPolicy NOTIFY verticalScrollBarPolicyChanged)
    Q_PROPERTY(bool smoothScrolling READ isSmoothScrollingEnabled WRITE setSmoothScrollingEnabled NOTIFY smoothScrollingEnabledChanged)
    Q_PROPERTY(int scrollAnimationDuration READ scrollAnimationDuration WRITE setScrollAnimationDuration NOTIFY scrollAnimationDurationChanged)
    Q_PROPERTY(QEasingCurve::Type scrollEasingCurve READ scrollEasingCurve WRITE setScrollEasingCurve NOTIFY scrollEasingCurveChanged)
    Q_PROPERTY(bool autoHideScrollBars READ autoHideScrollBars WRITE setAutoHideScrollBars NOTIFY autoHideScrollBarsChanged)
    Q_PROPERTY(int scrollBarFadeDelay READ scrollBarFadeDelay WRITE setScrollBarFadeDelay NOTIFY scrollBarFadeDelayChanged)

public:
    explicit QWinUIScrollView(QWidget* parent = nullptr);
    ~QWinUIScrollView();

    // 内容控件
    QWidget* widget() const;
    void setWidget(QWidget* widget);
    QWidget* takeWidget();

    // 滚动条策略
    Qt::ScrollBarPolicy horizontalScrollBarPolicy() const;
    void setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy policy);
    Qt::ScrollBarPolicy verticalScrollBarPolicy() const;
    void setVerticalScrollBarPolicy(Qt::ScrollBarPolicy policy);

    // 滚动条可见性
    bool isHorizontalScrollBarVisible() const;
    void setHorizontalScrollBarVisible(bool visible);
    bool isVerticalScrollBarVisible() const;
    void setVerticalScrollBarVisible(bool visible);

    // 平滑滚动
    bool isSmoothScrollingEnabled() const;
    void setSmoothScrollingEnabled(bool enabled);
    int scrollAnimationDuration() const;
    void setScrollAnimationDuration(int duration);
    QEasingCurve::Type scrollEasingCurve() const;
    void setScrollEasingCurve(QEasingCurve::Type curve);

    // 自动隐藏滚动条
    bool autoHideScrollBars() const;
    void setAutoHideScrollBars(bool autoHide);
    int scrollBarFadeDelay() const;
    void setScrollBarFadeDelay(int delay);

    // 滚动位置
    int horizontalScrollValue() const;
    int verticalScrollValue() const;
    int horizontalScrollMaximum() const;
    int verticalScrollMaximum() const;
    int horizontalScrollMinimum() const;
    int verticalScrollMinimum() const;

    // 视口
    QRect viewportRect() const;
    QSize viewportSize() const;
    QWidget* viewport() const;

    // 滚动条
    QWinUIScrollBar* horizontalScrollBar() const;
    QWinUIScrollBar* verticalScrollBar() const;

public slots:
    // 滚动控制
    void scrollTo(int x, int y);
    void scrollToTop();
    void scrollToBottom();
    void scrollToLeft();
    void scrollToRight();
    void scrollToCenter();
    
    // 平滑滚动
    void smoothScrollTo(int x, int y);
    void smoothScrollBy(int dx, int dy);

    // 滚轮滚动（内部使用）
    void wheelScrollBy(int dx, int dy);
    
    // 滚动条控制
    void setHorizontalScrollValue(int value);
    void setVerticalScrollValue(int value);
    
    // 确保可见
    void ensureVisible(int x, int y, int xMargin = 50, int yMargin = 50);
    void ensureWidgetVisible(QWidget* childWidget, int xMargin = 50, int yMargin = 50);

signals:
    void horizontalScrollBarVisibleChanged(bool visible);
    void verticalScrollBarVisibleChanged(bool visible);
    void horizontalScrollBarPolicyChanged(Qt::ScrollBarPolicy policy);
    void verticalScrollBarPolicyChanged(Qt::ScrollBarPolicy policy);
    void smoothScrollingEnabledChanged(bool enabled);
    void scrollAnimationDurationChanged(int duration);
    void scrollEasingCurveChanged(QEasingCurve::Type curve);
    void autoHideScrollBarsChanged(bool autoHide);
    void scrollBarFadeDelayChanged(int delay);
    
    void horizontalScrollValueChanged(int value);
    void verticalScrollValueChanged(int value);
    void scrollStarted();
    void scrollFinished();

protected:
    // QWidget 重写
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

    // 主题相关
    void onThemeChanged() override;

private slots:
    void onHorizontalScrollValueChanged(int value);
    void onVerticalScrollValueChanged(int value);
    void onScrollAnimationFinished();
    void onScrollBarFadeTimer();
    void onContentSizeChanged();

private:
    void initializeScrollView();
    void setupScrollBars();
    void updateScrollBars();
    void updateScrollBarVisibility();
    void updateScrollBarGeometry();
    void updateContentGeometry();
    
    // 动画相关
    void startScrollAnimation(int targetX, int targetY);
    void stopScrollAnimation();
    
    // 滚动条自动隐藏
    void showScrollBars();
    void hideScrollBars();
    void resetScrollBarFadeTimer();
    
    // 主题相关
    void updateScrollBarTheme();
    void propagateThemeToChildren(QWidget* widget);

    // 工具方法
    QPoint constrainScrollPosition(const QPoint& position) const;
    bool isScrollBarNeeded(Qt::Orientation orientation) const;

private:
    QScrollArea* m_scrollArea;
    QWidget* m_contentWidget;
    QWinUIScrollBar* m_horizontalScrollBar;
    QWinUIScrollBar* m_verticalScrollBar;
    
    // 滚动策略
    Qt::ScrollBarPolicy m_horizontalPolicy;
    Qt::ScrollBarPolicy m_verticalPolicy;
    
    // 平滑滚动
    bool m_smoothScrollingEnabled;
    int m_scrollAnimationDuration;
    QEasingCurve::Type m_scrollEasingCurve;
    QPropertyAnimation* m_horizontalScrollAnimation;
    QPropertyAnimation* m_verticalScrollAnimation;
    
    // 自动隐藏滚动条
    bool m_autoHideScrollBars;
    int m_scrollBarFadeDelay;
    QTimer* m_scrollBarFadeTimer;
    bool m_scrollBarsVisible;
    
    // 鼠标交互
    bool m_isDragging;
    QPoint m_lastMousePos;
    QPoint m_dragStartPos;
    Qt::Orientation m_dragDirection;
    bool m_dragDirectionLocked;
    
    // 状态
    bool m_isScrolling;
};

QT_END_NAMESPACE

#endif // QWINUISCROLLVIEW_H
