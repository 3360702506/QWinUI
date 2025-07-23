#include "QWinUI/Controls/QWinUIMenuFlyout.h"
#include "QWinUI/QWinUITheme.h"
#include "QWinUI/Controls/QWinUIToolTip.h"
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QApplication>
#include <QScreen>
#include <QTimer>
#include <QGraphicsDropShadowEffect>
#include <QFontMetrics>
#include <QDebug>

// QWinUIMenuFlyoutItem 实现
QWinUIMenuFlyoutItem::QWinUIMenuFlyoutItem(const QString& text, QWidget* parent)
    : QWinUIButton(text, parent)
    , m_itemType(QWinUIMenuFlyoutItemType::Standard)
    , m_checked(false)
    , m_subMenu(nullptr)
    , m_hoverAnimation(nullptr)
    , m_pressAnimation(nullptr)
    , m_hoverProgress(0.0)
    , m_pressProgress(0.0)
{
    initializeItem();
}

QWinUIMenuFlyoutItem::QWinUIMenuFlyoutItem(const QString& text, const QIcon& icon, QWidget* parent)
    : QWinUIButton(text, parent)
    , m_itemType(QWinUIMenuFlyoutItemType::Standard)
    , m_checked(false)
    , m_subMenu(nullptr)
    , m_hoverAnimation(nullptr)
    , m_pressAnimation(nullptr)
    , m_hoverProgress(0.0)
    , m_pressProgress(0.0)
{
    setIcon(icon);
    initializeItem();
}

QWinUIMenuFlyoutItem::QWinUIMenuFlyoutItem(QWinUIMenuFlyoutItemType type, QWidget* parent)
    : QWinUIButton(parent)
    , m_itemType(type)
    , m_checked(false)
    , m_subMenu(nullptr)
    , m_hoverAnimation(nullptr)
    , m_pressAnimation(nullptr)
    , m_hoverProgress(0.0)
    , m_pressProgress(0.0)
{
    initializeItem();
}

QWinUIMenuFlyoutItem::~QWinUIMenuFlyoutItem()
{
    if (m_subMenu) {
        m_subMenu->deleteLater();
    }
}

void QWinUIMenuFlyoutItem::initializeItem()
{
    setFixedHeight(ITEM_HEIGHT);
    setButtonStyle(QWinUIButton::Subtle);
    setFocusPolicy(Qt::StrongFocus);

    // 创建悬浮动画 - 100ms平滑过渡
    m_hoverAnimation = new QPropertyAnimation(this, "hoverProgress", this);
    m_hoverAnimation->setDuration(100);
    m_hoverAnimation->setEasingCurve(QEasingCurve::OutCubic);

    // 创建按下动画 - 100ms平滑过渡
    m_pressAnimation = new QPropertyAnimation(this, "pressProgress", this);
    m_pressAnimation->setDuration(100);
    m_pressAnimation->setEasingCurve(QEasingCurve::OutCubic);

    // 连接信号
    connect(this, &QWinUIButton::clicked, this, &QWinUIMenuFlyoutItem::triggered);

    updateItemAppearance();
}

void QWinUIMenuFlyoutItem::updateItemAppearance()
{
    update();
}

// 属性访问器
QWinUIMenuFlyoutItemType QWinUIMenuFlyoutItem::itemType() const
{
    return m_itemType;
}

void QWinUIMenuFlyoutItem::setItemType(QWinUIMenuFlyoutItemType type)
{
    if (m_itemType != type) {
        m_itemType = type;
        updateItemAppearance();
    }
}

bool QWinUIMenuFlyoutItem::isChecked() const
{
    return m_checked;
}

void QWinUIMenuFlyoutItem::setChecked(bool checked)
{
    if (m_checked != checked) {
        m_checked = checked;
        
        // 如果是单选按钮，取消同组其他按钮的选中状态
        if (checked && m_itemType == QWinUIMenuFlyoutItemType::Radio && !m_radioGroup.isEmpty()) {
            QWidget* parent = parentWidget();
            if (parent) {
                QList<QWinUIMenuFlyoutItem*> siblings = parent->findChildren<QWinUIMenuFlyoutItem*>();
                for (QWinUIMenuFlyoutItem* sibling : siblings) {
                    if (sibling != this && 
                        sibling->itemType() == QWinUIMenuFlyoutItemType::Radio &&
                        sibling->radioGroup() == m_radioGroup) {
                        sibling->setChecked(false);
                    }
                }
            }
        }
        
        updateItemAppearance();
        emit checkedChanged(checked);
    }
}

QString QWinUIMenuFlyoutItem::radioGroup() const
{
    return m_radioGroup;
}

void QWinUIMenuFlyoutItem::setRadioGroup(const QString& group)
{
    m_radioGroup = group;
}

QWinUIMenuFlyout* QWinUIMenuFlyoutItem::subMenu() const
{
    return m_subMenu;
}

void QWinUIMenuFlyoutItem::setSubMenu(QWinUIMenuFlyout* menu)
{
    if (m_subMenu != menu) {
        if (m_subMenu) {
            m_subMenu->deleteLater();
        }
        m_subMenu = menu;
        m_itemType = QWinUIMenuFlyoutItemType::SubMenu;
        updateItemAppearance();
    }
}

QString QWinUIMenuFlyoutItem::shortcut() const
{
    return m_shortcut;
}

void QWinUIMenuFlyoutItem::setShortcut(const QString& shortcut)
{
    if (m_shortcut != shortcut) {
        m_shortcut = shortcut;
        updateItemAppearance();
    }
}

QSize QWinUIMenuFlyoutItem::sizeHint() const
{
    QFontMetrics fm(font());
    int textWidth = fm.horizontalAdvance(text());
    int shortcutWidth = m_shortcut.isEmpty() ? 0 : fm.horizontalAdvance(m_shortcut) + SHORTCUT_SPACING;

    // 基础宽度：左右边距 + 文本宽度
    int width = ITEM_PADDING * 2 + textWidth;

    // 添加图标空间
    if (!icon().isNull()) {
        width += ICON_SIZE + ITEM_PADDING;
    }

    // 添加选中指示器空间
    if (m_itemType == QWinUIMenuFlyoutItemType::Checkable ||
        m_itemType == QWinUIMenuFlyoutItemType::Radio) {
        width += CHECK_SIZE + ITEM_PADDING;
    }

    // 添加快捷键空间
    if (!m_shortcut.isEmpty()) {
        width += shortcutWidth;
    }

    // 添加子菜单箭头空间
    if (m_itemType == QWinUIMenuFlyoutItemType::SubMenu) {
        width += ARROW_SIZE + ITEM_PADDING;
    }

    // 确保最小宽度
    width = qMax(width, 120);

    return QSize(width, ITEM_HEIGHT);
}

