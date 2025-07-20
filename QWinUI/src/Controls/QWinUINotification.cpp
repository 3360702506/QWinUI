#include "QWinUI/Controls/QWinUINotification.h"
#include "QWinUI/QWinUITheme.h"
#include <QApplication>
#include <QScreen>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QDebug>

#ifdef Q_OS_WIN
#include <windows.h>
#include <roapi.h>
#include <wrl.h>
#include <wrl/wrappers/corewrappers.h>
#include <windows.ui.notifications.h>
#include <windows.data.xml.dom.h>
#include <windows.applicationmodel.core.h>
#include <combaseapi.h>

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace ABI::Windows::UI::Notifications;
using namespace ABI::Windows::Data::Xml::Dom;
using namespace ABI::Windows::ApplicationModel::Core;
using namespace ABI::Windows::Foundation;
#endif

QT_BEGIN_NAMESPACE

QWinUINotification* QWinUINotification::s_instance = nullptr;

QWinUINotification::QWinUINotification(QObject* parent)
    : QObject(parent)
    , m_type(QWinUINotificationType::Info)
    , m_duration(QWinUINotificationDuration::Medium)
    , m_autoHideTimer(new QTimer(this))
    , m_isShowing(false)
#ifdef Q_OS_WIN
    , m_toastNotifier(nullptr)
    , m_toastNotification(nullptr)
#endif
    , m_fallbackWindow(nullptr)
{
    m_autoHideTimer->setSingleShot(true);
    connect(m_autoHideTimer, &QTimer::timeout, this, &QWinUINotification::onAutoHideTimer);
    
    initializePlatform();
}

QWinUINotification::~QWinUINotification()
{
    cleanup();
}

void QWinUINotification::showInfo(const QString& title, const QString& message, QWinUINotificationDuration duration)
{
    showNotification(title, message, QWinUINotificationType::Info, duration);
}

void QWinUINotification::showWarning(const QString& title, const QString& message, QWinUINotificationDuration duration)
{
    showNotification(title, message, QWinUINotificationType::Warning, duration);
}

void QWinUINotification::showError(const QString& title, const QString& message, QWinUINotificationDuration duration)
{
    showNotification(title, message, QWinUINotificationType::Error, duration);
}

void QWinUINotification::showSuccess(const QString& title, const QString& message, QWinUINotificationDuration duration)
{
    showNotification(title, message, QWinUINotificationType::Success, duration);
}

void QWinUINotification::showNotification(const QString& title, const QString& message, 
                                         QWinUINotificationType type, QWinUINotificationDuration duration,
                                         const QPixmap& icon, const QList<QWinUINotificationAction>& actions)
{
    if (!s_instance) {
        s_instance = new QWinUINotification();
    }
    
    s_instance->setTitle(title);
    s_instance->setMessage(message);
    s_instance->setType(type);
    s_instance->setDuration(duration);
    s_instance->setIcon(icon);
    s_instance->setActions(actions);
    s_instance->show();
}

void QWinUINotification::setTitle(const QString& title)
{
    m_title = title;
}

void QWinUINotification::setMessage(const QString& message)
{
    m_message = message;
}

void QWinUINotification::setType(QWinUINotificationType type)
{
    m_type = type;
}

void QWinUINotification::setDuration(QWinUINotificationDuration duration)
{
    m_duration = duration;
}

void QWinUINotification::setIcon(const QPixmap& icon)
{
    m_icon = icon;
}

void QWinUINotification::setActions(const QList<QWinUINotificationAction>& actions)
{
    m_actions = actions;
}

void QWinUINotification::setApplicationName(const QString& appName)
{
    m_applicationName = appName;
}

void QWinUINotification::setApplicationIcon(const QPixmap& appIcon)
{
    m_applicationIcon = appIcon;
}

void QWinUINotification::show()
{
    if (m_isShowing) {
        hide();
    }
    
    m_isShowing = true;
    
#ifdef Q_OS_WIN
    if (isSupported()) {
        showWindowsNotification();
    } else {
        showFallbackNotification();
    }
#else
    showFallbackNotification();
#endif
    
    // 设置自动隐藏定时器
    if (m_duration != QWinUINotificationDuration::Persistent) {
        m_autoHideTimer->start(static_cast<int>(m_duration));
    }
}

void QWinUINotification::hide()
{
    if (!m_isShowing) {
        return;
    }
    
    m_isShowing = false;
    m_autoHideTimer->stop();
    
#ifdef Q_OS_WIN
    // 清理Windows通知
    if (m_toastNotification) {
        // 隐藏Toast通知的代码将在后续添加
    }
#endif
    
    // 隐藏回退窗口
    if (m_fallbackWindow) {
        m_fallbackWindow->hide();
        m_fallbackWindow->deleteLater();
        m_fallbackWindow = nullptr;
    }
    
    emit notificationDismissed();
}

