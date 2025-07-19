#ifndef QWINUIBLUREFFECT_H
#define QWINUIBLUREFFECT_H

#include "QWinUIGlobal.h"
#include <QWidget>

QT_BEGIN_NAMESPACE

class QWINUI_EXPORT QWinUIBlurEffect
{
public:
    enum BlurType {
        None = 0,
        Blur = 1,      // 基础模糊
        Acrylic = 2,   // Acrylic材质
        Mica = 3,      // Mica材质 (Windows 11)
        MicaAlt = 4    // Mica Alt材质 (Windows 11)
    };

    // 为窗口启用模糊效果
    static bool enableBlurBehindWindow(QWidget* widget, BlurType type = Blur);
    
    // 禁用窗口模糊效果
    static bool disableBlurBehindWindow(QWidget* widget);
    
    // 设置Acrylic效果
    static bool setAcrylicEffect(QWidget* widget, const QColor& tintColor = QColor(255, 255, 255, 200));
    
    // 设置Mica效果 (Windows 11)
    static bool setMicaEffect(QWidget* widget, bool isDark = false);
    
    // 检查系统是否支持指定的模糊类型
    static bool isBlurSupported(BlurType type);
    
    // 获取Windows版本信息
    static bool isWindows10OrGreater();
    static bool isWindows11OrGreater();

private:
    // Windows API相关的私有方法
#ifdef Q_OS_WIN
    static bool enableBlurBehindWindowWin10(HWND hwnd);
    static bool enableAcrylicEffectWin10(HWND hwnd, const QColor& tintColor);
    static bool enableMicaEffectWin11(HWND hwnd, bool isDark);
    static bool setWindowCompositionAttribute(HWND hwnd, int attribute, const void* data, int dataSize);
#endif
};

QT_END_NAMESPACE

#endif // QWINUIBLUREFFECT_H
