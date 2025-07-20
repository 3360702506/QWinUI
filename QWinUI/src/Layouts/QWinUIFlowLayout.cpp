#include "../../include/QWinUI/Layouts/QWinUIFlowLayout.h"
#include <QWidget>
#include <QStyle>
#include <QApplication>
#include <QDebug>

QT_BEGIN_NAMESPACE

QWinUIFlowLayout::QWinUIFlowLayout(QWidget* parent)
    : QLayout(parent)
    , m_hSpacing(-1)
    , m_vSpacing(-1)
    , m_animationDuration(DefaultAnimationDuration)
    , m_easingCurve(QEasingCurve::OutCubic)
    , m_animationEnabled(true)
    , m_animationGroup(nullptr)
    , m_layoutTimer(nullptr)
    , m_isAnimating(false)
{
    m_animationGroup = new QParallelAnimationGroup(this);
    connect(m_animationGroup, &QParallelAnimationGroup::finished,
            this, &QWinUIFlowLayout::onAnimationFinished);
    
    m_layoutTimer = new QTimer(this);
    m_layoutTimer->setSingleShot(true);
    m_layoutTimer->setInterval(LayoutDelay);
    connect(m_layoutTimer, &QTimer::timeout,
            this, &QWinUIFlowLayout::onDelayedLayout);
}

QWinUIFlowLayout::QWinUIFlowLayout(int hSpacing, int vSpacing, QWidget* parent)
    : QWinUIFlowLayout(parent)
{
    m_hSpacing = hSpacing;
    m_vSpacing = vSpacing;
}

QWinUIFlowLayout::~QWinUIFlowLayout()
{
    stopAnimations();
    
    while (!m_itemList.isEmpty()) {
        LayoutItemInfo info = m_itemList.takeFirst();
        delete info.item;
    }
}

int QWinUIFlowLayout::horizontalSpacing() const
{
    if (m_hSpacing >= 0) {
        return m_hSpacing;
    } else {
        return smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
    }
}

void QWinUIFlowLayout::setHorizontalSpacing(int spacing)
{
    if (m_hSpacing != spacing) {
        m_hSpacing = spacing;
        invalidate();
        emit horizontalSpacingChanged(spacing);
    }
}

int QWinUIFlowLayout::verticalSpacing() const
{
    if (m_vSpacing >= 0) {
        return m_vSpacing;
    } else {
        return smartSpacing(QStyle::PM_LayoutVerticalSpacing);
    }
}

void QWinUIFlowLayout::setVerticalSpacing(int spacing)
{
    if (m_vSpacing != spacing) {
        m_vSpacing = spacing;
        invalidate();
        emit verticalSpacingChanged(spacing);
    }
}

void QWinUIFlowLayout::setSpacing(int spacing)
{
    setHorizontalSpacing(spacing);
    setVerticalSpacing(spacing);
}

int QWinUIFlowLayout::animationDuration() const
{
    return m_animationDuration;
}

void QWinUIFlowLayout::setAnimationDuration(int duration)
{
    if (m_animationDuration != duration) {
        m_animationDuration = qMax(0, duration);
        emit animationDurationChanged(m_animationDuration);
    }
}

QEasingCurve::Type QWinUIFlowLayout::easingCurve() const
{
    return m_easingCurve;
}

void QWinUIFlowLayout::setEasingCurve(QEasingCurve::Type curve)
{
    if (m_easingCurve != curve) {
        m_easingCurve = curve;
        emit easingCurveChanged(curve);
    }
}

bool QWinUIFlowLayout::isAnimationEnabled() const
{
    return m_animationEnabled;
}

void QWinUIFlowLayout::setAnimationEnabled(bool enabled)
{
    if (m_animationEnabled != enabled) {
        m_animationEnabled = enabled;
        if (!enabled) {
            stopAnimations();
        }
        emit animationEnabledChanged(enabled);
    }
}

