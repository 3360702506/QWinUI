#include "QWinUI/Controls/QWinUITextInput.h"
#include "QWinUI/Controls/QWinUIRichEditBox.h"
#include "QWinUI/QWinUITheme.h"
#include <QPainter>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QFocusEvent>
#include <QInputMethodEvent>
#include <QApplication>
#include <QClipboard>
#include <QFontMetrics>
#include <QTextLayout>
#include <QDebug>

// QWinUITextCommand 实现
QWinUITextCommand::QWinUITextCommand(Type type, int position, const QString& text, const QTextCharFormat& format)
    : m_type(type), m_position(position), m_text(text), m_format(format), m_textInput(nullptr)
{
}

void QWinUITextCommand::setTextInput(QWinUITextInput* textInput)
{
    m_textInput = textInput;
}

void QWinUITextCommand::undo()
{
    if (!m_textInput) return;

    switch (m_type) {
    case Insert:
        // 直接访问私有方法，因为是友元类
        m_textInput->deleteText(m_position, m_text.length());
        break;
    case Delete:
        m_textInput->m_text.insert(m_position, m_text);
        m_textInput->setCursorPosition(m_position + m_text.length());
        break;
    case Format:
        // TODO: 实现格式撤销
        break;
    }
}

void QWinUITextCommand::redo()
{
    if (!m_textInput) return;

    switch (m_type) {
    case Insert:
        m_textInput->setCursorPosition(m_position);
        m_textInput->m_text.insert(m_position, m_text);
        break;
    case Delete:
        m_textInput->deleteText(m_position, m_text.length());
        break;
    case Format:
        // TODO: 实现格式重做
        break;
    }
}

// QWinUITextInput 实现
QWinUITextInput::QWinUITextInput(QWidget* parent)
    : QWinUIWidget(parent)
    , m_cursorPosition(0)
    , m_selectionStart(-1)
    , m_selectionEnd(-1)
    , m_cursorVisible(true)
    , m_cursorTimer(nullptr)
    , m_readOnly(false)
    , m_multiLine(false)
    , m_richTextEnabled(false)
    , m_maxLength(-1)
    , m_selecting(false)
    , m_undoStack(nullptr)
    , m_layoutDirty(true)
    , m_horizontalOffset(0)
    , m_cursorOpacity(1.0)
    , m_cursorAnimation(nullptr)
    , m_cursorVisibilityTimer(nullptr)
    , m_isTyping(false)
{
    initializeTextInput();
}

QWinUITextInput::~QWinUITextInput()
{
    if (m_cursorTimer) {
        m_cursorTimer->stop();
        delete m_cursorTimer;
    }

    if (m_cursorAnimation) {
        m_cursorAnimation->stop();
        delete m_cursorAnimation;
    }

    if (m_cursorVisibilityTimer) {
        m_cursorVisibilityTimer->stop();
        delete m_cursorVisibilityTimer;
    }

    if (m_undoStack) {
        delete m_undoStack;
    }
}

void QWinUITextInput::initializeTextInput()
{
    // 设置基本属性
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_InputMethodEnabled, true);
    
    // 初始化光标动画系统
    initializeCursorAnimation();
    
    // 初始化撤销栈
    m_undoStack = new QUndoStack(this);
    
    // 初始化颜色
    updateColors();

    // 连接主题变化信号
    QWinUITheme* theme = QWinUITheme::getInstance();
    if (theme) {
        connect(theme, &QWinUITheme::themeChanged, this, &QWinUITextInput::updateColors);
    }
    
    // 设置默认尺寸
    setMinimumSize(100, 32);
    setMaximumHeight(32);  // 限制最大高度
    resize(200, 32);
}

QString QWinUITextInput::text() const
{
    return m_text;
}

void QWinUITextInput::setText(const QString& text)
{
    if (m_text != text) {
        m_text = text;
        m_cursorPosition = qMin(m_cursorPosition, m_text.length());
        clearSelection();
        m_layoutDirty = true;
        updateTextLayout();
        update();
        emit textChanged(m_text);
    }
}