QSize QWinUIMenuFlyoutItem::minimumSizeHint() const
{
    return QSize(100, ITEM_HEIGHT);
}

// 动画属性实现
double QWinUIMenuFlyoutItem::hoverProgress() const
{
    return m_hoverProgress;
}

void QWinUIMenuFlyoutItem::setHoverProgress(double progress)
{
    if (qAbs(m_hoverProgress - progress) > 1e-6) {
        m_hoverProgress = progress;
        update();
    }
}

double QWinUIMenuFlyoutItem::pressProgress() const
{
    return m_pressProgress;
}

void QWinUIMenuFlyoutItem::setPressProgress(double progress)
{
    if (qAbs(m_pressProgress - progress) > 1e-6) {
        m_pressProgress = progress;
        update();
    }
}

void QWinUIMenuFlyoutItem::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // WinUI3 风格的菜单项背景绘制
    QWinUITheme* theme = QWinUITheme::getInstance();
    QColor backgroundColor = Qt::transparent;

    // 基础悬浮颜色 - 浅灰色
    if (m_hoverProgress > 0.0) {
        QColor hoverColor;
        if (theme && theme->isDarkMode()) {
            hoverColor = QColor(255, 255, 255, 12); // 深色模式：半透明白色
        } else {
            hoverColor = QColor(0, 0, 0, 8); // 浅色模式：半透明黑色（浅灰色效果）
        }

        // 应用悬浮进度
        hoverColor.setAlpha(static_cast<int>(hoverColor.alpha() * m_hoverProgress));
        backgroundColor = hoverColor;
    }

    // 按下颜色 - 更深的灰色
    if (m_pressProgress > 0.0) {
        QColor pressColor;
        if (theme && theme->isDarkMode()) {
            pressColor = QColor(255, 255, 255, 20); // 深色模式：更明显的白色
        } else {
            pressColor = QColor(0, 0, 0, 15); // 浅色模式：更深的灰色
        }

        // 应用按下进度，与悬浮颜色混合
        pressColor.setAlpha(static_cast<int>(pressColor.alpha() * m_pressProgress));

        if (backgroundColor != Qt::transparent) {
            // 混合悬浮和按下颜色
            backgroundColor = QColor::fromRgbF(
                backgroundColor.redF() + (pressColor.redF() - backgroundColor.redF()) * m_pressProgress,
                backgroundColor.greenF() + (pressColor.greenF() - backgroundColor.greenF()) * m_pressProgress,
                backgroundColor.blueF() + (pressColor.blueF() - backgroundColor.blueF()) * m_pressProgress,
                backgroundColor.alphaF() + (pressColor.alphaF() - backgroundColor.alphaF()) * m_pressProgress
            );
        } else {
            backgroundColor = pressColor;
        }
    }

    // 绘制圆角背景
    if (backgroundColor != Qt::transparent) {
        painter.setBrush(backgroundColor);
        painter.setPen(Qt::NoPen);
        // 使用4px圆角，符合WinUI3设计
        painter.drawRoundedRect(rect().adjusted(2, 1, -2, -1), 4, 4);
    }

    QRect rect = this->rect();
    int x = ITEM_PADDING;

    // 绘制选中指示器
    if (m_itemType == QWinUIMenuFlyoutItemType::Checkable ||
        m_itemType == QWinUIMenuFlyoutItemType::Radio) {
        QRect checkRect(x, (rect.height() - CHECK_SIZE) / 2, CHECK_SIZE, CHECK_SIZE);
        drawCheckIndicator(&painter, checkRect);
        x += CHECK_SIZE + ITEM_PADDING;
    }

    // 绘制图标
    if (!icon().isNull()) {
        QRect iconRect(x, (rect.height() - ICON_SIZE) / 2, ICON_SIZE, ICON_SIZE);
        icon().paint(&painter, iconRect);
        x += ICON_SIZE + ITEM_PADDING;
    }

    // 绘制文本
    QColor textColor = theme ? (theme->isDarkMode() ? QColor(255, 255, 255) : QColor(0, 0, 0)) : QColor(0, 0, 0);

    if (!isEnabled()) {
        textColor.setAlpha(128);
    }

    painter.setPen(textColor);
    painter.setFont(font());

    // 计算文本区域，为子菜单箭头和快捷键留出空间
    int rightMargin = ITEM_PADDING;
    if (m_itemType == QWinUIMenuFlyoutItemType::SubMenu) {
        rightMargin += ARROW_SIZE + ITEM_PADDING;
    }
    if (!m_shortcut.isEmpty()) {
        rightMargin += QFontMetrics(font()).horizontalAdvance(m_shortcut) + SHORTCUT_SPACING;
    }

    QRect textRect(x, 0, rect.width() - x - rightMargin, rect.height());
    painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, text());

    // 绘制快捷键
    if (!m_shortcut.isEmpty()) {
        QColor shortcutColor = textColor;
        shortcutColor.setAlpha(shortcutColor.alpha() * 0.7);
        painter.setPen(shortcutColor);

        int shortcutWidth = QFontMetrics(font()).horizontalAdvance(m_shortcut);
        int shortcutX = rect.width() - ITEM_PADDING - shortcutWidth;
        if (m_itemType == QWinUIMenuFlyoutItemType::SubMenu) {
            shortcutX -= ARROW_SIZE + ITEM_PADDING;
        }

        QRect shortcutRect(shortcutX, 0, shortcutWidth, rect.height());
        painter.drawText(shortcutRect, Qt::AlignLeft | Qt::AlignVCenter, m_shortcut);
    }

    // 绘制子菜单箭头
    if (m_itemType == QWinUIMenuFlyoutItemType::SubMenu) {
        QRect arrowRect(rect.width() - ITEM_PADDING - ARROW_SIZE,
                       (rect.height() - ARROW_SIZE) / 2, ARROW_SIZE, ARROW_SIZE);
        drawSubMenuArrow(&painter, arrowRect);
    }
}

