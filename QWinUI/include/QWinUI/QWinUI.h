#ifndef QWINUI_H
#define QWINUI_H

// QWinUI 主头文件 - 包含所有公共API

#include "QWinUIGlobal.h"
#include "QWinUIWidget.h"
#include "QWinUITheme.h"
#include "QWinUIIconManager.h"
#include "QWinUIIconConstants.h"
#include "QWinUIFluentIcons.h"
#include "QWinUIAnimation.h"
#include "QWinUIBlurEffect.h"

// Controls
#include "Controls/QWinUIButton.h"
#include "Controls/QWinUIIconButton.h"
#include "Controls/QWinUIToggleButton.h"
#include "Controls/QWinUIToggleSwitch.h"
#include "Controls/QWinUIToolTip.h"
#include "Controls/QWinUIProgressBar.h"
#include "Controls/QWinUIProgressRing.h"
#include "Controls/QWinUIIcon.h"
#include "Controls/QWinUIAcrylicBrush.h"
#include "Controls/QWinUIRichEditBox.h"
#include "Controls/QWinUITextInput.h"
#include "Controls/QWinUITextBox.h"
#include "Controls/QWinUIPasswordBox.h"
#include "Controls/QWinUIScrollView.h"
#include "Controls/QWinUIScrollBar.h"
#include "Controls/QWinUINotification.h"
#include "Controls/QWinUIAppBarSeparator.h"
#include "Controls/QWinUIVariableSizedWrapGrid.h"
#include "Controls/QWinUIMenuFlyout.h"
#include "Controls/QWinUISplitButton.h"
#include "Controls/QWinUIDropDownButton.h"
#include "Controls/QWinUISimpleCard.h"
#include "Controls/QWinUIContentDialog.h"
#include "Controls/QWinUIRadioButton.h"
#include "Controls/QWinUISlider.h"

// Layouts
#include "Layouts/QWinUIFlowLayout.h"
#include "Controls/QWinUIToolTip.h"

// 版本信息
#define QWINUI_VERSION_CHECK(major, minor, patch) \
    ((major << 16) | (minor << 8) | patch)

#define QWINUI_VERSION \
    QWINUI_VERSION_CHECK(QWINUI_VERSION_MAJOR, QWINUI_VERSION_MINOR, QWINUI_VERSION_PATCH)

QT_BEGIN_NAMESPACE

namespace QWinUI {
    // 版本信息函数
    QWINUI_EXPORT QString version();
    QWINUI_EXPORT int versionMajor();
    QWINUI_EXPORT int versionMinor();
    QWINUI_EXPORT int versionPatch();
    
    // 初始化函数
    QWINUI_EXPORT void initialize();
    QWINUI_EXPORT void cleanup();
    
    // 便利函数
    QWINUI_EXPORT void setGlobalTheme(QWinUIThemeMode mode);
    QWINUI_EXPORT void setGlobalAccentColor(const QColor& color);
    // applyGlobalStyle函数已移除，请直接使用QWinUITheme进行主题管理
}

QT_END_NAMESPACE

#endif // QWINUI_H
