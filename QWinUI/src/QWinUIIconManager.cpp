#include "QWinUI/QWinUIIconManager.h"
#include <QApplication>
#include <QDirIterator>
#include <QFileInfo>
#include <QPainter>
#include <QDateTime>
#include <QDebug>
#include <QStandardPaths>

QT_BEGIN_NAMESPACE

QWinUIIconManager* QWinUIIconManager::s_instance = nullptr;
QMutex QWinUIIconManager::s_mutex;
const QString QWinUIIconManager::DEFAULT_ICON_DIRECTORY = "Icon";
const QString QWinUIIconManager::RESOURCE_ICON_PREFIX = ":/icons";

QWinUIIconManager* QWinUIIconManager::getInstance()
{
    QMutexLocker locker(&s_mutex);
    if (!s_instance) {
        s_instance = new QWinUIIconManager();

        // 自动加载资源图标（只加载一次）
        static bool resourcesLoaded = false;
        if (!resourcesLoaded) {
            s_instance->loadIconsFromResources();
            resourcesLoaded = true;
        }

        // 自动加载默认图标目录（如果存在）
        QString defaultPath = QApplication::applicationDirPath() + "/" + DEFAULT_ICON_DIRECTORY;
        if (QDir(defaultPath).exists()) {
            s_instance->loadIconsFromDirectory(defaultPath);
        }
    }
    return s_instance;
}

QWinUIIconManager::QWinUIIconManager(QObject* parent)
    : QObject(parent)
    , m_cacheLimit(100)
{
}

QWinUIIconManager::~QWinUIIconManager()
{
    clearCache();
}

bool QWinUIIconManager::loadIconsFromDirectory(const QString& directory)
{
    QDir iconDir(directory);
    if (!iconDir.exists()) {
        qWarning() << "Icon directory does not exist:" << directory;
        return false;
    }
    
    QDirIterator iterator(directory, QStringList() << "*.svg", QDir::Files, QDirIterator::Subdirectories);
    int loadedCount = 0;
    
    while (iterator.hasNext()) {
        QString filePath = iterator.next();
        QFileInfo fileInfo(filePath);
        QString iconName = getIconNameFromPath(filePath);
        
        // 从目录结构推断分类
        QString relativePath = iconDir.relativeFilePath(filePath);
        QString category = QFileInfo(relativePath).dir().dirName();
        if (category == ".") {
            category = "General";
        }
        
        if (registerIcon(iconName, filePath, category)) {
            loadedCount++;
        }
    }
    
    qDebug() << "Loaded" << loadedCount << "icons from" << directory;
    return loadedCount > 0;
}

bool QWinUIIconManager::loadIconsFromResources(const QString& resourcePrefix)
{
    // 只加载测试需要的几个基础图标，避免启动卡顿
    QStringList basicIcons = {
        "solid/house",
        "solid/heart",
        "solid/arrows-rotate"
    };

    int loadedCount = 0;
    for (const QString& iconPath : basicIcons) {
        QString resourcePath = resourcePrefix + "/" + iconPath;
        QString iconName = iconPath;

        // 从路径推断分类
        QStringList parts = iconPath.split('/');
        QString category = parts.size() > 1 ? parts[0] : "general";

        if (registerResourceIcon(iconName, resourcePath, category)) {
            loadedCount++;
        }
    }

    qDebug() << "Loaded" << loadedCount << "basic icons from resources";
    return loadedCount > 0;
}

bool QWinUIIconManager::registerIcon(const QString& name, const QString& filePath, const QString& category)
{
    if (name.isEmpty() || filePath.isEmpty()) {
        return false;
    }
    
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists() || !fileInfo.isReadable()) {
        qWarning() << "Icon file does not exist or is not readable:" << filePath;
        return false;
    }
    
    QWinUIIconInfo iconInfo(name, filePath, category);
    
    // 尝试加载SVG以获取原始尺寸
    QSvgRenderer renderer(filePath);
    if (renderer.isValid()) {
        iconInfo.originalSize = renderer.defaultSize();
    } else {
        qWarning() << "Invalid SVG file:" << filePath;
        return false;
    }
    
    m_icons[name] = iconInfo;
    emit iconRegistered(name);
    
    return true;
}

bool QWinUIIconManager::registerResourceIcon(const QString& name, const QString& resourcePath, const QString& category)
{
    if (name.isEmpty() || resourcePath.isEmpty()) {
        qWarning() << "Invalid icon name or path";
        return false;
    }

    // 检查资源是否存在
    QFile resourceFile(resourcePath);
    if (!resourceFile.exists()) {
        qWarning() << "Resource icon does not exist:" << resourcePath;
        return false;
    }

    QWinUIIconInfo iconInfo(name, resourcePath, category);

    // 尝试加载SVG以获取原始尺寸，但不要因为失败而阻止注册
    try {
        QSvgRenderer renderer(resourcePath);
        if (renderer.isValid()) {
            iconInfo.originalSize = renderer.defaultSize();
        } else {
            qWarning() << "Invalid SVG resource, using default size:" << resourcePath;
            iconInfo.originalSize = QSize(24, 24); // 默认尺寸
        }
    } catch (...) {
        qWarning() << "Exception loading SVG, using default size:" << resourcePath;
        iconInfo.originalSize = QSize(24, 24);
    }

    m_icons[name] = iconInfo;
    emit iconRegistered(name);

    return true;
}

void QWinUIIconManager::unregisterIcon(const QString& name)
{
    if (m_icons.remove(name)) {
        clearCache(name);
        emit iconUnregistered(name);
    }
}

bool QWinUIIconManager::hasIcon(const QString& name) const
{
    return m_icons.contains(name);
}

