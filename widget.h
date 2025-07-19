#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QWinUI/QWinUIWidget.h>
#include <QWinUI/QWinUITheme.h>
#include <QWinUI/Controls/QWinUITextBlock.h>
#include <QWinUI/Controls/QWinUIRichTextBlock.h>
#include <QWinUI/Controls/QWinUIButton.h>
#include <QWinUI/Controls/QWinUIToolTip.h>

class Widget : public QWinUIWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void onButtonClicked();
    void onThemeToggled();
    void onFadeThemeToggled();

private:
    void setupRichTextContent();

private:
    QPoint m_lastMousePos; // 记录最后的鼠标位置

private:
    QWinUITransitionMode m_transitionType;

    QVBoxLayout* m_layout;
    QLabel* m_titleLabel;
    QPushButton* m_testButton;
    QPushButton* m_themeButton;
    QWinUIWidget* m_customWidget;
    QWinUITextBlock* m_textBlock;
    QWinUIRichTextBlock* m_richTextBlock;

    // QWinUI Button 示例
    QWinUIButton* m_standardButton;
    QWinUIButton* m_accentButton;
    QWinUIButton* m_subtleButton;
    QWinUIButton* m_hyperlinkButton;
    QWinUIButton* m_iconButton;
};

#endif // WIDGET_H
