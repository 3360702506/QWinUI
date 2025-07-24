#include "QWinUI/Controls/QWinUIContentDialog.h"
#include "QWinUI/QWinUITheme.h"
#include <QPainter>
#include <QApplication>
#include <QEventLoop>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QTimer>
#include <QDebug>

QWinUIContentDialog::QWinUIContentDialog(QWidget* parent)
    : QWinUIWidget(parent)
    , m_title("Dialog")
    , m_isModal(true)
    , m_overlayOpacity(DEFAULT_OVERLAY_OPACITY)
    , m_dialogSize(DEFAULT_DIALOG_WIDTH, DEFAULT_DIALOG_HEIGHT)
    , m_result(None)
    , m_dialogCard(nullptr)
    , m_overlayWidget(nullptr)
    , m_mainLayout(nullptr)
    , m_buttonLayout(nullptr)
    , m_titleLabel(nullptr)
    , m_contentWidget(nullptr)
    , m_separator(nullptr)
    , m_buttonArea(nullptr)
    , m_primaryButton(nullptr)
    , m_secondaryButton(nullptr)
    , m_closeButton(nullptr)
    , m_overlayAnimation(nullptr)
    , m_cardAnimation(nullptr)
    , m_showAnimationGroup(nullptr)
    , m_hideAnimationGroup(nullptr)
    , m_cardOpacityEffect(nullptr)
{
    initializeDialog();
}

QWinUIContentDialog::~QWinUIContentDialog()
{
    // Qt会自动清理子对象
}

void QWinUIContentDialog::initializeDialog()
{
    qDebug() << "QWinUIContentDialog::initializeDialog() - 开始初始化";

    // 设置基本属性
    setWindowFlags(Qt::Widget);
    setAttribute(Qt::WA_StyledBackground, true);
    setFocusPolicy(Qt::StrongFocus);

    // 初始状态隐藏
    hide();

    // 设置布局和UI
    qDebug() << "QWinUIContentDialog::initializeDialog() - 调用setupLayout";
    setupLayout();
    qDebug() << "QWinUIContentDialog::initializeDialog() - 调用setupAnimations";
    setupAnimations();

    // 更新主题
    qDebug() << "QWinUIContentDialog::initializeDialog() - 更新主题";
    updateSeparatorColors();

    qDebug() << "QWinUIContentDialog::initializeDialog() - 初始化完成";
}

void QWinUIContentDialog::setupLayout()
{
    qDebug() << "QWinUIContentDialog::setupLayout() - 开始设置布局";

    // 设置ContentDialog为全屏覆盖，用于显示遮罩
    setGeometry(0, 0, parentWidget()->width(), parentWidget()->height());

    // 不需要主布局，我们手动管理位置
    qDebug() << "QWinUIContentDialog::setupLayout() - 设置全屏覆盖完成";

    // 创建对话框卡片，作为子控件
    m_dialogCard = new QWinUISimpleCard(this);
    m_dialogCard->setFixedSize(m_dialogSize);
    m_dialogCard->setCornerRadius(8);
    qDebug() << "QWinUIContentDialog::setupLayout() - 对话框卡片创建完成，大小:" << m_dialogSize;
    
    // 创建所有子控件，使用SimpleCard的addWidget方法
    qDebug() << "QWinUIContentDialog::setupLayout() - 开始创建子控件";

    // 标题区域
    m_titleLabel = new QLabel(m_title);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet("font-size: 16px; font-weight: bold;");
    m_titleLabel->setFixedHeight(TITLE_HEIGHT);
    m_dialogCard->addWidget(m_titleLabel);
    qDebug() << "QWinUIContentDialog::setupLayout() - 标题标签创建完成";

    // 内容区域
    m_contentWidget = new QWidget();
    m_dialogCard->addWidget(m_contentWidget);
    qDebug() << "QWinUIContentDialog::setupLayout() - 内容区域创建完成";

    // 分隔线
    m_separator = new QFrame();
    m_separator->setFrameShape(QFrame::HLine);
    m_separator->setFixedHeight(1);
    m_dialogCard->addWidget(m_separator);
    qDebug() << "QWinUIContentDialog::setupLayout() - 分隔线创建完成";

    // 按钮区域
    m_buttonArea = new QWidget();
    m_buttonArea->setFixedHeight(BUTTON_HEIGHT + BUTTON_MARGIN * 2);

    m_buttonLayout = new QHBoxLayout(m_buttonArea);
    m_buttonLayout->setContentsMargins(BUTTON_MARGIN, BUTTON_MARGIN, BUTTON_MARGIN, BUTTON_MARGIN);
    m_buttonLayout->setSpacing(8);
    qDebug() << "QWinUIContentDialog::setupLayout() - 按钮布局创建完成";

    // 创建按钮
    m_primaryButton = new QWinUIButton("确定", m_buttonArea);
    m_primaryButton->setButtonStyle(QWinUIButton::Accent);
    m_primaryButton->hide(); // 默认隐藏
    qDebug() << "QWinUIContentDialog::setupLayout() - 主按钮创建完成";

    m_secondaryButton = new QWinUIButton("取消", m_buttonArea);
    m_secondaryButton->setButtonStyle(QWinUIButton::Standard);
    m_secondaryButton->hide(); // 默认隐藏
    qDebug() << "QWinUIContentDialog::setupLayout() - 次要按钮创建完成";

    m_closeButton = new QWinUIButton("关闭", m_buttonArea);
    m_closeButton->setButtonStyle(QWinUIButton::Standard);
    qDebug() << "QWinUIContentDialog::setupLayout() - 关闭按钮创建完成";

    // 按钮布局
    m_buttonLayout->addStretch();
    m_buttonLayout->addWidget(m_primaryButton);
    m_buttonLayout->addWidget(m_secondaryButton);
    m_buttonLayout->addWidget(m_closeButton);
    qDebug() << "QWinUIContentDialog::setupLayout() - 按钮添加到布局完成";

    // 将按钮区域添加到卡片
    m_dialogCard->addWidget(m_buttonArea);
    qDebug() << "QWinUIContentDialog::setupLayout() - 按钮区域添加到卡片完成";

    qDebug() << "QWinUIContentDialog::setupLayout() - 布局设置完成";
    
    // 连接信号
    connect(m_primaryButton, &QWinUIButton::clicked, this, &QWinUIContentDialog::onPrimaryButtonClicked);
    connect(m_secondaryButton, &QWinUIButton::clicked, this, &QWinUIContentDialog::onSecondaryButtonClicked);
    connect(m_closeButton, &QWinUIButton::clicked, this, &QWinUIContentDialog::onCloseButtonClicked);
}

