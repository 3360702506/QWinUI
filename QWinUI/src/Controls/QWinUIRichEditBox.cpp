#include "QWinUI/Controls/QWinUIRichEditBox.h"
#include "QWinUI/Controls/QWinUIScrollBar.h"
#include "QWinUI/QWinUITheme.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollBar>
#include <QApplication>
#include <QPainter>
#include <QPaintEvent>
#include <QPainterPath>

QWinUIRichEditBox::QWinUIRichEditBox(QWidget* parent)
    : QWinUIWidget(parent)
    , m_textEdit(nullptr)
    , m_underlineAnimation(nullptr)
    , m_borderColorAnimation(nullptr)
    , m_hasFocus(false)
    , m_isHovered(false)
    , m_underlineProgress(0.0)
    , m_maxLength(-1)
{
    initializeComponent();
}

QWinUIRichEditBox::~QWinUIRichEditBox()
{
    // 动画会被Qt的父子关系自动清理
}

void QWinUIRichEditBox::initializeComponent()
{
    // 设置基本属性
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_Hover);
    setMinimumHeight(32);
    // 不设置最大高度限制，允许自由调整大小

    // 创建文本编辑器
    m_textEdit = new QTextEdit(this);
    m_textEdit->setFrameStyle(QFrame::NoFrame);
    m_textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 禁用内置滚动条

    // 创建自定义滚动条 - 使用绝对定位，不占用布局空间
    m_customScrollBar = new QWinUIScrollBar(Qt::Vertical, this);
    m_customScrollBar->setAutoHide(true);
    m_customScrollBar->hide(); // 默认隐藏
    m_customScrollBar->setFixedWidth(8); // 更细的滚动条

    // 设置布局 - 只放置文本编辑器
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(2, 2, 2, 3);
    mainLayout->setSpacing(0);

    // 文本编辑器占全部空间
    mainLayout->addWidget(m_textEdit);

    // 设置动画
    m_underlineAnimation = new QPropertyAnimation(this, "underlineProgress", this);
    m_underlineAnimation->setDuration(200);
    m_underlineAnimation->setEasingCurve(QEasingCurve::OutCubic);

    // 连接动画值变化信号，确保重绘
    connect(m_underlineAnimation, &QPropertyAnimation::valueChanged, this, [this]() {
        update();
    });

    // 初始化颜色
    updateColors();

    // 连接信号
    connect(m_textEdit, &QTextEdit::textChanged, this, &QWinUIRichEditBox::onTextChanged);

    // 连接滚动条信号
    setupScrollBarConnections();

    // 安装事件过滤器来监听QTextEdit的焦点事件
    m_textEdit->installEventFilter(this);

    // 连接主题变化信号
    auto theme = QWinUITheme::getInstance();
    if (theme) {
        connect(theme, &QWinUITheme::themeChanged, this, &QWinUIRichEditBox::updateColors);
    }
}

