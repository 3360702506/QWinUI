#ifndef QWINUIICONBUTTON_H
#define QWINUIICONBUTTON_H

#include "QWinUIButton.h"
#include <QIcon>
#include <QPixmap>
#include <QChar>

QT_BEGIN_NAMESPACE

class QWINUI_EXPORT QWinUIIconButton : public QWinUIButton
{
    Q_OBJECT
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(QSize iconSize READ iconSize WRITE setIconSize NOTIFY iconSizeChanged)
    Q_PROPERTY(QString iconPath READ iconPath WRITE setIconPath NOTIFY iconPathChanged)
    Q_PROPERTY(QString iconName READ iconName WRITE setIconName NOTIFY iconNameChanged)
    Q_PROPERTY(QChar fluentIcon READ fluentIcon WRITE setFluentIcon NOTIFY fluentIconChanged)
    Q_PROPERTY(QColor iconColor READ iconColor WRITE setIconColor NOTIFY iconColorChanged)
    Q_PROPERTY(bool colorizeIcon READ colorizeIcon WRITE setColorizeIcon NOTIFY colorizeIconChanged)

public:
    explicit QWinUIIconButton(QWidget* parent = nullptr);
    explicit QWinUIIconButton(const QIcon& icon, QWidget* parent = nullptr);
    explicit QWinUIIconButton(const QString& iconPath, QWidget* parent = nullptr);
    explicit QWinUIIconButton(const QChar& fluentIcon, QWidget* parent = nullptr);
    ~QWinUIIconButton() override;

    // 图标属性
    QIcon icon() const;
    void setIcon(const QIcon& icon);

    QSize iconSize() const;
    void setIconSize(const QSize& size);

    QString iconPath() const;
    void setIconPath(const QString& path);

    QString iconName() const;
    void setIconName(const QString& name);

    QChar fluentIcon() const;
    void setFluentIcon(const QChar& icon);

    QColor iconColor() const;
    void setIconColor(const QColor& color);

    bool colorizeIcon() const;
    void setColorizeIcon(bool colorize);

    // 尺寸提示
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void iconChanged(const QIcon& icon);
    void iconSizeChanged(const QSize& size);
    void iconPathChanged(const QString& path);
    void iconNameChanged(const QString& name);
    void fluentIconChanged(const QChar& icon);
    void iconColorChanged(const QColor& color);
    void colorizeIconChanged(bool colorize);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void initializeIconButton();
    void updateIcon();
    QPixmap colorizePixmap(const QPixmap& pixmap, const QColor& color) const;
    QIcon createFluentIcon(const QChar& iconChar) const;

    QIcon m_icon;
    QSize m_iconSize;
    QString m_iconPath;
    QString m_iconName;
    QChar m_fluentIcon;
    QColor m_iconColor;
    bool m_colorizeIcon;
};

QT_END_NAMESPACE

#endif // QWINUIICONBUTTON_H
