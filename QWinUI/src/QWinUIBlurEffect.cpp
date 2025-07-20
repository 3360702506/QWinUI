#include "QWinUI/QWinUIBlurEffect.h"
#include <QWidget>
#include <QDebug>

#ifdef Q_OS_WIN
#include <Windows.h>
#include <dwmapi.h>

// Windows 10/11 未公开的API定义
#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

#ifndef DWMWA_MICA_EFFECT
#define DWMWA_MICA_EFFECT 1029
#endif

#ifndef DWMWA_SYSTEMBACKDROP_TYPE
#define DWMWA_SYSTEMBACKDROP_TYPE 38
#endif

// 系统背景类型 - 使用系统定义的枚举

// Windows 10 Acrylic效果相关结构
struct ACCENT_POLICY {
    int AccentState;
    int AccentFlags;
    int GradientColor;
    int AnimationId;
};

struct WINDOWCOMPOSITIONATTRIBDATA {
    int Attribute;
    PVOID Data;
    SIZE_T SizeOfData;
};

// AccentState枚举
enum AccentState {
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_GRADIENT = 1,
    ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,
    ACCENT_ENABLE_BLURBEHIND = 3,
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
    ACCENT_INVALID_STATE = 5
};

// 动态加载的函数指针
typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);
static pSetWindowCompositionAttribute SetWindowCompositionAttribute = nullptr;

// 初始化函数指针
static void initializeWinAPI() {
    static bool initialized = false;
    if (!initialized) {
        HMODULE user32 = GetModuleHandleW(L"user32.dll");
        if (user32) {
            SetWindowCompositionAttribute = 
                (pSetWindowCompositionAttribute)GetProcAddress(user32, "SetWindowCompositionAttribute");
        }
        initialized = true;
    }
}

#endif // Q_OS_WIN

QT_BEGIN_NAMESPACE

bool QWinUIBlurEffect::enableBlurBehindWindow(QWidget* widget, BlurType type)
{
    if (!widget) return false;

#ifdef Q_OS_WIN
    HWND hwnd = (HWND)widget->winId();
    if (!hwnd) return false;

    switch (type) {
    case None:
        return disableBlurBehindWindow(widget);
    case Blur:
        return enableBlurBehindWindowWin10(hwnd);
    case Acrylic:
        return enableAcrylicEffectWin10(hwnd, QColor(255, 255, 255, 200));
    case Mica:
    case MicaAlt:
        if (isWindows11OrGreater()) {
            return enableMicaEffectWin11(hwnd, type == MicaAlt);
        }
        // 在Windows 10上回退到Acrylic
        return enableAcrylicEffectWin10(hwnd, QColor(255, 255, 255, 200));
    }
#else
    Q_UNUSED(widget)
    Q_UNUSED(type)
#endif

    return false;
}

bool QWinUIBlurEffect::disableBlurBehindWindow(QWidget* widget)
{
    if (!widget) return false;

#ifdef Q_OS_WIN
    HWND hwnd = (HWND)widget->winId();
    if (!hwnd) return false;

    initializeWinAPI();
    if (!SetWindowCompositionAttribute) return false;

    ACCENT_POLICY accent = { ACCENT_DISABLED, 0, 0, 0 };
    WINDOWCOMPOSITIONATTRIBDATA data = { 19, &accent, sizeof(accent) };
    return SetWindowCompositionAttribute(hwnd, &data);
#else
    Q_UNUSED(widget)
#endif

    return false;
}

bool QWinUIBlurEffect::setAcrylicEffect(QWidget* widget, const QColor& tintColor)
{
    if (!widget) return false;

#ifdef Q_OS_WIN
    HWND hwnd = (HWND)widget->winId();
    if (!hwnd) return false;

    return enableAcrylicEffectWin10(hwnd, tintColor);
#else
    Q_UNUSED(widget)
    Q_UNUSED(tintColor)
#endif

    return false;
}

bool QWinUIBlurEffect::setMicaEffect(QWidget* widget, bool isDark)
{
    if (!widget) return false;

#ifdef Q_OS_WIN
    HWND hwnd = (HWND)widget->winId();
    if (!hwnd) return false;

    if (isWindows11OrGreater()) {
        return enableMicaEffectWin11(hwnd, isDark);
    }
#else
    Q_UNUSED(widget)
    Q_UNUSED(isDark)
#endif

    return false;
}