void QWinUIRichEditBox::updateColors()
{
    auto theme = QWinUITheme::getInstance();
    if (!theme) return;

    if (theme->isDarkMode()) {
        // 深色主题 - WinUI3标准颜色
        m_borderColor = QColor(80, 80, 80);           // 默认边框（更灰）
        m_hoverBorderColor = QColor(128, 128, 128);   // 悬停边框
        m_focusBorderColor = theme->accentColor();    // 聚焦边框
        m_backgroundColor = QColor(32, 32, 32);       // 背景色
        m_textColor = QColor(255, 255, 255);          // 文本色
    } else {
        // 浅色主题 - WinUI3标准颜色
        m_borderColor = QColor(220, 220, 220);        // 默认边框（更灰）
        m_hoverBorderColor = QColor(160, 160, 160);   // 悬停边框
        m_focusBorderColor = theme->accentColor();    // 聚焦边框
        m_backgroundColor = QColor(255, 255, 255);    // 背景色
        m_textColor = QColor(0, 0, 0);                // 文本色
    }

    // 应用样式到文本编辑器，包括WinUI风格的滚动条
    QString scrollBarHandleColor = theme->isDarkMode() ? "rgba(255, 255, 255, 0.3)" : "rgba(0, 0, 0, 0.3)";
    QString scrollBarHandleHoverColor = theme->isDarkMode() ? "rgba(255, 255, 255, 0.5)" : "rgba(0, 0, 0, 0.5)";

    QString textEditStyle = QString(
        "QTextEdit {"
        "    background-color: %1;"
        "    color: %2;"
        "    border: none;"
        "    padding: 8px 12px;"
        "    selection-background-color: %3;"
        "    selection-color: white;"
        "}"
        "QScrollBar:vertical {"
        "    background: transparent;"
        "    width: 8px;"
        "    border: none;"
        "    margin: 0px;"
        "}"
        "QScrollBar::handle:vertical {"
        "    background: %4;"
        "    border-radius: 4px;"
        "    min-height: 20px;"
        "    margin: 0px 2px;"
        "    width: 4px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "    background: %5;"
        "    width: 6px;"
        "    margin: 0px 1px;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "    height: 0px;"
        "    background: transparent;"
        "}"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
        "    background: transparent;"
        "}"
    ).arg(m_backgroundColor.name())
     .arg(m_textColor.name())
     .arg(m_focusBorderColor.name())
     .arg(scrollBarHandleColor)
     .arg(scrollBarHandleHoverColor);

    m_textEdit->setStyleSheet(textEditStyle);
    update();
}

// 事件处理
void QWinUIRichEditBox::paintEvent(QPaintEvent* event)
{
    // 首先调用父类的paintEvent来处理主题切换动画
    QWinUIWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect = this->rect();

    // 只有在没有主题切换动画时才绘制背景
    if (!isThemeTransitioning()) {
        painter.fillRect(rect, m_backgroundColor);
    } else {
        // 在主题切换动画期间，确保内部 QTextEdit 的背景透明
        if (m_textEdit) {
            m_textEdit->setStyleSheet("QTextEdit { background: transparent; border: none; }");
        }
    }

    // 绘制完整的圆角边框
    QColor borderColor = m_borderColor;
    if (m_isHovered && !m_hasFocus) {
        borderColor = m_hoverBorderColor;
    }

    QPen borderPen(borderColor, 1);
    painter.setPen(borderPen);
    painter.setBrush(Qt::NoBrush);

    // 绘制完整的圆角矩形边框
    int radius = 4; // WinUI3标准圆角
    QRectF borderRect = rect.adjusted(0.5, 0.5, -0.5, -0.5); // 调整半像素对齐
    painter.drawRoundedRect(borderRect, radius, radius);

    // 绘制底部线条
    paintBottomLine(&painter);
}

void QWinUIRichEditBox::paintBottomLine(QPainter* painter)
{
    QRect rect = this->rect();
    int y = rect.bottom() - 2; // 底部位置，稍微向上一点避免与边框重叠

    // 总是绘制灰色底线作为基础，但要考虑圆角
    QPen grayPen(m_borderColor, 1);
    painter->setPen(grayPen);
    int margin = 4; // 圆角半径，避免线条超出圆角
    painter->drawLine(rect.left() + margin, y, rect.right() - margin, y);

    // 如果有动画进度（聚焦或失焦动画中），绘制蓝色动画线
    if (m_underlineProgress > 0.0) {
        int centerX = rect.width() / 2;
        int maxWidth = rect.width() - 4; // 减少边距，让线条更长
        int currentWidth = static_cast<int>(maxWidth * m_underlineProgress);
        int startX = centerX - currentWidth / 2;
        int endX = centerX + currentWidth / 2;

        // 绘制蓝色聚焦线，使用矩形绘制加宽的线条
        painter->setPen(Qt::NoPen);
        painter->setBrush(QBrush(m_focusBorderColor));

        int lineHeight = 3; // 加宽的线条高度
        QRect focusRect(startX, y - lineHeight/2, endX - startX, lineHeight);
        painter->drawRoundedRect(focusRect, 1.5, 1.5); // 稍大的圆角
    }
}

