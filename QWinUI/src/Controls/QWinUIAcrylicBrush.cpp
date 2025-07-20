#include "QWinUI/Controls/QWinUIAcrylicBrush.h"
#include "QWinUI/QWinUITheme.h"
#include <QPainter>
#include <QPaintEvent>
#include <QGraphicsBlurEffect>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QApplication>
#include <QScreen>
#include <QWindow>
#include <QRandomGenerator>
#include <QtMath>
#include <QDebug>

QWinUIAcrylicBrush::QWinUIAcrylicBrush(QWidget* parent)
    : QWinUIWidget(parent)
    , m_tintColor(QColor(255, 255, 255, 204))  // 默认白色带透明度
    , m_tintOpacity(DEFAULT_TINT_OPACITY)
    , m_fallbackColor(QColor(243, 243, 243))   // 浅灰色备用
    , m_tintLuminosityOpacity(DEFAULT_LUMINOSITY_OPACITY)
    , m_backgroundSource(QWinUIAcrylicBackgroundSource::Backdrop)
    , m_isEffectEnabled(true)
    , m_blurRadius(DEFAULT_BLUR_RADIUS)
    , m_noiseOpacity(DEFAULT_NOISE_OPACITY)
    , m_needsBackgroundUpdate(true)
    , m_isCapturing(false)
    , m_tintOpacityAnimation(nullptr)
    , m_tintColorAnimation(nullptr)
    , m_updateTimer(nullptr)
{
    initializeComponent();
}

QWinUIAcrylicBrush::~QWinUIAcrylicBrush()
{
    if (m_updateTimer) {
        m_updateTimer->stop();
    }
}

void QWinUIAcrylicBrush::initializeComponent()
{
    // 设置基本属性
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);

    // 初始化主题颜色
    updateThemeColors();

    // 生成噪声纹理
    generateNoiseTexture();

    // 设置动画
    setupAnimations();

    // 应用亚克力样式
    applyAcrylicStyle();

    // 连接主题变化信号
    auto theme = QWinUITheme::getInstance();
    if (theme) {
        connect(theme, &QWinUITheme::themeChanged,
                this, &QWinUIAcrylicBrush::updateThemeColors);
    }
}

void QWinUIAcrylicBrush::setupAnimations()
{
    // 透明度动画
    m_tintOpacityAnimation = new QPropertyAnimation(this, "tintOpacity", this);
    m_tintOpacityAnimation->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_tintOpacityAnimation, &QPropertyAnimation::finished,
            this, &QWinUIAcrylicBrush::onTintOpacityAnimationFinished);
    
    // 颜色动画
    m_tintColorAnimation = new QPropertyAnimation(this, "tintColor", this);
    m_tintColorAnimation->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_tintColorAnimation, &QPropertyAnimation::finished,
            this, &QWinUIAcrylicBrush::onTintColorAnimationFinished);
}

QColor QWinUIAcrylicBrush::tintColor() const
{
    return m_tintColor;
}

void QWinUIAcrylicBrush::setTintColor(const QColor& color)
{
    if (m_tintColor != color) {
        m_tintColor = color;
        applyAcrylicStyle();
        emit tintColorChanged(color);
    }
}

double QWinUIAcrylicBrush::tintOpacity() const
{
    return m_tintOpacity;
}

void QWinUIAcrylicBrush::setTintOpacity(double opacity)
{
    opacity = qBound(0.0, opacity, 1.0);
    if (!qFuzzyCompare(m_tintOpacity, opacity)) {
        m_tintOpacity = opacity;
        applyAcrylicStyle();
        emit tintOpacityChanged(opacity);
    }
}

QColor QWinUIAcrylicBrush::fallbackColor() const
{
    return m_fallbackColor;
}

void QWinUIAcrylicBrush::setFallbackColor(const QColor& color)
{
    if (m_fallbackColor != color) {
        m_fallbackColor = color;
        if (shouldUseFallback()) {
            update();
        }
        emit fallbackColorChanged(color);
    }
}

