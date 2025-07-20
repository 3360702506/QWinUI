#pragma once

#include "QWinUI/QWinUIWidget.h"
#include <QObject>
#include <QString>
#include <QPixmap>
#include <QTimer>

QT_BEGIN_NAMESPACE

// 通知类型枚举
enum class QWinUINotificationType {
    Info,
    Warning,
    Error,
    Success
};

// 通知持续时间枚举
enum class QWinUINotificationDuration {
    Short = 3000,    // 3秒
    Medium = 5000,   // 5秒
    Long = 8000,     // 8秒
    Persistent = 0   // 持续显示，需要手动关闭
};

// 通知按钮结构
struct QWinUINotificationAction {
    QString text;
    QString actionId;
    bool isDefault = false;
};

class QWINUI_EXPORT QWinUINotification : public QObject
{
    Q_OBJECT

public:
    explicit QWinUINotification(QObject* parent = nullptr);
    ~QWinUINotification();

    // 静态便捷方法
    static void showInfo(const QString& title, const QString& message, 
                        QWinUINotificationDuration duration = QWinUINotificationDuration::Medium);
    static void showWarning(const QString& title, const QString& message,
                           QWinUINotificationDuration duration = QWinUINotificationDuration::Medium);
    static void showError(const QString& title, const QString& message,
                         QWinUINotificationDuration duration = QWinUINotificationDuration::Long);
    static void showSuccess(const QString& title, const QString& message,
                           QWinUINotificationDuration duration = QWinUINotificationDuration::Medium);

    // 高级通知方法
    static void showNotification(const QString& title, 
                                const QString& message,
                                QWinUINotificationType type = QWinUINotificationType::Info,
                                QWinUINotificationDuration duration = QWinUINotificationDuration::Medium,
                                const QPixmap& icon = QPixmap(),
                                const QList<QWinUINotificationAction>& actions = {});

    // 实例方法
    void setTitle(const QString& title);
    void setMessage(const QString& message);
    void setType(QWinUINotificationType type);
    void setDuration(QWinUINotificationDuration duration);
    void setIcon(const QPixmap& icon);
    void setActions(const QList<QWinUINotificationAction>& actions);
    void setApplicationName(const QString& appName);
    void setApplicationIcon(const QPixmap& appIcon);

    // 显示和隐藏
    void show();
    void hide();

    // 平台支持检查
    static bool isSupported();
    static QString platformName();

signals:
    void notificationClicked();
    void actionTriggered(const QString& actionId);
    void notificationDismissed();
    void notificationFailed(const QString& error);

private slots:
    void onAutoHideTimer();

private:
    void initializePlatform();
    void showWindowsNotification();
    void showFallbackNotification();
    void cleanup();

    // Windows特定方法
#ifdef Q_OS_WIN
    void initializeWindowsNotification();
    void showWindowsToast();
    bool registerAppForNotifications();
    QString createToastXml();
#endif

    // 成员变量
    QString m_title;
    QString m_message;
    QWinUINotificationType m_type;
    QWinUINotificationDuration m_duration;
    QPixmap m_icon;
    QList<QWinUINotificationAction> m_actions;
    QString m_applicationName;
    QPixmap m_applicationIcon;
    
    QTimer* m_autoHideTimer;
    bool m_isShowing;
    
    // 平台特定数据
#ifdef Q_OS_WIN
    void* m_toastNotifier;  // IToastNotifier*
    void* m_toastNotification; // IToastNotification*
    QString m_appUserModelId;
#endif

    // 回退通知窗口
    QWinUIWidget* m_fallbackWindow;
    
    static QWinUINotification* s_instance;
};

QT_END_NAMESPACE
