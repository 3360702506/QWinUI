#pragma once

#include "QWinUI/QWinUIGlobal.h"
#include <QObject>
#include <QSvgRenderer>
#include <QPixmap>
#include <QColor>
#include <QHash>
#include <QStringList>
#include <QMutex>
#include <QDir>

QT_BEGIN_NAMESPACE

// 图标信息结构
struct QWinUIIconInfo {
    QString name;
    QString filePath;
    QString category;
    QStringList tags;
    QSize originalSize;
    
    QWinUIIconInfo() = default;
    QWinUIIconInfo(const QString& iconName, const QString& path, const QString& cat = QString())
        : name(iconName), filePath(path), category(cat) {}
};

// 图标缓存项
struct QWinUIIconCacheItem {
    QSvgRenderer* renderer;
    QPixmap pixmap;
    QColor lastColor;
    QSize lastSize;
    qint64 lastAccessed;
    
    QWinUIIconCacheItem() : renderer(nullptr), lastAccessed(0) {}
    ~QWinUIIconCacheItem() { delete renderer; }
};

class QWINUI_EXPORT QWinUIIconManager : public QObject
{
    Q_OBJECT

public:
    static QWinUIIconManager* getInstance();
    
    // 图标加载和管理
    bool loadIconsFromDirectory(const QString& directory);
    bool loadIconsFromResources(const QString& resourcePrefix = ":/icons");
    bool registerIcon(const QString& name, const QString& filePath, const QString& category = QString());
    bool registerResourceIcon(const QString& name, const QString& resourcePath, const QString& category = QString());
    void unregisterIcon(const QString& name);
    
    // 图标查询
    bool hasIcon(const QString& name) const;
    QWinUIIconInfo getIconInfo(const QString& name) const;
    QStringList getIconNames() const;
    QStringList getIconsByCategory(const QString& category) const;
    QStringList getIconsByTag(const QString& tag) const;
    QStringList getCategories() const;
    
    // 图标渲染
    QPixmap getIcon(const QString& name, const QSize& size = QSize(16, 16), 
                   const QColor& color = QColor()) const;
    QSvgRenderer* getRenderer(const QString& name) const;
    
    // 缓存管理
    void clearCache();
    void clearCache(const QString& name);
    void setCacheLimit(int maxItems);
    int getCacheSize() const;
    
    // 工具方法
    static QPixmap colorizePixmap(const QPixmap& pixmap, const QColor& color);
    static QString getIconNameFromPath(const QString& filePath);

signals:
    void iconRegistered(const QString& name);
    void iconUnregistered(const QString& name);
    void cacheCleared();

private:
    explicit QWinUIIconManager(QObject* parent = nullptr);
    ~QWinUIIconManager();
    
    void cleanupCache() const;
    QString getCacheKey(const QString& name, const QSize& size, const QColor& color) const;
    
    static QWinUIIconManager* s_instance;
    static QMutex s_mutex;
    
    QHash<QString, QWinUIIconInfo> m_icons;
    mutable QHash<QString, QWinUIIconCacheItem*> m_cache;
    mutable QMutex m_cacheMutex;
    int m_cacheLimit;
    
    // 默认图标目录和资源路径
    static const QString DEFAULT_ICON_DIRECTORY;
    static const QString RESOURCE_ICON_PREFIX;
};

QT_END_NAMESPACE
