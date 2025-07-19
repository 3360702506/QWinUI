# WinUI 3 自定义控件编写完整指南

## 📋 目录结构

```
QWinUI/
├── include/QWinUI/
│   ├── Controls/
│   │   ├── QWinUIButton.h
│   │   ├── QWinUITextBox.h
│   │   ├── QWinUIComboBox.h
│   │   ├── QWinUICheckBox.h
│   │   ├── QWinUIRadioButton.h
│   │   ├── QWinUISlider.h
│   │   ├── QWinUIProgressBar.h
│   │   ├── QWinUIToggleSwitch.h
│   │   ├── QWinUIListView.h
│   │   ├── QWinUITreeView.h
│   │   ├── QWinUITabView.h
│   │   ├── QWinUINavigationView.h
│   │   ├── QWinUICard.h
│   │   ├── QWinUIExpander.h
│   │   ├── QWinUIInfoBar.h
│   │   ├── QWinUITeachingTip.h
│   │   ├── QWinUIDialog.h
│   │   ├── QWinUIFlyout.h
│   │   ├── QWinUIMenuBar.h
│   │   ├── QWinUIToolBar.h
│   │   ├── QWinUIStatusBar.h
│   │   ├── QWinUIBreadcrumb.h
│   │   ├── QWinUIRating.h
│   │   ├── QWinUICalendar.h
│   │   ├── QWinUITimePicker.h
│   │   ├── QWinUIDatePicker.h
│   │   ├── QWinUIColorPicker.h
│   │   ├── QWinUINumberBox.h
│   │   ├── QWinUIPasswordBox.h
│   │   ├── QWinUISearchBox.h
│   │   ├── QWinUIRichTextBox.h
│   │   ├── QWinUIScrollViewer.h
│   │   ├── QWinUIGridView.h
│   │   ├── QWinUIFlipView.h
│   │   ├── QWinUIWebView.h
│   │   ├── QWinUIMediaPlayer.h
│   │   ├── QWinUIImage.h
│   │   ├── QWinUIIcon.h
│   │   ├── QWinUIBadge.h
│   │   ├── QWinUIChip.h
│   │   ├── QWinUIAvatar.h
│   │   ├── QWinUISpinner.h
│   │   ├── QWinUITooltip.h
│   │   ├── QWinUIContextMenu.h
│   │   ├── QWinUICommandBar.h
│   │   ├── QWinUIAppBar.h
│   │   ├── QWinUITitleBar.h
│   │   ├── QWinUIFrame.h
│   │   ├── QWinUIPanel.h
│   │   ├── QWinUIStackPanel.h
│   │   ├── QWinUIGrid.h
│   │   ├── QWinUICanvas.h
│   │   ├── QWinUIBorder.h
│   │   ├── QWinUIViewBox.h
│   │   ├── QWinUIScrollBar.h
│   │   ├── QWinUIThumb.h
│   │   ├── QWinUIRepeatButton.h
│   │   ├── QWinUIToggleButton.h
│   │   ├── QWinUIHyperlinkButton.h
│   │   ├── QWinUIDropDownButton.h
│   │   ├── QWinUISplitButton.h
│   │   ├── QWinUIMenuFlyout.h
│   │   ├── QWinUIPopup.h
│   │   ├── QWinUIContentDialog.h
│   │   ├── QWinUIMessageDialog.h
│   │   ├── QWinUIFileDialog.h
│   │   ├── QWinUIFolderDialog.h
│   │   ├── QWinUIColorDialog.h
│   │   ├── QWinUIFontDialog.h
│   │   ├── QWinUIPrintDialog.h
│   │   ├── QWinUIPageSetupDialog.h
│   │   ├── QWinUIProgressDialog.h
│   │   ├── QWinUIInputDialog.h
│   │   ├── QWinUIAboutDialog.h
│   │   ├── QWinUISettingsDialog.h
│   │   ├── QWinUIPreferencesDialog.h
│   │   ├── QWinUIWizard.h
│   │   ├── QWinUIPropertySheet.h
│   │   ├── QWinUITaskDialog.h
│   │   ├── QWinUIBalloonTip.h
│   │   ├── QWinUINotification.h
│   │   ├── QWinUIToast.h
│   │   ├── QWinUISystemTray.h
│   │   ├── QWinUIJumpList.h
│   │   ├── QWinUITaskbar.h
│   │   ├── QWinUILiveTitle.h
│   │   ├── QWinUIBadgeNotification.h
│   │   ├── QWinUITileNotification.h
│   │   ├── QWinUIBackgroundTask.h
│   │   ├── QWinUIAppService.h
│   │   ├── QWinUIProtocolHandler.h
│   │   ├── QWinUIFileAssociation.h
│   │   ├── QWinUIShortcut.h
│   │   ├── QWinUIHotkey.h
│   │   ├── QWinUIGesture.h
│   │   ├── QWinUITouch.h
│   │   ├── QWinUIInk.h
│   │   ├── QWinUIHandwriting.h
│   │   ├── QWinUIVoice.h
│   │   ├── QWinUISpeech.h
│   │   ├── QWinUIAccessibility.h
│   │   ├── QWinUIAutomation.h
│   │   ├── QWinUIHighContrast.h
│   │   ├── QWinUIScreenReader.h
│   │   ├── QWinUIKeyboardNavigation.h
│   │   ├── QWinUIFocusManager.h
│   │   ├── QWinUIInputMethod.h
│   │   ├── QWinUITextServices.h
│   │   ├── QWinUIClipboard.h
│   │   ├── QWinUIDragDrop.h
│   │   ├── QWinUIDataTransfer.h
│   │   ├── QWinUIShare.h
│   │   ├── QWinUIContract.h
│   │   ├── QWinUIActivation.h
│   │   ├── QWinUILauncher.h
│   │   ├── QWinUIPicker.h
│   │   ├── QWinUICapture.h
│   │   ├── QWinUICamera.h
│   │   ├── QWinUILocation.h
│   │   ├── QWinUIMap.h
│   │   ├── QWinUIBluetooth.h
│   │   ├── QWinUIWiFi.h
│   │   ├── QWinUINetwork.h
│   │   ├── QWinUIHttp.h
│   │   ├── QWinUIWebSocket.h
│   │   ├── QWinUIDatabase.h
│   │   ├── QWinUIStorage.h
│   │   ├── QWinUIFile.h
│   │   ├── QWinUIFolder.h
│   │   ├── QWinUIStream.h
│   │   ├── QWinUIBuffer.h
│   │   ├── QWinUIEncryption.h
│   │   ├── QWinUICompression.h
│   │   ├── QWinUISerialization.h
│   │   ├── QWinUIJson.h
│   │   ├── QWinUIXml.h
│   │   ├── QWinUIConfig.h
│   │   ├── QWinUISettings.h
│   │   ├── QWinUIRegistry.h
│   │   ├── QWinUIEnvironment.h
│   │   ├── QWinUIProcess.h
│   │   ├── QWinUIThread.h
│   │   ├── QWinUITimer.h
│   │   ├── QWinUIScheduler.h
│   │   ├── QWinUIWorker.h
│   │   ├── QWinUITask.h
│   │   ├── QWinUIAsync.h
│   │   ├── QWinUIPromise.h
│   │   ├── QWinUIFuture.h
│   │   ├── QWinUIObservable.h
│   │   ├── QWinUIReactive.h
│   │   ├── QWinUIBinding.h
│   │   ├── QWinUICommand.h
│   │   ├── QWinUIViewModel.h
│   │   ├── QWinUIModel.h
│   │   ├── QWinUICollection.h
│   │   ├── QWinUIProperty.h
│   │   ├── QWinUIEvent.h
│   │   ├── QWinUISignal.h
│   │   ├── QWinUISlot.h
│   │   ├── QWinUIDelegate.h
│   │   ├── QWinUICallback.h
│   │   ├── QWinUILambda.h
│   │   ├── QWinUIFunction.h
│   │   ├── QWinUIAction.h
│   │   ├── QWinUIBehavior.h
│   │   ├── QWinUITrigger.h
│   │   ├── QWinUICondition.h
│   │   ├── QWinUIRule.h
│   │   ├── QWinUIValidator.h
│   │   ├── QWinUIConverter.h
│   │   ├── QWinUIFormatter.h
│   │   ├── QWinUIParser.h
│   │   ├── QWinUISerializer.h
│   │   ├── QWinUIDeserializer.h
│   │   ├── QWinUIMapper.h
│   │   ├── QWinUIAdapter.h
│   │   ├── QWinUIProxy.h
│   │   ├── QWinUIDecorator.h
│   │   ├── QWinUIFacade.h
│   │   ├── QWinUIBridge.h
│   │   ├── QWinUIComposite.h
│   │   ├── QWinUIStrategy.h
│   │   ├── QWinUITemplate.h
│   │   ├── QWinUIFactory.h
│   │   ├── QWinUIBuilder.h
│   │   ├── QWinUIManager.h
│   │   ├── QWinUIController.h
│   │   ├── QWinUIService.h
│   │   ├── QWinUIProvider.h
│   │   ├── QWinUIRepository.h
│   │   ├── QWinUICache.h
│   │   ├── QWinUIPool.h
│   │   ├── QWinUIQueue.h
│   │   ├── QWinUIStack.h
│   │   ├── QWinUIList.h
│   │   ├── QWinUISet.h
│   │   ├── QWinUIMap.h
│   │   ├── QWinUITree.h
│   │   ├── QWinUIGraph.h
│   │   ├── QWinUIMatrix.h
│   │   ├── QWinUIVector.h
│   │   ├── QWinUIPoint.h
│   │   ├── QWinUISize.h
│   │   ├── QWinUIRect.h
│   │   ├── QWinUIMargin.h
│   │   ├── QWinUIPadding.h
│   │   ├── QWinUIThickness.h
│   │   ├── QWinUICornerRadius.h
│   │   ├── QWinUIBorderRadius.h
│   │   ├── QWinUITransform.h
│   │   ├── QWinUIRotation.h
│   │   ├── QWinUIScale.h
│   │   ├── QWinUITranslation.h
│   │   ├── QWinUISkew.h
│   │   ├── QWinUIMatrix3D.h
│   │   ├── QWinUIQuaternion.h
│   │   ├── QWinUIEuler.h
│   │   ├── QWinUIAxisAngle.h
│   │   ├── QWinUIPlane.h
│   │   ├── QWinUIRay.h
│   │   ├── QWinUIBoundingBox.h
│   │   ├── QWinUIBoundingSphere.h
│   │   ├── QWinUIFrustum.h
│   │   ├── QWinUIViewport.h
│   │   ├── QWinUICamera3D.h
│   │   ├── QWinUILight.h
│   │   ├── QWinUIMaterial.h
│   │   ├── QWinUITexture.h
│   │   ├── QWinUIShader.h
│   │   ├── QWinUIMesh.h
│   │   ├── QWinUIModel3D.h
│   │   ├── QWinUIScene.h
│   │   ├── QWinUIRenderer.h
│   │   ├── QWinUIGraphics.h
│   │   ├── QWinUICanvas2D.h
│   │   ├── QWinUIPath.h
│   │   ├── QWinUIGeometry.h
│   │   ├── QWinUIBrush.h
│   │   ├── QWinUIPen.h
│   │   ├── QWinUIFont.h
│   │   ├── QWinUIText.h
│   │   ├── QWinUIGlyph.h
│   │   ├── QWinUITypography.h
│   │   ├── QWinUILayout.h
│   │   ├── QWinUIAlignment.h
│   │   ├── QWinUIOrientation.h
│   │   ├── QWinUIDirection.h
│   │   ├── QWinUIFlow.h
│   │   ├── QWinUIWrap.h
│   │   ├── QWinUIStretch.h
│   │   ├── QWinUIGravity.h
│   │   ├── QWinUIWeight.h
│   │   ├── QWinUIFlex.h
│   │   ├── QWinUIGrid.h
│   │   └── QWinUIResponsive.h
│   └── QWinUI.h
└── src/
    └── Controls/
        ├── QWinUIButton.cpp
        ├── QWinUITextBox.cpp
        ├── QWinUIComboBox.cpp
        └── ... (对应的实现文件)
```