QString QWinUITextInput::placeholderText() const
{
    return m_placeholderText;
}

void QWinUITextInput::setPlaceholderText(const QString& text)
{
    if (m_placeholderText != text) {
        m_placeholderText = text;
        if (m_text.isEmpty()) {
            update();
        }
    }
}

bool QWinUITextInput::isReadOnly() const
{
    return m_readOnly;
}

void QWinUITextInput::setReadOnly(bool readOnly)
{
    if (m_readOnly != readOnly) {
        m_readOnly = readOnly;
        if (m_readOnly) {
            stopCursorAnimation();
            clearSelection(); // 清除选择
        } else if (hasFocus()) {
            startCursorAnimation();
        }

        // 更新颜色以反映只读状态
        updateColors();
    }
}

int QWinUITextInput::maxLength() const
{
    return m_maxLength;
}

void QWinUITextInput::setMaxLength(int length)
{
    m_maxLength = length;
}

bool QWinUITextInput::isMultiLine() const
{
    return m_multiLine;
}

void QWinUITextInput::setMultiLine(bool multiLine)
{
    if (m_multiLine != multiLine) {
        m_multiLine = multiLine;
        m_layoutDirty = true;
        updateTextLayout();
        update();
    }
}

bool QWinUITextInput::isRichTextEnabled() const
{
    return m_richTextEnabled;
}

void QWinUITextInput::setRichTextEnabled(bool enabled)
{
    if (m_richTextEnabled != enabled) {
        m_richTextEnabled = enabled;
        if (!enabled) {
            // 清除所有格式
            m_fragments.clear();
        }
        update();
    }
}

QColor QWinUITextInput::textColor() const
{
    return m_textColor;
}

void QWinUITextInput::setTextColor(const QColor& color)
{
    if (m_textColor != color) {
        m_textColor = color;
        update();
    }
}

QColor QWinUITextInput::selectionColor() const
{
    return m_selectionColor;
}

void QWinUITextInput::setSelectionColor(const QColor& color)
{
    if (m_selectionColor != color) {
        m_selectionColor = color;
        update();
    }
}

QColor QWinUITextInput::cursorColor() const
{
    return m_cursorColor;
}

void QWinUITextInput::setCursorColor(const QColor& color)
{
    if (m_cursorColor != color) {
        m_cursorColor = color;
        update();
    }
}

qreal QWinUITextInput::cursorOpacity() const
{
    return m_cursorOpacity;
}

void QWinUITextInput::setCursorOpacity(qreal opacity)
{
    opacity = qBound(0.0, opacity, 1.0);
    if (qAbs(m_cursorOpacity - opacity) > 0.01) {
        m_cursorOpacity = opacity;
        update();
    }
}

void QWinUITextInput::setFont(const QFont& font)
{
    QWidget::setFont(font);
    m_layoutDirty = true;
    updateTextLayout();
    update();
}

QFont QWinUITextInput::font() const
{
    return QWidget::font();
}

int QWinUITextInput::cursorPosition() const
{
    return m_cursorPosition;
}

void QWinUITextInput::setCursorPosition(int position)
{
    position = qBound(0, position, m_text.length());
    if (m_cursorPosition != position) {
        m_cursorPosition = position;
        clearSelection();
        ensureCursorVisible();
        update();
        emit cursorPositionChanged(m_cursorPosition);
    }
}

bool QWinUITextInput::hasSelection() const
{
    return m_selectionStart >= 0 && m_selectionEnd >= 0 && m_selectionStart != m_selectionEnd;
}

QString QWinUITextInput::selectedText() const
{
    if (!hasSelection()) {
        return QString();
    }
    
    int start = qMin(m_selectionStart, m_selectionEnd);
    int end = qMax(m_selectionStart, m_selectionEnd);
    return m_text.mid(start, end - start);
}

void QWinUITextInput::selectAll()
{
    if (!m_text.isEmpty()) {
        m_selectionStart = 0;
        m_selectionEnd = m_text.length();
        m_cursorPosition = m_text.length();
        update();
        emit selectionChanged();
    }
}