void QWinUIMenuFlyoutItem::drawCheckIndicator(QPainter* painter, const QRect& rect)
{
    if (!m_checked) return;

    painter->save();

    QWinUITheme* theme = QWinUITheme::getInstance();
    QColor checkColor;

    // 根据主题设置勾选标记颜色
    if (theme) {
        if (theme->isDarkMode()) {
            // 深色主题：使用白色或浅色
            checkColor = QColor(255, 255, 255);
        } else {
            // 浅色主题：使用深色
            checkColor = QColor(0, 0, 0);
        }
    } else {
        checkColor = QColor(0, 0, 0);
    }

    // 使用Fluent Icons字体绘制勾选标记
    QFont fluentFont("Segoe Fluent Icons", 12);
    painter->setFont(fluentFont);
    painter->setPen(checkColor);

    if (m_itemType == QWinUIMenuFlyoutItemType::Radio) {
        // 单选按钮：使用Fluent Icons的RadioButton图标
        painter->drawText(rect, Qt::AlignCenter, "\uE915"); // RadioButton filled icon
    } else {
        // 复选框：使用Fluent Icons的CheckMark图标
        painter->drawText(rect, Qt::AlignCenter, "\uE73E"); // CheckMark icon
    }

    painter->restore();
}

void QWinUIMenuFlyoutItem::drawSubMenuArrow(QPainter* painter, const QRect& rect)
{
    painter->save();

    QWinUITheme* theme = QWinUITheme::getInstance();
    QColor arrowColor = theme ? (theme->isDarkMode() ? QColor(255, 255, 255) : QColor(0, 0, 0)) : QColor(0, 0, 0);

    if (!isEnabled()) {
        arrowColor.setAlpha(128);
    }

    // 使用Fluent Icons字体绘制右箭头
    QFont fluentFont("Segoe Fluent Icons", 10);
    painter->setFont(fluentFont);
    painter->setPen(arrowColor);

    // 绘制右箭头图标
    painter->drawText(rect, Qt::AlignCenter, "\uE76C"); // ChevronRight icon

    painter->restore();
}

void QWinUIMenuFlyoutItem::mousePressEvent(QMouseEvent* event)
{
    // 启动按下动画 - 100ms平滑过渡到更深的灰色
    if (m_pressAnimation && event->button() == Qt::LeftButton) {
        m_pressAnimation->stop();
        m_pressAnimation->setStartValue(m_pressProgress);
        m_pressAnimation->setEndValue(1.0);
        m_pressAnimation->start();
    }

    QWinUIButton::mousePressEvent(event);
}

void QWinUIMenuFlyoutItem::mouseReleaseEvent(QMouseEvent* event)
{
    // 启动释放动画 - 100ms平滑过渡回悬浮状态
    if (m_pressAnimation && event->button() == Qt::LeftButton) {
        m_pressAnimation->stop();
        m_pressAnimation->setStartValue(m_pressProgress);
        m_pressAnimation->setEndValue(0.0);
        m_pressAnimation->start();
    }

    if (event->button() == Qt::LeftButton && rect().contains(event->pos())) {
        if (m_itemType == QWinUIMenuFlyoutItemType::SubMenu && m_subMenu) {
            emit subMenuRequested();
            // 子菜单项不触发triggered信号，避免关闭父菜单
        } else if (m_itemType == QWinUIMenuFlyoutItemType::Checkable) {
            setChecked(!m_checked);
            emit triggered();
        } else if (m_itemType == QWinUIMenuFlyoutItemType::Radio) {
            setChecked(true);
            emit triggered();
        } else {
            emit triggered();
        }
    }

    QWinUIButton::mouseReleaseEvent(event);
}

void QWinUIMenuFlyoutItem::enterEvent(QEnterEvent* event)
{
    // 启动悬浮动画 - 100ms平滑过渡到浅灰色
    if (m_hoverAnimation) {
        m_hoverAnimation->stop();
        m_hoverAnimation->setStartValue(m_hoverProgress);
        m_hoverAnimation->setEndValue(1.0);
        m_hoverAnimation->start();
    }

    // 子菜单延迟显示 - 300ms延迟
    if (m_itemType == QWinUIMenuFlyoutItemType::SubMenu && m_subMenu) {
        QTimer::singleShot(300, this, &QWinUIMenuFlyoutItem::subMenuRequested);
    }

    QWinUIButton::enterEvent(event);
}

void QWinUIMenuFlyoutItem::leaveEvent(QEvent* event)
{
    // 启动离开动画 - 100ms平滑过渡回透明
    if (m_hoverAnimation) {
        m_hoverAnimation->stop();
        m_hoverAnimation->setStartValue(m_hoverProgress);
        m_hoverAnimation->setEndValue(0.0);
        m_hoverAnimation->start();
    }

    QWinUIButton::leaveEvent(event);
}

// QWinUIMenuFlyoutSeparator 现在使用 QWinUIAppBarSeparator

// QWinUIMenuFlyout 实现
QWinUIMenuFlyout::QWinUIMenuFlyout(QWidget* parent)
    : QWinUIWidget(parent)
    , m_layout(nullptr)
    , m_parentMenu(nullptr)
    , m_currentSubMenu(nullptr)
    , m_currentItem(nullptr)
    , m_selectedIndex(-1)
    , m_isVisible(false)
    , m_autoClose(true)
    , m_animationDuration(DEFAULT_ANIMATION_DURATION)
    , m_maxWidth(DEFAULT_MAX_WIDTH)
    , m_minWidth(DEFAULT_MIN_WIDTH)
    , m_showAnimation(nullptr)
    , m_hideAnimation(nullptr)
    , m_heightAnimation(nullptr)
    , m_hideHeightAnimation(nullptr)
    , m_shadowEffect(nullptr)
    , m_subMenuTimer(nullptr)
    , m_heightProgress(1.0)
    , m_targetHeight(0)
    , m_currentAnimatedHeight(0)
    , m_isAnimating(false)
    , m_baselinePosition(0, 0)
    , m_finalPosition(0, 0)
    , m_expandUpward(true)
    , m_animationOffset(0)
    , m_triggerButtonSize(0, 0)
{
    initializeMenuFlyout();
}

QWinUIMenuFlyout::~QWinUIMenuFlyout()
{
    // 断开主题连接，避免悬空指针
    QWinUITheme* theme = QWinUITheme::getInstance();
    if (theme) {
        disconnect(theme, &QWinUITheme::themeChanged, this, &QWinUIMenuFlyout::onThemeChanged);
    }

    // 移除事件过滤器
    // qApp->removeEventFilter(this);

    clear();
}