double QWinUIAcrylicBrush::tintLuminosityOpacity() const
{
    return m_tintLuminosityOpacity;
}

void QWinUIAcrylicBrush::setTintLuminosityOpacity(double opacity)
{
    opacity = qBound(0.0, opacity, 1.0);
    if (!qFuzzyCompare(m_tintLuminosityOpacity, opacity)) {
        m_tintLuminosityOpacity = opacity;
        m_needsBackgroundUpdate = true;
        update();
        emit tintLuminosityOpacityChanged(opacity);
    }
}

QWinUIAcrylicBackgroundSource QWinUIAcrylicBrush::backgroundSource() const
{
    return m_backgroundSource;
}

void QWinUIAcrylicBrush::setBackgroundSource(QWinUIAcrylicBackgroundSource source)
{
    if (m_backgroundSource != source) {
        m_backgroundSource = source;
        m_needsBackgroundUpdate = true;
        update();
        emit backgroundSourceChanged(source);
    }
}

bool QWinUIAcrylicBrush::isEffectEnabled() const
{
    return m_isEffectEnabled;
}

void QWinUIAcrylicBrush::setEffectEnabled(bool enabled)
{
    if (m_isEffectEnabled != enabled) {
        m_isEffectEnabled = enabled;
        applyAcrylicStyle();
        emit effectEnabledChanged(enabled);
    }
}

double QWinUIAcrylicBrush::blurRadius() const
{
    return m_blurRadius;
}

void QWinUIAcrylicBrush::setBlurRadius(double radius)
{
    radius = qMax(0.0, radius);
    if (!qFuzzyCompare(m_blurRadius, radius)) {
        m_blurRadius = radius;
        applyAcrylicStyle();
        emit blurRadiusChanged(radius);
    }
}

double QWinUIAcrylicBrush::noiseOpacity() const
{
    return m_noiseOpacity;
}

void QWinUIAcrylicBrush::setNoiseOpacity(double opacity)
{
    opacity = qBound(0.0, opacity, 1.0);
    if (!qFuzzyCompare(m_noiseOpacity, opacity)) {
        m_noiseOpacity = opacity;
        update();
        emit noiseOpacityChanged(opacity);
    }
}

void QWinUIAcrylicBrush::updateThemeColors()
{
    auto theme = QWinUITheme::getInstance();

    if (theme && theme->isDarkMode()) {
        // 深色主题
        m_tintColor = QColor(32, 32, 32, 204);
        m_fallbackColor = QColor(32, 32, 32);
    } else {
        // 浅色主题
        m_tintColor = QColor(243, 243, 243, 204);
        m_fallbackColor = QColor(243, 243, 243);
    }

    applyAcrylicStyle();
}

QSize QWinUIAcrylicBrush::sizeHint() const
{
    return QSize(200, 100);
}

QSize QWinUIAcrylicBrush::minimumSizeHint() const
{
    return QSize(50, 50);
}

// paintEvent已移除，使用样式表和图形效果实现亚克力效果

void QWinUIAcrylicBrush::resizeEvent(QResizeEvent* event)
{
    QWinUIWidget::resizeEvent(event);
    m_needsBackgroundUpdate = true;

    // 使用单次定时器延迟更新，避免频繁重绘
    QTimer::singleShot(100, this, [this]() {
        if (m_needsBackgroundUpdate) {
            update();
        }
    });
}

void QWinUIAcrylicBrush::showEvent(QShowEvent* event)
{
    QWinUIWidget::showEvent(event);
    m_needsBackgroundUpdate = true;

    // 延迟更新，避免在显示事件中立即触发重绘
    QTimer::singleShot(50, this, [this]() {
        if (isVisible() && m_needsBackgroundUpdate) {
            update();
        }
    });
}

void QWinUIAcrylicBrush::hideEvent(QHideEvent* event)
{
    QWinUIWidget::hideEvent(event);

    if (m_updateTimer) {
        m_updateTimer->stop();
    }
}