void QWinUITextInput::clearSelection()
{
    if (hasSelection()) {
        m_selectionStart = -1;
        m_selectionEnd = -1;
        update();
        emit selectionChanged();
    }
}

QSize QWinUITextInput::sizeHint() const
{
    if (m_multiLine) {
        return QSize(200, 100);
    } else {
        return QSize(200, 32);
    }
}

QSize QWinUITextInput::minimumSizeHint() const
{
    if (m_multiLine) {
        return QSize(100, 60);
    } else {
        return QSize(100, 32);
    }
}

void QWinUITextInput::onCursorBlink()
{
    // 保留原有的简单闪烁逻辑作为备用
    if (hasFocus() && !m_readOnly && !m_cursorAnimation) {
        m_cursorVisible = !m_cursorVisible;
        update();
    }
}

void QWinUITextInput::onCursorVisibilityTimeout()
{
    // 当动画完成时，检查是否需要重新开始循环
    if (hasFocus() && !m_readOnly && !m_isTyping) {
        if (m_cursorAnimation->state() == QPropertyAnimation::Stopped) {
            // 如果刚完成淡出，开始淡入
            if (m_cursorOpacity <= 0.1) {
                m_cursorAnimation->setStartValue(0.0);
                m_cursorAnimation->setEndValue(1.0);
                m_cursorAnimation->start();
            }
            // 如果刚完成淡入，等待一段时间后开始淡出
            else {
                m_cursorVisibilityTimer->start();
            }
        }
    }
}

void QWinUITextInput::startCursorFadeOut()
{
    if (hasFocus() && !m_readOnly && !m_isTyping) {
        // 开始淡出动画
        m_cursorAnimation->setStartValue(1.0);
        m_cursorAnimation->setEndValue(0.0);
        m_cursorAnimation->start();
    }
}

void QWinUITextInput::updateColors()
{
    QWinUITheme* theme = QWinUITheme::getInstance();
    if (!theme) return;

    if (theme->isDarkMode()) {
        m_textColor = QColor(255, 255, 255);
        m_borderColor = QColor(80, 80, 80);
        m_placeholderColor = QColor(150, 150, 150);
        m_selectionColor = QColor(0, 120, 215, 100);
        m_cursorColor = QColor(255, 255, 255);

        // 只读状态下使用不同的背景色
        if (m_readOnly) {
            m_backgroundColor = QColor(45, 45, 45); // 稍微亮一点的灰色
        } else {
            m_backgroundColor = QColor(32, 32, 32);
        }
    } else {
        m_textColor = QColor(0, 0, 0);
        m_borderColor = QColor(200, 200, 200);
        m_placeholderColor = QColor(100, 100, 100);
        m_selectionColor = QColor(0, 120, 215, 100);
        m_cursorColor = QColor(0, 0, 0);

        // 只读状态下使用不同的背景色
        if (m_readOnly) {
            m_backgroundColor = QColor(245, 245, 245); // 浅灰色
        } else {
            m_backgroundColor = QColor(255, 255, 255);
        }
    }

    update();
}

void QWinUITextInput::paintEvent(QPaintEvent* event)
{
    // 首先调用父类的paintEvent来处理主题切换动画
    QWinUIWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    QRect rect = this->rect();

    // 只有在没有主题切换动画时才绘制背景
    // 如果父控件是 QWinUIRichEditBox，则不绘制背景，让父控件处理
    bool shouldDrawBackground = !isThemeTransitioning();
    if (shouldDrawBackground && parent()) {
        // 检查父控件是否是 QWinUIRichEditBox
        if (qobject_cast<QWinUIRichEditBox*>(parent())) {
            shouldDrawBackground = false;
        }
    }

    if (shouldDrawBackground) {
        drawBackground(&painter, rect);
    }

    // 计算文本区域
    m_textRect = rect.adjusted(DEFAULT_PADDING, DEFAULT_PADDING, -DEFAULT_PADDING, -DEFAULT_PADDING);

    // 绘制选择背景
    if (hasSelection()) {
        drawSelection(&painter, m_textRect);
    }

    // 绘制文本或占位符
    if (m_text.isEmpty() && !m_placeholderText.isEmpty() && !hasFocus()) {
        drawPlaceholder(&painter, m_textRect);
    } else {
        drawText(&painter, m_textRect);
    }

    // 绘制光标
    if (hasFocus() && !m_readOnly && m_cursorVisible) {
        drawCursor(&painter, m_textRect);
    }
}