void QWinUIRichEditBox::resizeEvent(QResizeEvent* event)
{
    QWinUIWidget::resizeEvent(event);

    // 重新定位滚动条到右侧
    if (m_customScrollBar) {
        int scrollBarWidth = 8;
        int margin = 4; // 距离边缘的距离
        int scrollBarHeight = height() - 20; // 比控件高度小一些
        int scrollBarY = 10; // 距离顶部的距离

        m_customScrollBar->setGeometry(
            width() - scrollBarWidth - margin,
            scrollBarY,
            scrollBarWidth,
            scrollBarHeight
        );
    }
}

void QWinUIRichEditBox::focusInEvent(QFocusEvent* event)
{
    QWinUIWidget::focusInEvent(event);
    m_hasFocus = true;

    // 将焦点传递给文本编辑器
    if (m_textEdit) {
        m_textEdit->setFocus();
    }

    // 启动下划线展开动画
    if (m_underlineAnimation) {
        m_underlineAnimation->stop();
        m_underlineAnimation->setStartValue(0.0);
        m_underlineAnimation->setEndValue(1.0);
        m_underlineAnimation->start();
    }

    update();
    emit focusIn();
}

void QWinUIRichEditBox::focusOutEvent(QFocusEvent* event)
{
    QWinUIWidget::focusOutEvent(event);
    m_hasFocus = false;

    // 启动下划线收缩动画
    if (m_underlineAnimation) {
        m_underlineAnimation->stop();
        m_underlineAnimation->setStartValue(m_underlineProgress);
        m_underlineAnimation->setEndValue(0.0);
        m_underlineAnimation->start();
    }

    update();
    emit focusOut();
}

void QWinUIRichEditBox::enterEvent(QEnterEvent* event)
{
    QWinUIWidget::enterEvent(event);
    m_isHovered = true;
    update();
}

void QWinUIRichEditBox::leaveEvent(QEvent* event)
{
    QWinUIWidget::leaveEvent(event);
    m_isHovered = false;
    update();
}

// 文本属性实现
QString QWinUIRichEditBox::text() const
{
    return m_textEdit ? m_textEdit->toPlainText() : QString();
}

void QWinUIRichEditBox::setText(const QString& text)
{
    if (m_textEdit) {
        m_textEdit->setPlainText(text);
        emit textChanged();
    }
}

QString QWinUIRichEditBox::placeholderText() const
{
    return m_textEdit ? m_textEdit->placeholderText() : QString();
}

void QWinUIRichEditBox::setPlaceholderText(const QString& text)
{
    if (m_textEdit) {
        m_textEdit->setPlaceholderText(text);
        emit placeholderTextChanged(text);
    }
}

bool QWinUIRichEditBox::isReadOnly() const
{
    return m_textEdit ? m_textEdit->isReadOnly() : false;
}

void QWinUIRichEditBox::setReadOnly(bool readOnly)
{
    if (m_textEdit && m_textEdit->isReadOnly() != readOnly) {
        m_textEdit->setReadOnly(readOnly);
        emit readOnlyChanged(readOnly);
    }
}

int QWinUIRichEditBox::maxLength() const
{
    return m_maxLength;
}

void QWinUIRichEditBox::setMaxLength(int length)
{
    if (m_maxLength != length) {
        m_maxLength = length;
        emit maxLengthChanged(length);
    }
}

bool QWinUIRichEditBox::acceptRichText() const
{
    return m_textEdit ? m_textEdit->acceptRichText() : true;
}

void QWinUIRichEditBox::setAcceptRichText(bool accept)
{
    if (m_textEdit && m_textEdit->acceptRichText() != accept) {
        m_textEdit->setAcceptRichText(accept);
        emit acceptRichTextChanged(accept);
    }
}

// 外观属性实现
QColor QWinUIRichEditBox::borderColor() const
{
    return m_borderColor;
}

void QWinUIRichEditBox::setBorderColor(const QColor& color)
{
    if (m_borderColor != color) {
        m_borderColor = color;
        update();
        emit borderColorChanged(color);
    }
}