bool QWinUINotification::isSupported()
{
#ifdef Q_OS_WIN
    // 由于Windows 10/11的兼容性行为，版本检查不可靠
    // 直接尝试初始化Windows Runtime来检查支持
    HRESULT hr = RoInitialize(RO_INIT_MULTITHREADED);
    if (SUCCEEDED(hr) || hr == RPC_E_CHANGED_MODE) {
        qDebug() << "Windows Runtime initialized successfully, Toast notifications supported";
        return true;
    } else {
        qDebug() << "Windows Runtime initialization failed, Toast notifications not supported";
        return false;
    }
#else
    return false;
#endif
}

QString QWinUINotification::platformName()
{
#ifdef Q_OS_WIN
    return "Windows";
#elif defined(Q_OS_MACOS)
    return "macOS";
#elif defined(Q_OS_LINUX)
    return "Linux";
#else
    return "Unknown";
#endif
}

void QWinUINotification::onAutoHideTimer()
{
    hide();
}

void QWinUINotification::initializePlatform()
{
#ifdef Q_OS_WIN
    if (isSupported()) {
        initializeWindowsNotification();
    }
#endif
    
    // 设置默认应用程序信息
    if (m_applicationName.isEmpty()) {
        m_applicationName = QApplication::applicationName();
        if (m_applicationName.isEmpty()) {
            m_applicationName = "QWinUI Application";
        }
    }
}

void QWinUINotification::showWindowsNotification()
{
#ifdef Q_OS_WIN
    showWindowsToast();
#else
    showFallbackNotification();
#endif
}

void QWinUINotification::showFallbackNotification()
{
    // 创建回退通知窗口
    if (m_fallbackWindow) {
        m_fallbackWindow->deleteLater();
    }
    
    m_fallbackWindow = new QWinUIWidget();
    m_fallbackWindow->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    m_fallbackWindow->setAttribute(Qt::WA_ShowWithoutActivating);
    m_fallbackWindow->setCornerRadius(8);
    m_fallbackWindow->setFixedSize(350, 100);
    
    // 设置布局和内容
    QVBoxLayout* layout = new QVBoxLayout(m_fallbackWindow);
    layout->setContentsMargins(16, 12, 16, 12);
    layout->setSpacing(8);
    
    // 标题
    QLabel* titleLabel = new QLabel(m_title, m_fallbackWindow);
    titleLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    layout->addWidget(titleLabel);
    
    // 消息
    QLabel* messageLabel = new QLabel(m_message, m_fallbackWindow);
    messageLabel->setWordWrap(true);
    messageLabel->setStyleSheet("font-size: 12px;");
    layout->addWidget(messageLabel);
    
    // 定位到屏幕右下角
    QScreen* screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();
    int x = screenGeometry.right() - m_fallbackWindow->width() - 20;
    int y = screenGeometry.bottom() - m_fallbackWindow->height() - 20;
    m_fallbackWindow->move(x, y);
    
    // 显示动画
    m_fallbackWindow->show();
    
    // 添加阴影效果
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0, 0, 0, 80));
    shadow->setOffset(0, 4);
    m_fallbackWindow->setGraphicsEffect(shadow);
}

void QWinUINotification::cleanup()
{
#ifdef Q_OS_WIN
    if (m_toastNotification) {
        // 清理Windows通知资源
        m_toastNotification = nullptr;
    }
    if (m_toastNotifier) {
        m_toastNotifier = nullptr;
    }
#endif

    if (m_fallbackWindow) {
        m_fallbackWindow->deleteLater();
        m_fallbackWindow = nullptr;
    }
}

#ifdef Q_OS_WIN
void QWinUINotification::initializeWindowsNotification()
{
    // 初始化Windows Runtime
    HRESULT hr = RoInitialize(RO_INIT_MULTITHREADED);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) {
        qDebug() << "Failed to initialize Windows Runtime for notifications";
        return;
    }

    // 设置应用程序用户模型ID
    if (m_appUserModelId.isEmpty()) {
        m_appUserModelId = QString("QWinUI.%1.%2")
                          .arg(QApplication::organizationName().isEmpty() ? "DefaultOrg" : QApplication::organizationName())
                          .arg(QApplication::applicationName().isEmpty() ? "DefaultApp" : QApplication::applicationName());
    }

    // 注册应用程序以支持通知
    registerAppForNotifications();
}