void QWinUIMenuFlyout::initializeMenuFlyout()
{
    // 设置窗口属性 - 简化窗口标志，避免透明背景冲突
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_DeleteOnClose, false);
    setFocusPolicy(Qt::StrongFocus);

    // 设置最小尺寸
    setMinimumSize(m_minWidth, 32);

    // 创建布局
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(MENU_PADDING, MENU_PADDING, MENU_PADDING, MENU_PADDING);
    m_layout->setSpacing(0);

    // 暂时禁用阴影效果，避免与透明窗口冲突
    // setupShadowEffect();

    // 创建淡入动画
    m_showAnimation = new QPropertyAnimation(this, "windowOpacity", this);
    m_showAnimation->setDuration(250); // 250ms，更慢的动画
    m_showAnimation->setStartValue(0.0);
    m_showAnimation->setEndValue(1.0);
    m_showAnimation->setEasingCurve(QEasingCurve::OutCubic); // Ease-Out

    // 创建高度展开动画
    m_heightAnimation = new QPropertyAnimation(this, "heightProgress", this);
    m_heightAnimation->setDuration(250); // 250ms，更慢的动画
    m_heightAnimation->setStartValue(0.5); // 从一半高度开始
    m_heightAnimation->setEndValue(1.0);   // 到完整高度
    m_heightAnimation->setEasingCurve(QEasingCurve::OutCubic);

    // 创建淡出动画
    m_hideAnimation = new QPropertyAnimation(this, "windowOpacity", this);
    m_hideAnimation->setDuration(200); // 200ms，隐藏稍快一些
    m_hideAnimation->setStartValue(1.0);
    m_hideAnimation->setEndValue(0.0);
    m_hideAnimation->setEasingCurve(QEasingCurve::InCubic);

    // 创建高度收缩动画
    m_hideHeightAnimation = new QPropertyAnimation(this, "heightProgress", this);
    m_hideHeightAnimation->setDuration(200); // 200ms，隐藏稍快一些
    m_hideHeightAnimation->setStartValue(1.0);   // 从完整高度开始
    m_hideHeightAnimation->setEndValue(0.5);     // 到一半高度
    m_hideHeightAnimation->setEasingCurve(QEasingCurve::InCubic);

    connect(m_hideAnimation, &QPropertyAnimation::finished, this, &QWinUIMenuFlyout::onAnimationFinished);
    connect(m_heightAnimation, &QPropertyAnimation::finished, this, &QWinUIMenuFlyout::onAnimationFinished);
    connect(m_hideHeightAnimation, &QPropertyAnimation::finished, this, &QWinUIMenuFlyout::onAnimationFinished);

    // 创建子菜单定时器
    m_subMenuTimer = new QTimer(this);
    m_subMenuTimer->setSingleShot(true);
    m_subMenuTimer->setInterval(SUBMENU_DELAY);

    // 初始化颜色
    updateColors();

    // 连接主题变化信号
    QWinUITheme* theme = QWinUITheme::getInstance();
    if (theme) {
        connect(theme, &QWinUITheme::themeChanged, this, &QWinUIMenuFlyout::onThemeChanged);
    }

    // 暂时不安装全局事件过滤器，避免潜在的崩溃问题
    // qApp->installEventFilter(this);
}

void QWinUIMenuFlyout::setupShadowEffect()
{
    m_shadowEffect = new QGraphicsDropShadowEffect(this);

    // WinUI 3 风格的深度阴影
    m_shadowEffect->setBlurRadius(32);              // 更大的模糊半径，营造深度感
    m_shadowEffect->setOffset(0, 12);               // 更明显的向下偏移
    m_shadowEffect->setColor(QColor(0, 0, 0, 60));  // 更深的阴影，增强层级感

    setGraphicsEffect(m_shadowEffect);
}

void QWinUIMenuFlyout::updateColors()
{
    // 颜色将在 paintEvent 中处理，不使用样式表
    update();
}

// 菜单项管理方法
QWinUIMenuFlyoutItem* QWinUIMenuFlyout::addItem(const QString& text, const QIcon& icon)
{
    QWinUIMenuFlyoutItem* item = new QWinUIMenuFlyoutItem(text, icon, this);

    // 设置菜单项样式
    item->setButtonStyle(QWinUIButton::Subtle);
    item->setFixedHeight(32);

    m_layout->addWidget(item);
    m_menuItems.append(item);

    connect(item, &QWinUIMenuFlyoutItem::triggered, this, &QWinUIMenuFlyout::onItemTriggered);
    connect(item, &QWinUIMenuFlyoutItem::subMenuRequested, this, &QWinUIMenuFlyout::onItemSubMenuRequested);

    updateLayout();
    return item;
}

QWinUIMenuFlyoutItem* QWinUIMenuFlyout::addCheckableItem(const QString& text, bool checked, const QIcon& icon)
{
    QWinUIMenuFlyoutItem* item = new QWinUIMenuFlyoutItem(text, icon, this);
    item->setItemType(QWinUIMenuFlyoutItemType::Checkable);
    item->setChecked(checked);

    m_layout->addWidget(item);
    m_menuItems.append(item);

    connect(item, &QWinUIMenuFlyoutItem::triggered, this, &QWinUIMenuFlyout::onItemTriggered);

    updateLayout();
    return item;
}

QWinUIMenuFlyoutItem* QWinUIMenuFlyout::addRadioItem(const QString& text, const QString& group, bool checked, const QIcon& icon)
{
    QWinUIMenuFlyoutItem* item = new QWinUIMenuFlyoutItem(text, icon, this);
    item->setItemType(QWinUIMenuFlyoutItemType::Radio);
    item->setRadioGroup(group);
    item->setChecked(checked);

    m_layout->addWidget(item);
    m_menuItems.append(item);

    connect(item, &QWinUIMenuFlyoutItem::triggered, this, &QWinUIMenuFlyout::onItemTriggered);

    updateLayout();
    return item;
}

QWinUIMenuFlyout* QWinUIMenuFlyout::addSubMenu(const QString& text, const QIcon& icon)
{
    QWinUIMenuFlyoutItem* item = new QWinUIMenuFlyoutItem(text, icon, this);
    QWinUIMenuFlyout* subMenu = new QWinUIMenuFlyout(this);

    item->setSubMenu(subMenu);
    subMenu->m_parentMenu = this;

    m_layout->addWidget(item);
    m_menuItems.append(item);

    connect(item, &QWinUIMenuFlyoutItem::subMenuRequested, this, &QWinUIMenuFlyout::onItemSubMenuRequested);
    connect(subMenu, &QWinUIMenuFlyout::aboutToShow, this, &QWinUIMenuFlyout::onSubMenuAboutToShow);
    connect(subMenu, &QWinUIMenuFlyout::aboutToHide, this, &QWinUIMenuFlyout::onSubMenuAboutToHide);

    updateLayout();
    return subMenu;
}

