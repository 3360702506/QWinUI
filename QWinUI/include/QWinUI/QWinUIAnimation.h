#ifndef QWINUIANIMATION_H
#define QWINUIANIMATION_H

#include "QWinUIGlobal.h"
#include <QObject>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QEasingCurve>
#include <QGraphicsOpacityEffect>
#include <QWidget>

QT_BEGIN_NAMESPACE

class QWINUI_EXPORT QWinUIAnimation : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int duration READ duration WRITE setDuration NOTIFY durationChanged)
    Q_PROPERTY(QEasingCurve easingCurve READ easingCurve WRITE setEasingCurve NOTIFY easingCurveChanged)
    Q_PROPERTY(bool running READ isRunning NOTIFY runningChanged)

public:
    explicit QWinUIAnimation(QWidget* target = nullptr, QObject* parent = nullptr);
    ~QWinUIAnimation();

    // 目标控件
    QWidget* target() const;
    void setTarget(QWidget* target);

    // 动画持续时间
    int duration() const;
    void setDuration(int duration);

    // 缓动曲线
    QEasingCurve easingCurve() const;
    void setEasingCurve(const QEasingCurve& curve);

    // 动画状态
    bool isRunning() const;
    bool isPaused() const;

    // 预定义的缓动曲线
    static QEasingCurve fluentEaseOut();
    static QEasingCurve fluentEaseIn();
    static QEasingCurve fluentEaseInOut();
    static QEasingCurve fluentSpring();

    // 基础动画
    void fadeIn(int duration = 200);
    void fadeOut(int duration = 200);
    void fadeToOpacity(double opacity, int duration = 200);

    void slideIn(Qt::Edge edge, int duration = 300);
    void slideOut(Qt::Edge edge, int duration = 300);
    void slideToPosition(const QPoint& position, int duration = 300);

    void scaleIn(int duration = 200);
    void scaleOut(int duration = 200);
    void scaleToSize(const QSize& size, int duration = 300);
    void scaleToFactor(double factor, int duration = 200);

    void rotateToAngle(double angle, int duration = 300);
    void rotateBy(double angle, int duration = 300);

    // 组合动画
    void bounceIn(int duration = 400);
    void bounceOut(int duration = 400);
    void pulseEffect(int duration = 600, int pulseCount = 3);
    void shakeEffect(int duration = 500, int shakeDistance = 10);

    // 页面转换动画
    void pushTransition(QWidget* newWidget, Qt::Edge direction, int duration = 300);
    void popTransition(Qt::Edge direction, int duration = 300);
    void flipTransition(Qt::Orientation orientation, int duration = 400);
    void crossFadeTransition(QWidget* newWidget, int duration = 300);

    // 控制方法
    void start();
    void start(QWinUIAnimationType type, int duration = 200);
    void stop();
    void pause();
    void resume();
    void restart();

    // 动画链
    QWinUIAnimation* then(QWinUIAnimationType type, int duration = 200);
    QWinUIAnimation* wait(int delay);
    QWinUIAnimation* parallel(QWinUIAnimationType type, int duration = 200);

signals:
    void durationChanged(int duration);
    void easingCurveChanged(const QEasingCurve& curve);
    void runningChanged(bool running);
    void started();
    void finished();
    void paused();
    void resumed();

private slots:
    void onAnimationFinished();
    void onAnimationStateChanged(QAbstractAnimation::State newState, QAbstractAnimation::State oldState);

private:
    void setupAnimation();
    void cleanupAnimation();
    
    QPropertyAnimation* createOpacityAnimation(double startOpacity, double endOpacity, int duration);
    QPropertyAnimation* createGeometryAnimation(const QRect& startGeometry, const QRect& endGeometry, int duration);
    QPropertyAnimation* createPositionAnimation(const QPoint& startPos, const QPoint& endPos, int duration);
    QPropertyAnimation* createSizeAnimation(const QSize& startSize, const QSize& endSize, int duration);
    
    void ensureOpacityEffect();
    void removeOpacityEffect();
    
    QRect calculateSlideGeometry(Qt::Edge edge, bool isSlideIn);
    QRect calculateScaleGeometry(double factor);

private:
    QWidget* m_target;
    QAbstractAnimation* m_currentAnimation;
    QGraphicsOpacityEffect* m_opacityEffect;
    
    int m_duration;
    QEasingCurve m_easingCurve;
    
    // 动画链支持
    QSequentialAnimationGroup* m_sequentialGroup;
    QParallelAnimationGroup* m_parallelGroup;
    bool m_isChaining;
    
    // 原始状态保存
    QRect m_originalGeometry;
    double m_originalOpacity;
    bool m_originalOpacityEffectEnabled;
};

// 便利函数
namespace QWinUIAnimations {
    QWINUI_EXPORT void fadeIn(QWidget* widget, int duration = 200);
    QWINUI_EXPORT void fadeOut(QWidget* widget, int duration = 200);
    QWINUI_EXPORT void slideIn(QWidget* widget, Qt::Edge edge, int duration = 300);
    QWINUI_EXPORT void slideOut(QWidget* widget, Qt::Edge edge, int duration = 300);
    QWINUI_EXPORT void scaleIn(QWidget* widget, int duration = 200);
    QWINUI_EXPORT void scaleOut(QWidget* widget, int duration = 200);
    QWINUI_EXPORT void bounceIn(QWidget* widget, int duration = 400);
    QWINUI_EXPORT void pulseEffect(QWidget* widget, int duration = 600, int pulseCount = 3);
    QWINUI_EXPORT void shakeEffect(QWidget* widget, int duration = 500, int shakeDistance = 10);
}

QT_END_NAMESPACE

#endif // QWINUIANIMATION_H
