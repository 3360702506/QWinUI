#include "QWinUI/Controls/QWinUIIcon.h"
#include "QWinUI/QWinUIIconManager.h"
#include "QWinUI/QWinUITheme.h"
#include <QPainter>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QtMath>
#include <QDebug>

QT_BEGIN_NAMESPACE

QWinUIIcon::QWinUIIcon(QWidget* parent)
    : QWinUIWidget(parent)
    , m_iconSize(DEFAULT_ICON_SIZE, DEFAULT_ICON_SIZE)
    , m_rotation(DEFAULT_ROTATION)
    , m_iconOpacity(DEFAULT_OPACITY)
    , m_useThemeColor(true)
    , m_smoothScaling(true)
    , m_isPressed(false)
    , m_isHovered(false)
    , m_isDynamicSvg(false)
    , m_isFontIcon(false)
    , m_rotationAnimation(nullptr)
    , m_opacityAnimation(nullptr)
    , m_spinAnimation(nullptr)
{
    initializeComponent();
}

QWinUIIcon::QWinUIIcon(const QString& iconName, QWidget* parent)
    : QWinUIIcon(parent)
{
    setIconName(iconName);
}

QWinUIIcon::QWinUIIcon(const QString& iconName, const QSize& size, QWidget* parent)
    : QWinUIIcon(iconName, parent)
{
    setIconSize(size);
}

QWinUIIcon::~QWinUIIcon()
{
    if (m_rotationAnimation) {
        m_rotationAnimation->stop();
    }
    if (m_opacityAnimation) {
        m_opacityAnimation->stop();
    }
    if (m_spinAnimation) {
        m_spinAnimation->stop();
    }
}

void QWinUIIcon::initializeComponent()
{
    setFixedSize(m_iconSize);
    setCursor(Qt::PointingHandCursor);
    
    // 创建旋转动画
    m_rotationAnimation = new QPropertyAnimation(this, "rotation", this);
    m_rotationAnimation->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_rotationAnimation, &QPropertyAnimation::finished, 
            this, &QWinUIIcon::onRotationAnimationFinished);
    
    // 创建透明度动画
    m_opacityAnimation = new QPropertyAnimation(this, "opacity", this);
    m_opacityAnimation->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_opacityAnimation, &QPropertyAnimation::finished,
            this, &QWinUIIcon::onOpacityAnimationFinished);
    
    // 创建旋转动画
    m_spinAnimation = new QPropertyAnimation(this, "rotation", this);
    m_spinAnimation->setEasingCurve(QEasingCurve::Linear);
    m_spinAnimation->setLoopCount(-1); // 无限循环
    
    updateThemeColor();
}

QString QWinUIIcon::iconName() const
{
    return m_iconName;
}

void QWinUIIcon::setIconName(const QString& name)
{
    if (m_iconName == name) {
        return;
    }

    m_iconName = name;
    m_isDynamicSvg = false;  // 重置动态SVG标志
    m_isFontIcon = false;    // 重置字体图标标志
    m_dynamicSvgData.clear();
    updateIcon();
    emit iconNameChanged(m_iconName);

    // 更新工具提示
    if (!m_iconName.isEmpty()) {
        setToolTipText(QString("图标: %1").arg(m_iconName));
    }
}

QSize QWinUIIcon::iconSize() const
{
    return m_iconSize;
}

void QWinUIIcon::setIconSize(const QSize& size)
{
    QSize newSize = size.isValid() ? size : QSize(DEFAULT_ICON_SIZE, DEFAULT_ICON_SIZE);
    if (m_iconSize == newSize) {
        return;
    }
    
    m_iconSize = newSize;
    setFixedSize(m_iconSize);
    updateIcon();
    emit iconSizeChanged(m_iconSize);
}

void QWinUIIcon::setIconSize(int width, int height)
{
    setIconSize(QSize(width, height));
}

bool QWinUIIcon::loadSvgFromFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open SVG file:" << filePath;
        return false;
    }

    QByteArray svgData = file.readAll();
    return loadSvgFromData(svgData);
}

bool QWinUIIcon::loadSvgFromData(const QByteArray& svgData)
{
    if (svgData.isEmpty()) {
        qWarning() << "Empty SVG data";
        return false;
    }

    // 测试SVG数据是否有效
    QSvgRenderer testRenderer(svgData);
    if (!testRenderer.isValid()) {
        qWarning() << "Invalid SVG data";
        return false;
    }

    m_dynamicSvgData = svgData;
    m_isDynamicSvg = true;
    m_iconName = "dynamic_svg";  // 设置一个标识名称

    updateIcon();
    emit iconNameChanged(m_iconName);

    return true;
}