void QWinUIAcrylicBrush::onThemeChanged()
{
    updateThemeColors();
}

void QWinUIAcrylicBrush::updateBackgroundCapture()
{
    if (!isVisible() || m_isCapturing) {
        return;
    }

    // 停止定时器，避免频繁更新
    if (m_updateTimer) {
        m_updateTimer->stop();
    }

    m_needsBackgroundUpdate = true;
    update();
}

void QWinUIAcrylicBrush::captureBackground()
{
    if (m_isCapturing || !isVisible()) {
        return;
    }

    m_isCapturing = true;

    // 暂时使用简单的渐变背景，避免复杂的背景捕获导致的性能问题
    QPixmap background(size());
    background.fill(Qt::transparent);

    QPainter painter(&background);
    painter.setRenderHint(QPainter::Antialiasing);

    // 创建简单的渐变背景
    QLinearGradient gradient(0, 0, width(), height());
    gradient.setColorAt(0, QColor(200, 200, 200, 100));
    gradient.setColorAt(1, QColor(150, 150, 150, 100));

    painter.fillRect(background.rect(), gradient);

    if (!background.isNull()) {
        m_backgroundCapture = background;
        m_cachedEffect = applyAcrylicEffect(background);
    }

    m_isCapturing = false;
}

QPixmap QWinUIAcrylicBrush::applyAcrylicEffect(const QPixmap& background)
{
    if (background.isNull()) {
        return QPixmap();
    }

    QPixmap result = background;

    // 1. 应用模糊效果
    if (m_blurRadius > 0.0) {
        QGraphicsScene scene;
        QGraphicsPixmapItem* item = scene.addPixmap(result);

        QGraphicsBlurEffect* blurEffect = new QGraphicsBlurEffect();
        blurEffect->setBlurRadius(m_blurRadius);
        blurEffect->setBlurHints(QGraphicsBlurEffect::QualityHint);
        item->setGraphicsEffect(blurEffect);

        QPixmap blurred(result.size());
        blurred.fill(Qt::transparent);

        QPainter painter(&blurred);
        painter.setRenderHint(QPainter::Antialiasing);
        scene.render(&painter);

        result = blurred;
    }

    // 2. 应用着色层
    QPainter painter(&result);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    // 计算亮度调节颜色
    QColor luminosityColor = calculateLuminosityColor();
    luminosityColor.setAlphaF(m_tintLuminosityOpacity);

    // 绘制亮度层
    painter.fillRect(result.rect(), luminosityColor);

    // 绘制着色层
    QColor tintColorWithOpacity = m_tintColor;
    tintColorWithOpacity.setAlphaF(m_tintOpacity);
    painter.fillRect(result.rect(), tintColorWithOpacity);

    return result;
}

QColor QWinUIAcrylicBrush::calculateLuminosityColor() const
{
    // 根据着色颜色计算亮度调节颜色
    // 这是WinUI3中的标准算法

    float h, s, l, a;
    m_tintColor.getHslF(&h, &s, &l, &a);

    // 调整亮度以创建深度感
    if (l > 0.5f) {
        l = qMax(0.0f, l - 0.1f); // 稍微变暗
    } else {
        l = qMin(1.0f, l + 0.1f); // 稍微变亮
    }

    QColor luminosityColor;
    luminosityColor.setHslF(h, s * 0.5f, l); // 降低饱和度

    return luminosityColor;
}

void QWinUIAcrylicBrush::generateNoiseTexture()
{
    // 生成细微的噪声纹理以增加真实感
    const int textureSize = 64;
    QImage noiseImage(textureSize, textureSize, QImage::Format_ARGB32);

    QRandomGenerator* generator = QRandomGenerator::global();

    for (int y = 0; y < textureSize; ++y) {
        for (int x = 0; x < textureSize; ++x) {
            // 生成随机灰度值
            int gray = generator->bounded(20, 235); // 避免极端值
            QColor noiseColor(gray, gray, gray, 25); // 低透明度
            noiseImage.setPixelColor(x, y, noiseColor);
        }
    }

    m_noiseTexture = QPixmap::fromImage(noiseImage);
}