void QWinUIContentDialog::setupAnimations()
{
    // 暂时完全跳过动画设置
    m_cardOpacityEffect = nullptr;
    m_overlayAnimation = nullptr;
    m_cardAnimation = nullptr;
    m_showAnimationGroup = nullptr;
    m_hideAnimationGroup = nullptr;
}

void QWinUIContentDialog::setTitle(const QString& title)
{
    if (m_title != title) {
        m_title = title;
        if (m_titleLabel) {
            m_titleLabel->setText(title);
        }
        emit titleChanged(title);
    }
}

void QWinUIContentDialog::setModal(bool modal)
{
    if (m_isModal != modal) {
        m_isModal = modal;
        updateOverlay();
        emit modalChanged(modal);
    }
}

void QWinUIContentDialog::setOverlayOpacity(double opacity)
{
    m_overlayOpacity = qBound(0.0, opacity, 1.0);
    updateOverlay();
}

void QWinUIContentDialog::setPrimaryButtonText(const QString& text)
{
    if (m_primaryButton) {
        m_primaryButton->setText(text);
        m_primaryButton->setVisible(!text.isEmpty());
    }
}

void QWinUIContentDialog::setSecondaryButtonText(const QString& text)
{
    if (m_secondaryButton) {
        m_secondaryButton->setText(text);
        m_secondaryButton->setVisible(!text.isEmpty());
    }
}

void QWinUIContentDialog::setCloseButtonText(const QString& text)
{
    if (m_closeButton) {
        m_closeButton->setText(text);
        m_closeButton->setVisible(!text.isEmpty());
    }
}

QString QWinUIContentDialog::primaryButtonText() const
{
    return m_primaryButton ? m_primaryButton->text() : QString();
}

QString QWinUIContentDialog::secondaryButtonText() const
{
    return m_secondaryButton ? m_secondaryButton->text() : QString();
}

QString QWinUIContentDialog::closeButtonText() const
{
    return m_closeButton ? m_closeButton->text() : QString();
}

void QWinUIContentDialog::setPrimaryButtonEnabled(bool enabled)
{
    if (m_primaryButton) {
        m_primaryButton->setEnabled(enabled);
    }
}

void QWinUIContentDialog::setSecondaryButtonEnabled(bool enabled)
{
    if (m_secondaryButton) {
        m_secondaryButton->setEnabled(enabled);
    }
}

void QWinUIContentDialog::setCloseButtonEnabled(bool enabled)
{
    if (m_closeButton) {
        m_closeButton->setEnabled(enabled);
    }
}

