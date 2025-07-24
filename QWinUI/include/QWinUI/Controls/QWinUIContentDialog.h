#ifndef QWINUICONTENTDIALOG_H
#define QWINUICONTENTDIALOG_H

#include "../QWinUIWidget.h"
#include "../QWinUIGlobal.h"
#include "QWinUISimpleCard.h"
#include "QWinUIButton.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

class QWINUI_EXPORT QWinUIContentDialog : public QWinUIWidget
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(bool isModal READ isModal WRITE setModal NOTIFY modalChanged)
    Q_PROPERTY(double overlayOpacity READ overlayOpacity WRITE setOverlayOpacity)
    Q_PROPERTY(QSize dialogCardSize READ dialogCardSize WRITE setDialogCardSize)

public:
    enum DialogResult {
        None = 0,
        Primary = 1,
        Secondary = 2,
        Close = 3
    };
    Q_ENUM(DialogResult)

    explicit QWinUIContentDialog(QWidget* parent = nullptr);
    ~QWinUIContentDialog() override;

    // 标题
    QString title() const { return m_title; }
    void setTitle(const QString& title);

    // 模态设置
    bool isModal() const { return m_isModal; }
    void setModal(bool modal);

    // 遮罩透明度
    double overlayOpacity() const { return m_overlayOpacity; }
    void setOverlayOpacity(double opacity);

    // 内容区域访问
    QWidget* contentWidget() const { return m_contentWidget; }
    
    // 按钮设置
    void setPrimaryButtonText(const QString& text);
    void setSecondaryButtonText(const QString& text);
    void setCloseButtonText(const QString& text);
    
    QString primaryButtonText() const;
    QString secondaryButtonText() const;
    QString closeButtonText() const;
    
    void setPrimaryButtonEnabled(bool enabled);
    void setSecondaryButtonEnabled(bool enabled);
    void setCloseButtonEnabled(bool enabled);

    // 显示/隐藏
    void showDialog();
    void hideDialog();
    DialogResult exec(); // 模态显示

    // 尺寸设置
    void setDialogSize(const QSize& size);
    QSize dialogSize() const;

    // 动画用的卡片尺寸属性
    QSize dialogCardSize() const;
    void setDialogCardSize(const QSize& size);

signals:
    void titleChanged(const QString& title);
    void modalChanged(bool modal);
    void primaryButtonClicked();
    void secondaryButtonClicked();
    void closeButtonClicked();
    void dialogClosed(DialogResult result);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    
    // 重写QWinUIWidget的主题变化处理
    void onThemeChanged() override;

private slots:
    void onPrimaryButtonClicked();
    void onSecondaryButtonClicked();
    void onCloseButtonClicked();
    void onShowAnimationFinished();
    void onHideAnimationFinished();

private:
    void initializeDialog();
    void setupLayout();
    void setupAnimations();
    void updateOverlay();
    void updateDialogPosition();
    void updateDialogLayout();
    void updateSeparatorColors();
    
    // 动画
    void startShowAnimation();
    void startHideAnimation();
    
    // 颜色获取
    QColor getOverlayColor() const;
    QColor getSeparatorColor() const;

private:
    // 基本属性
    QString m_title;
    bool m_isModal;
    double m_overlayOpacity;
    QSize m_dialogSize;
    DialogResult m_result;
    
    // UI组件
    QWinUISimpleCard* m_dialogCard;
    QWidget* m_overlayWidget;
    
    // 布局
    QVBoxLayout* m_mainLayout;
    QHBoxLayout* m_buttonLayout;
    
    // 内容区域
    QLabel* m_titleLabel;
    QWidget* m_contentWidget;
    QFrame* m_separator;
    QWidget* m_buttonArea;
    
    // 按钮
    QWinUIButton* m_primaryButton;
    QWinUIButton* m_secondaryButton;
    QWinUIButton* m_closeButton;
    
    // 动画
    QPropertyAnimation* m_overlayAnimation;
    QPropertyAnimation* m_cardAnimation;
    QParallelAnimationGroup* m_showAnimationGroup;
    QParallelAnimationGroup* m_hideAnimationGroup;
    QGraphicsOpacityEffect* m_cardOpacityEffect;
    
    // 常量
    static constexpr int DEFAULT_DIALOG_WIDTH = 400;
    static constexpr int DEFAULT_DIALOG_HEIGHT = 300;
    static constexpr int TITLE_HEIGHT = 48;
    static constexpr int BUTTON_HEIGHT = 48;
    static constexpr int CONTENT_MARGIN = 24;
    static constexpr int BUTTON_MARGIN = 16;
    static constexpr double DEFAULT_OVERLAY_OPACITY = 0.3;
    static constexpr int ANIMATION_DURATION = 250;
};

#endif // QWINUICONTENTDIALOG_H
