#include "widget.h"

#include <QApplication>
#include <QWinUI/QWinUI.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 设置应用程序信息
    app.setApplicationName("QWinUI Demo");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("QWinUI Project");

    // 初始化QWinUI
    QWinUI::initialize();

    // 设置全局主题为浅色模式
    QWinUI::setGlobalTheme(QWinUIThemeMode::Light);

    // 设置全局强调色
    QWinUI::setGlobalAccentColor(QColor(0, 120, 215));

    // 创建主窗口
    Widget widget;
    widget.show();

    return app.exec();
}