## 🎯 核心控件优先级

### 🔥 高优先级 (必须实现)
1. **QWinUIButton** - 按钮控件
2. **QWinUITextBox** - 文本输入框
3. **QWinUICheckBox** - 复选框
4. **QWinUIRadioButton** - 单选按钮
5. **QWinUIComboBox** - 下拉框
6. **QWinUISlider** - 滑块
7. **QWinUIProgressBar** - 进度条
8. **QWinUIListView** - 列表视图
9. **QWinUICard** - 卡片容器
10. **QWinUIDialog** - 对话框

### 🔶 中优先级 (常用控件)
11. **QWinUIToggleSwitch** - 开关
12. **QWinUITabView** - 标签页
13. **QWinUINavigationView** - 导航视图
14. **QWinUIExpander** - 展开器
15. **QWinUIInfoBar** - 信息栏
16. **QWinUIMenuBar** - 菜单栏
17. **QWinUIToolBar** - 工具栏
18. **QWinUISearchBox** - 搜索框
19. **QWinUINumberBox** - 数字输入框
20. **QWinUIDatePicker** - 日期选择器

### 🔷 低优先级 (高级控件)
21. **QWinUITreeView** - 树形视图
22. **QWinUICalendar** - 日历
23. **QWinUIColorPicker** - 颜色选择器
24. **QWinUIRating** - 评分控件
25. **QWinUIWebView** - 网页视图