QWinUIIconInfo QWinUIIconManager::getIconInfo(const QString& name) const
{
    return m_icons.value(name);
}

QStringList QWinUIIconManager::getIconNames() const
{
    return m_icons.keys();
}

QStringList QWinUIIconManager::getIconsByCategory(const QString& category) const
{
    QStringList result;
    for (auto it = m_icons.begin(); it != m_icons.end(); ++it) {
        if (it.value().category == category) {
            result.append(it.key());
        }
    }
    return result;
}

QStringList QWinUIIconManager::getIconsByTag(const QString& tag) const
{
    QStringList result;
    for (auto it = m_icons.begin(); it != m_icons.end(); ++it) {
        if (it.value().tags.contains(tag)) {
            result.append(it.key());
        }
    }
    return result;
}

QStringList QWinUIIconManager::getCategories() const
{
    QStringList categories;
    for (auto it = m_icons.begin(); it != m_icons.end(); ++it) {
        const QString& category = it.value().category;
        if (!category.isEmpty() && !categories.contains(category)) {
            categories.append(category);
        }
    }
    categories.sort();
    return categories;
}

QPixmap QWinUIIconManager::getIcon(const QString& name, const QSize& size, const QColor& color) const
{
    if (!hasIcon(name)) {
        qWarning() << "Icon not found:" << name;
        return QPixmap();
    }
    
    QString cacheKey = getCacheKey(name, size, color);
    
    QMutexLocker locker(&m_cacheMutex);
    
    // 检查缓存
    if (m_cache.contains(cacheKey)) {
        QWinUIIconCacheItem* item = m_cache[cacheKey];
        item->lastAccessed = QDateTime::currentMSecsSinceEpoch();
        return item->pixmap;
    }
    
    // 创建新的缓存项
    QWinUIIconCacheItem* item = new QWinUIIconCacheItem();
    const QWinUIIconInfo& iconInfo = m_icons[name];
    
    item->renderer = new QSvgRenderer(iconInfo.filePath);
    if (!item->renderer->isValid()) {
        delete item;
        return QPixmap();
    }
    
    // 渲染图标
    QSize renderSize = size.isValid() ? size : QSize(16, 16);
    item->pixmap = QPixmap(renderSize);
    item->pixmap.fill(Qt::transparent);
    
    QPainter painter(&item->pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    item->renderer->render(&painter);
    
    // 应用颜色
    if (color.isValid()) {
        item->pixmap = colorizePixmap(item->pixmap, color);
    }
    
    item->lastColor = color;
    item->lastSize = renderSize;
    item->lastAccessed = QDateTime::currentMSecsSinceEpoch();
    
    // 添加到缓存
    m_cache[cacheKey] = item;
    
    // 清理缓存
    if (m_cache.size() > m_cacheLimit) {
        cleanupCache();
    }
    
    return item->pixmap;
}

QSvgRenderer* QWinUIIconManager::getRenderer(const QString& name) const
{
    if (!hasIcon(name)) {
        return nullptr;
    }
    
    const QWinUIIconInfo& iconInfo = m_icons[name];
    return new QSvgRenderer(iconInfo.filePath);
}

void QWinUIIconManager::clearCache()
{
    QMutexLocker locker(&m_cacheMutex);
    qDeleteAll(m_cache);
    m_cache.clear();
    emit cacheCleared();
}

void QWinUIIconManager::clearCache(const QString& name)
{
    QMutexLocker locker(&m_cacheMutex);
    auto it = m_cache.begin();
    while (it != m_cache.end()) {
        if (it.key().startsWith(name + "|")) {
            delete it.value();
            it = m_cache.erase(it);
        } else {
            ++it;
        }
    }
}

void QWinUIIconManager::setCacheLimit(int maxItems)
{
    m_cacheLimit = qMax(10, maxItems);
    if (m_cache.size() > m_cacheLimit) {
        cleanupCache();
    }
}

int QWinUIIconManager::getCacheSize() const
{
    QMutexLocker locker(&m_cacheMutex);
    return m_cache.size();
}

QPixmap QWinUIIconManager::colorizePixmap(const QPixmap& pixmap, const QColor& color)
{
    if (pixmap.isNull() || !color.isValid()) {
        return pixmap;
    }
    
    QPixmap colorized = pixmap;
    QPainter painter(&colorized);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(colorized.rect(), color);
    return colorized;
}

QString QWinUIIconManager::getIconNameFromPath(const QString& filePath)
{
    QFileInfo fileInfo(filePath);
    return fileInfo.baseName();
}

void QWinUIIconManager::cleanupCache() const
{
    if (m_cache.size() <= m_cacheLimit) {
        return;
    }
    
    // 按最后访问时间排序，删除最旧的项
    QList<QPair<qint64, QString>> accessTimes;
    for (auto it = m_cache.begin(); it != m_cache.end(); ++it) {
        accessTimes.append(qMakePair(it.value()->lastAccessed, it.key()));
    }
    
    std::sort(accessTimes.begin(), accessTimes.end());
    
    int itemsToRemove = m_cache.size() - m_cacheLimit + 10; // 多删除一些以避免频繁清理
    for (int i = 0; i < itemsToRemove && i < accessTimes.size(); ++i) {
        const QString& key = accessTimes[i].second;
        delete m_cache[key];
        m_cache.remove(key);
    }
}

QString QWinUIIconManager::getCacheKey(const QString& name, const QSize& size, const QColor& color) const
{
    return QString("%1|%2x%3|%4").arg(name).arg(size.width()).arg(size.height()).arg(color.name());
}

QT_END_NAMESPACE
