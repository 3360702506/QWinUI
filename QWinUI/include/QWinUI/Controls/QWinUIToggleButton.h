#ifndef QWINUITOGGLEBUTTON_H
#define QWINUITOGGLEBUTTON_H

#include "QWinUIButton.h"
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

QT_BEGIN_NAMESPACE

class QWINUI_EXPORT QWinUIToggleButton : public QWinUIButton
{
    Q_OBJECT
    Q_PROPERTY(bool isChecked READ isChecked WRITE setChecked NOTIFY checkedChanged)
    Q_PROPERTY(QString checkedText READ checkedText WRITE setCheckedText NOTIFY checkedTextChanged)
    Q_PROPERTY(QString uncheckedText READ uncheckedText WRITE setUncheckedText NOTIFY uncheckedTextChanged)
    Q_PROPERTY(QColor checkedBackgroundColor READ checkedBackgroundColor WRITE setCheckedBackgroundColor)
    Q_PROPERTY(QColor uncheckedBackgroundColor READ uncheckedBackgroundColor WRITE setUncheckedBackgroundColor)
    Q_PROPERTY(QColor checkedTextColor READ checkedTextColor WRITE setCheckedTextColor)
    Q_PROPERTY(QColor uncheckedTextColor READ uncheckedTextColor WRITE setUncheckedTextColor)
    Q_PROPERTY(QColor currentBackgroundColor READ currentBackgroundColor WRITE setCurrentBackgroundColor)
    Q_PROPERTY(QColor currentTextColor READ currentTextColor WRITE setCurrentTextColor)

public:
    explicit QWinUIToggleButton(QWidget* parent = nullptr);
    explicit QWinUIToggleButton(const QString& text, QWidget* parent = nullptr);
    explicit QWinUIToggleButton(const QString& checkedText, const QString& uncheckedText, QWidget* parent = nullptr);
    ~QWinUIToggleButton() override;

    // 核心属性
    bool isChecked() const;
    void setChecked(bool checked);

    QString checkedText() const;
    void setCheckedText(const QString& text);

    QString uncheckedText() const;
    void setUncheckedText(const QString& text);

    // 颜色属性
    QColor checkedBackgroundColor() const;
    void setCheckedBackgroundColor(const QColor& color);

    QColor uncheckedBackgroundColor() const;
    void setUncheckedBackgroundColor(const QColor& color);

    QColor checkedTextColor() const;
    void setCheckedTextColor(const QColor& color);

    QColor uncheckedTextColor() const;
    void setUncheckedTextColor(const QColor& color);

    // 当前动画颜色（用于属性动画）
    QColor currentBackgroundColor() const;
    void setCurrentBackgroundColor(const QColor& color);

    QColor currentTextColor() const;
    void setCurrentTextColor(const QColor& color);

    // 便利方法
    void toggle();

public slots:
    void setChecked(bool checked, bool animated);

signals:
    void checkedChanged(bool checked);
    void toggled(bool checked);
    void checkedTextChanged(const QString& text);
    void uncheckedTextChanged(const QString& text);
    void currentBackgroundColorChanged(const QColor& color);
    void currentTextColorChanged(const QColor& color);

protected:
    // 事件处理
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

    // 主题变化处理
    void onThemeChanged() override;

private slots:
    void onToggleAnimationFinished();
    void updateCurrentColors();

private:
    // 初始化
    void initializeToggleButton();
    void updateColors();
    void updateButtonText();

    // 动画
    void startToggleAnimation();
    void setupColorAnimations();

    // 状态管理
    void updateToggleState(bool checked, bool animated = true);

private:
    // 状态
    bool m_isChecked;
    QString m_checkedText;
    QString m_uncheckedText;

    // 颜色
    QColor m_checkedBackgroundColor;
    QColor m_uncheckedBackgroundColor;
    QColor m_checkedTextColor;
    QColor m_uncheckedTextColor;
    QColor m_currentBackgroundColor;
    QColor m_currentTextColor;

    // 动画
    QParallelAnimationGroup* m_toggleAnimation;
    QPropertyAnimation* m_backgroundColorAnimation;
    QPropertyAnimation* m_textColorAnimation;
    bool m_animationEnabled;

    // 常量
    static constexpr int TOGGLE_ANIMATION_DURATION = 250;
    static constexpr double CHECKED_HOVER_FACTOR = 1.1;
    static constexpr double UNCHECKED_HOVER_FACTOR = 1.05;
    static constexpr double PRESSED_FACTOR = 0.9;
};

QT_END_NAMESPACE

#endif // QWINUITOGGLEBUTTON_H