bool QWinUIAcrylicBrush::shouldUseFallback() const
{
    // 检查是否应该使用备用颜色
    // 在以下情况下使用备用颜色：
    // 1. 效果被禁用
    // 2. 系统性能不足
    // 3. 在某些特殊情况下（如高对比度模式）

    if (!m_isEffectEnabled) {
        return true;
    }

    // 检查系统是否支持透明效果
    // 在Qt6中，我们假设现代系统都支持合成
    // 可以根据需要添加更具体的检查

    return false;
}

void QWinUIAcrylicBrush::updateEffectState()
{
    if (m_updateTimer) {
        m_updateTimer->stop();
    }

    if (m_isEffectEnabled && isVisible()) {
        m_needsBackgroundUpdate = true;
    }

    update();
}

// 预设样式方法
void QWinUIAcrylicBrush::applyNavigationPanelStyle()
{
    auto theme = QWinUITheme::getInstance();

    if (theme && theme->isDarkMode()) {
        setTintColor(QColor(32, 32, 32, 153));  // 60% 透明度
        setFallbackColor(QColor(32, 32, 32));
    } else {
        setTintColor(QColor(243, 243, 243, 153));
        setFallbackColor(QColor(243, 243, 243));
    }

    setTintOpacity(0.6);
    setTintLuminosityOpacity(0.9);
    setBlurRadius(30.0);
    setNoiseOpacity(0.02);
    setBackgroundSource(QWinUIAcrylicBackgroundSource::Backdrop);

    applyAcrylicStyle();
}

void QWinUIAcrylicBrush::applyContentDialogStyle()
{
    auto theme = QWinUITheme::getInstance();

    if (theme && theme->isDarkMode()) {
        setTintColor(QColor(45, 45, 45, 204));  // 80% 透明度
        setFallbackColor(QColor(45, 45, 45));
    } else {
        setTintColor(QColor(255, 255, 255, 204));
        setFallbackColor(QColor(255, 255, 255));
    }

    setTintOpacity(0.8);
    setTintLuminosityOpacity(0.85);
    setBlurRadius(40.0);
    setNoiseOpacity(0.03);
    setBackgroundSource(QWinUIAcrylicBackgroundSource::HostBackdrop);

    applyAcrylicStyle();
}

void QWinUIAcrylicBrush::applySidebarStyle()
{
    auto theme = QWinUITheme::getInstance();

    if (theme && theme->isDarkMode()) {
        setTintColor(QColor(28, 28, 28, 178));  // 70% 透明度
        setFallbackColor(QColor(28, 28, 28));
    } else {
        setTintColor(QColor(248, 248, 248, 178));
        setFallbackColor(QColor(248, 248, 248));
    }

    setTintOpacity(0.7);
    setTintLuminosityOpacity(0.8);
    setBlurRadius(25.0);
    setNoiseOpacity(0.015);
    setBackgroundSource(QWinUIAcrylicBackgroundSource::Backdrop);

    applyAcrylicStyle();
}

void QWinUIAcrylicBrush::applyTooltipStyle()
{
    auto theme = QWinUITheme::getInstance();

    if (theme && theme->isDarkMode()) {
        setTintColor(QColor(50, 50, 50, 230));  // 90% 透明度
        setFallbackColor(QColor(50, 50, 50));
    } else {
        setTintColor(QColor(255, 255, 255, 230));
        setFallbackColor(QColor(255, 255, 255));
    }

    setTintOpacity(0.9);
    setTintLuminosityOpacity(0.95);
    setBlurRadius(20.0);
    setNoiseOpacity(0.01);
    setBackgroundSource(QWinUIAcrylicBackgroundSource::HostBackdrop);

    applyAcrylicStyle();
}