QWinUIMenuFlyoutSeparator* QWinUIMenuFlyout::addSeparator()
{
    QWinUIMenuFlyoutSeparator* separator = new QWinUIMenuFlyoutSeparator(Qt::Horizontal, this);
    separator->setFixedHeight(9); // 设置分割线高度
    m_layout->addWidget(separator);
    m_menuItems.append(separator);

    updateLayout();
    return separator;
}

void QWinUIMenuFlyout::clear()
{
    hideAllSubMenus();

    for (QWidget* item : m_menuItems) {
        m_layout->removeWidget(item);
        item->deleteLater();
    }
    m_menuItems.clear();

    m_selectedIndex = -1;
    m_currentItem = nullptr;
}

// 显示控制
void QWinUIMenuFlyout::showAt(QWidget* parent, const QPoint& position)
{
    if (parent) {
        // 计算按钮的左下角位置作为菜单的起始点
        QPoint buttonBottomLeft = parent->mapToGlobal(QPoint(0, parent->height()));

        // 保存按钮信息用于位置计算
        m_triggerButtonSize = parent->size();

        // 使用按钮左下角作为基准线
        showAt(buttonBottomLeft);
    }
}

void QWinUIMenuFlyout::showAt(const QPoint& globalPosition)
{
    if (m_isVisible) {
        return;
    }

    emit aboutToShow();

    // 确保菜单有内容
    if (m_menuItems.isEmpty()) {
        return;
    }

    // 调整大小
    updateLayout();
    adjustSize();

    // 保存基准线位置（触发点）
    m_baselinePosition = globalPosition;

    // 计算最终位置和展开方向
    calculateAnimationParameters(globalPosition);

    // 显示菜单
    m_isVisible = true;

    // 设置窗口透明度为0，准备动画
    setWindowOpacity(0.0);
    show();
    raise();  // 确保菜单在最上层
    activateWindow();
    setFocus();

    // 启动显示动画
    startShowAnimation();
}

void QWinUIMenuFlyout::hide()
{
    if (!m_isVisible) return;

    emit aboutToHide();

    hideAllSubMenus();
    m_isVisible = false;

    // 启动隐藏动画
    startHideAnimation();
}

// 属性访问器
bool QWinUIMenuFlyout::autoClose() const
{
    return m_autoClose;
}

void QWinUIMenuFlyout::setAutoClose(bool autoClose)
{
    if (m_autoClose != autoClose) {
        m_autoClose = autoClose;
        emit autoCloseChanged(autoClose);
    }
}

int QWinUIMenuFlyout::animationDuration() const
{
    return m_animationDuration;
}

void QWinUIMenuFlyout::setAnimationDuration(int duration)
{
    if (m_animationDuration != duration) {
        m_animationDuration = duration;
        m_showAnimation->setDuration(duration);
        m_hideAnimation->setDuration(duration);
        emit animationDurationChanged(duration);
    }
}

int QWinUIMenuFlyout::maxWidth() const
{
    return m_maxWidth;
}

void QWinUIMenuFlyout::setMaxWidth(int width)
{
    if (m_maxWidth != width) {
        m_maxWidth = width;
        updateLayout();
        emit maxWidthChanged(width);
    }
}

int QWinUIMenuFlyout::minWidth() const
{
    return m_minWidth;
}

void QWinUIMenuFlyout::setMinWidth(int width)
{
    if (m_minWidth != width) {
        m_minWidth = width;
        updateLayout();
        emit minWidthChanged(width);
    }
}

// 高度动画属性
double QWinUIMenuFlyout::heightProgress() const
{
    return m_heightProgress;
}

void QWinUIMenuFlyout::setHeightProgress(double progress)
{
    if (qAbs(m_heightProgress - progress) > 1e-6) {
        m_heightProgress = progress;

        // 在动画过程中只更新Y位置
        if (m_isAnimating && m_targetHeight > 0) {
            // 计算当前Y位置：从初始位置插值到最终位置
            // progress从0.5到1.0，我们需要将其映射到0.0到1.0
            double normalizedProgress = (progress - 0.5) / 0.5;
            normalizedProgress = qBound(0.0, normalizedProgress, 1.0);

            // 计算初始Y位置（基准线居中）
            QSize menuSize = size();
            if (menuSize.width() > 0 && menuSize.height() > 0) {
                int initialY = m_baselinePosition.y() - menuSize.height() / 2;

                // 插值计算当前Y位置，X位置保持不变
                int currentY = static_cast<int>(initialY +
                              (m_finalPosition.y() - initialY) * normalizedProgress);

                move(m_finalPosition.x(), currentY);
            }
        }

        // 动画完成时重置状态并确保在最终位置
        if (progress >= 1.0) {
            m_isAnimating = false;
            if (!m_finalPosition.isNull()) {
                move(m_finalPosition);
            }
        }

        // 触发重绘以更新剪裁区域
        update();
    }
}

// 查找方法
QList<QWinUIMenuFlyoutItem*> QWinUIMenuFlyout::items() const
{
    QList<QWinUIMenuFlyoutItem*> itemList;
    for (QWidget* widget : m_menuItems) {
        QWinUIMenuFlyoutItem* item = qobject_cast<QWinUIMenuFlyoutItem*>(widget);
        if (item) {
            itemList.append(item);
        }
    }
    return itemList;
}

QWinUIMenuFlyoutItem* QWinUIMenuFlyout::itemAt(int index) const
{
    if (index >= 0 && index < m_menuItems.count()) {
        return qobject_cast<QWinUIMenuFlyoutItem*>(m_menuItems[index]);
    }
    return nullptr;
}

int QWinUIMenuFlyout::itemCount() const
{
    int count = 0;
    for (QWidget* widget : m_menuItems) {
        if (qobject_cast<QWinUIMenuFlyoutItem*>(widget)) {
            count++;
        }
    }
    return count;
}

QSize QWinUIMenuFlyout::sizeHint() const
{
    int width = m_minWidth;
    int height = MENU_PADDING * 2;

    for (QWidget* widget : m_menuItems) {
        QSize itemSize = widget->sizeHint();
        width = qMax(width, itemSize.width() + MENU_PADDING * 2);
        height += itemSize.height();
    }

    width = qMin(width, m_maxWidth);

    return QSize(width, height);
}

