#ifndef QWINUIBUTTON_H
#define QWINUIBUTTON_H

#include "../QWinUIWidget.h"
#include <QIcon>
#include <QString>

QT_BEGIN_NAMESPACE

class QWINUI_EXPORT QWinUIButton : public QWinUIWidget
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(QWinUIButtonStyle buttonStyle READ buttonStyle WRITE setButtonStyle NOTIFY buttonStyleChanged)
    Q_PROPERTY(bool isDefault READ isDefault WRITE setDefault NOTIFY defaultChanged)
    Q_PROPERTY(double hoverProgress READ hoverProgress WRITE setHoverProgress)
    Q_PROPERTY(double pressProgress READ pressProgress WRITE setPressProgress)

public:
    enum QWinUIButtonStyle {
        Standard,   // 标准按钮
        Accent,     // 强调按钮（蓝色）
        Subtle,     // 微妙按钮（透明背景）
        Hyperlink   // 超链接样式
    };
    Q_ENUM(QWinUIButtonStyle)

    explicit QWinUIButton(QWidget* parent = nullptr);
    explicit QWinUIButton(const QString& text, QWidget* parent = nullptr);
    ~QWinUIButton();

    // 属性访问器
    QString text() const;
    void setText(const QString& text);

    QIcon icon() const;
    void setIcon(const QIcon& icon);

    QWinUIButtonStyle buttonStyle() const;
    void setButtonStyle(QWinUIButtonStyle style);

    bool isDefault() const;
    void setDefault(bool isDefault);

    // 动画属性
    double hoverProgress() const { return m_hoverProgress; }
    void setHoverProgress(double progress) { m_hoverProgress = progress; update(); }

    double pressProgress() const { return m_pressProgress; }
    void setPressProgress(double progress) { m_pressProgress = progress; update(); }

    // 尺寸相关
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void clicked();
    void pressed();
    void released();
    void textChanged(const QString& text);
    void iconChanged(const QIcon& icon);
    void buttonStyleChanged(QWinUIButtonStyle style);
    void defaultChanged(bool isDefault);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;

    // 为子类提供的状态设置方法
    void setCheckedState(bool checked);
    bool isCheckedState() const;

private slots:
    void onThemeChanged();

private:
    void initializeButton();
    void updateButtonAppearance();
    
    // 绘制方法
    void drawBackground(QPainter* painter, const QRect& rect);
    void drawContent(QPainter* painter, const QRect& rect);
    void drawFocusRect(QPainter* painter, const QRect& rect);
    
    // 颜色获取方法
    QColor getBackgroundColor() const;
    QColor getBorderColor() const;
    QColor getTextColor() const;
    
    // 状态检查
    bool isHovered() const;
    bool isPressed() const;
    bool isFocused() const;

private:
    QString m_text;
    QIcon m_icon;
    QWinUIButtonStyle m_buttonStyle;
    bool m_isDefault;
    bool m_isPressed;
    bool m_isChecked;
    bool m_isHovered;
    bool m_isFocused;
    
    // 动画相关
    QPropertyAnimation* m_hoverAnimation;
    QPropertyAnimation* m_pressAnimation;
    double m_hoverProgress;
    double m_pressProgress;
};

QT_END_NAMESPACE

#endif // QWINUIBUTTON_H
