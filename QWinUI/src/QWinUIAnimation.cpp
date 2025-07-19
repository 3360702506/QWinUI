#include "QWinUI/QWinUIAnimation.h"
#include <QWidget>
#include <QApplication>
#include <QGraphicsOpacityEffect>
#include <QTimer>
#include <QtMath>

QT_BEGIN_NAMESPACE

QWinUIAnimation::QWinUIAnimation(QWidget* target, QObject* parent)
    : QObject(parent)
    , m_target(target)
    , m_currentAnimation(nullptr)
    , m_opacityEffect(nullptr)
    , m_duration(200)
    , m_easingCurve(fluentEaseOut())
    , m_sequentialGroup(nullptr)
    , m_parallelGroup(nullptr)
    , m_isChaining(false)
    , m_originalOpacity(1.0)
    , m_originalOpacityEffectEnabled(false)
{
    if (m_target) {
        m_originalGeometry = m_target->geometry();
        m_originalOpacity = m_target->windowOpacity();
    }
}

QWinUIAnimation::~QWinUIAnimation()
{
    cleanupAnimation();
    removeOpacityEffect();
}

QWidget* QWinUIAnimation::target() const
{
    return m_target;
}

void QWinUIAnimation::setTarget(QWidget* target)
{
    if (m_target != target) {
        cleanupAnimation();
        removeOpacityEffect();
        
        m_target = target;
        
        if (m_target) {
            m_originalGeometry = m_target->geometry();
            m_originalOpacity = m_target->windowOpacity();
        }
    }
}

int QWinUIAnimation::duration() const
{
    return m_duration;
}

void QWinUIAnimation::setDuration(int duration)
{
    if (m_duration != duration) {
        m_duration = duration;
        emit durationChanged(duration);
    }
}

QEasingCurve QWinUIAnimation::easingCurve() const
{
    return m_easingCurve;
}

void QWinUIAnimation::setEasingCurve(const QEasingCurve& curve)
{
    if (m_easingCurve != curve) {
        m_easingCurve = curve;
        emit easingCurveChanged(curve);
    }
}

bool QWinUIAnimation::isRunning() const
{
    return m_currentAnimation && m_currentAnimation->state() == QAbstractAnimation::Running;
}

bool QWinUIAnimation::isPaused() const
{
    return m_currentAnimation && m_currentAnimation->state() == QAbstractAnimation::Paused;
}

// 预定义的Fluent Design缓动曲线
QEasingCurve QWinUIAnimation::fluentEaseOut()
{
    // Windows 11 Fluent Design 标准缓动曲线
    QEasingCurve curve(QEasingCurve::BezierSpline);
    curve.addCubicBezierSegment(QPointF(0.1, 0.9), QPointF(0.2, 1.0), QPointF(1.0, 1.0));
    return curve;
}

QEasingCurve QWinUIAnimation::fluentEaseIn()
{
    QEasingCurve curve(QEasingCurve::BezierSpline);
    curve.addCubicBezierSegment(QPointF(0.7, 0.0), QPointF(1.0, 0.5), QPointF(1.0, 1.0));
    return curve;
}

QEasingCurve QWinUIAnimation::fluentEaseInOut()
{
    QEasingCurve curve(QEasingCurve::BezierSpline);
    curve.addCubicBezierSegment(QPointF(0.1, 0.0), QPointF(0.9, 1.0), QPointF(1.0, 1.0));
    return curve;
}

QEasingCurve QWinUIAnimation::fluentSpring()
{
    return QEasingCurve(QEasingCurve::OutBack);
}

