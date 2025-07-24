#pragma once

#include <QWidget>
#include <QHBoxLayout>
#include <QIcon>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QTimer>

#include "../QWinUIWidget.h"
#include "../QWinUIGlobal.h"
#include "../QWinUIFluentIcons.h"
#include "QWinUIButton.h"
#include "QWinUIDropDownButton.h"
#include "QWinUIMenuFlyout.h"

class QWINUI_EXPORT QWinUISplitButton : public QWinUIWidget
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(double hoverProgress READ hoverProgress WRITE setHoverProgress)
    Q_PROPERTY(double pressProgress READ pressProgress WRITE setPressProgress)
    Q_PROPERTY(double arrowOffset READ arrowOffset WRITE setArrowOffset)

public:
    explicit QWinUISplitButton(QWidget* parent = nullptr);
    explicit QWinUISplitButton(const QString& text, QWidget* parent = nullptr);
    explicit QWinUISplitButton(const QString& text, const QIcon& icon, QWidget* parent = nullptr);
    ~QWinUISplitButton();

    // 属性访问器
    QString text() const { return m_text; }
    void setText(const QString& text);

    QIcon icon() const { return m_icon; }
    void setIcon(const QIcon& icon);

    // 菜单管理
    QWinUIMenuFlyout* flyout() const { return m_flyout; }
    void setFlyout(QWinUIMenuFlyout* flyout);

    // 状态管理
    void setEnabled(bool enabled);

    // 动画属性（现在由内部按钮处理，保留接口兼容性）
    double hoverProgress() const { return 0.0; }
    void setHoverProgress(double progress);

    double pressProgress() const { return 0.0; }
    void setPressProgress(double progress);

    double arrowOffset() const { return m_dropDownOffset; }
    void setArrowOffset(double offset);

    // 尺寸相关
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void textChanged(const QString& text);
    void iconChanged(const QIcon& icon);
    void enabledChanged(bool enabled);
    void clicked();
    void dropDownClicked();

protected:
    void showEvent(QShowEvent* event) override;

private slots:
    void onFlyoutAboutToShow();
    void onFlyoutAboutToHide();

private slots:
    void onMainButtonClicked();
    void onDropDownButtonClicked();
    void onDropDownButtonPressed();
    void onDropDownButtonReleased();

private:
    void initializeButton();
    void updateLayout();
    void showFlyout();
    void hideFlyout();
    void startArrowAnimation(bool pressed);

private:
    // UI组件
    QWinUIButton* m_mainButton;
    QWinUIDropDownButton* m_dropDownButton;
    QHBoxLayout* m_layout;

    // 基本属性
    QString m_text;
    QIcon m_icon;
    QWinUIMenuFlyout* m_flyout;

    // 下拉箭头动画
    QPropertyAnimation* m_dropDownAnimation;
    double m_dropDownOffset;

    // 布局常量
    static constexpr int BUTTON_HEIGHT = 28;  // 减小高度以确保边框显示
    static constexpr int DROPDOWN_WIDTH = 28; // 相应调整下拉按钮宽度
    static constexpr int SEPARATOR_WIDTH = 1;
    static constexpr int ANIMATION_DURATION = 150;
    static constexpr double MAX_DROPDOWN_OFFSET = 2.0;
};