## 🚀 开始实现 - 基础控件模板

### 1. QWinUIButton 实现示例

#### 头文件 (QWinUIButton.h)
```cpp
#ifndef QWINUIBUTTON_H
#define QWINUIBUTTON_H

#include "../QWinUIWidget.h"
#include <QPushButton>

QT_BEGIN_NAMESPACE

class QWINUI_EXPORT QWinUIButton : public QWinUIWidget
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(QWinUIButtonStyle buttonStyle READ buttonStyle WRITE setButtonStyle NOTIFY buttonStyleChanged)
    Q_PROPERTY(bool isDefault READ isDefault WRITE setDefault NOTIFY defaultChanged)

public:
    enum QWinUIButtonStyle {
        Standard,
        Accent,
        Subtle,
        Hyperlink
    };
    Q_ENUM(QWinUIButtonStyle)

    explicit QWinUIButton(QWidget* parent = nullptr);
    explicit QWinUIButton(const QString& text, QWidget* parent = nullptr);
    ~QWinUIButton();

    // 属性访问器
    QString text() const;
    void setText(const QString& text);

    QIcon icon() const;
    void setIcon(const QIcon& icon);

    QWinUIButtonStyle buttonStyle() const;
    void setButtonStyle(QWinUIButtonStyle style);

    bool isDefault() const;
    void setDefault(bool isDefault);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

signals:
    void clicked();
    void textChanged(const QString& text);
    void iconChanged(const QIcon& icon);
    void buttonStyleChanged(QWinUIButtonStyle style);
    void defaultChanged(bool isDefault);

private:
    void initializeButton();
    void updateButtonAppearance();
    QColor getBackgroundColor() const;
    QColor getTextColor() const;
    QColor getBorderColor() const;

private:
    QString m_text;
    QIcon m_icon;
    QWinUIButtonStyle m_buttonStyle;
    bool m_isDefault;
    bool m_isPressed;
};

QT_END_NAMESPACE

#endif // QWINUIBUTTON_H
```

