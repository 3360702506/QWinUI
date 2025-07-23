#ifndef QWINUIRADIOBUTTON_H
#define QWINUIRADIOBUTTON_H

#include "../QWinUIWidget.h"
#include <QWidget>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QTimer>

QT_BEGIN_NAMESPACE

class QWINUI_EXPORT QWinUIRadioButton : public QWinUIWidget
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(bool checked READ isChecked WRITE setChecked NOTIFY checkedChanged)
    Q_PROPERTY(QString groupName READ groupName WRITE setGroupName NOTIFY groupNameChanged)
    Q_PROPERTY(int animationDuration READ animationDuration WRITE setAnimationDuration NOTIFY animationDurationChanged)
    Q_PROPERTY(double hoverProgress READ hoverProgress WRITE setHoverProgress)
    Q_PROPERTY(double pressProgress READ pressProgress WRITE setPressProgress)
    Q_PROPERTY(double checkProgress READ checkProgress WRITE setCheckProgress)

public:
    explicit QWinUIRadioButton(QWidget* parent = nullptr);
    explicit QWinUIRadioButton(const QString& text, QWidget* parent = nullptr);
    ~QWinUIRadioButton() override;

    // 文本属性
    QString text() const;
    void setText(const QString& text);

    // 选中状态
    bool isChecked() const;
    void setChecked(bool checked);

    // 分组属性
    QString groupName() const;
    void setGroupName(const QString& groupName);

    // 动画属性
    int animationDuration() const;
    void setAnimationDuration(int duration);

    // 动画进度属性
    double hoverProgress() const { return m_hoverProgress; }
    void setHoverProgress(double progress) { m_hoverProgress = progress; update(); }

    double pressProgress() const { return m_pressProgress; }
    void setPressProgress(double progress) { m_pressProgress = progress; update(); }

    double checkProgress() const { return m_checkProgress; }
    void setCheckProgress(double progress) { m_checkProgress = progress; update(); }

    // 尺寸相关
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    // 静态方法 - 分组管理
    static QList<QWinUIRadioButton*> getGroupButtons(QWidget* parent, const QString& groupName = QString());
    static QWinUIRadioButton* getCheckedButton(QWidget* parent, const QString& groupName = QString());

public slots:
    void click();

signals:
    void clicked();
    void checkedChanged(bool checked);
    void textChanged(const QString& text);
    void groupNameChanged(const QString& groupName);
    void animationDurationChanged(int duration);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;

    // 主题变化处理
    void onThemeChanged() override;

private slots:
    void onHoverAnimationFinished();
    void onPressAnimationFinished();
    void onCheckAnimationFinished();

private:
    // 初始化
    void initializeRadioButton();
    void setupAnimations();

    // 绘制方法
    void drawRadioIndicator(QPainter* painter, const QRect& rect);
    void drawText(QPainter* painter, const QRect& rect);

    // 动画控制
    void startHoverAnimation(bool hovered);
    void startPressAnimation(bool pressed);
    void startCheckAnimation(bool checked);

    // 分组管理
    void updateGroupSelection();
    QList<QWinUIRadioButton*> getSiblingButtons() const;

    // 键盘导航
    void navigateToNextButton();
    void navigateToPreviousButton();

    // 状态检查
    bool isHovered() const;
    bool isPressed() const;
    bool isFocused() const;

private:
    // 基本属性
    QString m_text;
    bool m_checked;
    QString m_groupName;
    int m_animationDuration;

    // 状态
    bool m_isHovered;
    bool m_isPressed;
    bool m_isFocused;

    // 动画进度
    double m_hoverProgress;
    double m_pressProgress;
    double m_checkProgress;

    // 动画对象
    QPropertyAnimation* m_hoverAnimation;
    QPropertyAnimation* m_pressAnimation;
    QPropertyAnimation* m_checkAnimation;

    // 常量
    static constexpr int INDICATOR_SIZE = 20;
    static constexpr int INDICATOR_INNER_SIZE = 8;  // 调整内圆大小，更符合 WinUI 3
    static constexpr int TEXT_SPACING = 8;
    static constexpr int DEFAULT_ANIMATION_DURATION = 150;
    static constexpr int FOCUS_BORDER_WIDTH = 2;
    static constexpr int MINIMUM_HEIGHT = 32;
};

QT_END_NAMESPACE

#endif // QWINUIRADIOBUTTON_H