void QWinUIMenuFlyout::closeAllSubMenus()
{
    hideAllSubMenus();
}

// 私有槽函数
void QWinUIMenuFlyout::onItemTriggered()
{
    QWinUIMenuFlyoutItem* item = qobject_cast<QWinUIMenuFlyoutItem*>(sender());
    if (item) {
        emit itemClicked(item);

        if (m_autoClose && item->itemType() != QWinUIMenuFlyoutItemType::SubMenu) {
            hide();
        }
    }
}

void QWinUIMenuFlyout::onItemSubMenuRequested()
{
    QWinUIMenuFlyoutItem* item = qobject_cast<QWinUIMenuFlyoutItem*>(sender());
    if (item && item->subMenu()) {
        showSubMenu(item);
    }
}

void QWinUIMenuFlyout::onSubMenuAboutToShow()
{
    // 子菜单即将显示
}

void QWinUIMenuFlyout::onSubMenuAboutToHide()
{
    // 子菜单即将隐藏
}

int QWinUIMenuFlyout::calculateTotalHeight() const
{
    int totalHeight = 0;

    // 计算所有菜单项的高度
    for (QWidget* widget : m_menuItems) {
        if (widget && widget->isVisible()) {
            totalHeight += widget->sizeHint().height();
        }
    }

    // 添加内边距和布局间距
    totalHeight += MENU_PADDING * 2;
    if (m_layout) {
        totalHeight += m_layout->spacing() * qMax(0, m_menuItems.count() - 1);
    }

    return totalHeight;
}

void QWinUIMenuFlyout::onAnimationFinished()
{
    m_isAnimating = false;

    if (!m_isVisible) {
        QWidget::hide();
    } else {
        // 显示动画完成，恢复正常尺寸策略
        setMinimumHeight(0);
        setMaximumHeight(QWIDGETSIZE_MAX);
        adjustSize();
    }
}

// 事件处理
void QWinUIMenuFlyout::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QWinUITheme* theme = QWinUITheme::getInstance();

    // WinUI 3 风格的背景和边框颜色
    QColor backgroundColor;
    QColor borderColor;
    QColor shadowColor;

    if (theme && theme->isDarkMode()) {
        // 深色模式：不透明背景和微妙的边框
        backgroundColor = QColor(32, 32, 32);          // 不透明的深灰背景
        borderColor = QColor(255, 255, 255, 20);       // 微妙的白色边框
        shadowColor = QColor(0, 0, 0, 120);            // 更深的阴影
    } else {
        // 浅色模式：不透明背景和微妙的边框
        backgroundColor = QColor(255, 255, 255);       // 不透明的白色背景
        borderColor = QColor(0, 0, 0, 15);             // 微妙的黑色边框
        shadowColor = QColor(0, 0, 0, 80);             // 更深的阴影
    }

    QRect bgRect = rect();

    // 实现剪裁式展开动画
    if (m_isAnimating && m_heightProgress < 1.0 && bgRect.width() > 0 && bgRect.height() > 0) {
        // 计算基准线在当前widget中的相对位置
        QPoint relativeBaseline = m_baselinePosition - pos();

        // 将progress从0.5-1.0映射到0.0-1.0
        double normalizedProgress = (m_heightProgress - 0.5) / 0.5;
        normalizedProgress = qBound(0.0, normalizedProgress, 1.0);

        // 计算当前应该显示的区域
        int totalHeight = bgRect.height();
        int totalWidth = bgRect.width();

        // 确保基准线在合理范围内
        if (relativeBaseline.y() >= -totalHeight && relativeBaseline.y() <= totalHeight * 2) {
            QRect clipRect;
            if (m_expandUpward) {
                // 向上展开：从基准线向上逐渐显示更多内容
                int revealHeight = static_cast<int>(totalHeight * (0.5 + normalizedProgress * 0.5));
                int clipBottom = qBound(0, relativeBaseline.y(), totalHeight);
                int clipTop = qMax(0, clipBottom - revealHeight);
                int clipHeight = qMin(totalHeight, clipBottom - clipTop);
                clipRect = QRect(0, clipTop, totalWidth, clipHeight);
            } else {
                // 向下展开：从基准线向下逐渐显示更多内容
                int revealHeight = static_cast<int>(totalHeight * (0.5 + normalizedProgress * 0.5));
                int clipTop = qBound(0, relativeBaseline.y(), totalHeight);
                int clipHeight = qMin(totalHeight - clipTop, revealHeight);
                clipRect = QRect(0, clipTop, totalWidth, clipHeight);
            }

            // 确保剪裁区域有效且在widget范围内
            clipRect = clipRect.intersected(bgRect);
            if (clipRect.width() > 0 && clipRect.height() > 0) {
                painter.setClipRect(clipRect);
            }
        }
    }

    // 绘制多层阴影效果，营造深度感
    if (!m_isAnimating || m_heightProgress > 0.55) {
        // 在动画早期就显示阴影，增强视觉效果
        painter.setPen(Qt::NoPen);

        // 第一层阴影（最远，最淡）
        painter.setBrush(QColor(shadowColor.red(), shadowColor.green(), shadowColor.blue(), shadowColor.alpha() / 3));
        QRect shadow1 = bgRect.adjusted(6, 6, 6, 6);
        painter.drawRoundedRect(shadow1, 8, 8);

        // 第二层阴影（中等距离）
        painter.setBrush(QColor(shadowColor.red(), shadowColor.green(), shadowColor.blue(), shadowColor.alpha() / 2));
        QRect shadow2 = bgRect.adjusted(4, 4, 4, 4);
        painter.drawRoundedRect(shadow2, 7, 7);

        // 第三层阴影（最近，最深）
        painter.setBrush(shadowColor);
        QRect shadow3 = bgRect.adjusted(2, 2, 2, 2);
        painter.drawRoundedRect(shadow3, 6, 6);
    }

    // 绘制背景 - 轻微圆角，使用当前widget的实际尺寸
    painter.setBrush(backgroundColor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(bgRect, 6, 6); // 轻微圆角

    // 绘制微妙的边框
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(borderColor, 1));
    painter.drawRoundedRect(bgRect.adjusted(0.5, 0.5, -0.5, -0.5), 6, 6);
}