#### 实现文件 (QWinUIButton.cpp)
```cpp
#include "QWinUIButton.h"
#include "../QWinUITheme.h"
#include <QPainter>
#include <QMouseEvent>
#include <QFontMetrics>

QT_BEGIN_NAMESPACE

QWinUIButton::QWinUIButton(QWidget* parent)
    : QWinUIWidget(parent)
    , m_buttonStyle(Standard)
    , m_isDefault(false)
    , m_isPressed(false)
{
    initializeButton();
}

QWinUIButton::QWinUIButton(const QString& text, QWidget* parent)
    : QWinUIWidget(parent)
    , m_text(text)
    , m_buttonStyle(Standard)
    , m_isDefault(false)
    , m_isPressed(false)
{
    initializeButton();
}

QWinUIButton::~QWinUIButton() = default;

void QWinUIButton::initializeButton()
{
    setMinimumSize(120, 32);
    setCornerRadius(4);
    setCursor(Qt::PointingHandCursor);
    updateButtonAppearance();
}

QString QWinUIButton::text() const
{
    return m_text;
}

void QWinUIButton::setText(const QString& text)
{
    if (m_text != text) {
        m_text = text;
        update();
        emit textChanged(text);
    }
}

QIcon QWinUIButton::icon() const
{
    return m_icon;
}

void QWinUIButton::setIcon(const QIcon& icon)
{
    if (m_icon.cacheKey() != icon.cacheKey()) {
        m_icon = icon;
        update();
        emit iconChanged(icon);
    }
}

QWinUIButton::QWinUIButtonStyle QWinUIButton::buttonStyle() const
{
    return m_buttonStyle;
}

void QWinUIButton::setButtonStyle(QWinUIButtonStyle style)
{
    if (m_buttonStyle != style) {
        m_buttonStyle = style;
        updateButtonAppearance();
        emit buttonStyleChanged(style);
    }
}

bool QWinUIButton::isDefault() const
{
    return m_isDefault;
}

void QWinUIButton::setDefault(bool isDefault)
{
    if (m_isDefault != isDefault) {
        m_isDefault = isDefault;
        updateButtonAppearance();
        emit defaultChanged(isDefault);
    }
}

void QWinUIButton::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect = this->rect();

    // 绘制背景
    QColor bgColor = getBackgroundColor();
    if (cornerRadius() > 0) {
        QPainterPath path;
        path.addRoundedRect(rect, cornerRadius(), cornerRadius());
        painter.fillPath(path, bgColor);
    } else {
        painter.fillRect(rect, bgColor);
    }

    // 绘制边框
    QColor borderColor = getBorderColor();
    if (borderColor.isValid()) {
        painter.setPen(QPen(borderColor, 1));
        if (cornerRadius() > 0) {
            painter.drawRoundedRect(rect.adjusted(0, 0, -1, -1), cornerRadius(), cornerRadius());
        } else {
            painter.drawRect(rect.adjusted(0, 0, -1, -1));
        }
    }

    // 绘制图标和文本
    QRect contentRect = rect.adjusted(12, 0, -12, 0);

    if (!m_icon.isNull() && !m_text.isEmpty()) {
        // 图标 + 文本
        QSize iconSize(16, 16);
        QRect iconRect(contentRect.left(), (contentRect.height() - iconSize.height()) / 2,
                      iconSize.width(), iconSize.height());
        painter.drawPixmap(iconRect, m_icon.pixmap(iconSize));

        QRect textRect = contentRect.adjusted(iconSize.width() + 8, 0, 0, 0);
        painter.setPen(getTextColor());
        painter.drawText(textRect, Qt::AlignCenter, m_text);
    } else if (!m_icon.isNull()) {
        // 仅图标
        QSize iconSize(16, 16);
        QRect iconRect((contentRect.width() - iconSize.width()) / 2,
                      (contentRect.height() - iconSize.height()) / 2,
                      iconSize.width(), iconSize.height());
        painter.drawPixmap(iconRect, m_icon.pixmap(iconSize));
    } else if (!m_text.isEmpty()) {
        // 仅文本
        painter.setPen(getTextColor());
        painter.drawText(contentRect, Qt::AlignCenter, m_text);
    }
}

void QWinUIButton::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_isPressed = true;
        update();
    }
    QWinUIWidget::mousePressEvent(event);
}

void QWinUIButton::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_isPressed) {
        m_isPressed = false;
        update();
        if (rect().contains(event->pos())) {
            emit clicked();
        }
    }
    QWinUIWidget::mouseReleaseEvent(event);
}

void QWinUIButton::enterEvent(QEnterEvent* event)
{
    update();
    QWinUIWidget::enterEvent(event);
}

void QWinUIButton::leaveEvent(QEvent* event)
{
    m_isPressed = false;
    update();
    QWinUIWidget::leaveEvent(event);
}

void QWinUIButton::updateButtonAppearance()
{
    update();
}

QColor QWinUIButton::getBackgroundColor() const
{
    QWinUITheme* theme = QWinUITheme::getInstance();
    bool isDark = theme && theme->themeMode() == QWinUIThemeMode::Dark;

    switch (m_buttonStyle) {
    case Accent:
        if (m_isPressed) return isDark ? QColor(0, 95, 184) : QColor(0, 95, 184);
        if (isHovered()) return isDark ? QColor(0, 103, 192) : QColor(0, 103, 192);
        return isDark ? QColor(0, 120, 215) : QColor(0, 120, 215);

    case Subtle:
        if (m_isPressed) return isDark ? QColor(255, 255, 255, 15) : QColor(0, 0, 0, 15);
        if (isHovered()) return isDark ? QColor(255, 255, 255, 10) : QColor(0, 0, 0, 10);
        return Qt::transparent;

    case Hyperlink:
        return Qt::transparent;

    default: // Standard
        if (m_isPressed) return isDark ? QColor(255, 255, 255, 15) : QColor(0, 0, 0, 15);
        if (isHovered()) return isDark ? QColor(255, 255, 255, 10) : QColor(0, 0, 0, 10);
        return isDark ? QColor(255, 255, 255, 8) : QColor(0, 0, 0, 8);
    }
}

QColor QWinUIButton::getTextColor() const
{
    QWinUITheme* theme = QWinUITheme::getInstance();
    bool isDark = theme && theme->themeMode() == QWinUIThemeMode::Dark;

    switch (m_buttonStyle) {
    case Accent:
        return QColor(255, 255, 255);
    case Hyperlink:
        return isDark ? QColor(99, 162, 255) : QColor(0, 120, 215);
    default:
        return isDark ? QColor(255, 255, 255) : QColor(0, 0, 0);
    }
}

QColor QWinUIButton::getBorderColor() const
{
    if (m_buttonStyle == Subtle || m_buttonStyle == Hyperlink) {
        return QColor(); // 无边框
    }

    QWinUITheme* theme = QWinUITheme::getInstance();
    bool isDark = theme && theme->themeMode() == QWinUIThemeMode::Dark;

    return isDark ? QColor(255, 255, 255, 20) : QColor(0, 0, 0, 20);
}

QT_END_NAMESPACE
```