void QWinUITextInput::drawBackground(QPainter* painter, const QRect& rect)
{
    painter->save();

    // 绘制背景
    painter->setBrush(m_backgroundColor);
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(rect, 4, 4);

    // 绘制边框
    painter->setBrush(Qt::NoBrush);
    painter->setPen(QPen(m_borderColor, 1));
    painter->drawRoundedRect(rect.adjusted(0.5, 0.5, -0.5, -0.5), 4, 4);

    painter->restore();
}

void QWinUITextInput::drawText(QPainter* painter, const QRect& rect)
{
    if (m_text.isEmpty()) return;

    painter->save();
    painter->setPen(m_textColor);
    painter->setFont(font());

    // 设置裁剪区域，防止文本绘制超出边界
    painter->setClipRect(rect);

    if (m_multiLine) {
        // 多行文本绘制
        QTextLayout layout(m_text, font());
        layout.beginLayout();

        qreal y = rect.top();
        QFontMetrics fm(font());
        int lineHeight = fm.height();

        while (true) {
            QTextLine line = layout.createLine();
            if (!line.isValid()) break;

            line.setLineWidth(rect.width());
            line.setPosition(QPointF(rect.left(), y));

            if (y + lineHeight > rect.bottom()) break;

            y += lineHeight;
        }

        layout.endLayout();
        layout.draw(painter, rect.topLeft());
    } else {
        // 单行文本绘制，支持水平滚动
        QFontMetrics fm(font());

        // 计算文本绘制位置（考虑水平偏移）
        int textX = rect.left() - m_horizontalOffset;
        int textY = rect.top() + (rect.height() - fm.height()) / 2 + fm.ascent();

        // 绘制完整文本（不省略）
        painter->drawText(textX, textY, m_text);
    }

    painter->restore();
}

void QWinUITextInput::drawSelection(QPainter* painter, const QRect& rect)
{
    if (!hasSelection()) return;

    painter->save();

    int start = qMin(m_selectionStart, m_selectionEnd);
    int end = qMax(m_selectionStart, m_selectionEnd);

    QFontMetrics fm(font());

    if (m_multiLine) {
        // TODO: 多行选择绘制
    } else {
        // 单行选择绘制
        QString beforeSelection = m_text.left(start);
        QString selection = m_text.mid(start, end - start);

        int beforeWidth = fm.horizontalAdvance(beforeSelection);
        int selectionWidth = fm.horizontalAdvance(selection);

        QRect selectionRect(
            rect.left() + beforeWidth,
            rect.top(),
            selectionWidth,
            rect.height()
        );

        painter->fillRect(selectionRect, m_selectionColor);
    }

    painter->restore();
}

void QWinUITextInput::drawCursor(QPainter* painter, const QRect& rect)
{
    // 如果光标完全透明，不绘制
    if (m_cursorOpacity <= 0.0) {
        return;
    }

    painter->save();

    QFontMetrics fm(font());

    // 确保光标位置在有效范围内
    int validCursorPos = qBound(0, m_cursorPosition, m_text.length());

    // 计算光标前的文本宽度
    QString beforeCursor = m_text.left(validCursorPos);
    int textWidth = fm.horizontalAdvance(beforeCursor);

    // 光标X位置 = 文本区域左边界 + 光标前文本的宽度 - 水平偏移
    int cursorX = rect.left() + textWidth - m_horizontalOffset;

    // 只有当光标在可见区域内时才绘制
    if (cursorX >= rect.left() && cursorX <= rect.right()) {
        // 光标高度应该和字体高度一致，垂直居中
        int fontHeight = fm.height();
        int cursorY = rect.top() + (rect.height() - fontHeight) / 2;

        // 应用透明度到光标颜色
        QColor cursorColor = m_cursorColor;
        cursorColor.setAlphaF(m_cursorOpacity);

        // 绘制光标线
        QRect cursorRect(cursorX, cursorY, DEFAULT_CURSOR_WIDTH, fontHeight);
        painter->fillRect(cursorRect, cursorColor);
    }

    painter->restore();
}

