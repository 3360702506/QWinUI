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
#include "QWinUIMenuFlyout.h"

// 自定义下拉按钮类，支持箭头动画
class QWINUI_EXPORT QWinUIDropDownButton : public QWinUIButton
{
    Q_OBJECT
    Q_PROPERTY(double arrowOffset READ arrowOffset WRITE setArrowOffset)

public:
    explicit QWinUIDropDownButton(QWidget* parent = nullptr);

    double arrowOffset() const { return m_arrowOffset; }
    void setArrowOffset(double offset);

    // 菜单功能
    QWinUIMenuFlyout* flyout() const { return m_flyout; }
    void setFlyout(QWinUIMenuFlyout* flyout);

    // 便捷方法添加菜单项
    QWinUIMenuFlyoutItem* addItem(const QString& text);
    QWinUIMenuFlyoutItem* addItem(const QIcon& icon, const QString& text);
    void addSeparator();

signals:
    void flyoutAboutToShow();
    void flyoutAboutToHide();

protected:
    void paintEvent(QPaintEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private slots:
    void showFlyout();

private:
    double m_arrowOffset;
    QFont m_fluentFont;
    QWinUIMenuFlyout* m_flyout;
};
