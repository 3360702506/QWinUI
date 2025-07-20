#pragma once

#include "QWinUI/QWinUIWidget.h"
#include <QSvgRenderer>
#include <QPropertyAnimation>

QT_BEGIN_NAMESPACE

class QWINUI_EXPORT QWinUIIcon : public QWinUIWidget
{
    Q_OBJECT
    Q_PROPERTY(QString iconName READ iconName WRITE setIconName NOTIFY iconNameChanged)
    Q_PROPERTY(QSize iconSize READ iconSize WRITE setIconSize NOTIFY iconSizeChanged)
    Q_PROPERTY(QColor iconColor READ iconColor WRITE setIconColor NOTIFY iconColorChanged)
    Q_PROPERTY(double rotation READ rotation WRITE setRotation NOTIFY rotationChanged)
    Q_PROPERTY(double opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)
    Q_PROPERTY(bool useThemeColor READ useThemeColor WRITE setUseThemeColor NOTIFY useThemeColorChanged)
    Q_PROPERTY(bool smoothScaling READ smoothScaling WRITE setSmoothScaling NOTIFY smoothScalingChanged)

public:
    explicit QWinUIIcon(QWidget* parent = nullptr);
    explicit QWinUIIcon(const QString& iconName, QWidget* parent = nullptr);
    explicit QWinUIIcon(const QString& iconName, const QSize& size, QWidget* parent = nullptr);
    ~QWinUIIcon();

    // 图标属性
    QString iconName() const;
    void setIconName(const QString& name);

    // 动态加载SVG
    bool loadSvgFromFile(const QString& filePath);
    bool loadSvgFromData(const QByteArray& svgData);
    bool loadSvgFromString(const QString& svgContent);

    // 字体图标支持
    void setFontIcon(const QString& fontFamily, QChar iconChar);
    void setFluentIcon(QChar iconChar);  // 使用Segoe Fluent Icons
    void setFluentIcon(const QString& iconName);  // 使用图标名称

    QSize iconSize() const;
    void setIconSize(const QSize& size);
    void setIconSize(int width, int height);

    QColor iconColor() const;
    void setIconColor(const QColor& color);

    // 变换属性
    double rotation() const;
    void setRotation(double angle);

    double opacity() const;
    void setOpacity(double opacity);

    // 渲染选项
    bool useThemeColor() const;
    void setUseThemeColor(bool use);

    bool smoothScaling() const;
    void setSmoothScaling(bool smooth);

    // 动画方法
    void animateRotation(double targetAngle, int duration = 300);
    void animateOpacity(double targetOpacity, int duration = 300);
    void startSpinAnimation(int duration = 1000);
    void stopSpinAnimation();

    // 工具方法
    bool isValid() const;
    QPixmap getPixmap() const;
    QSvgRenderer* getRenderer() const;

    // 尺寸建议
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void iconNameChanged(const QString& name);
    void iconSizeChanged(const QSize& size);
    void iconColorChanged(const QColor& color);
    void rotationChanged(double angle);
    void opacityChanged(double opacity);
    void useThemeColorChanged(bool use);
    void smoothScalingChanged(bool smooth);
    void clicked();

protected:
    // 事件处理
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

    // 主题处理
    void onThemeChanged() override;

private slots:
    void onRotationAnimationFinished();
    void onOpacityAnimationFinished();

private:
    void initializeComponent();
    void updateIcon();
    void updateThemeColor();
    QColor getEffectiveIconColor() const;
    QTransform getTransform() const;
    QString getResourcePath(const QString& iconName) const;
    QChar getFluentIconChar(const QString& iconName) const;
    QPixmap renderFontIcon() const;

    // 成员变量
    QString m_iconName;
    QSize m_iconSize;
    QColor m_iconColor;
    double m_rotation;
    double m_iconOpacity;
    bool m_useThemeColor;
    bool m_smoothScaling;
    bool m_isPressed;
    bool m_isHovered;

    // 动态SVG数据
    QByteArray m_dynamicSvgData;
    bool m_isDynamicSvg;

    // 字体图标数据
    QString m_fontFamily;
    QChar m_fontIconChar;
    bool m_isFontIcon;

    // 缓存
    QPixmap m_cachedPixmap;
    QString m_lastCacheKey;

    // 动画
    QPropertyAnimation* m_rotationAnimation;
    QPropertyAnimation* m_opacityAnimation;
    QPropertyAnimation* m_spinAnimation;

    // 常量
    static constexpr int DEFAULT_ICON_SIZE = 16;
    static constexpr double DEFAULT_OPACITY = 1.0;
    static constexpr double DEFAULT_ROTATION = 0.0;
};

QT_END_NAMESPACE
