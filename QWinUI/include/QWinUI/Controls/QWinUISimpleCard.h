#ifndef QWINUISIMPLECARD_H
#define QWINUISIMPLECARD_H

#include "../QWinUIWidget.h"
#include "../QWinUIGlobal.h"
#include <QVBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>

class QWINUI_EXPORT QWinUISimpleCard : public QWinUIWidget
{
    Q_OBJECT
    Q_PROPERTY(double hoverProgress READ hoverProgress WRITE setHoverProgress)
    Q_PROPERTY(int cornerRadius READ cornerRadius WRITE setCornerRadius NOTIFY cornerRadiusChanged)
    Q_PROPERTY(QMargins contentMargins READ contentMargins WRITE setContentMargins NOTIFY contentMarginsChanged)
    Q_PROPERTY(bool hoverEnabled READ isHoverEnabled WRITE setHoverEnabled NOTIFY hoverEnabledChanged)

public:
    explicit QWinUISimpleCard(QWidget* parent = nullptr);
    ~QWinUISimpleCard() override;

    // 布局管理
    QVBoxLayout* layout() const { return m_layout; }
    void addWidget(QWidget* widget);
    void removeWidget(QWidget* widget);
    void clearWidgets();

    // 外观属性
    int cornerRadius() const { return m_cornerRadius; }
    void setCornerRadius(int radius);

    QMargins contentMargins() const { return m_contentMargins; }
    void setContentMargins(const QMargins& margins);
    void setContentMargins(int left, int top, int right, int bottom);

    // 交互属性
    bool isHoverEnabled() const { return m_hoverEnabled; }
    void setHoverEnabled(bool enabled);

    // 动画属性
    double hoverProgress() const { return m_hoverProgress; }
    void setHoverProgress(double progress);

    // 尺寸提示
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void cornerRadiusChanged(int radius);
    void contentMarginsChanged(const QMargins& margins);
    void hoverEnabledChanged(bool enabled);
    void cardHovered(bool hovered);

protected:
    // 事件处理
    void paintEvent(QPaintEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

    // 重写QWinUIWidget的主题变化处理
    void onThemeChanged() override;

private:
    void initializeCard();
    void updateTheme();
    void updateShadowEffect();
    void startHoverAnimation(bool hovered);

    // 颜色获取方法
    QColor getBackgroundColor() const;
    QColor getBorderColor() const;
    QColor getHoverBackgroundColor() const;

    // 绘制方法
    void drawBackground(QPainter* painter, const QRect& rect);
    void drawBorder(QPainter* painter, const QRect& rect);

private:
    // UI组件
    QVBoxLayout* m_layout;
    QGraphicsDropShadowEffect* m_shadowEffect;
    
    // 外观属性
    int m_cornerRadius;
    QMargins m_contentMargins;
    
    // 交互属性
    bool m_hoverEnabled;
    bool m_isHovered;
    
    // 动画
    QPropertyAnimation* m_hoverAnimation;
    double m_hoverProgress;
    
    // 常量
    static constexpr int DEFAULT_CORNER_RADIUS = 8;
    static constexpr int DEFAULT_CONTENT_MARGIN = 16;
    static constexpr int SHADOW_BLUR_RADIUS = 16;
    static constexpr int SHADOW_OFFSET = 2;
    static constexpr int ANIMATION_DURATION = 150;
    static constexpr double HOVER_OPACITY_FACTOR = 0.1;
};

#endif // QWINUISIMPLECARD_H