// 动画方法
void QWinUIAcrylicBrush::animateTintOpacity(double targetOpacity, int duration)
{
    if (m_tintOpacityAnimation->state() == QAbstractAnimation::Running) {
        m_tintOpacityAnimation->stop();
    }

    m_tintOpacityAnimation->setDuration(duration);
    m_tintOpacityAnimation->setStartValue(m_tintOpacity);
    m_tintOpacityAnimation->setEndValue(qBound(0.0, targetOpacity, 1.0));
    m_tintOpacityAnimation->start();
}

void QWinUIAcrylicBrush::animateTintColor(const QColor& targetColor, int duration)
{
    if (m_tintColorAnimation->state() == QAbstractAnimation::Running) {
        m_tintColorAnimation->stop();
    }

    m_tintColorAnimation->setDuration(duration);
    m_tintColorAnimation->setStartValue(m_tintColor);
    m_tintColorAnimation->setEndValue(targetColor);
    m_tintColorAnimation->start();
}

void QWinUIAcrylicBrush::onTintOpacityAnimationFinished()
{
    // 透明度动画完成后的处理
    update();
}

void QWinUIAcrylicBrush::onTintColorAnimationFinished()
{
    // 颜色动画完成后的处理
    applyAcrylicStyle();
}

void QWinUIAcrylicBrush::applyAcrylicStyle()
{
    if (!m_isEffectEnabled) {
        // 禁用效果时使用简单背景
        setStyleSheet(QString("QWinUIAcrylicBrush { background-color: %1; border-radius: 8px; }")
                     .arg(m_fallbackColor.name()));
        return;
    }

    // 创建亚克力效果的样式表
    QString styleSheet = createAcrylicStyleSheet();
    setStyleSheet(styleSheet);

    // 应用模糊效果
    applyBlurEffect();
}

QString QWinUIAcrylicBrush::createAcrylicStyleSheet()
{
    // 计算颜色值
    QColor baseColor = m_tintColor;
    baseColor.setAlphaF(m_tintOpacity);

    // 创建渐变效果模拟亚克力材质
    QColor lightColor = baseColor.lighter(110);
    QColor darkColor = baseColor.darker(110);

    // 添加微妙的渐变和边框
    QString styleSheet = QString(
        "QWinUIAcrylicBrush {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "        stop:0 rgba(%1, %2, %3, %4), "
        "        stop:0.3 rgba(%5, %6, %7, %8), "
        "        stop:0.7 rgba(%9, %10, %11, %12), "
        "        stop:1 rgba(%13, %14, %15, %16));"
        "    border: 1px solid rgba(%17, %18, %19, 0.2);"
        "    border-radius: 8px;"
        "}"
    ).arg(lightColor.red()).arg(lightColor.green()).arg(lightColor.blue()).arg(int(lightColor.alphaF() * 255))
     .arg(baseColor.red()).arg(baseColor.green()).arg(baseColor.blue()).arg(int(baseColor.alphaF() * 255))
     .arg(baseColor.red()).arg(baseColor.green()).arg(baseColor.blue()).arg(int(baseColor.alphaF() * 255))
     .arg(darkColor.red()).arg(darkColor.green()).arg(darkColor.blue()).arg(int(darkColor.alphaF() * 255))
     .arg(baseColor.red()).arg(baseColor.green()).arg(baseColor.blue());

    return styleSheet;
}

void QWinUIAcrylicBrush::applyBlurEffect()
{
    // 移除现有的图形效果
    if (graphicsEffect()) {
        delete graphicsEffect();
    }

    if (!m_isEffectEnabled || m_blurRadius <= 0) {
        return;
    }

    // 创建模糊效果
    QGraphicsBlurEffect* blurEffect = new QGraphicsBlurEffect(this);
    blurEffect->setBlurRadius(m_blurRadius * 0.3); // 减少模糊半径，避免过度模糊
    blurEffect->setBlurHints(QGraphicsBlurEffect::QualityHint);

    setGraphicsEffect(blurEffect);
}