### 2. QWinUITextBox 实现示例

#### 头文件 (QWinUITextBox.h)
```cpp
#ifndef QWINUITEXTBOX_H
#define QWINUITEXTBOX_H

#include "../QWinUIWidget.h"
#include <QLineEdit>

QT_BEGIN_NAMESPACE

class QWINUI_EXPORT QWinUITextBox : public QWinUIWidget
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString placeholderText READ placeholderText WRITE setPlaceholderText NOTIFY placeholderTextChanged)
    Q_PROPERTY(bool isReadOnly READ isReadOnly WRITE setReadOnly NOTIFY readOnlyChanged)
    Q_PROPERTY(bool hasError READ hasError WRITE setHasError NOTIFY hasErrorChanged)

public:
    explicit QWinUITextBox(QWidget* parent = nullptr);
    ~QWinUITextBox();

    QString text() const;
    void setText(const QString& text);

    QString placeholderText() const;
    void setPlaceholderText(const QString& text);

    bool isReadOnly() const;
    void setReadOnly(bool readOnly);

    bool hasError() const;
    void setHasError(bool hasError);

protected:
    void paintEvent(QPaintEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

signals:
    void textChanged(const QString& text);
    void placeholderTextChanged(const QString& text);
    void readOnlyChanged(bool readOnly);
    void hasErrorChanged(bool hasError);
    void editingFinished();

private:
    void initializeTextBox();
    void updateAppearance();
    QColor getBackgroundColor() const;
    QColor getBorderColor() const;
    QColor getTextColor() const;

private:
    QLineEdit* m_lineEdit;
    QString m_placeholderText;
    bool m_hasError;
    bool m_isFocused;
};

QT_END_NAMESPACE

#endif // QWINUITEXTBOX_H
```