bool QWinUIIcon::loadSvgFromString(const QString& svgContent)
{
    return loadSvgFromData(svgContent.toUtf8());
}

void QWinUIIcon::setFontIcon(const QString& fontFamily, QChar iconChar)
{
    m_fontFamily = fontFamily;
    m_fontIconChar = iconChar;
    m_isFontIcon = true;
    m_isDynamicSvg = false;
    m_dynamicSvgData.clear();
    m_iconName = QString("font_%1_%2").arg(fontFamily).arg(iconChar.unicode());

    updateIcon();
    emit iconNameChanged(m_iconName);
}

void QWinUIIcon::setFluentIcon(QChar iconChar)
{
    setFontIcon("Segoe Fluent Icons", iconChar);
}

void QWinUIIcon::setFluentIcon(const QString& iconName)
{
    // 这里可以添加图标名称到字符的映射
    QChar iconChar = getFluentIconChar(iconName);
    if (!iconChar.isNull()) {
        setFluentIcon(iconChar);
    } else {
        qWarning() << "Unknown Fluent icon name:" << iconName;
    }
}

QColor QWinUIIcon::iconColor() const
{
    return m_iconColor;
}

void QWinUIIcon::setIconColor(const QColor& color)
{
    if (m_iconColor == color) {
        return;
    }
    
    m_iconColor = color;
    m_useThemeColor = false;
    updateIcon();
    emit iconColorChanged(m_iconColor);
}

double QWinUIIcon::rotation() const
{
    return m_rotation;
}

void QWinUIIcon::setRotation(double angle)
{
    // 标准化角度到 0-360 范围
    while (angle < 0) angle += 360;
    while (angle >= 360) angle -= 360;
    
    if (qFuzzyCompare(m_rotation, angle)) {
        return;
    }
    
    m_rotation = angle;
    update();
    emit rotationChanged(m_rotation);
}

double QWinUIIcon::opacity() const
{
    return m_iconOpacity;
}

void QWinUIIcon::setOpacity(double opacity)
{
    double newOpacity = qBound(0.0, opacity, 1.0);
    if (qFuzzyCompare(m_iconOpacity, newOpacity)) {
        return;
    }
    
    m_iconOpacity = newOpacity;
    update();
    emit opacityChanged(m_iconOpacity);
}

bool QWinUIIcon::useThemeColor() const
{
    return m_useThemeColor;
}

void QWinUIIcon::setUseThemeColor(bool use)
{
    if (m_useThemeColor == use) {
        return;
    }
    
    m_useThemeColor = use;
    if (m_useThemeColor) {
        updateThemeColor();
    }
    updateIcon();
    emit useThemeColorChanged(m_useThemeColor);
}

bool QWinUIIcon::smoothScaling() const
{
    return m_smoothScaling;
}

void QWinUIIcon::setSmoothScaling(bool smooth)
{
    if (m_smoothScaling == smooth) {
        return;
    }
    
    m_smoothScaling = smooth;
    updateIcon();
    emit smoothScalingChanged(m_smoothScaling);
}

void QWinUIIcon::animateRotation(double targetAngle, int duration)
{
    if (m_rotationAnimation->state() == QPropertyAnimation::Running) {
        m_rotationAnimation->stop();
    }
    
    m_rotationAnimation->setDuration(duration);
    m_rotationAnimation->setStartValue(m_rotation);
    m_rotationAnimation->setEndValue(targetAngle);
    m_rotationAnimation->start();
}

void QWinUIIcon::animateOpacity(double targetOpacity, int duration)
{
    if (m_opacityAnimation->state() == QPropertyAnimation::Running) {
        m_opacityAnimation->stop();
    }
    
    m_opacityAnimation->setDuration(duration);
    m_opacityAnimation->setStartValue(m_iconOpacity);
    m_opacityAnimation->setEndValue(qBound(0.0, targetOpacity, 1.0));
    m_opacityAnimation->start();
}

void QWinUIIcon::startSpinAnimation(int duration)
{
    if (m_spinAnimation->state() == QPropertyAnimation::Running) {
        return;
    }
    
    m_spinAnimation->setDuration(duration);
    m_spinAnimation->setStartValue(0.0);
    m_spinAnimation->setEndValue(360.0);
    m_spinAnimation->start();
}

