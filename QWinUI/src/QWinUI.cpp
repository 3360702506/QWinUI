#include "QWinUI/QWinUI.h"
#include <QApplication>

QT_BEGIN_NAMESPACE

namespace QWinUI {

QString version()
{
    return QString(QWINUI_VERSION_STR);
}

int versionMajor()
{
    return QWINUI_VERSION_MAJOR;
}

int versionMinor()
{
    return QWINUI_VERSION_MINOR;
}

int versionPatch()
{
    return QWINUI_VERSION_PATCH;
}

void initialize()
{
    // 初始化QWinUI库
    // 确保主题单例被创建
    QWinUITheme::getInstance();
}

void cleanup()
{
    // 清理QWinUI库
    QWinUITheme::destroyInstance();
}

void setGlobalTheme(QWinUIThemeMode mode)
{
    QWinUITheme* theme = QWinUITheme::getInstance();
    theme->setThemeMode(mode);
}

void setGlobalAccentColor(const QColor& color)
{
    QWinUITheme* theme = QWinUITheme::getInstance();
    theme->setAccentColor(color);
}

// applyGlobalStyle函数已移除，因为不再使用QWinUIStyle
// 用户可以直接使用QWinUITheme进行主题管理

} // namespace QWinUI

QT_END_NAMESPACE