### 3. QWinUICheckBox 实现示例

#### 头文件 (QWinUICheckBox.h)
```cpp
#ifndef QWINUICHECKBOX_H
#define QWINUICHECKBOX_H

#include "../QWinUIWidget.h"

QT_BEGIN_NAMESPACE

class QWINUI_EXPORT QWinUICheckBox : public QWinUIWidget
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(bool checked READ isChecked WRITE setChecked NOTIFY checkedChanged)
    Q_PROPERTY(Qt::CheckState checkState READ checkState WRITE setCheckState NOTIFY checkStateChanged)
    Q_PROPERTY(bool tristate READ isTristate WRITE setTristate NOTIFY tristateChanged)

public:
    explicit QWinUICheckBox(QWidget* parent = nullptr);
    explicit QWinUICheckBox(const QString& text, QWidget* parent = nullptr);
    ~QWinUICheckBox();

    QString text() const;
    void setText(const QString& text);

    bool isChecked() const;
    void setChecked(bool checked);

    Qt::CheckState checkState() const;
    void setCheckState(Qt::CheckState state);

    bool isTristate() const;
    void setTristate(bool tristate);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

signals:
    void textChanged(const QString& text);
    void checkedChanged(bool checked);
    void checkStateChanged(Qt::CheckState state);
    void tristateChanged(bool tristate);
    void clicked();
    void toggled(bool checked);

private:
    void initializeCheckBox();
    void toggle();
    void drawCheckBox(QPainter& painter, const QRect& rect);
    void drawCheckMark(QPainter& painter, const QRect& rect);
    QRect getCheckBoxRect() const;
    QRect getTextRect() const;

private:
    QString m_text;
    Qt::CheckState m_checkState;
    bool m_tristate;
    static const int CheckBoxSize = 20;
    static const int Spacing = 8;
};

QT_END_NAMESPACE

#endif // QWINUICHECKBOX_H
```

