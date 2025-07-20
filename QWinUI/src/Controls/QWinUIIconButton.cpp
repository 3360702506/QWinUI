#include "QWinUI/Controls/QWinUIIconButton.h"
#include "QWinUI/QWinUITheme.h"
#include "QWinUI/QWinUIIconManager.h"
#include "QWinUI/QWinUIFluentIcons.h"
#include <QPainter>
#include <QPaintEvent>
#include <QApplication>
#include <QFontDatabase>
#include <QFont>
#include <QDebug>

QWinUIIconButton::QWinUIIconButton(QWidget* parent)
    : QWinUIButton(parent)
    , m_iconSize(24, 24)
    , m_fluentIcon(QChar())
    , m_iconColor(Qt::black)
    , m_colorizeIcon(false)
{
    initializeIconButton();
}

QWinUIIconButton::QWinUIIconButton(const QIcon& icon, QWidget* parent)
    : QWinUIButton(parent)
    , m_icon(icon)
    , m_iconSize(24, 24)
    , m_fluentIcon(QChar())
    , m_iconColor(Qt::black)
    , m_colorizeIcon(false)
{
    initializeIconButton();
}

QWinUIIconButton::QWinUIIconButton(const QString& iconPath, QWidget* parent)
    : QWinUIButton(parent)
    , m_iconPath(iconPath)
    , m_iconSize(24, 24)
    , m_fluentIcon(QChar())
    , m_iconColor(Qt::black)
    , m_colorizeIcon(false)
{
    initializeIconButton();
    setIconPath(iconPath);
}

QWinUIIconButton::QWinUIIconButton(const QChar& fluentIcon, QWidget* parent)
    : QWinUIButton(parent)
    , m_iconSize(24, 24)
    , m_fluentIcon(fluentIcon)
    , m_iconColor(Qt::black)
    , m_colorizeIcon(false)
{
    initializeIconButton();
    setFluentIcon(fluentIcon);
}

QWinUIIconButton::~QWinUIIconButton()
{
    // 基类会自动清理
}

void QWinUIIconButton::initializeIconButton()
{
    // 设置默认大小
    setFixedSize(40, 40);
    
    // 隐藏文本，只显示图标
    setText("");
    
    // 连接主题变化信号
    QWinUITheme* theme = QWinUITheme::getInstance();
    if (theme) {
        connect(theme, &QWinUITheme::themeModeChanged, this, [this]() {
            // 主题变化时更新图标颜色
            if (m_colorizeIcon) {
                QWinUITheme* theme = QWinUITheme::getInstance();
                if (theme) {
                    setIconColor(theme->isDarkMode() ? Qt::white : Qt::black);
                }
            }

            // 重新生成Fluent图标以应用新的主题颜色
            if (!m_fluentIcon.isNull()) {
                m_icon = createFluentIcon(m_fluentIcon);
                update();
            }
        });
    }
}

QIcon QWinUIIconButton::icon() const
{
    return m_icon;
}

void QWinUIIconButton::setIcon(const QIcon& icon)
{
    if (m_icon.cacheKey() != icon.cacheKey()) {
        m_icon = icon;
        m_iconPath.clear(); // 清除路径，因为现在使用QIcon
        update();
        emit iconChanged(icon);
    }
}

QSize QWinUIIconButton::iconSize() const
{
    return m_iconSize;
}

void QWinUIIconButton::setIconSize(const QSize& size)
{
    if (m_iconSize != size) {
        m_iconSize = size;
        update();
        emit iconSizeChanged(size);
    }
}

QString QWinUIIconButton::iconPath() const
{
    return m_iconPath;
}

void QWinUIIconButton::setIconPath(const QString& path)
{
    if (m_iconPath != path) {
        m_iconPath = path;
        m_iconName.clear();
        m_fluentIcon = QChar();

        if (!path.isEmpty()) {
            m_icon = QIcon(path);
        } else {
            m_icon = QIcon();
        }
        update();
        emit iconPathChanged(path);
    }
}

QString QWinUIIconButton::iconName() const
{
    return m_iconName;
}

void QWinUIIconButton::setIconName(const QString& name)
{
    if (m_iconName != name) {
        m_iconName = name;
        m_iconPath.clear();
        m_fluentIcon = QChar();

        if (!name.isEmpty()) {
            QWinUIIconManager* iconManager = QWinUIIconManager::getInstance();
            if (iconManager && iconManager->hasIcon(name)) {
                // 获取不同大小的图标并创建QIcon
                QIcon icon;
                QList<int> sizes = {16, 24, 32, 48, 64};

                for (int size : sizes) {
                    QPixmap pixmap = iconManager->getIcon(name, QSize(size, size));
                    if (!pixmap.isNull()) {
                        icon.addPixmap(pixmap);
                    }
                }
                m_icon = icon;
            } else {
                m_icon = QIcon();
            }
        } else {
            m_icon = QIcon();
        }
        update();
        emit iconNameChanged(name);
    }
}