bool QWinUIBlurEffect::isBlurSupported(BlurType type)
{
#ifdef Q_OS_WIN
    switch (type) {
    case None:
        return true;
    case Blur:
    case Acrylic:
        return isWindows10OrGreater();
    case Mica:
    case MicaAlt:
        return isWindows11OrGreater();
    }
#else
    Q_UNUSED(type)
#endif
    return false;
}

bool QWinUIBlurEffect::isWindows10OrGreater()
{
#ifdef Q_OS_WIN
    OSVERSIONINFOEXW osvi = {};
    osvi.dwOSVersionInfoSize = sizeof(osvi);
    osvi.dwMajorVersion = 10;
    osvi.dwMinorVersion = 0;
    
    DWORDLONG conditionMask = 0;
    VER_SET_CONDITION(conditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
    VER_SET_CONDITION(conditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
    
    return VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION, conditionMask);
#else
    return false;
#endif
}

bool QWinUIBlurEffect::isWindows11OrGreater()
{
#ifdef Q_OS_WIN
    OSVERSIONINFOEXW osvi = {};
    osvi.dwOSVersionInfoSize = sizeof(osvi);
    osvi.dwMajorVersion = 10;
    osvi.dwMinorVersion = 0;
    osvi.dwBuildNumber = 22000; // Windows 11的最小构建号
    
    DWORDLONG conditionMask = 0;
    VER_SET_CONDITION(conditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
    VER_SET_CONDITION(conditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
    VER_SET_CONDITION(conditionMask, VER_BUILDNUMBER, VER_GREATER_EQUAL);
    
    return VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_BUILDNUMBER, conditionMask);
#else
    return false;
#endif
}

#ifdef Q_OS_WIN
bool QWinUIBlurEffect::enableBlurBehindWindowWin10(HWND hwnd)
{
    initializeWinAPI();
    if (!SetWindowCompositionAttribute) return false;

    ACCENT_POLICY accent = { ACCENT_ENABLE_BLURBEHIND, 0, 0, 0 };
    WINDOWCOMPOSITIONATTRIBDATA data = { 19, &accent, sizeof(accent) };
    return SetWindowCompositionAttribute(hwnd, &data);
}

bool QWinUIBlurEffect::enableAcrylicEffectWin10(HWND hwnd, const QColor& tintColor)
{
    initializeWinAPI();
    if (!SetWindowCompositionAttribute) return false;

    // 将QColor转换为Windows ARGB格式
    int gradientColor = (tintColor.alpha() << 24) | 
                       (tintColor.blue() << 16) | 
                       (tintColor.green() << 8) | 
                       tintColor.red();

    ACCENT_POLICY accent = { ACCENT_ENABLE_ACRYLICBLURBEHIND, 0, gradientColor, 0 };
    WINDOWCOMPOSITIONATTRIBDATA data = { 19, &accent, sizeof(accent) };
    return SetWindowCompositionAttribute(hwnd, &data);
}

bool QWinUIBlurEffect::enableMicaEffectWin11(HWND hwnd, bool isDark)
{
    // 设置深色模式
    BOOL darkMode = isDark ? TRUE : FALSE;
    DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &darkMode, sizeof(darkMode));

    // 设置Mica效果
    DWM_SYSTEMBACKDROP_TYPE backdrop = DWMSBT_MAINWINDOW;
    HRESULT hr = DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdrop, sizeof(backdrop));
    
    return SUCCEEDED(hr);
}

bool QWinUIBlurEffect::setWindowCompositionAttribute(HWND hwnd, int attribute, const void* data, int dataSize)
{
    initializeWinAPI();
    if (!SetWindowCompositionAttribute) return false;

    WINDOWCOMPOSITIONATTRIBDATA compData = { attribute, (PVOID)data, (SIZE_T)dataSize };
    return SetWindowCompositionAttribute(hwnd, &compData);
}
#endif // Q_OS_WIN

QT_END_NAMESPACE
