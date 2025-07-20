#ifndef QWINUIFLOWLAYOUT_H
#define QWINUIFLOWLAYOUT_H

#include "../QWinUIGlobal.h"
#include <QLayout>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QEasingCurve>
#include <QTimer>
#include <QStyle>

QT_BEGIN_NAMESPACE

class QWINUI_EXPORT QWinUIFlowLayout : public QLayout
{
    Q_OBJECT
    Q_PROPERTY(int horizontalSpacing READ horizontalSpacing WRITE setHorizontalSpacing NOTIFY horizontalSpacingChanged)
    Q_PROPERTY(int verticalSpacing READ verticalSpacing WRITE setVerticalSpacing NOTIFY verticalSpacingChanged)
    Q_PROPERTY(int animationDuration READ animationDuration WRITE setAnimationDuration NOTIFY animationDurationChanged)
    Q_PROPERTY(QEasingCurve::Type easingCurve READ easingCurve WRITE setEasingCurve NOTIFY easingCurveChanged)
    Q_PROPERTY(bool animationEnabled READ isAnimationEnabled WRITE setAnimationEnabled NOTIFY animationEnabledChanged)

public:
    explicit QWinUIFlowLayout(QWidget* parent = nullptr);
    QWinUIFlowLayout(int hSpacing, int vSpacing, QWidget* parent = nullptr);
    ~QWinUIFlowLayout();

    // 间距设置
    int horizontalSpacing() const;
    void setHorizontalSpacing(int spacing);
    
    int verticalSpacing() const;
    void setVerticalSpacing(int spacing);
    
    void setSpacing(int spacing);

    // 动画设置
    int animationDuration() const;
    void setAnimationDuration(int duration);
    
    QEasingCurve::Type easingCurve() const;
    void setEasingCurve(QEasingCurve::Type curve);
    
    bool isAnimationEnabled() const;
    void setAnimationEnabled(bool enabled);

    // QLayout 接口实现
    void addItem(QLayoutItem* item) override;
    int count() const override;
    QLayoutItem* itemAt(int index) const override;
    QLayoutItem* takeAt(int index) override;
    Qt::Orientations expandingDirections() const override;
    bool hasHeightForWidth() const override;
    int heightForWidth(int width) const override;
    QSize minimumSize() const override;
    void setGeometry(const QRect& rect) override;
    QSize sizeHint() const override;
    void invalidate() override;

    // 便利方法
    void addWidget(QWidget* widget);
    void insertWidget(int index, QWidget* widget);
    void removeWidget(QWidget* widget);
    void clear();

signals:
    void horizontalSpacingChanged(int spacing);
    void verticalSpacingChanged(int spacing);
    void animationDurationChanged(int duration);
    void easingCurveChanged(QEasingCurve::Type curve);
    void animationEnabledChanged(bool enabled);
    void animationFinished();

private slots:
    void onAnimationFinished();
    void onDelayedLayout();

private:
    struct LayoutItemInfo {
        QLayoutItem* item;
        QRect targetGeometry;
        QRect currentGeometry;
        QPropertyAnimation* animation;
        
        LayoutItemInfo(QLayoutItem* layoutItem = nullptr)
            : item(layoutItem), animation(nullptr) {}
    };

    void calculateLayout(const QRect& rect, bool testOnly = false);
    QSize doLayout(const QRect& rect, bool testOnly) const;
    void startAnimations();
    void stopAnimations();
    void updateItemGeometry(LayoutItemInfo& info, const QRect& geometry);
    int smartSpacing(QStyle::PixelMetric pm) const;
    
    // 动画相关
    void createAnimation(LayoutItemInfo& info);
    void animateToPosition(LayoutItemInfo& info, const QRect& targetRect);
    bool shouldAnimate() const;

private:
    QList<LayoutItemInfo> m_itemList;
    int m_hSpacing;
    int m_vSpacing;
    
    // 动画相关
    int m_animationDuration;
    QEasingCurve::Type m_easingCurve;
    bool m_animationEnabled;
    QParallelAnimationGroup* m_animationGroup;
    QTimer* m_layoutTimer;
    bool m_isAnimating;
    
    // 缓存
    mutable QSize m_cachedSizeHint;
    mutable QSize m_cachedMinimumSize;
    mutable QRect m_lastRect;
    
    // 常量
    static const int DefaultAnimationDuration = 250;
    static const int LayoutDelay = 16; // ~60 FPS
};

QT_END_NAMESPACE

#endif // QWINUIFLOWLAYOUT_H