## 📝 实现步骤指南

### 第一阶段：基础控件 (1-2周)
1. **QWinUIButton** - 按钮控件 ✅
2. **QWinUITextBox** - 文本输入框
3. **QWinUICheckBox** - 复选框
4. **QWinUIRadioButton** - 单选按钮
5. **QWinUILabel** - 标签控件

### 第二阶段：输入控件 (2-3周)
6. **QWinUIComboBox** - 下拉框
7. **QWinUISlider** - 滑块
8. **QWinUIProgressBar** - 进度条
9. **QWinUIToggleSwitch** - 开关
10. **QWinUINumberBox** - 数字输入框

### 第三阶段：容器控件 (3-4周)
11. **QWinUICard** - 卡片容器
12. **QWinUIExpander** - 展开器
13. **QWinUITabView** - 标签页
14. **QWinUIStackPanel** - 堆栈面板
15. **QWinUIGrid** - 网格布局

### 第四阶段：列表控件 (4-5周)
16. **QWinUIListView** - 列表视图
17. **QWinUITreeView** - 树形视图
18. **QWinUIGridView** - 网格视图
19. **QWinUIComboBox** - 下拉列表
20. **QWinUIMenuBar** - 菜单栏

### 第五阶段：对话框控件 (5-6周)
21. **QWinUIDialog** - 基础对话框
22. **QWinUIMessageDialog** - 消息对话框
23. **QWinUIFileDialog** - 文件对话框
24. **QWinUIColorDialog** - 颜色对话框
25. **QWinUIFontDialog** - 字体对话框

## 🎨 设计原则

### WinUI 3 设计语言
- **Fluent Design System** - 现代化设计语言
- **圆角设计** - 4px 默认圆角
- **阴影效果** - 深度感和层次感
- **动画过渡** - 流畅的状态转换
- **响应式设计** - 适配不同屏幕尺寸

### 颜色系统
- **主题适配** - 支持浅色/深色主题
- **强调色** - 系统强调色支持
- **语义颜色** - 成功、警告、错误状态
- **透明度** - Acrylic 材质效果

### 交互规范
- **悬停效果** - 鼠标悬停状态
- **按下效果** - 点击反馈
- **焦点指示** - 键盘导航支持
- **禁用状态** - 不可用状态显示

## 🔧 开发工具

### CMake 配置
```cmake
# 添加新控件到 CMakeLists.txt
set(QWINUI_CONTROLS_HEADERS
    include/QWinUI/Controls/QWinUIButton.h
    include/QWinUI/Controls/QWinUITextBox.h
    include/QWinUI/Controls/QWinUICheckBox.h
    # ... 更多控件
)

set(QWINUI_CONTROLS_SOURCES
    src/Controls/QWinUIButton.cpp
    src/Controls/QWinUITextBox.cpp
    src/Controls/QWinUICheckBox.cpp
    # ... 更多控件
)
```