void QWinUIAnimation::fadeIn(int duration)
{
    if (!m_target) return;
    
    cleanupAnimation();
    ensureOpacityEffect();
    
    m_currentAnimation = createOpacityAnimation(0.0, 1.0, duration);
    m_target->show();
    m_currentAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void QWinUIAnimation::fadeOut(int duration)
{
    if (!m_target) return;
    
    cleanupAnimation();
    ensureOpacityEffect();
    
    m_currentAnimation = createOpacityAnimation(1.0, 0.0, duration);
    connect(m_currentAnimation, &QAbstractAnimation::finished, [this]() {
        if (m_target) {
            m_target->hide();
        }
    });
    m_currentAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void QWinUIAnimation::fadeToOpacity(double opacity, int duration)
{
    if (!m_target) return;
    
    cleanupAnimation();
    ensureOpacityEffect();
    
    double currentOpacity = m_opacityEffect ? m_opacityEffect->opacity() : 1.0;
    m_currentAnimation = createOpacityAnimation(currentOpacity, opacity, duration);
    m_currentAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void QWinUIAnimation::slideIn(Qt::Edge edge, int duration)
{
    if (!m_target) return;
    
    cleanupAnimation();
    
    QRect startGeometry = calculateSlideGeometry(edge, false);
    QRect endGeometry = m_originalGeometry;
    
    m_target->setGeometry(startGeometry);
    m_target->show();
    
    m_currentAnimation = createGeometryAnimation(startGeometry, endGeometry, duration);
    m_currentAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void QWinUIAnimation::slideOut(Qt::Edge edge, int duration)
{
    if (!m_target) return;
    
    cleanupAnimation();
    
    QRect startGeometry = m_target->geometry();
    QRect endGeometry = calculateSlideGeometry(edge, false);
    
    m_currentAnimation = createGeometryAnimation(startGeometry, endGeometry, duration);
    connect(m_currentAnimation, &QAbstractAnimation::finished, [this]() {
        if (m_target) {
            m_target->hide();
            m_target->setGeometry(m_originalGeometry);
        }
    });
    m_currentAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void QWinUIAnimation::slideToPosition(const QPoint& position, int duration)
{
    if (!m_target) return;
    
    cleanupAnimation();
    
    QPoint startPos = m_target->pos();
    m_currentAnimation = createPositionAnimation(startPos, position, duration);
    m_currentAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void QWinUIAnimation::scaleIn(int duration)
{
    if (!m_target) return;
    
    cleanupAnimation();
    
    QRect startGeometry = calculateScaleGeometry(0.0);
    QRect endGeometry = m_originalGeometry;
    
    m_target->setGeometry(startGeometry);
    m_target->show();
    
    m_currentAnimation = createGeometryAnimation(startGeometry, endGeometry, duration);
    m_currentAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void QWinUIAnimation::scaleOut(int duration)
{
    if (!m_target) return;
    
    cleanupAnimation();
    
    QRect startGeometry = m_target->geometry();
    QRect endGeometry = calculateScaleGeometry(0.0);
    
    m_currentAnimation = createGeometryAnimation(startGeometry, endGeometry, duration);
    connect(m_currentAnimation, &QAbstractAnimation::finished, [this]() {
        if (m_target) {
            m_target->hide();
            m_target->setGeometry(m_originalGeometry);
        }
    });
    m_currentAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void QWinUIAnimation::scaleToSize(const QSize& size, int duration)
{
    if (!m_target) return;
    
    cleanupAnimation();
    
    QSize startSize = m_target->size();
    m_currentAnimation = createSizeAnimation(startSize, size, duration);
    m_currentAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void QWinUIAnimation::scaleToFactor(double factor, int duration)
{
    if (!m_target) return;
    
    cleanupAnimation();
    
    QRect startGeometry = m_target->geometry();
    QRect endGeometry = calculateScaleGeometry(factor);
    
    m_currentAnimation = createGeometryAnimation(startGeometry, endGeometry, duration);
    m_currentAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void QWinUIAnimation::bounceIn(int duration)
{
    if (!m_target) return;
    
    cleanupAnimation();
    
    // 创建弹跳进入动画序列
    m_sequentialGroup = new QSequentialAnimationGroup(this);
    
    // 第一阶段：快速放大到1.1倍
    QRect startGeometry = calculateScaleGeometry(0.0);
    QRect overGeometry = calculateScaleGeometry(1.1);
    QPropertyAnimation* phase1 = createGeometryAnimation(startGeometry, overGeometry, duration * 0.6);
    phase1->setEasingCurve(QEasingCurve::OutQuad);
    
    // 第二阶段：回弹到正常大小
    QPropertyAnimation* phase2 = createGeometryAnimation(overGeometry, m_originalGeometry, duration * 0.4);
    phase2->setEasingCurve(QEasingCurve::OutBounce);
    
    m_sequentialGroup->addAnimation(phase1);
    m_sequentialGroup->addAnimation(phase2);
    
    m_target->setGeometry(startGeometry);
    m_target->show();
    
    m_currentAnimation = m_sequentialGroup;
    connect(m_currentAnimation, &QAbstractAnimation::finished, this, &QWinUIAnimation::onAnimationFinished);
    m_currentAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void QWinUIAnimation::pulseEffect(int duration, int pulseCount)
{
    if (!m_target) return;
    
    cleanupAnimation();
    ensureOpacityEffect();
    
    m_sequentialGroup = new QSequentialAnimationGroup(this);
    
    int pulseDuration = duration / (pulseCount * 2);
    
    for (int i = 0; i < pulseCount; ++i) {
        // 淡出
        QPropertyAnimation* fadeOut = createOpacityAnimation(1.0, 0.3, pulseDuration);
        fadeOut->setEasingCurve(QEasingCurve::InOutQuad);
        
        // 淡入
        QPropertyAnimation* fadeIn = createOpacityAnimation(0.3, 1.0, pulseDuration);
        fadeIn->setEasingCurve(QEasingCurve::InOutQuad);
        
        m_sequentialGroup->addAnimation(fadeOut);
        m_sequentialGroup->addAnimation(fadeIn);
    }
    
    m_currentAnimation = m_sequentialGroup;
    connect(m_currentAnimation, &QAbstractAnimation::finished, this, &QWinUIAnimation::onAnimationFinished);
    m_currentAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void QWinUIAnimation::shakeEffect(int duration, int shakeDistance)
{
    if (!m_target) return;
    
    cleanupAnimation();
    
    m_sequentialGroup = new QSequentialAnimationGroup(this);
    
    QPoint originalPos = m_target->pos();
    int shakeDuration = duration / 8; // 8个摇摆动作
    
    QPoint positions[] = {
        originalPos + QPoint(shakeDistance, 0),
        originalPos + QPoint(-shakeDistance, 0),
        originalPos + QPoint(shakeDistance, 0),
        originalPos + QPoint(-shakeDistance, 0),
        originalPos + QPoint(shakeDistance / 2, 0),
        originalPos + QPoint(-shakeDistance / 2, 0),
        originalPos + QPoint(shakeDistance / 4, 0),
        originalPos
    };
    
    for (int i = 0; i < 8; ++i) {
        QPoint startPos = (i == 0) ? originalPos : positions[i - 1];
        QPropertyAnimation* shake = createPositionAnimation(startPos, positions[i], shakeDuration);
        shake->setEasingCurve(QEasingCurve::InOutQuad);
        m_sequentialGroup->addAnimation(shake);
    }
    
    m_currentAnimation = m_sequentialGroup;
    connect(m_currentAnimation, &QAbstractAnimation::finished, this, &QWinUIAnimation::onAnimationFinished);
    m_currentAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void QWinUIAnimation::start()
{
    if (m_currentAnimation) {
        m_currentAnimation->start();
        emit started();
    }
}

void QWinUIAnimation::start(QWinUIAnimationType type, int duration)
{
    switch (type) {
    case QWinUIAnimationType::FadeIn:
        fadeIn(duration);
        break;
    case QWinUIAnimationType::FadeOut:
        fadeOut(duration);
        break;
    case QWinUIAnimationType::SlideIn:
        slideIn(Qt::BottomEdge, duration);
        break;
    case QWinUIAnimationType::SlideOut:
        slideOut(Qt::BottomEdge, duration);
        break;
    case QWinUIAnimationType::ScaleIn:
        scaleIn(duration);
        break;
    case QWinUIAnimationType::ScaleOut:
        scaleOut(duration);
        break;
    }
}

void QWinUIAnimation::stop()
{
    if (m_currentAnimation) {
        m_currentAnimation->stop();
    }
}

void QWinUIAnimation::pause()
{
    if (m_currentAnimation) {
        m_currentAnimation->pause();
        emit paused();
    }
}

void QWinUIAnimation::resume()
{
    if (m_currentAnimation) {
        m_currentAnimation->resume();
        emit resumed();
    }
}

void QWinUIAnimation::restart()
{
    if (m_currentAnimation) {
        m_currentAnimation->stop();
        m_currentAnimation->start();
    }
}

QPropertyAnimation* QWinUIAnimation::createOpacityAnimation(double startOpacity, double endOpacity, int duration)
{
    if (!m_opacityEffect) return nullptr;
    
    QPropertyAnimation* animation = new QPropertyAnimation(m_opacityEffect, "opacity");
    animation->setDuration(duration);
    animation->setStartValue(startOpacity);
    animation->setEndValue(endOpacity);
    animation->setEasingCurve(m_easingCurve);
    
    connect(animation, &QAbstractAnimation::finished, this, &QWinUIAnimation::onAnimationFinished);
    connect(animation, &QAbstractAnimation::stateChanged, this, &QWinUIAnimation::onAnimationStateChanged);
    
    return animation;
}

QPropertyAnimation* QWinUIAnimation::createGeometryAnimation(const QRect& startGeometry, const QRect& endGeometry, int duration)
{
    if (!m_target) return nullptr;
    
    QPropertyAnimation* animation = new QPropertyAnimation(m_target, "geometry");
    animation->setDuration(duration);
    animation->setStartValue(startGeometry);
    animation->setEndValue(endGeometry);
    animation->setEasingCurve(m_easingCurve);
    
    connect(animation, &QAbstractAnimation::finished, this, &QWinUIAnimation::onAnimationFinished);
    connect(animation, &QAbstractAnimation::stateChanged, this, &QWinUIAnimation::onAnimationStateChanged);
    
    return animation;
}

QPropertyAnimation* QWinUIAnimation::createPositionAnimation(const QPoint& startPos, const QPoint& endPos, int duration)
{
    if (!m_target) return nullptr;
    
    QPropertyAnimation* animation = new QPropertyAnimation(m_target, "pos");
    animation->setDuration(duration);
    animation->setStartValue(startPos);
    animation->setEndValue(endPos);
    animation->setEasingCurve(m_easingCurve);
    
    connect(animation, &QAbstractAnimation::finished, this, &QWinUIAnimation::onAnimationFinished);
    connect(animation, &QAbstractAnimation::stateChanged, this, &QWinUIAnimation::onAnimationStateChanged);
    
    return animation;
}

QPropertyAnimation* QWinUIAnimation::createSizeAnimation(const QSize& startSize, const QSize& endSize, int duration)
{
    if (!m_target) return nullptr;
    
    QPropertyAnimation* animation = new QPropertyAnimation(m_target, "size");
    animation->setDuration(duration);
    animation->setStartValue(startSize);
    animation->setEndValue(endSize);
    animation->setEasingCurve(m_easingCurve);
    
    connect(animation, &QAbstractAnimation::finished, this, &QWinUIAnimation::onAnimationFinished);
    connect(animation, &QAbstractAnimation::stateChanged, this, &QWinUIAnimation::onAnimationStateChanged);
    
    return animation;
}

void QWinUIAnimation::ensureOpacityEffect()
{
    if (!m_target || m_opacityEffect) return;
    
    m_opacityEffect = new QGraphicsOpacityEffect(this);
    m_opacityEffect->setOpacity(1.0);
    m_target->setGraphicsEffect(m_opacityEffect);
}

void QWinUIAnimation::removeOpacityEffect()
{
    if (m_target && m_opacityEffect) {
        m_target->setGraphicsEffect(nullptr);
        m_opacityEffect = nullptr;
    }
}

QRect QWinUIAnimation::calculateSlideGeometry(Qt::Edge edge, bool isSlideIn)
{
    if (!m_target || !m_target->parentWidget()) return QRect();
    
    QRect parentRect = m_target->parentWidget()->rect();
    QRect targetRect = m_originalGeometry;
    
    switch (edge) {
    case Qt::TopEdge:
        targetRect.moveTop(-targetRect.height());
        break;
    case Qt::BottomEdge:
        targetRect.moveTop(parentRect.height());
        break;
    case Qt::LeftEdge:
        targetRect.moveLeft(-targetRect.width());
        break;
    case Qt::RightEdge:
        targetRect.moveLeft(parentRect.width());
        break;
    }
    
    return targetRect;
}

QRect QWinUIAnimation::calculateScaleGeometry(double factor)
{
    if (!m_target) return QRect();
    
    QRect originalRect = m_originalGeometry;
    QSize newSize = originalRect.size() * factor;
    QPoint newPos = originalRect.center() - QPoint(newSize.width() / 2, newSize.height() / 2);
    
    return QRect(newPos, newSize);
}

void QWinUIAnimation::setupAnimation()
{
    // 设置动画的通用属性
}

void QWinUIAnimation::cleanupAnimation()
{
    if (m_currentAnimation) {
        m_currentAnimation->stop();
        if (m_currentAnimation->parent() == this) {
            m_currentAnimation->deleteLater();
        }
        m_currentAnimation = nullptr;
    }
    
    if (m_sequentialGroup) {
        m_sequentialGroup = nullptr;
    }
    
    if (m_parallelGroup) {
        m_parallelGroup = nullptr;
    }
}

void QWinUIAnimation::onAnimationFinished()
{
    emit finished();
}

void QWinUIAnimation::onAnimationStateChanged(QAbstractAnimation::State newState, QAbstractAnimation::State oldState)
{
    Q_UNUSED(oldState)
    
    if (newState == QAbstractAnimation::Running) {
        emit runningChanged(true);
        emit started();
    } else if (newState == QAbstractAnimation::Stopped) {
        emit runningChanged(false);
        emit finished();
    }
}

// 便利函数实现
namespace QWinUIAnimations {
    void fadeIn(QWidget* widget, int duration)
    {
        if (widget) {
            QWinUIAnimation* animation = new QWinUIAnimation(widget, widget);
            animation->fadeIn(duration);
        }
    }
    
    void fadeOut(QWidget* widget, int duration)
    {
        if (widget) {
            QWinUIAnimation* animation = new QWinUIAnimation(widget, widget);
            animation->fadeOut(duration);
        }
    }
    
    void slideIn(QWidget* widget, Qt::Edge edge, int duration)
    {
        if (widget) {
            QWinUIAnimation* animation = new QWinUIAnimation(widget, widget);
            animation->slideIn(edge, duration);
        }
    }
    
    void slideOut(QWidget* widget, Qt::Edge edge, int duration)
    {
        if (widget) {
            QWinUIAnimation* animation = new QWinUIAnimation(widget, widget);
            animation->slideOut(edge, duration);
        }
    }
    
    void scaleIn(QWidget* widget, int duration)
    {
        if (widget) {
            QWinUIAnimation* animation = new QWinUIAnimation(widget, widget);
            animation->scaleIn(duration);
        }
    }
    
    void scaleOut(QWidget* widget, int duration)
    {
        if (widget) {
            QWinUIAnimation* animation = new QWinUIAnimation(widget, widget);
            animation->scaleOut(duration);
        }
    }
    
    void bounceIn(QWidget* widget, int duration)
    {
        if (widget) {
            QWinUIAnimation* animation = new QWinUIAnimation(widget, widget);
            animation->bounceIn(duration);
        }
    }
    
    void pulseEffect(QWidget* widget, int duration, int pulseCount)
    {
        if (widget) {
            QWinUIAnimation* animation = new QWinUIAnimation(widget, widget);
            animation->pulseEffect(duration, pulseCount);
        }
    }
    
    void shakeEffect(QWidget* widget, int duration, int shakeDistance)
    {
        if (widget) {
            QWinUIAnimation* animation = new QWinUIAnimation(widget, widget);
            animation->shakeEffect(duration, shakeDistance);
        }
    }
}

QT_END_NAMESPACE
