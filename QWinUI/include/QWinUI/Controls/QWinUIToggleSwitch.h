#ifndef QWINUITOGGLESWITCH_H
#define QWINUITOGGLESWITCH_H

#include "../QWinUIWidget.h"
#include <QString>

QT_BEGIN_NAMESPACE

class QPropertyAnimation;

class QWINUI_EXPORT QWinUIToggleSwitch : public QWinUIWidget
{
    Q_OBJECT
    Q_PROPERTY(bool isOn READ isOn WRITE setIsOn NOTIFY toggled)
    Q_PROPERTY(QString onContent READ onContent WRITE setOnContent NOTIFY onContentChanged)
    Q_PROPERTY(QString offContent READ offContent WRITE setOffContent NOTIFY offContentChanged)
    Q_PROPERTY(double thumbPosition READ thumbPosition WRITE setThumbPosition)
    Q_PROPERTY(double hoverProgress READ hoverProgress WRITE setHoverProgress)
    Q_PROPERTY(double pressProgress READ pressProgress WRITE setPressProgress)

public:
    explicit QWinUIToggleSwitch(QWidget* parent = nullptr);
    explicit QWinUIToggleSwitch(const QString& text, QWidget* parent = nullptr);
    virtual ~QWinUIToggleSwitch();

    // 属性访问器
    bool isOn() const { return m_isOn; }
    void setIsOn(bool on);

    QString onContent() const { return m_onContent; }
    void setOnContent(const QString& content);

    QString offContent() const { return m_offContent; }
    void setOffContent(const QString& content);

    // 动画属性
    double thumbPosition() const { return m_thumbPosition; }
    void setThumbPosition(double position);

    double hoverProgress() const { return m_hoverProgress; }
    void setHoverProgress(double progress);

    double pressProgress() const { return m_pressProgress; }
    void setPressProgress(double progress);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

signals:
    void toggled(bool checked);
    void onContentChanged(const QString& content);
    void offContentChanged(const QString& content);

private slots:
    void onThemeChanged();

private:
    void initializeToggleSwitch();
    void updateToggleSwitch();
    void toggle();
    
    // 绘制方法
    void drawTrack(QPainter* painter, const QRect& rect);
    void drawThumb(QPainter* painter, const QRect& rect);
    void drawContent(QPainter* painter, const QRect& rect);
    
    // 颜色获取方法
    QColor getTrackColor() const;
    QColor getThumbColor() const;
    QColor getContentColor() const;

private:
    bool m_isOn;
    QString m_onContent;
    QString m_offContent;
    
    // 状态
    bool m_isHovered;
    bool m_isPressed;
    bool m_isDragging;

    // 拖动相关
    QPoint m_dragStartPos;
    double m_dragStartThumbPos;
    
    // 动画属性
    double m_thumbPosition;    // 0.0 (左) 到 1.0 (右)
    double m_hoverProgress;    // 0.0 到 1.0
    double m_pressProgress;    // 0.0 到 1.0
    
    // 动画
    QPropertyAnimation* m_thumbAnimation;
    QPropertyAnimation* m_hoverAnimation;
    QPropertyAnimation* m_pressAnimation;
    
    // 尺寸常量
    static constexpr int TRACK_WIDTH = 44;
    static constexpr int TRACK_HEIGHT = 20;
    static constexpr int THUMB_SIZE = 14;
    static constexpr int CONTENT_SPACING = 8;
};

QT_END_NAMESPACE

#endif // QWINUITOGGLESWITCH_H