void QWinUIFlowLayout::addItem(QLayoutItem* item)
{
    LayoutItemInfo info(item);
    if (item->widget()) {
        info.currentGeometry = item->widget()->geometry();
    }
    m_itemList.append(info);
    invalidate();
}

int QWinUIFlowLayout::count() const
{
    return m_itemList.size();
}

QLayoutItem* QWinUIFlowLayout::itemAt(int index) const
{
    if (index >= 0 && index < m_itemList.size()) {
        return m_itemList.at(index).item;
    }
    return nullptr;
}

QLayoutItem* QWinUIFlowLayout::takeAt(int index)
{
    if (index >= 0 && index < m_itemList.size()) {
        LayoutItemInfo info = m_itemList.takeAt(index);
        if (info.animation) {
            info.animation->stop();
            delete info.animation;
        }
        invalidate();
        return info.item;
    }
    return nullptr;
}

Qt::Orientations QWinUIFlowLayout::expandingDirections() const
{
    return Qt::Orientations();
}

bool QWinUIFlowLayout::hasHeightForWidth() const
{
    return true;
}

int QWinUIFlowLayout::heightForWidth(int width) const
{
    return doLayout(QRect(0, 0, width, 0), true).height();
}

QSize QWinUIFlowLayout::minimumSize() const
{
    if (m_cachedMinimumSize.isValid()) {
        return m_cachedMinimumSize;
    }
    
    QSize size;
    for (const LayoutItemInfo& info : m_itemList) {
        size = size.expandedTo(info.item->minimumSize());
    }
    
    QMargins margins = contentsMargins();
    size += QSize(margins.left() + margins.right(), margins.top() + margins.bottom());
    
    m_cachedMinimumSize = size;
    return size;
}

void QWinUIFlowLayout::setGeometry(const QRect& rect)
{
    QLayout::setGeometry(rect);

    // 清除缓存，确保重新计算
    m_cachedSizeHint = QSize();
    m_cachedMinimumSize = QSize();

    // 如果正在动画中，等待动画完成
    if (m_isAnimating) {
        // 延迟布局，等待当前动画完成
        m_layoutTimer->start();
        return;
    }

    // 检查几何形状是否真的变化了
    if (rect.size() != m_lastRect.size()) {
        m_lastRect = rect;

        if (shouldAnimate()) {
            // 延迟布局，避免频繁的布局计算
            m_layoutTimer->start();
        } else {
            calculateLayout(rect);
        }
    } else {
        m_lastRect = rect;
    }
}

QSize QWinUIFlowLayout::sizeHint() const
{
    if (m_cachedSizeHint.isValid()) {
        return m_cachedSizeHint;
    }
    
    QSize size = doLayout(QRect(0, 0, 0, 0), true);
    QMargins margins = contentsMargins();
    size += QSize(margins.left() + margins.right(), margins.top() + margins.bottom());
    
    m_cachedSizeHint = size;
    return size;
}

void QWinUIFlowLayout::addWidget(QWidget* widget)
{
    addItem(new QWidgetItem(widget));
}

void QWinUIFlowLayout::insertWidget(int index, QWidget* widget)
{
    LayoutItemInfo info(new QWidgetItem(widget));
    info.currentGeometry = widget->geometry();
    
    if (index < 0 || index >= m_itemList.size()) {
        m_itemList.append(info);
    } else {
        m_itemList.insert(index, info);
    }
    invalidate();
}

void QWinUIFlowLayout::removeWidget(QWidget* widget)
{
    for (int i = 0; i < m_itemList.size(); ++i) {
        if (m_itemList[i].item->widget() == widget) {
            LayoutItemInfo info = m_itemList.takeAt(i);
            if (info.animation) {
                info.animation->stop();
                delete info.animation;
            }
            delete info.item;
            invalidate();
            break;
        }
    }
}

void QWinUIFlowLayout::clear()
{
    stopAnimations();
    
    while (!m_itemList.isEmpty()) {
        LayoutItemInfo info = m_itemList.takeFirst();
        delete info.item;
    }
    invalidate();
}