void QWinUITextInput::drawPlaceholder(QPainter* painter, const QRect& rect)
{
    painter->save();
    painter->setPen(m_placeholderColor);
    painter->setFont(font());

    painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, m_placeholderText);

    painter->restore();
}

void QWinUITextInput::keyPressEvent(QKeyEvent* event)
{
    if (m_readOnly) {
        QWinUIWidget::keyPressEvent(event);
        return;
    }

    bool handled = true;

    switch (event->key()) {
    case Qt::Key_Left:
        if (event->modifiers() & Qt::ShiftModifier) {
            // 扩展选择
            if (!hasSelection()) {
                m_selectionStart = m_cursorPosition;
            }
            moveCursor(qMax(0, m_cursorPosition - 1), true);
        } else {
            moveCursor(qMax(0, m_cursorPosition - 1));
        }
        break;

    case Qt::Key_Right:
        if (event->modifiers() & Qt::ShiftModifier) {
            // 扩展选择
            if (!hasSelection()) {
                m_selectionStart = m_cursorPosition;
            }
            moveCursor(qMin(m_text.length(), m_cursorPosition + 1), true);
        } else {
            moveCursor(qMin(m_text.length(), m_cursorPosition + 1));
        }
        break;

    case Qt::Key_Home:
        if (event->modifiers() & Qt::ShiftModifier) {
            if (!hasSelection()) {
                m_selectionStart = m_cursorPosition;
            }
            moveCursor(0, true);
        } else {
            moveCursor(0);
        }
        break;

    case Qt::Key_End:
        if (event->modifiers() & Qt::ShiftModifier) {
            if (!hasSelection()) {
                m_selectionStart = m_cursorPosition;
            }
            moveCursor(m_text.length(), true);
        } else {
            moveCursor(m_text.length());
        }
        break;

    case Qt::Key_Backspace:
        if (hasSelection()) {
            deleteSelection();
        } else if (m_cursorPosition > 0) {
            deleteText(m_cursorPosition - 1, 1);
            moveCursor(m_cursorPosition - 1);
        }
        break;

    case Qt::Key_Delete:
        if (hasSelection()) {
            deleteSelection();
        } else if (m_cursorPosition < m_text.length()) {
            deleteText(m_cursorPosition, 1);
        }
        break;

    case Qt::Key_Return:
    case Qt::Key_Enter:
        if (m_multiLine) {
            insertText("\n");
        } else {
            // 单行模式下不插入换行，只发送编辑完成信号
            emit editingFinished();
        }
        break;

    case Qt::Key_A:
        if (event->modifiers() & Qt::ControlModifier) {
            selectAll();
        } else {
            handled = false;
        }
        break;

    case Qt::Key_C:
        if (event->modifiers() & Qt::ControlModifier) {
            copy();
        } else {
            handled = false;
        }
        break;

    case Qt::Key_V:
        if (event->modifiers() & Qt::ControlModifier) {
            paste();
        } else {
            handled = false;
        }
        break;

    case Qt::Key_X:
        if (event->modifiers() & Qt::ControlModifier) {
            cut();
        } else {
            handled = false;
        }
        break;

    case Qt::Key_Z:
        if (event->modifiers() & Qt::ControlModifier) {
            undo();
        } else {
            handled = false;
        }
        break;

    case Qt::Key_Y:
        if (event->modifiers() & Qt::ControlModifier) {
            redo();
        } else {
            handled = false;
        }
        break;

    default:
        if (!event->text().isEmpty() && event->text().at(0).isPrint()) {
            insertText(event->text());
        } else {
            handled = false;
        }
        break;
    }

    if (!handled) {
        QWinUIWidget::keyPressEvent(event);
    }
}