### 测试框架
```cpp
// 为每个控件创建测试用例
class QWinUIButtonTest : public QObject
{
    Q_OBJECT

private slots:
    void testButtonCreation();
    void testButtonText();
    void testButtonClick();
    void testButtonStyles();
    void testButtonThemes();
};
```

## 📚 资源和参考

### 官方文档
- [WinUI 3 Controls Gallery](https://github.com/microsoft/WinUI-Gallery)
- [Windows App SDK Documentation](https://docs.microsoft.com/en-us/windows/apps/windows-app-sdk/)
- [Fluent Design System](https://www.microsoft.com/design/fluent/)

### 设计资源
- [Fluent UI Icons](https://github.com/microsoft/fluentui-system-icons)
- [Windows 11 Design Principles](https://docs.microsoft.com/en-us/windows/apps/design/)
- [Color Palette Generator](https://docs.microsoft.com/en-us/windows/apps/design/style/color)

### 开发工具
- **Qt Designer** - 可视化界面设计
- **Qt Creator** - 集成开发环境
- **CMake** - 构建系统
- **Git** - 版本控制

## 🎯 项目里程碑

### 里程碑 1: 基础框架 (第1周)
- [x] QWinUIWidget 基类完成
- [x] QWinUITheme 主题系统
- [x] QWinUIBlurEffect 模糊效果
- [ ] 基础控件模板

### 里程碑 2: 核心控件 (第2-4周)
- [ ] QWinUIButton 完成
- [ ] QWinUITextBox 完成
- [ ] QWinUICheckBox 完成
- [ ] QWinUIRadioButton 完成
- [ ] QWinUIComboBox 完成

### 里程碑 3: 高级控件 (第5-8周)
- [ ] QWinUIListView 完成
- [ ] QWinUITreeView 完成
- [ ] QWinUITabView 完成
- [ ] QWinUINavigationView 完成
- [ ] QWinUIDialog 完成

### 里程碑 4: 完善和优化 (第9-12周)
- [ ] 所有控件测试完成
- [ ] 文档编写完成
- [ ] 性能优化完成
- [ ] 示例程序完成

## 🚀 快速开始

### 1. 创建第一个控件
```bash
# 创建控件目录
mkdir -p QWinUI/include/QWinUI/Controls
mkdir -p QWinUI/src/Controls

# 复制模板文件
cp templates/QWinUIControl.h QWinUI/include/QWinUI/Controls/QWinUIButton.h
cp templates/QWinUIControl.cpp QWinUI/src/Controls/QWinUIButton.cpp
```

### 2. 修改 CMakeLists.txt
```cmake
# 添加控件目录
include_directories(include/QWinUI/Controls)

# 添加控件文件
list(APPEND QWINUI_HEADERS include/QWinUI/Controls/QWinUIButton.h)
list(APPEND QWINUI_SOURCES src/Controls/QWinUIButton.cpp)
```

### 3. 测试控件
```cpp
#include <QWinUI/Controls/QWinUIButton.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWinUIButton button("Hello WinUI 3!");
    button.setButtonStyle(QWinUIButton::Accent);
    button.show();

    return app.exec();
}
```

## 📋 控件检查清单

### 每个控件必须实现的功能
- [ ] 基本属性 (text, enabled, visible)
- [ ] 主题适配 (浅色/深色)
- [ ] 鼠标交互 (hover, press, release)
- [ ] 键盘导航 (focus, tab)
- [ ] 无障碍支持 (screen reader)
- [ ] 动画效果 (状态转换)
- [ ] 单元测试 (功能验证)
- [ ] 文档说明 (API文档)

### 质量标准
- **代码覆盖率** > 80%
- **内存泄漏** = 0
- **性能测试** 通过
- **多平台兼容** Windows 10/11
- **高DPI支持** 100%-300%
- **触摸支持** 触摸屏设备

---

## 📞 联系方式

如有问题或建议，请通过以下方式联系：
- **GitHub Issues**: 项目问题追踪
- **Email**: 技术支持邮箱
- **Discord**: 开发者社区

---

**祝你在 WinUI 3 控件开发之路上一切顺利！** 🎉