void QWinUIFlowLayout::onAnimationFinished()
{
    m_isAnimating = false;
    emit animationFinished();
}

void QWinUIFlowLayout::onDelayedLayout()
{
    calculateLayout(geometry());
}

void QWinUIFlowLayout::invalidate()
{
    m_cachedSizeHint = QSize();
    m_cachedMinimumSize = QSize();
    QLayout::invalidate();
}

void QWinUIFlowLayout::calculateLayout(const QRect& rect, bool testOnly)
{
    if (m_itemList.isEmpty()) {
        return;
    }

    QMargins margins = contentsMargins();
    QRect effectiveRect = rect.adjusted(margins.left(), margins.top(),
                                       -margins.right(), -margins.bottom());

    int x = effectiveRect.x();
    int y = effectiveRect.y();
    int lineHeight = 0;
    int hSpacing = horizontalSpacing();
    int vSpacing = verticalSpacing();

    QList<QRect> newGeometries;

    for (int i = 0; i < m_itemList.size(); ++i) {
        LayoutItemInfo& info = m_itemList[i];
        QLayoutItem* item = info.item;

        QSize itemSizeHint = item->sizeHint();

        // 检查是否需要换行（除了第一个控件）
        if (i > 0 && x + itemSizeHint.width() > effectiveRect.right()) {
            x = effectiveRect.x();
            y = y + lineHeight + vSpacing;
            lineHeight = 0;
        }

        QRect itemRect(QPoint(x, y), itemSizeHint);
        newGeometries.append(itemRect);

        // 更新下一个控件的 x 位置
        x = x + itemSizeHint.width() + hSpacing;
        lineHeight = qMax(lineHeight, itemSizeHint.height());
    }

    if (testOnly) {
        return;
    }

    // 应用新的几何形状
    bool hasAnimations = false;
    for (int i = 0; i < m_itemList.size(); ++i) {
        LayoutItemInfo& info = m_itemList[i];
        QRect newGeometry = newGeometries[i];

        if (shouldAnimate() && info.item->widget()) {
            // 使用动画
            info.targetGeometry = newGeometry;
            QWidget* widget = info.item->widget();
            QRect currentRect = widget->geometry();

            // 只有位置真正变化时才创建动画
            if (currentRect != newGeometry) {
                animateToPosition(info, newGeometry);
                hasAnimations = true;
            } else {
                info.currentGeometry = newGeometry;
            }
        } else {
            // 直接设置位置
            info.item->setGeometry(newGeometry);
            info.currentGeometry = newGeometry;
            info.targetGeometry = newGeometry;
        }
    }

    // 如果有动画，标记为正在动画
    if (hasAnimations) {
        m_isAnimating = true;
    }
}

QSize QWinUIFlowLayout::doLayout(const QRect& rect, bool testOnly) const
{
    QMargins margins = contentsMargins();
    QRect effectiveRect = rect.adjusted(margins.left(), margins.top(),
                                       -margins.right(), -margins.bottom());

    int x = effectiveRect.x();
    int y = effectiveRect.y();
    int lineHeight = 0;
    int maxWidth = 0;
    int hSpacing = horizontalSpacing();
    int vSpacing = verticalSpacing();

    for (int i = 0; i < m_itemList.size(); ++i) {
        const LayoutItemInfo& info = m_itemList[i];
        QLayoutItem* item = info.item;
        QSize itemSizeHint = item->sizeHint();

        // 检查是否需要换行（除了第一个控件）
        if (i > 0 && x + itemSizeHint.width() > effectiveRect.right()) {
            maxWidth = qMax(maxWidth, x - hSpacing);
            x = effectiveRect.x();
            y = y + lineHeight + vSpacing;
            lineHeight = 0;
        }

        if (!testOnly) {
            item->setGeometry(QRect(QPoint(x, y), itemSizeHint));
        }

        // 更新下一个控件的 x 位置
        x = x + itemSizeHint.width() + hSpacing;
        lineHeight = qMax(lineHeight, itemSizeHint.height());
    }

    maxWidth = qMax(maxWidth, x - hSpacing);
    int totalHeight = y + lineHeight;

    return QSize(maxWidth + margins.left() + margins.right(),
                totalHeight + margins.top() + margins.bottom());
}