void QWinUITextInput::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        setFocus();

        // 计算点击位置对应的文本位置
        int position = positionFromPoint(event->pos());

        if (event->modifiers() & Qt::ShiftModifier && hasSelection()) {
            // Shift+点击：扩展选择
            updateSelection(m_selectionStart, position);
            m_cursorPosition = position;
        } else {
            // 普通点击：设置新的光标位置并清除选择
            clearSelection();
            m_cursorPosition = qBound(0, position, m_text.length());
            m_selecting = true;
        }

        // 重置光标动画
        resetCursorAnimation();

        update();
        emit cursorPositionChanged(m_cursorPosition);
    }

    QWinUIWidget::mousePressEvent(event);
}

void QWinUITextInput::mouseMoveEvent(QMouseEvent* event)
{
    if (m_selecting && (event->buttons() & Qt::LeftButton)) {
        int position = positionFromPoint(event->pos());

        if (!hasSelection()) {
            m_selectionStart = m_cursorPosition;
        }

        updateSelection(m_selectionStart, position);
        m_cursorPosition = position;
        update();
    }

    QWinUIWidget::mouseMoveEvent(event);
}

void QWinUITextInput::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_selecting = false;
    }

    QWinUIWidget::mouseReleaseEvent(event);
}

void QWinUITextInput::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && !m_text.isEmpty()) {
        // 双击选择单词
        int position = positionFromPoint(event->pos());

        // 找到单词边界
        int start = position;
        int end = position;

        while (start > 0 && m_text.at(start - 1).isLetterOrNumber()) {
            start--;
        }

        while (end < m_text.length() && m_text.at(end).isLetterOrNumber()) {
            end++;
        }

        if (start < end) {
            updateSelection(start, end);
            m_cursorPosition = end;
            update();
        }
    }

    QWinUIWidget::mouseDoubleClickEvent(event);
}

void QWinUITextInput::focusInEvent(QFocusEvent* event)
{
    QWinUIWidget::focusInEvent(event);

    if (!m_readOnly) {
        startCursorAnimation();
    }

    update();
}

void QWinUITextInput::focusOutEvent(QFocusEvent* event)
{
    QWinUIWidget::focusOutEvent(event);

    stopCursorAnimation();

    update();
    emit editingFinished();
}

void QWinUITextInput::inputMethodEvent(QInputMethodEvent* event)
{
    if (m_readOnly) return;

    if (!event->commitString().isEmpty()) {
        insertText(event->commitString());
    }

    QWinUIWidget::inputMethodEvent(event);
}

QVariant QWinUITextInput::inputMethodQuery(Qt::InputMethodQuery query) const
{
    switch (query) {
    case Qt::ImCursorPosition:
        return m_cursorPosition;
    case Qt::ImSurroundingText:
        return m_text;
    case Qt::ImCurrentSelection:
        return selectedText();
    default:
        return QWinUIWidget::inputMethodQuery(query);
    }
}

void QWinUITextInput::onThemeChanged()
{
    updateColors();
}

// 编辑操作实现
void QWinUITextInput::cut()
{
    if (hasSelection() && !m_readOnly) {
        copy();
        deleteSelection();
    }
}

void QWinUITextInput::copy()
{
    if (hasSelection()) {
        QApplication::clipboard()->setText(selectedText());
    }
}

void QWinUITextInput::paste()
{
    if (m_readOnly) return;

    QString clipboardText = QApplication::clipboard()->text();
    if (!clipboardText.isEmpty()) {
        if (!m_multiLine) {
            // 单行模式下移除换行符
            clipboardText.replace('\n', ' ').replace('\r', ' ');
        }
        insertText(clipboardText);
    }
}

void QWinUITextInput::undo()
{
    if (m_undoStack && m_undoStack->canUndo()) {
        m_undoStack->undo();
    }
}

void QWinUITextInput::redo()
{
    if (m_undoStack && m_undoStack->canRedo()) {
        m_undoStack->redo();
    }
}