QColor QWinUIRichEditBox::focusBorderColor() const
{
    return m_focusBorderColor;
}

void QWinUIRichEditBox::setFocusBorderColor(const QColor& color)
{
    if (m_focusBorderColor != color) {
        m_focusBorderColor = color;
        if (m_hasFocus) {
            update();
        }
        emit focusBorderColorChanged(color);
    }
}

double QWinUIRichEditBox::underlineProgress() const
{
    return m_underlineProgress;
}

void QWinUIRichEditBox::setUnderlineProgress(double progress)
{
    progress = qBound(0.0, progress, 1.0);
    if (!qFuzzyCompare(m_underlineProgress, progress)) {
        m_underlineProgress = progress;
        update();
    }
}



// 富文本格式化方法 - 简化实现
void QWinUIRichEditBox::setFontBold(bool bold)
{
    if (m_textEdit) {
        m_textEdit->setFontWeight(bold ? QFont::Bold : QFont::Normal);
    }
}

void QWinUIRichEditBox::setFontItalic(bool italic)
{
    if (m_textEdit) {
        m_textEdit->setFontItalic(italic);
    }
}

void QWinUIRichEditBox::setFontUnderline(bool underline)
{
    if (m_textEdit) {
        m_textEdit->setFontUnderline(underline);
    }
}

void QWinUIRichEditBox::setFontSize(int size)
{
    if (m_textEdit && size > 0) {
        m_textEdit->setFontPointSize(size);
    }
}

void QWinUIRichEditBox::setFontFamily(const QString& family)
{
    if (m_textEdit && !family.isEmpty()) {
        m_textEdit->setFontFamily(family);
    }
}

void QWinUIRichEditBox::setTextColor(const QColor& color)
{
    if (m_textEdit && color.isValid()) {
        m_textEdit->setTextColor(color);
    }
}

void QWinUIRichEditBox::setTextBackgroundColor(const QColor& color)
{
    if (m_textEdit && color.isValid()) {
        m_textEdit->setTextBackgroundColor(color);
    }
}

// 获取当前格式 - 简化实现
bool QWinUIRichEditBox::isFontBold() const
{
    return m_textEdit ? m_textEdit->fontWeight() == QFont::Bold : false;
}

bool QWinUIRichEditBox::isFontItalic() const
{
    return m_textEdit ? m_textEdit->fontItalic() : false;
}

bool QWinUIRichEditBox::isFontUnderline() const
{
    return m_textEdit ? m_textEdit->fontUnderline() : false;
}

int QWinUIRichEditBox::fontSize() const
{
    return m_textEdit ? static_cast<int>(m_textEdit->fontPointSize()) : 0;
}

QString QWinUIRichEditBox::fontFamily() const
{
    return m_textEdit ? m_textEdit->fontFamily() : QString();
}

QColor QWinUIRichEditBox::textColor() const
{
    return m_textEdit ? m_textEdit->textColor() : QColor();
}

QColor QWinUIRichEditBox::textBackgroundColor() const
{
    return m_textEdit ? m_textEdit->textBackgroundColor() : QColor();
}

// 便利方法
void QWinUIRichEditBox::selectAll()
{
    if (m_textEdit) m_textEdit->selectAll();
}

void QWinUIRichEditBox::copy()
{
    if (m_textEdit) m_textEdit->copy();
}

void QWinUIRichEditBox::cut()
{
    if (m_textEdit) m_textEdit->cut();
}

void QWinUIRichEditBox::paste()
{
    if (m_textEdit) m_textEdit->paste();
}

void QWinUIRichEditBox::undo()
{
    if (m_textEdit) m_textEdit->undo();
}

void QWinUIRichEditBox::redo()
{
    if (m_textEdit) m_textEdit->redo();
}

void QWinUIRichEditBox::clear()
{
    if (m_textEdit) m_textEdit->clear();
}

// 尺寸提示
QSize QWinUIRichEditBox::sizeHint() const
{
    return QSize(250, 100);
}