void QWinUIIcon::stopSpinAnimation()
{
    if (m_spinAnimation->state() == QPropertyAnimation::Running) {
        m_spinAnimation->stop();
        setRotation(0.0);
    }
}

bool QWinUIIcon::isValid() const
{
    if (m_isFontIcon) {
        return !m_fontFamily.isEmpty() && !m_fontIconChar.isNull();
    }

    if (m_isDynamicSvg) {
        return !m_dynamicSvgData.isEmpty();
    }

    if (m_iconName.isEmpty()) {
        return false;
    }

    // 直接检查资源文件是否存在，不依赖图标管理器
    QString resourcePath = getResourcePath(m_iconName);
    QFile file(resourcePath);
    return file.exists();
}

QPixmap QWinUIIcon::getPixmap() const
{
    if (!isValid()) {
        return QPixmap();
    }

    // 字体图标渲染
    if (m_isFontIcon) {
        return renderFontIcon();
    }

    QSvgRenderer renderer;
    QString debugPath = "unknown";

    if (m_isDynamicSvg) {
        // 使用动态SVG数据
        qDebug() << "Loading dynamic SVG data, size:" << m_dynamicSvgData.size();
        renderer.load(m_dynamicSvgData);
        debugPath = "dynamic_svg_data";
    } else {
        // 从资源加载SVG
        QString resourcePath = getResourcePath(m_iconName);
        qDebug() << "Loading icon:" << m_iconName << "from path:" << resourcePath;
        renderer.load(resourcePath);
        debugPath = resourcePath;
    }

    if (!renderer.isValid()) {
        qWarning() << "Failed to load SVG from:" << debugPath;
        // 创建一个调试用的彩色方块
        QPixmap debugPixmap(m_iconSize);
        debugPixmap.fill(Qt::red);
        return debugPixmap;
    }

    // 创建指定尺寸的像素图
    QPixmap pixmap(m_iconSize);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    // 先渲染SVG
    renderer.render(&painter);

    // 如果需要应用颜色，使用不同的方法
    QColor effectiveColor = getEffectiveIconColor();
    if (effectiveColor.isValid() && effectiveColor != Qt::black) {
        // 创建一个新的像素图用于着色
        QPixmap coloredPixmap(m_iconSize);
        coloredPixmap.fill(Qt::transparent);

        QPainter colorPainter(&coloredPixmap);
        colorPainter.setRenderHint(QPainter::Antialiasing);

        // 使用源图像作为蒙版
        colorPainter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        colorPainter.drawPixmap(0, 0, pixmap);

        colorPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
        colorPainter.fillRect(coloredPixmap.rect(), effectiveColor);

        return coloredPixmap;
    }

    return pixmap;
}

QSvgRenderer* QWinUIIcon::getRenderer() const
{
    if (!isValid()) {
        return nullptr;
    }

    QSvgRenderer* renderer = new QSvgRenderer();

    if (m_isDynamicSvg) {
        // 使用动态SVG数据
        renderer->load(m_dynamicSvgData);
    } else {
        // 从资源加载SVG
        QString resourcePath = getResourcePath(m_iconName);
        renderer->load(resourcePath);
    }

    if (!renderer->isValid()) {
        delete renderer;
        return nullptr;
    }

    return renderer;
}

QSize QWinUIIcon::sizeHint() const
{
    return m_iconSize;
}

QSize QWinUIIcon::minimumSizeHint() const
{
    return QSize(8, 8);
}

void QWinUIIcon::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    
    if (!isValid()) {
        return;
    }
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, m_smoothScaling);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, m_smoothScaling);
    
    // 应用透明度
    painter.setOpacity(m_iconOpacity);
    
    // 应用变换
    QTransform transform = getTransform();
    painter.setTransform(transform);
    
    // 获取图标
    QString cacheKey = QString("%1_%2x%3_%4_%5").arg(m_iconName)
                      .arg(m_iconSize.width()).arg(m_iconSize.height())
                      .arg(getEffectiveIconColor().name()).arg(m_rotation);
    
    if (m_lastCacheKey != cacheKey || m_cachedPixmap.isNull()) {
        m_cachedPixmap = getPixmap();
        m_lastCacheKey = cacheKey;
    }
    
    if (!m_cachedPixmap.isNull()) {
        QRect targetRect = rect();
        targetRect.moveCenter(rect().center());
        painter.drawPixmap(targetRect, m_cachedPixmap);
    }
}