void QWinUITextInput::clear()
{
    setText(QString());
}

// 辅助方法实现
int QWinUITextInput::positionFromPoint(const QPoint& point) const
{
    QFontMetrics fm(font());

    // 计算相对于文本区域的点击位置，考虑水平偏移
    int clickX = point.x() - m_textRect.left() + m_horizontalOffset;

    if (m_multiLine) {
        // TODO: 多行位置计算
        return 0;
    } else {
        // 单行位置计算
        if (clickX <= 0) {
            return 0; // 点击在文本开始之前
        }

        // 遍历每个字符位置，找到最接近的位置
        for (int i = 0; i <= m_text.length(); ++i) {
            QString textPart = m_text.left(i);
            int textWidth = fm.horizontalAdvance(textPart);

            if (i == m_text.length()) {
                // 到达文本末尾
                return i;
            }

            // 计算下一个字符的位置
            QString nextTextPart = m_text.left(i + 1);
            int nextTextWidth = fm.horizontalAdvance(nextTextPart);

            // 如果点击位置在当前字符和下一个字符之间
            if (clickX >= textWidth && clickX < nextTextWidth) {
                // 判断更接近哪个位置
                int distToCurrent = qAbs(clickX - textWidth);
                int distToNext = qAbs(clickX - nextTextWidth);

                return (distToCurrent <= distToNext) ? i : i + 1;
            }
        }

        // 如果点击在文本末尾之后
        return m_text.length();
    }
}

void QWinUITextInput::insertText(const QString& text)
{
    if (m_readOnly || text.isEmpty()) return;

    // 处理文本：在单行模式下移除换行符
    QString processedText = text;
    if (!m_multiLine) {
        processedText.replace('\n', ' ').replace('\r', ' ');
    }

    // 检查最大长度
    if (m_maxLength >= 0 && m_text.length() + processedText.length() > m_maxLength) {
        return;
    }

    // 如果有选择，先删除选择的文本
    if (hasSelection()) {
        deleteSelection();
    }

    // 插入新文本
    m_text.insert(m_cursorPosition, processedText);
    m_cursorPosition += processedText.length();

    // 确保光标位置在有效范围内
    m_cursorPosition = qBound(0, m_cursorPosition, m_text.length());

    // 重置光标动画
    resetCursorAnimation();

    m_layoutDirty = true;
    updateTextLayout();
    update();

    emit textChanged(m_text);
    emit cursorPositionChanged(m_cursorPosition);
}

void QWinUITextInput::deleteText(int start, int length)
{
    if (m_readOnly || start < 0 || length <= 0 || start >= m_text.length()) return;

    length = qMin(length, m_text.length() - start);
    m_text.remove(start, length);

    // 更新光标位置
    if (m_cursorPosition > start) {
        m_cursorPosition = qMax(start, m_cursorPosition - length);
    }

    // 确保光标位置在有效范围内
    m_cursorPosition = qBound(0, m_cursorPosition, m_text.length());

    clearSelection();

    // 重置光标动画
    resetCursorAnimation();

    m_layoutDirty = true;
    updateTextLayout();
    update();

    emit textChanged(m_text);
    emit cursorPositionChanged(m_cursorPosition);
}

void QWinUITextInput::deleteSelection()
{
    if (!hasSelection()) return;

    int start = qMin(m_selectionStart, m_selectionEnd);
    int end = qMax(m_selectionStart, m_selectionEnd);

    deleteText(start, end - start);
    setCursorPosition(start);
}

void QWinUITextInput::moveCursor(int position, bool select)
{
    position = qBound(0, position, m_text.length());

    if (select) {
        if (!hasSelection()) {
            m_selectionStart = m_cursorPosition;
        }
        m_selectionEnd = position;

        if (m_selectionStart == m_selectionEnd) {
            clearSelection();
        }
    } else {
        clearSelection();
    }

    m_cursorPosition = position;
    ensureCursorVisible();
    update();

    emit cursorPositionChanged(m_cursorPosition);
    if (select || !select) { // 总是发送选择变化信号
        emit selectionChanged();
    }
}