QChar QWinUIIconButton::fluentIcon() const
{
    return m_fluentIcon;
}

void QWinUIIconButton::setFluentIcon(const QChar& icon)
{
    if (m_fluentIcon != icon) {
        m_fluentIcon = icon;
        m_iconPath.clear();
        m_iconName.clear();

        if (!icon.isNull()) {
            m_icon = createFluentIcon(icon);
        } else {
            m_icon = QIcon();
        }
        update();
        emit fluentIconChanged(icon);
    }
}

QColor QWinUIIconButton::iconColor() const
{
    return m_iconColor;
}

void QWinUIIconButton::setIconColor(const QColor& color)
{
    if (m_iconColor != color) {
        m_iconColor = color;
        update();
        emit iconColorChanged(color);
    }
}

bool QWinUIIconButton::colorizeIcon() const
{
    return m_colorizeIcon;
}

void QWinUIIconButton::setColorizeIcon(bool colorize)
{
    if (m_colorizeIcon != colorize) {
        m_colorizeIcon = colorize;
        update();
        emit colorizeIconChanged(colorize);
    }
}

QSize QWinUIIconButton::sizeHint() const
{
    return QSize(40, 40);
}

QSize QWinUIIconButton::minimumSizeHint() const
{
    return QSize(24, 24);
}

void QWinUIIconButton::paintEvent(QPaintEvent* event)
{
    // 先绘制基类的背景和边框
    QWinUIButton::paintEvent(event);
    
    // 然后绘制图标
    if (!m_icon.isNull()) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        
        // 计算图标绘制位置（居中）
        QRect iconRect;
        iconRect.setSize(m_iconSize);
        iconRect.moveCenter(rect().center());
        
        // 获取图标像素图
        QPixmap pixmap = m_icon.pixmap(m_iconSize);
        
        // 如果需要着色，应用颜色
        if (m_colorizeIcon && !m_iconColor.isValid() == false) {
            pixmap = colorizePixmap(pixmap, m_iconColor);
        }
        
        // 绘制图标
        painter.drawPixmap(iconRect, pixmap);
    }
}

QPixmap QWinUIIconButton::colorizePixmap(const QPixmap& pixmap, const QColor& color) const
{
    QPixmap colorizedPixmap = pixmap;
    QPainter painter(&colorizedPixmap);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(colorizedPixmap.rect(), color);
    painter.end();
    return colorizedPixmap;
}

QIcon QWinUIIconButton::createFluentIcon(const QChar& iconChar) const
{
    if (iconChar.isNull()) {
        return QIcon();
    }

    // 创建不同大小的图标
    QIcon icon;
    QList<int> sizes = {16, 24, 32, 48, 64};

    for (int size : sizes) {
        QPixmap pixmap(size, size);
        pixmap.fill(Qt::transparent);

        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::TextAntialiasing);

        // 尝试使用多个可能的字体名称
        QStringList fontNames = {
            "Segoe Fluent Icons",
            "Segoe MDL2 Assets",
            "Segoe UI Symbol",
            "Segoe UI Emoji",
            "Arial Unicode MS"
        };

        QFont font;
        QString usedFontName = "Unknown";

        for (const QString& fontName : fontNames) {
            font = QFont(fontName, size * 0.6); // 稍微小一点
            QFontInfo fontInfo(font);
            usedFontName = fontInfo.family();

            // 检查字体是否真的可用
            if (QFontDatabase::families().contains(fontName, Qt::CaseInsensitive)) {
                break;
            }
        }

        // 确保字体大小合适
        int pixelSize = qMax(8, static_cast<int>(size * 0.6)); // 确保最小8像素
        font.setPixelSize(pixelSize);
        font.setStyleStrategy(QFont::PreferAntialias);

        painter.setFont(font);

        // 设置颜色 - 支持主题联动
        QColor textColor;
        if (m_colorizeIcon) {
            textColor = m_iconColor;
        } else {
            // 根据主题自动选择颜色
            QWinUITheme* theme = QWinUITheme::getInstance();
            if (theme && theme->isDarkMode()) {
                textColor = QColor(255, 255, 255, 200); // 深色主题用浅色图标
            } else {
                textColor = QColor(0, 0, 0, 180); // 浅色主题用深色图标
            }
        }
        painter.setPen(textColor);

        // 绘制图标字符
        QRect textRect(0, 0, size, size);
        QString iconText = QString(iconChar);
        painter.drawText(textRect, Qt::AlignCenter, iconText);

        icon.addPixmap(pixmap);
    }

    return icon;
}