void QWinUIFlowLayout::startAnimations()
{
    if (!m_animationEnabled || m_isAnimating) {
        return;
    }

    m_animationGroup->clear();
    bool hasAnimations = false;

    for (LayoutItemInfo& info : m_itemList) {
        if (info.animation && info.animation->state() != QPropertyAnimation::Running) {
            m_animationGroup->addAnimation(info.animation);
            hasAnimations = true;
        }
    }

    if (hasAnimations) {
        m_isAnimating = true;
        m_animationGroup->start();
    } else {
        // 如果没有动画，直接标记为完成
        m_isAnimating = false;
    }
}

void QWinUIFlowLayout::stopAnimations()
{
    if (m_animationGroup) {
        m_animationGroup->stop();
        m_animationGroup->clear();
    }

    for (LayoutItemInfo& info : m_itemList) {
        if (info.animation) {
            info.animation->stop();
            delete info.animation;
            info.animation = nullptr;
        }
    }

    m_isAnimating = false;
}

void QWinUIFlowLayout::updateItemGeometry(LayoutItemInfo& info, const QRect& geometry)
{
    info.item->setGeometry(geometry);
    info.currentGeometry = geometry;
}

int QWinUIFlowLayout::smartSpacing(QStyle::PixelMetric pm) const
{
    QObject* parent = this->parent();
    if (!parent) {
        return -1;
    } else if (parent->isWidgetType()) {
        QWidget* pw = static_cast<QWidget*>(parent);
        return pw->style()->pixelMetric(pm, nullptr, pw);
    } else {
        return static_cast<QLayout*>(parent)->spacing();
    }
}

void QWinUIFlowLayout::createAnimation(LayoutItemInfo& info)
{
    if (!info.item->widget()) {
        return;
    }

    if (info.animation) {
        info.animation->stop();
        info.animation->deleteLater();
        info.animation = nullptr;
    }

    info.animation = new QPropertyAnimation(info.item->widget(), "geometry", this);
    info.animation->setDuration(m_animationDuration);
    info.animation->setEasingCurve(m_easingCurve);
}

void QWinUIFlowLayout::animateToPosition(LayoutItemInfo& info, const QRect& targetRect)
{
    if (!info.item->widget()) {
        return;
    }

    QWidget* widget = info.item->widget();
    QRect currentRect = widget->geometry();

    // 如果位置没有变化，不需要动画
    if (currentRect == targetRect) {
        info.currentGeometry = targetRect;
        info.targetGeometry = targetRect;
        return;
    }

    createAnimation(info);

    if (info.animation) {
        info.animation->setStartValue(currentRect);
        info.animation->setEndValue(targetRect);

        // 连接动画完成信号
        connect(info.animation, &QPropertyAnimation::finished, this, [this, &info, targetRect]() {
            info.currentGeometry = targetRect;

            // 检查是否所有动画都完成了
            bool allFinished = true;
            for (const LayoutItemInfo& checkInfo : m_itemList) {
                if (checkInfo.animation && checkInfo.animation->state() == QPropertyAnimation::Running) {
                    allFinished = false;
                    break;
                }
            }

            if (allFinished) {
                m_isAnimating = false;
                emit animationFinished();

                // 检查是否需要重新布局（可能在动画期间窗口大小又变了）
                if (m_layoutTimer->isActive()) {
                    m_layoutTimer->stop();
                    QTimer::singleShot(50, this, &QWinUIFlowLayout::onDelayedLayout);
                }
            }
        });

        // 立即启动单个动画
        info.animation->start();
    }
}

bool QWinUIFlowLayout::shouldAnimate() const
{
    return m_animationEnabled && m_animationDuration > 0 && !m_isAnimating;
}

QT_END_NAMESPACE