void QWinUITextInput::updateSelection(int start, int end)
{
    m_selectionStart = start;
    m_selectionEnd = end;

    if (m_selectionStart == m_selectionEnd) {
        clearSelection();
    } else {
        emit selectionChanged();
    }
}

void QWinUITextInput::updateTextLayout()
{
    if (m_layoutDirty) {
        // TODO: 实现文本布局缓存
        m_layoutDirty = false;
    }
}

void QWinUITextInput::ensureCursorVisible()
{
    if (m_multiLine) {
        // TODO: 多行模式的滚动
        return;
    }

    // 单行模式的水平滚动
    QFontMetrics fm(font());
    QString beforeCursor = m_text.left(m_cursorPosition);
    int cursorX = fm.horizontalAdvance(beforeCursor);

    // 计算可见区域宽度（减去padding）
    int visibleWidth = width() - 2 * DEFAULT_PADDING;

    // 如果光标在可见区域左侧
    if (cursorX < m_horizontalOffset) {
        m_horizontalOffset = cursorX;
    }
    // 如果光标在可见区域右侧
    else if (cursorX > m_horizontalOffset + visibleWidth) {
        m_horizontalOffset = cursorX - visibleWidth;
    }

    // 确保偏移不为负数
    m_horizontalOffset = qMax(0, m_horizontalOffset);
}

void QWinUITextInput::initializeCursorAnimation()
{
    // 初始化光标可见性定时器（控制光标显示1000ms）
    m_cursorVisibilityTimer = new QTimer(this);
    m_cursorVisibilityTimer->setSingleShot(true);
    m_cursorVisibilityTimer->setInterval(CURSOR_VISIBLE_DURATION);
    connect(m_cursorVisibilityTimer, &QTimer::timeout, this, &QWinUITextInput::startCursorFadeOut);

    // 初始化光标透明度动画
    m_cursorAnimation = new QPropertyAnimation(this, "cursorOpacity", this);
    m_cursorAnimation->setDuration(CURSOR_FADE_DURATION);
    m_cursorAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    // 连接动画完成信号
    connect(m_cursorAnimation, &QPropertyAnimation::finished, this, &QWinUITextInput::onCursorVisibilityTimeout);

    // 保留原有的定时器作为备用（兼容性）
    m_cursorTimer = new QTimer(this);
    m_cursorTimer->setInterval(CURSOR_BLINK_INTERVAL);
    connect(m_cursorTimer, &QTimer::timeout, this, &QWinUITextInput::onCursorBlink);
}

void QWinUITextInput::startCursorAnimation()
{
    if (m_readOnly) return;

    // 停止所有现有动画
    stopCursorAnimation();

    // 设置光标为完全可见
    m_cursorOpacity = 1.0;
    m_cursorVisible = true;
    update();

    // 启动可见性定时器
    m_cursorVisibilityTimer->start();
}

void QWinUITextInput::stopCursorAnimation()
{
    if (m_cursorAnimation) {
        m_cursorAnimation->stop();
    }

    if (m_cursorVisibilityTimer) {
        m_cursorVisibilityTimer->stop();
    }

    if (m_cursorTimer) {
        m_cursorTimer->stop();
    }

    m_cursorVisible = false;
    m_cursorOpacity = 0.0;
    update();
}

void QWinUITextInput::resetCursorAnimation()
{
    if (m_readOnly || !hasFocus()) return;

    // 重置动画状态
    m_isTyping = true;

    // 停止当前动画
    if (m_cursorAnimation) {
        m_cursorAnimation->stop();
    }

    if (m_cursorVisibilityTimer) {
        m_cursorVisibilityTimer->stop();
    }

    // 立即显示光标
    m_cursorOpacity = 1.0;
    m_cursorVisible = true;
    update();

    // 延迟重新启动动画（模拟VSCode的行为）
    QTimer::singleShot(100, this, [this]() {
        m_isTyping = false;
        if (hasFocus() && !m_readOnly) {
            startCursorAnimation();
        }
    });
}