void QWinUIMenuFlyout::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
    case Qt::Key_Up:
        navigateUp();
        event->accept();
        break;
    case Qt::Key_Down:
        navigateDown();
        event->accept();
        break;
    case Qt::Key_Left:
        navigateLeft();
        event->accept();
        break;
    case Qt::Key_Right:
        navigateRight();
        event->accept();
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        activateCurrentItem();
        event->accept();
        break;
    case Qt::Key_Escape:
        hide();
        event->accept();
        break;
    default:
        QWinUIWidget::keyPressEvent(event);
        break;
    }
}

void QWinUIMenuFlyout::focusOutEvent(QFocusEvent* event)
{
    if (m_autoClose && !m_currentSubMenu) {
        hide();
    }
    QWinUIWidget::focusOutEvent(event);
}

void QWinUIMenuFlyout::showEvent(QShowEvent* event)
{
    QWinUIWidget::showEvent(event);
}

void QWinUIMenuFlyout::hideEvent(QHideEvent* event)
{
    hideAllSubMenus();
    QWinUIWidget::hideEvent(event);
}

bool QWinUIMenuFlyout::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonPress && m_autoClose) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if (!geometry().contains(mouseEvent->globalPosition().toPoint())) {
            hide();
            return true;
        }
    }

    return QWinUIWidget::eventFilter(obj, event);
}

void QWinUIMenuFlyout::onThemeChanged()
{
    updateColors();
}

// 私有方法
void QWinUIMenuFlyout::updateLayout()
{
    if (m_menuItems.isEmpty()) return;

    // 计算所需的最大宽度
    int maxWidth = m_minWidth;

    for (QWidget* widget : m_menuItems) {
        QSize itemSize = widget->sizeHint();
        maxWidth = qMax(maxWidth, itemSize.width() + MENU_PADDING * 2);
    }

    maxWidth = qMin(maxWidth, m_maxWidth);

    // 设置所有菜单项的宽度
    for (QWidget* widget : m_menuItems) {
        widget->setMinimumWidth(maxWidth - MENU_PADDING * 2);
        widget->setMaximumWidth(maxWidth - MENU_PADDING * 2);
    }

    // 计算菜单总高度
    int totalHeight = MENU_PADDING * 2;
    for (QWidget* widget : m_menuItems) {
        totalHeight += widget->sizeHint().height();
    }

    // 设置菜单固定尺寸
    setFixedSize(maxWidth, totalHeight);
}

void QWinUIMenuFlyout::positionMenu(const QPoint& globalPos)
{
    QRect availableGeometry = getAvailableGeometry(globalPos);
    QSize menuSize = sizeHint();

    int x = globalPos.x();
    int y = globalPos.y();

    // 确保菜单在屏幕范围内
    if (x + menuSize.width() > availableGeometry.right()) {
        x = availableGeometry.right() - menuSize.width();
    }
    if (x < availableGeometry.left()) {
        x = availableGeometry.left();
    }

    if (y + menuSize.height() > availableGeometry.bottom()) {
        y = globalPos.y() - menuSize.height();
    }
    if (y < availableGeometry.top()) {
        y = availableGeometry.top();
    }

    move(x, y);
}

void QWinUIMenuFlyout::positionSubMenu(QWinUIMenuFlyout* subMenu, QWinUIMenuFlyoutItem* parentItem)
{
    if (!subMenu || !parentItem) return;

    QPoint parentGlobalPos = parentItem->mapToGlobal(QPoint(0, 0));
    QSize parentSize = parentItem->size();
    QSize subMenuSize = subMenu->sizeHint();

    int x = parentGlobalPos.x() + parentSize.width() + SUBMENU_OFFSET;
    int y = parentGlobalPos.y();

    QRect availableGeometry = getAvailableGeometry(QPoint(x, y));

    // 如果右侧空间不够，尝试左侧
    if (x + subMenuSize.width() > availableGeometry.right()) {
        x = parentGlobalPos.x() - subMenuSize.width() - SUBMENU_OFFSET;
    }

    // 确保垂直位置在屏幕范围内
    if (y + subMenuSize.height() > availableGeometry.bottom()) {
        y = availableGeometry.bottom() - subMenuSize.height();
    }
    if (y < availableGeometry.top()) {
        y = availableGeometry.top();
    }

    subMenu->move(x, y);
}

QRect QWinUIMenuFlyout::getAvailableGeometry(const QPoint& pos) const
{
    QScreen* screen = QApplication::screenAt(pos);
    if (screen) {
        return screen->availableGeometry();
    }
    return QApplication::primaryScreen()->availableGeometry();
}

void QWinUIMenuFlyout::calculateAnimationParameters(const QPoint& triggerPosition)
{
    QRect availableGeometry = getAvailableGeometry(triggerPosition);
    QSize menuSize = sizeHint();

    // 检查输入有效性
    if (menuSize.width() <= 0 || menuSize.height() <= 0) {
        qWarning("QWinUIMenuFlyout: Invalid menu size in calculateAnimationParameters");
        m_finalPosition = triggerPosition;
        m_expandUpward = false;
        return;
    }

    if (availableGeometry.isEmpty()) {
        qWarning("QWinUIMenuFlyout: Invalid available geometry");
        m_finalPosition = triggerPosition;
        m_expandUpward = false;
        return;
    }

    // triggerPosition现在是按钮的左下角位置
    // 菜单应该与按钮左对齐
    int x = triggerPosition.x(); // 直接使用按钮左边缘的x坐标
    int y;

    // 水平位置调整，确保菜单不超出屏幕边界
    if (x + menuSize.width() > availableGeometry.right()) {
        x = availableGeometry.right() - menuSize.width();
    }
    if (x < availableGeometry.left()) {
        x = availableGeometry.left();
    }

    // 垂直位置和展开方向计算
    // triggerPosition现在是按钮的左下角，所以triggerPosition.y()是按钮底部
    int buttonBottom = triggerPosition.y();
    int buttonTop = buttonBottom - m_triggerButtonSize.height();

    int spaceBelow = availableGeometry.bottom() - buttonBottom;
    int spaceAbove = buttonTop - availableGeometry.top();

    const int MENU_BUTTON_SPACING = 4; // 菜单与按钮的间距

    if (spaceBelow >= menuSize.height() + MENU_BUTTON_SPACING) {
        // 向下展开：菜单顶部距离按钮底部4px
        m_expandUpward = false;
        y = buttonBottom + MENU_BUTTON_SPACING;
    } else if (spaceAbove >= menuSize.height() + MENU_BUTTON_SPACING) {
        // 向上展开：菜单底部距离按钮顶部4px
        m_expandUpward = true;
        y = buttonTop - menuSize.height() - MENU_BUTTON_SPACING;
    } else {
        // 空间不足，选择空间较大的方向
        if (spaceBelow > spaceAbove) {
            m_expandUpward = false;
            y = qMax(availableGeometry.top(), availableGeometry.bottom() - menuSize.height());
        } else {
            m_expandUpward = true;
            y = qMax(availableGeometry.top(), qMin(availableGeometry.bottom() - menuSize.height(), buttonTop - menuSize.height()));
        }
    }

    m_finalPosition = QPoint(x, y);
}

