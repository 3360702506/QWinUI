#include <QApplication>
#include <QWinUI/QWinUITheme.h>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 设置应用程序信息
    app.setApplicationName("QWinUI Basic Example");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("QWinUI Project");
    
    // 初始化QWinUI主题
    QWinUITheme* theme = QWinUITheme::getInstance();
    theme->setThemeMode(QWinUIThemeMode::Auto);

    
    // 创建主窗口
    MainWindow window;
    window.show();
    
    return app.exec();
}