void QWinUIContentDialog::showDialog()
{
    if (isVisible()) return;

    // 直接显示对话框，不使用动画
    show();
    raise();
    setFocus();
}

void QWinUIContentDialog::hideDialog()
{
    if (!isVisible()) return;

    // 直接隐藏对话框，不使用动画
    hide();
}

QWinUIContentDialog::DialogResult QWinUIContentDialog::exec()
{
    if (!m_isModal) {
        setModal(true);
    }

    m_result = None;
    showDialog();

    // 创建事件循环等待对话框关闭
    QEventLoop loop;
    connect(this, &QWinUIContentDialog::dialogClosed, &loop, &QEventLoop::quit);
    loop.exec();

    return m_result;
}

void QWinUIContentDialog::setDialogSize(const QSize& size)
{
    m_dialogSize = size;
    if (m_dialogCard) {
        m_dialogCard->setFixedSize(size);
        updateDialogPosition();
    }
}

QSize QWinUIContentDialog::dialogSize() const
{
    return m_dialogSize;
}

QSize QWinUIContentDialog::dialogCardSize() const
{
    return m_dialogCard ? m_dialogCard->size() : m_dialogSize;
}

void QWinUIContentDialog::setDialogCardSize(const QSize& size)
{
    if (m_dialogCard) {
        m_dialogCard->setFixedSize(size);
        updateDialogPosition();
    }
}

// 事件处理
void QWinUIContentDialog::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制半透明遮罩背景
    if (m_isModal && isVisible()) {
        QColor overlayColor = getOverlayColor();
        overlayColor.setAlphaF(m_overlayOpacity);
        painter.fillRect(rect(), overlayColor);
    }

    QWidget::paintEvent(event);
}

void QWinUIContentDialog::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    updateDialogPosition();
}

void QWinUIContentDialog::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    updateDialogPosition();
}

void QWinUIContentDialog::hideEvent(QHideEvent* event)
{
    QWinUIWidget::hideEvent(event);
}

void QWinUIContentDialog::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape && m_isModal) {
        onCloseButtonClicked();
        return;
    }
    QWinUIWidget::keyPressEvent(event);
}

void QWinUIContentDialog::mousePressEvent(QMouseEvent* event)
{
    if (m_isModal && !m_dialogCard->geometry().contains(event->pos())) {
        // 点击遮罩区域时关闭对话框（可选行为）
        // onCloseButtonClicked();
        return;
    }
    QWinUIWidget::mousePressEvent(event);
}

void QWinUIContentDialog::onThemeChanged()
{
    updateSeparatorColors();
    update();
}

// 私有槽函数
void QWinUIContentDialog::onPrimaryButtonClicked()
{
    m_result = Primary;
    emit primaryButtonClicked();
    emit dialogClosed(m_result);
    hideDialog();
}

void QWinUIContentDialog::onSecondaryButtonClicked()
{
    m_result = Secondary;
    emit secondaryButtonClicked();
    emit dialogClosed(m_result);
    hideDialog();
}

void QWinUIContentDialog::onCloseButtonClicked()
{
    m_result = Close;
    emit closeButtonClicked();
    emit dialogClosed(m_result);
    hideDialog();
}

void QWinUIContentDialog::onShowAnimationFinished()
{
    // 显示动画完成
}

void QWinUIContentDialog::onHideAnimationFinished()
{
    // 隐藏动画完成，隐藏对话框
    hide();
}

// 私有方法
void QWinUIContentDialog::updateOverlay()
{
    qDebug() << "QWinUIContentDialog::updateOverlay() - 开始";

    if (m_overlayWidget) {
        qDebug() << "QWinUIContentDialog::updateOverlay() - 设置遮罩可见性:" << (m_isModal && isVisible());
        m_overlayWidget->setVisible(m_isModal && isVisible());
    } else {
        qDebug() << "QWinUIContentDialog::updateOverlay() - m_overlayWidget为空";
    }

    qDebug() << "QWinUIContentDialog::updateOverlay() - 调用update()";
    update();
    qDebug() << "QWinUIContentDialog::updateOverlay() - 完成";
}

void QWinUIContentDialog::updateDialogPosition()
{
    if (!m_dialogCard || !parentWidget()) {
        return;
    }

    // 首先确保ContentDialog覆盖整个父控件
    QRect parentRect = parentWidget()->rect();
    setGeometry(parentRect);

    // 将对话框卡片居中显示在ContentDialog中
    QSize dialogSize = size();
    QSize cardSize = m_dialogCard->size();

    int x = (dialogSize.width() - cardSize.width()) / 2;
    int y = (dialogSize.height() - cardSize.height()) / 2;

    m_dialogCard->move(x, y);
}