QSize QWinUIRichEditBox::minimumSizeHint() const
{
    return QSize(100, 32);
}

// 槽函数
void QWinUIRichEditBox::onTextChanged()
{
    emit textChanged();
}

void QWinUIRichEditBox::onSelectionChanged()
{
    emit selectionChanged();
}

void QWinUIRichEditBox::onCurrentCharFormatChanged(const QTextCharFormat& format)
{
    emit currentCharFormatChanged(format);
}

void QWinUIRichEditBox::onFocusAnimationFinished()
{
    // 动画完成
}

void QWinUIRichEditBox::updatePlaceholderVisibility()
{
    // 占位符可见性更新
}

bool QWinUIRichEditBox::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == m_textEdit) {
        if (event->type() == QEvent::FocusIn) {
            if (!m_hasFocus) {
                m_hasFocus = true;

                // 启动下划线展开动画
                if (m_underlineAnimation) {
                    m_underlineAnimation->stop();
                    m_underlineAnimation->setStartValue(0.0);
                    m_underlineAnimation->setEndValue(1.0);
                    m_underlineAnimation->start();
                }

                update();
                emit focusIn();
            }
        } else if (event->type() == QEvent::FocusOut) {
            if (m_hasFocus) {
                m_hasFocus = false;

                // 启动下划线收缩动画
                if (m_underlineAnimation) {
                    m_underlineAnimation->stop();
                    m_underlineAnimation->setStartValue(m_underlineProgress);
                    m_underlineAnimation->setEndValue(0.0);
                    m_underlineAnimation->start();
                }

                update();
                emit focusOut();
            }
        }
    }

    return QWinUIWidget::eventFilter(obj, event);
}

void QWinUIRichEditBox::setupScrollBarConnections()
{
    if (!m_textEdit || !m_customScrollBar) return;

    // 获取QTextEdit的内部滚动条来同步数值
    QScrollBar* textScrollBar = m_textEdit->verticalScrollBar();

    // 连接QTextEdit滚动条的变化到自定义滚动条
    connect(textScrollBar, &QScrollBar::rangeChanged, this, [this](int min, int max) {
        m_customScrollBar->setMinimum(min);
        m_customScrollBar->setMaximum(max);
        bool needScrollBar = (max > min);

        if (needScrollBar) {
            m_customScrollBar->show();
            m_customScrollBar->raise(); // 确保在最上层
        } else {
            m_customScrollBar->hide();
        }
    });

    connect(textScrollBar, &QScrollBar::valueChanged, this, [this](int value) {
        m_customScrollBar->setValue(value);
    });

    // QScrollBar没有pageStepChanged信号，我们手动同步
    // connect(textScrollBar, &QScrollBar::pageStepChanged, this, [this](int step) {
    //     m_customScrollBar->setPageStep(step);
    // });

    // 连接自定义滚动条的变化到QTextEdit
    connect(m_customScrollBar, &QWinUIScrollBar::valueChanged, this, [this](int value) {
        m_textEdit->verticalScrollBar()->setValue(value);
    });

    // 初始同步
    m_customScrollBar->setMinimum(textScrollBar->minimum());
    m_customScrollBar->setMaximum(textScrollBar->maximum());
    m_customScrollBar->setValue(textScrollBar->value());
    m_customScrollBar->setPageStep(textScrollBar->pageStep());
    m_customScrollBar->setVisible(textScrollBar->maximum() > textScrollBar->minimum());
}

void QWinUIRichEditBox::onThemeChanged()
{
    updateColors();

    // 主题切换完成后，恢复 QTextEdit 的正常样式
    if (m_textEdit) {
        QString styleSheet = QString(
            "QTextEdit {"
            "    background-color: %1;"
            "    color: %2;"
            "    border: none;"
            "    selection-background-color: %3;"
            "}"
        ).arg(m_backgroundColor.name())
         .arg(m_textColor.name())
         .arg(QWinUITheme::getInstance()->accentColor().name());

        m_textEdit->setStyleSheet(styleSheet);
    }
}