void QWinUIIcon::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_isPressed = true;
        if (m_useThemeColor) {
            updateThemeColor();
        }
        update();
    }
    QWinUIWidget::mousePressEvent(event);
}

void QWinUIIcon::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_isPressed) {
        m_isPressed = false;
        if (m_useThemeColor) {
            updateThemeColor();
        }
        if (rect().contains(event->pos())) {
            emit clicked();
        }
        update();
    }
    QWinUIWidget::mouseReleaseEvent(event);
}

void QWinUIIcon::enterEvent(QEnterEvent* event)
{
    m_isHovered = true;
    if (m_useThemeColor) {
        updateThemeColor();
    }
    update();
    QWinUIWidget::enterEvent(event);
}

void QWinUIIcon::leaveEvent(QEvent* event)
{
    m_isHovered = false;
    m_isPressed = false;
    if (m_useThemeColor) {
        updateThemeColor();
    }
    update();
    QWinUIWidget::leaveEvent(event);
}

void QWinUIIcon::onThemeChanged()
{
    if (m_useThemeColor) {
        updateThemeColor();
        updateIcon();
    }
    QWinUIWidget::onThemeChanged();
}

void QWinUIIcon::onRotationAnimationFinished()
{
    // 旋转动画完成后的处理
}

void QWinUIIcon::onOpacityAnimationFinished()
{
    // 透明度动画完成后的处理
}

void QWinUIIcon::updateIcon()
{
    m_cachedPixmap = QPixmap(); // 清除缓存
    m_lastCacheKey.clear();
    update();
}

QString QWinUIIcon::getResourcePath(const QString& iconName) const
{
    if (iconName.startsWith(":/")) {
        // 已经是完整的资源路径
        return iconName;
    } else if (iconName.contains("/")) {
        // 相对路径，添加资源前缀
        return ":/icons/" + iconName;
    } else {
        // 简单名称，尝试在solid目录中查找
        return ":/icons/solid/" + iconName;
    }
}

QPixmap QWinUIIcon::renderFontIcon() const
{
    QPixmap pixmap(m_iconSize);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    // 设置字体
    QFont font(m_fontFamily);
    font.setPixelSize(qMin(m_iconSize.width(), m_iconSize.height()));
    painter.setFont(font);

    // 设置颜色
    QColor effectiveColor = getEffectiveIconColor();
    painter.setPen(effectiveColor);

    // 绘制字符
    QRect textRect = pixmap.rect();
    painter.drawText(textRect, Qt::AlignCenter, QString(m_fontIconChar));

    return pixmap;
}