void QWinUINotification::showWindowsToast()
{
    try {
        qDebug() << "Attempting to show Windows Toast notification";

        // 创建Toast通知管理器
        ComPtr<IToastNotificationManagerStatics> toastStatics;
        HString toastManagerClass;
        toastManagerClass.Set(RuntimeClass_Windows_UI_Notifications_ToastNotificationManager);
        HRESULT hr = RoGetActivationFactory(toastManagerClass.Get(), IID_PPV_ARGS(&toastStatics));

        if (FAILED(hr)) {
            qDebug() << "Failed to get ToastNotificationManager factory, HRESULT:" << QString::number(hr, 16);
            showFallbackNotification();
            return;
        }

        qDebug() << "ToastNotificationManager factory created successfully";

        // 创建Toast通知器
        ComPtr<IToastNotifier> toastNotifier;
        HString appId;
        appId.Set(reinterpret_cast<const wchar_t*>(m_appUserModelId.utf16()));
        qDebug() << "Using App ID:" << m_appUserModelId;
        hr = toastStatics->CreateToastNotifierWithId(appId.Get(), &toastNotifier);

        if (FAILED(hr)) {
            qDebug() << "Failed to create ToastNotifier, HRESULT:" << QString::number(hr, 16);
            showFallbackNotification();
            return;
        }

        qDebug() << "ToastNotifier created successfully";

        // 创建Toast XML模板
        QString toastXml = createToastXml();

        // 使用Toast模板而不是手动创建XML
        ComPtr<IXmlDocument> xmlDoc;
        hr = toastStatics->GetTemplateContent(ToastTemplateType_ToastText02, &xmlDoc);

        if (FAILED(hr)) {
            qDebug() << "Failed to get toast template, HRESULT:" << QString::number(hr, 16);
            showFallbackNotification();
            return;
        }

        qDebug() << "Toast template obtained successfully";

        // 获取文本节点并设置内容
        ComPtr<IXmlNodeList> textNodes;
        HString textTag;
        textTag.Set(L"text");
        hr = xmlDoc->GetElementsByTagName(textTag.Get(), &textNodes);

        if (SUCCEEDED(hr)) {
            ComPtr<IXmlNode> titleNode;
            hr = textNodes->Item(0, &titleNode);
            if (SUCCEEDED(hr) && titleNode) {
                HString titleText;
                titleText.Set(reinterpret_cast<const wchar_t*>(m_title.utf16()));
                ComPtr<IXmlText> titleTextNode;
                hr = xmlDoc->CreateTextNode(titleText.Get(), &titleTextNode);
                if (SUCCEEDED(hr)) {
                    ComPtr<IXmlNode> titleTextNodeAsNode;
                    hr = titleTextNode.As(&titleTextNodeAsNode);
                    if (SUCCEEDED(hr)) {
                        ComPtr<IXmlNode> appendedChild;
                        hr = titleNode->AppendChild(titleTextNodeAsNode.Get(), &appendedChild);
                    }
                }
            }

            ComPtr<IXmlNode> messageNode;
            hr = textNodes->Item(1, &messageNode);
            if (SUCCEEDED(hr) && messageNode) {
                HString messageText;
                messageText.Set(reinterpret_cast<const wchar_t*>(m_message.utf16()));
                ComPtr<IXmlText> messageTextNode;
                hr = xmlDoc->CreateTextNode(messageText.Get(), &messageTextNode);
                if (SUCCEEDED(hr)) {
                    ComPtr<IXmlNode> messageTextNodeAsNode;
                    hr = messageTextNode.As(&messageTextNodeAsNode);
                    if (SUCCEEDED(hr)) {
                        ComPtr<IXmlNode> appendedChild;
                        hr = messageNode->AppendChild(messageTextNodeAsNode.Get(), &appendedChild);
                    }
                }
            }
        }

        // 创建Toast通知
        ComPtr<IToastNotificationFactory> toastFactory;
        HString toastClass;
        toastClass.Set(RuntimeClass_Windows_UI_Notifications_ToastNotification);
        hr = RoGetActivationFactory(toastClass.Get(), IID_PPV_ARGS(&toastFactory));

        if (FAILED(hr)) {
            qDebug() << "Failed to get ToastNotification factory";
            showFallbackNotification();
            return;
        }

        ComPtr<IToastNotification> toast;
        hr = toastFactory->CreateToastNotification(xmlDoc.Get(), &toast);

        if (FAILED(hr)) {
            qDebug() << "Failed to create ToastNotification";
            showFallbackNotification();
            return;
        }

        // 显示Toast通知
        hr = toastNotifier->Show(toast.Get());

        if (FAILED(hr)) {
            qDebug() << "Failed to show ToastNotification";
            showFallbackNotification();
            return;
        }

        qDebug() << "Windows Toast notification shown successfully";

    } catch (...) {
        qDebug() << "Exception occurred while showing Windows Toast notification";
        showFallbackNotification();
    }
}

QString QWinUINotification::createToastXml()
{
    QString iconPath;
    switch (m_type) {
    case QWinUINotificationType::Info:
        iconPath = "ms-appx:///Assets/info.png";
        break;
    case QWinUINotificationType::Warning:
        iconPath = "ms-appx:///Assets/warning.png";
        break;
    case QWinUINotificationType::Error:
        iconPath = "ms-appx:///Assets/error.png";
        break;
    case QWinUINotificationType::Success:
        iconPath = "ms-appx:///Assets/success.png";
        break;
    }

    QString xml = QString(
        "<toast>"
        "  <visual>"
        "    <binding template=\"ToastGeneric\">"
        "      <text>%1</text>"
        "      <text>%2</text>"
        "    </binding>"
        "  </visual>"
        "</toast>")
        .arg(m_title.toHtmlEscaped())
        .arg(m_message.toHtmlEscaped());

    return xml;
}

bool QWinUINotification::registerAppForNotifications()
{
    // 在实际应用中，这里应该注册应用程序以支持Toast通知
    // 这通常需要在应用程序安装时完成，或者使用快捷方式注册
    return true;
}
#endif

QT_END_NAMESPACE