void QWinUIContentDialog::updateDialogLayout()
{
    qDebug() << "QWinUIContentDialog::updateDialogLayout() - 开始";

    if (!m_dialogCard) {
        qDebug() << "QWinUIContentDialog::updateDialogLayout() - m_dialogCard为空";
        return;
    }
    if (!m_titleLabel) {
        qDebug() << "QWinUIContentDialog::updateDialogLayout() - m_titleLabel为空";
        return;
    }
    if (!m_contentWidget) {
        qDebug() << "QWinUIContentDialog::updateDialogLayout() - m_contentWidget为空";
        return;
    }
    if (!m_separator) {
        qDebug() << "QWinUIContentDialog::updateDialogLayout() - m_separator为空";
        return;
    }
    if (!m_buttonArea) {
        qDebug() << "QWinUIContentDialog::updateDialogLayout() - m_buttonArea为空";
        return;
    }

    QSize cardSize = m_dialogCard->size();
    qDebug() << "QWinUIContentDialog::updateDialogLayout() - 卡片大小:" << cardSize;

    if (cardSize.width() <= 0 || cardSize.height() <= 0) {
        qDebug() << "QWinUIContentDialog::updateDialogLayout() - 卡片大小无效";
        return;
    }

    int currentY = 0;

    // 标题区域
    qDebug() << "QWinUIContentDialog::updateDialogLayout() - 设置标题区域";
    m_titleLabel->setGeometry(0, currentY, cardSize.width(), TITLE_HEIGHT);
    currentY += TITLE_HEIGHT;

    // 按钮区域高度
    int buttonAreaHeight = BUTTON_HEIGHT + BUTTON_MARGIN * 2;

    // 内容区域（占用剩余空间，减去分隔线高度）
    int contentHeight = cardSize.height() - currentY - 1 - buttonAreaHeight; // 1是分隔线高度
    qDebug() << "QWinUIContentDialog::updateDialogLayout() - 内容区域高度:" << contentHeight;

    if (contentHeight > 0) {
        qDebug() << "QWinUIContentDialog::updateDialogLayout() - 设置内容区域";
        m_contentWidget->setGeometry(0, currentY, cardSize.width(), contentHeight);
        currentY += contentHeight;
    }

    // 分隔线
    qDebug() << "QWinUIContentDialog::updateDialogLayout() - 设置分隔线";
    m_separator->setGeometry(0, currentY, cardSize.width(), 1);
    currentY += 1;

    // 按钮区域
    qDebug() << "QWinUIContentDialog::updateDialogLayout() - 设置按钮区域";
    m_buttonArea->setGeometry(0, currentY, cardSize.width(), buttonAreaHeight);

    qDebug() << "QWinUIContentDialog::updateDialogLayout() - 完成";
}

void QWinUIContentDialog::updateSeparatorColors()
{
    if (!m_separator) return;

    QColor separatorColor = getSeparatorColor();
    QString styleSheet = QString("QFrame { background-color: %1; border: none; }")
                        .arg(separatorColor.name(QColor::HexArgb));
    m_separator->setStyleSheet(styleSheet);
}

void QWinUIContentDialog::startShowAnimation()
{
    // 简化的显示动画，只使用透明度
    if (m_cardAnimation && m_cardOpacityEffect) {
        m_cardOpacityEffect->setOpacity(0.0);
        m_cardAnimation->setStartValue(0.0);
        m_cardAnimation->setEndValue(1.0);
        m_cardAnimation->start();
    }
}

void QWinUIContentDialog::startHideAnimation()
{
    // 简化的隐藏动画，只使用透明度
    if (m_cardAnimation && m_cardOpacityEffect) {
        m_cardAnimation->setStartValue(1.0);
        m_cardAnimation->setEndValue(0.0);
        m_cardAnimation->start();
    } else {
        hide();
    }
}

// 颜色获取方法
QColor QWinUIContentDialog::getOverlayColor() const
{
    bool isDark = isDarkMode();
    return isDark ? QColor(0, 0, 0) : QColor(0, 0, 0);
}

QColor QWinUIContentDialog::getSeparatorColor() const
{
    QWinUITheme* theme = QWinUITheme::getInstance();
    if (theme) {
        return theme->controlStrokeColorDefault();
    }

    bool isDark = isDarkMode();
    return isDark ? QColor(255, 255, 255, 18) : QColor(117, 117, 117, 102);
}