QChar QWinUIIcon::getFluentIconChar(const QString& iconName) const
{
    // Segoe Fluent Icons 字符映射表
    static QHash<QString, QChar> fluentIconMap = {
        // 常用图标
        {"home", QChar(0xE80F)},
        {"settings", QChar(0xE713)},
        {"search", QChar(0xE721)},
        {"add", QChar(0xE710)},
        {"remove", QChar(0xE738)},
        {"delete", QChar(0xE74D)},
        {"edit", QChar(0xE70F)},
        {"save", QChar(0xE74E)},
        {"open", QChar(0xE8E5)},
        {"close", QChar(0xE8BB)},
        {"back", QChar(0xE72B)},
        {"forward", QChar(0xE72A)},
        {"refresh", QChar(0xE72C)},
        {"download", QChar(0xE896)},
        {"upload", QChar(0xE898)},
        {"copy", QChar(0xE8C8)},
        {"paste", QChar(0xE77F)},
        {"cut", QChar(0xE8C6)},
        {"undo", QChar(0xE7A7)},
        {"redo", QChar(0xE7A6)},
        {"play", QChar(0xE768)},
        {"pause", QChar(0xE769)},
        {"stop", QChar(0xE71A)},
        {"volume", QChar(0xE767)},
        {"mute", QChar(0xE74F)},
        {"favorite", QChar(0xE734)},
        {"heart", QChar(0xE734)},
        {"star", QChar(0xE735)},
        {"bookmark", QChar(0xE8A4)},
        {"folder", QChar(0xE8B7)},
        {"file", QChar(0xE8A5)},
        {"document", QChar(0xE8A5)},
        {"mail", QChar(0xE715)},
        {"message", QChar(0xE8BD)},
        {"phone", QChar(0xE717)},
        {"contact", QChar(0xE77B)},
        {"calendar", QChar(0xE787)},
        {"clock", QChar(0xE823)},
        {"camera", QChar(0xE722)},
        {"picture", QChar(0xE91B)},
        {"video", QChar(0xE714)},
        {"music", QChar(0xE8D6)},
        {"microphone", QChar(0xE720)},
        {"speaker", QChar(0xE7F5)},
        {"wifi", QChar(0xE701)},
        {"bluetooth", QChar(0xE702)},
        {"battery", QChar(0xE83F)},
        {"power", QChar(0xE7E8)},
        {"lock", QChar(0xE72E)},
        {"unlock", QChar(0xE785)},
        {"user", QChar(0xE77B)},
        {"people", QChar(0xE716)},
        {"globe", QChar(0xE774)},
        {"location", QChar(0xE81D)},
        {"map", QChar(0xE707)},
        {"car", QChar(0xE804)},
        {"airplane", QChar(0xE709)},
        {"train", QChar(0xE7C0)},
        {"shopping", QChar(0xE7BF)},
        {"cart", QChar(0xE7BF)},
        {"money", QChar(0xE8D4)},
        {"calculator", QChar(0xE8EF)},
        {"keyboard", QChar(0xE765)},
        {"mouse", QChar(0xE962)},
        {"print", QChar(0xE749)},
        {"scan", QChar(0xE8FE)},
        {"share", QChar(0xE72D)},
        {"link", QChar(0xE71B)},
        {"attach", QChar(0xE723)},
        {"flag", QChar(0xE7C1)},
        {"tag", QChar(0xE8EC)},
        {"filter", QChar(0xE71C)},
        {"sort", QChar(0xE8CB)},
        {"view", QChar(0xE890)},
        {"list", QChar(0xE8FD)},
        {"grid", QChar(0xE80A)},
        {"zoom_in", QChar(0xE8A3)},
        {"zoom_out", QChar(0xE71F)},
        {"full_screen", QChar(0xE740)},
        {"minimize", QChar(0xE921)},
        {"maximize", QChar(0xE922)},
        {"restore", QChar(0xE923)},
        {"help", QChar(0xE897)},
        {"info", QChar(0xE946)},
        {"warning", QChar(0xE7BA)},
        {"error", QChar(0xE783)},
        {"success", QChar(0xE73E)},
        {"check", QChar(0xE73E)},
        {"cancel", QChar(0xE711)},
        {"clear", QChar(0xE894)},
        {"more", QChar(0xE712)},
        {"menu", QChar(0xE700)},
        {"hamburger", QChar(0xE700)},
        {"dots", QChar(0xE712)},
        {"arrow_up", QChar(0xE70E)},
        {"arrow_down", QChar(0xE70D)},
        {"arrow_left", QChar(0xE70C)},
        {"arrow_right", QChar(0xE70B)},
        {"chevron_up", QChar(0xE70E)},
        {"chevron_down", QChar(0xE70D)},
        {"chevron_left", QChar(0xE76B)},
        {"chevron_right", QChar(0xE76C)}
    };

    return fluentIconMap.value(iconName.toLower(), QChar());
}

void QWinUIIcon::updateThemeColor()
{
    if (m_useThemeColor && theme()) {
        if (theme()->themeMode() == QWinUIThemeMode::Dark) {
            // 深色模式：使用浅色图标
            m_iconColor = QColor(255, 255, 255, 220);
        } else {
            // 浅色模式：使用深色图标
            m_iconColor = QColor(32, 32, 32, 200);
        }

        // 如果图标处于悬停或按下状态，可以使用主题强调色
        if (m_isHovered || m_isPressed) {
            QColor accentColor = theme()->accentColor();
            if (accentColor.isValid()) {
                m_iconColor = accentColor;
                m_iconColor.setAlpha(m_isPressed ? 255 : 200);
            }
        }
    }
}

QColor QWinUIIcon::getEffectiveIconColor() const
{
    QColor color = m_iconColor;

    // 应用悬停和按下状态的透明度调整
    if (m_isPressed) {
        color.setAlpha(color.alpha() * 0.7);
    } else if (m_isHovered) {
        color.setAlpha(color.alpha() * 0.9);
    }

    return color;
}

QTransform QWinUIIcon::getTransform() const
{
    QTransform transform;

    if (!qFuzzyIsNull(m_rotation)) {
        QPointF center = rect().center();
        transform.translate(center.x(), center.y());
        transform.rotate(m_rotation);
        transform.translate(-center.x(), -center.y());
    }

    return transform;
}

QT_END_NAMESPACE