void QWinUIMenuFlyout::startShowAnimation()
{
    // 首先让菜单以完整尺寸布局，以便计算正确的目标高度
    updateLayout();
    adjustSize();

    // 计算目标高度
    QSize menuSize = sizeHint();
    m_targetHeight = menuSize.height();

    // 检查尺寸有效性
    if (m_targetHeight <= 0 || menuSize.width() <= 0) {
        qWarning("QWinUIMenuFlyout: Invalid menu size, skipping animation");
        m_isAnimating = false;
        return;
    }

    // 设置初始状态：从一半进度开始
    m_heightProgress = 0.5;
    m_isAnimating = true;

    // 设置初始位置：X轴使用最终位置，Y轴从基准线开始
    QPoint initialPosition(m_finalPosition.x(),
                          m_baselinePosition.y() - menuSize.height() / 2);

    // 设置初始位置和完整尺寸
    move(initialPosition);
    resize(menuSize);

    // 停止正在运行的动画
    if (m_showAnimation && m_showAnimation->state() == QAbstractAnimation::Running) {
        m_showAnimation->stop();
    }
    if (m_heightAnimation && m_heightAnimation->state() == QAbstractAnimation::Running) {
        m_heightAnimation->stop();
    }

    // 同时启动淡入和剪裁展开动画
    if (m_showAnimation && m_heightAnimation) {
        m_showAnimation->start();
        m_heightAnimation->start();
    }
}

void QWinUIMenuFlyout::startHideAnimation()
{
    // 确保有正确的目标高度用于反向动画
    if (m_targetHeight == 0) {
        m_targetHeight = sizeHint().height();
    }

    // 开始隐藏动画
    m_isAnimating = true;

    // 停止正在运行的动画
    if (m_hideAnimation && m_hideAnimation->state() == QAbstractAnimation::Running) {
        m_hideAnimation->stop();
    }
    if (m_hideHeightAnimation && m_hideHeightAnimation->state() == QAbstractAnimation::Running) {
        m_hideHeightAnimation->stop();
    }

    // 同时启动淡出和高度收缩动画
    if (m_hideAnimation && m_hideHeightAnimation) {
        m_hideAnimation->start();
        m_hideHeightAnimation->start();
    }
}

// 键盘导航
void QWinUIMenuFlyout::navigateUp()
{
    QList<QWinUIMenuFlyoutItem*> itemList = items();
    if (itemList.isEmpty()) return;

    int newIndex = m_selectedIndex - 1;
    while (newIndex >= 0 && !itemList[newIndex]->isEnabled()) {
        newIndex--;
    }

    if (newIndex >= 0) {
        selectItem(newIndex);
    } else {
        // 循环到最后一个可用项
        newIndex = itemList.count() - 1;
        while (newIndex >= 0 && !itemList[newIndex]->isEnabled()) {
            newIndex--;
        }
        if (newIndex >= 0) {
            selectItem(newIndex);
        }
    }
}

void QWinUIMenuFlyout::navigateDown()
{
    QList<QWinUIMenuFlyoutItem*> itemList = items();
    if (itemList.isEmpty()) return;

    int newIndex = m_selectedIndex + 1;
    while (newIndex < itemList.count() && !itemList[newIndex]->isEnabled()) {
        newIndex++;
    }

    if (newIndex < itemList.count()) {
        selectItem(newIndex);
    } else {
        // 循环到第一个可用项
        newIndex = 0;
        while (newIndex < itemList.count() && !itemList[newIndex]->isEnabled()) {
            newIndex++;
        }
        if (newIndex < itemList.count()) {
            selectItem(newIndex);
        }
    }
}

void QWinUIMenuFlyout::navigateLeft()
{
    if (m_parentMenu) {
        hide();
        m_parentMenu->setFocus();
    }
}

void QWinUIMenuFlyout::navigateRight()
{
    if (m_currentItem && m_currentItem->subMenu()) {
        showSubMenu(m_currentItem);
    }
}

void QWinUIMenuFlyout::activateCurrentItem()
{
    if (m_currentItem) {
        if (m_currentItem->itemType() == QWinUIMenuFlyoutItemType::SubMenu && m_currentItem->subMenu()) {
            showSubMenu(m_currentItem);
        } else {
            emit m_currentItem->triggered();
        }
    }
}

void QWinUIMenuFlyout::selectItem(int index)
{
    QList<QWinUIMenuFlyoutItem*> itemList = items();
    if (index >= 0 && index < itemList.count()) {
        if (m_currentItem) {
            m_currentItem->clearFocus();
        }

        m_selectedIndex = index;
        m_currentItem = itemList[index];
        m_currentItem->setFocus();

        // 隐藏当前子菜单（如果不是当前项的子菜单）
        if (m_currentSubMenu && m_currentSubMenu != m_currentItem->subMenu()) {
            hideSubMenu();
        }
    }
}

// 子菜单管理
void QWinUIMenuFlyout::showSubMenu(QWinUIMenuFlyoutItem* item)
{
    if (!item || !item->subMenu()) return;

    // 隐藏当前子菜单
    if (m_currentSubMenu && m_currentSubMenu != item->subMenu()) {
        hideSubMenu();
    }

    m_currentSubMenu = item->subMenu();

    // 定位并显示子菜单
    positionSubMenu(m_currentSubMenu, item);
    m_currentSubMenu->show();
    m_currentSubMenu->setFocus();
}

void QWinUIMenuFlyout::hideSubMenu()
{
    if (m_currentSubMenu) {
        m_currentSubMenu->hide();
        m_currentSubMenu = nullptr;
    }
}

void QWinUIMenuFlyout::hideAllSubMenus()
{
    hideSubMenu();

    // 递归隐藏所有子菜单
    for (QWidget* widget : m_menuItems) {
        QWinUIMenuFlyoutItem* item = qobject_cast<QWinUIMenuFlyoutItem*>(widget);
        if (item && item->subMenu()) {
            item->subMenu()->hideAllSubMenus();
            item->subMenu()->hide();
        }
    }
}
