#include "QWinUI/Controls/QWinUITextBlock.h"
#include "QWinUI/QWinUITheme.h"
#include <QPainter>
#include <QFontMetrics>
#include <QTextLayout>
#include <QTextDocument>
#include <QTextCursor>
#include <QAbstractTextDocumentLayout>
#include <QApplication>
#include <QClipboard>
#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTextBlock>
#include <QTextBlockFormat>
#include <QTextCharFormat>
#include <cmath>

QT_BEGIN_NAMESPACE

QWinUITextBlock::QWinUITextBlock(QWidget* parent)
    : QWinUIWidget(parent)
    , m_textAlignment(Qt::AlignLeft | Qt::AlignTop)
    , m_textWrapping(NoWrap)
    , m_textTrimming(None)
    , m_maxLines(-1)
    , m_lineHeight(1.0)
    , m_textSelectionEnabled(false)
    , m_textDocument(nullptr)
    , m_useInlines(false)
    , m_isSelecting(false)
    , m_selectionStart(-1)
    , m_selectionEnd(-1)
    , m_sizeHintDirty(true)
{
    initializeTextBlock();
}

QWinUITextBlock::QWinUITextBlock(const QString& text, QWidget* parent)
    : QWinUIWidget(parent)
    , m_text(text)
    , m_textAlignment(Qt::AlignLeft | Qt::AlignTop)
    , m_textWrapping(NoWrap)
    , m_textTrimming(None)
    , m_maxLines(-1)
    , m_lineHeight(1.0)
    , m_textSelectionEnabled(false)
    , m_textDocument(nullptr)
    , m_useInlines(false)
    , m_isSelecting(false)
    , m_selectionStart(-1)
    , m_selectionEnd(-1)
    , m_sizeHintDirty(true)
{
    initializeTextBlock();
}

QWinUITextBlock::~QWinUITextBlock()
{
    if (m_textDocument) {
        delete m_textDocument;
    }
}

void QWinUITextBlock::initializeTextBlock()
{
    // 设置基本属性
    setFocusPolicy(m_textSelectionEnabled ? Qt::StrongFocus : Qt::NoFocus);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    
    // 创建文本文档
    m_textDocument = new QTextDocument(this);
    
    // 设置默认字体
    m_font = QApplication::font();
    
    // 设置文本选项
    m_textOption.setAlignment(m_textAlignment);
    m_textOption.setWrapMode(QTextOption::NoWrap);
    
    // 更新文本文档
    updateTextDocument();
    
    // 连接主题变化信号
    connect(this, &QWinUIWidget::themeChanged, this, &QWinUITextBlock::onThemeChanged);
}

QString QWinUITextBlock::text() const
{
    return m_text;
}

void QWinUITextBlock::setText(const QString& text)
{
    if (m_text != text) {
        m_text = text;

        // 清空内联元素，使用纯文本模式
        m_inlines.clear();
        m_useInlines = false;

        m_sizeHintDirty = true;
        updateTextDocument();
        update();
        emit textChanged(text);
    }
}

QFont QWinUITextBlock::font() const
{
    return m_font;
}

void QWinUITextBlock::setFont(const QFont& font)
{
    if (m_font != font) {
        m_font = font;
        m_sizeHintDirty = true;
        updateTextDocument();
        update();
        emit fontChanged(font);
    }
}

Qt::Alignment QWinUITextBlock::textAlignment() const
{
    return m_textAlignment;
}

void QWinUITextBlock::setTextAlignment(Qt::Alignment alignment)
{
    if (m_textAlignment != alignment) {
        m_textAlignment = alignment;
        m_textOption.setAlignment(alignment);
        updateTextDocument();
        update();
        emit textAlignmentChanged(alignment);
    }
}

QWinUITextBlock::QWinUITextWrapping QWinUITextBlock::textWrapping() const
{
    return m_textWrapping;
}

void QWinUITextBlock::setTextWrapping(QWinUITextWrapping wrapping)
{
    if (m_textWrapping != wrapping) {
        m_textWrapping = wrapping;
        m_sizeHintDirty = true;
        
        // 更新文本选项
        switch (wrapping) {
        case NoWrap:
            m_textOption.setWrapMode(QTextOption::NoWrap);
            break;
        case Wrap:
            m_textOption.setWrapMode(QTextOption::WrapAnywhere);
            break;
        case WrapWholeWords:
            m_textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
            break;
        }
        
        updateTextDocument();
        update();
        emit textWrappingChanged(wrapping);
    }
}

QWinUITextBlock::QWinUITextTrimming QWinUITextBlock::textTrimming() const
{
    return m_textTrimming;
}

void QWinUITextBlock::setTextTrimming(QWinUITextTrimming trimming)
{
    if (m_textTrimming != trimming) {
        m_textTrimming = trimming;
        update();
        emit textTrimmingChanged(trimming);
    }
}

int QWinUITextBlock::maxLines() const
{
    return m_maxLines;
}

void QWinUITextBlock::setMaxLines(int maxLines)
{
    if (m_maxLines != maxLines) {
        m_maxLines = maxLines;
        m_sizeHintDirty = true;
        update();
        emit maxLinesChanged(maxLines);
    }
}

double QWinUITextBlock::lineHeight() const
{
    return m_lineHeight;
}

void QWinUITextBlock::setLineHeight(double height)
{
    if (qAbs(m_lineHeight - height) > 0.001) {
        m_lineHeight = height;
        m_sizeHintDirty = true;
        updateTextDocument();
        update();
        emit lineHeightChanged(height);
    }
}

QColor QWinUITextBlock::foreground() const
{
    return m_foreground;
}

void QWinUITextBlock::setForeground(const QColor& color)
{
    if (m_foreground != color) {
        m_foreground = color;
        update();
        emit foregroundChanged(color);
    }
}

bool QWinUITextBlock::isTextSelectionEnabled() const
{
    return m_textSelectionEnabled;
}

void QWinUITextBlock::setTextSelectionEnabled(bool enabled)
{
    if (m_textSelectionEnabled != enabled) {
        m_textSelectionEnabled = enabled;
        setFocusPolicy(enabled ? Qt::StrongFocus : Qt::NoFocus);
        if (!enabled) {
            clearSelection();
        }
        emit textSelectionEnabledChanged(enabled);
    }
}

QString QWinUITextBlock::selectedText() const
{
    if (m_selectionStart >= 0 && m_selectionEnd >= 0 && m_selectionStart != m_selectionEnd) {
        int start = qMin(m_selectionStart, m_selectionEnd);
        int end = qMax(m_selectionStart, m_selectionEnd);
        return m_text.mid(start, end - start);
    }
    return QString();
}

void QWinUITextBlock::selectAll()
{
    if (m_textSelectionEnabled && !m_text.isEmpty()) {
        m_selectionStart = 0;
        m_selectionEnd = m_text.length();
        update();
        emit selectionChanged();
    }
}

void QWinUITextBlock::clearSelection()
{
    if (m_selectionStart >= 0 || m_selectionEnd >= 0) {
        m_selectionStart = -1;
        m_selectionEnd = -1;
        update();
        emit selectionChanged();
    }
}

void QWinUITextBlock::selectWordAt(int position)
{
    if (!m_textSelectionEnabled || m_text.isEmpty() || position < 0 || position >= m_text.length()) {
        return;
    }

    // 查找单词边界
    int start = position;
    int end = position;

    // 向前查找单词开始
    while (start > 0 && (m_text[start - 1].isLetterOrNumber() || m_text[start - 1] == '_')) {
        start--;
    }

    // 向后查找单词结束
    while (end < m_text.length() && (m_text[end].isLetterOrNumber() || m_text[end] == '_')) {
        end++;
    }

    // 设置选择范围
    if (start < end) {
        m_selectionStart = start;
        m_selectionEnd = end;
        update();
        emit selectionChanged();
    }
}

QSize QWinUITextBlock::sizeHint() const
{
    if (m_sizeHintDirty) {
        m_cachedSizeHint = calculateSizeHint();
        m_sizeHintDirty = false;
    }
    return m_cachedSizeHint;
}

QSize QWinUITextBlock::minimumSizeHint() const
{
    QFontMetrics fm(getEffectiveFont());
    return QSize(fm.horizontalAdvance("..."), fm.height());
}

void QWinUITextBlock::updateTextDocument()
{
    if (!m_textDocument) return;

    // 如果使用内联元素，则构建内联文档
    if (m_useInlines && !m_inlines.isEmpty()) {
        buildDocumentFromInlines();
        return;
    }

    // 设置文档字体
    QFont effectiveFont = getEffectiveFont();
    m_textDocument->setDefaultFont(effectiveFont);

    // 设置文档内容
    m_textDocument->setPlainText(m_text);

    // 设置文本颜色和格式
    QTextCursor cursor(m_textDocument);
    cursor.select(QTextCursor::Document);

    QTextCharFormat charFormat;
    charFormat.setFont(effectiveFont);
    charFormat.setForeground(getEffectiveForeground());
    cursor.mergeCharFormat(charFormat);

    // 设置行高
    if (m_lineHeight != 1.0) {
        QTextBlockFormat blockFormat;
        blockFormat.setLineHeight(m_lineHeight * 100, QTextBlockFormat::ProportionalHeight);
        cursor.mergeBlockFormat(blockFormat);
    }

    // 设置文档宽度（用于换行）
    if (m_textWrapping != NoWrap) {
        m_textDocument->setTextWidth(width());
    } else {
        m_textDocument->setTextWidth(-1);
    }
}

QSize QWinUITextBlock::calculateSizeHint() const
{
    if (m_text.isEmpty()) {
        QFontMetrics fm(getEffectiveFont());
        return QSize(0, fm.height());
    }
    
    // 创建临时文档来计算尺寸
    QTextDocument tempDoc;
    tempDoc.setDefaultFont(getEffectiveFont());
    tempDoc.setPlainText(m_text);
    
    if (m_textWrapping != NoWrap) {
        tempDoc.setTextWidth(width() > 0 ? width() : 200); // 默认宽度
    } else {
        tempDoc.setTextWidth(-1);
    }
    
    QSize docSize = tempDoc.size().toSize();
    
    // 应用最大行数限制
    if (m_maxLines > 0) {
        QFontMetrics fm(getEffectiveFont());
        int maxHeight = fm.height() * m_maxLines * m_lineHeight;
        docSize.setHeight(qMin(docSize.height(), maxHeight));
    }
    
    return docSize;
}

void QWinUITextBlock::paintEvent(QPaintEvent* event)
{
    // 先绘制基础背景
    QWinUIWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    QRect textRect = getTextRect();

    // 绘制选择背景
    if (m_textSelectionEnabled && m_selectionStart >= 0 && m_selectionEnd >= 0 && m_selectionStart != m_selectionEnd) {
        drawSelection(&painter, textRect);
    }

    // 绘制文本
    drawText(&painter, textRect);
}

void QWinUITextBlock::drawText(QPainter* painter, const QRect& rect)
{
    if (m_text.isEmpty()) return;

    painter->save();

    // 设置文本颜色
    QColor textColor = getEffectiveForeground();
    painter->setPen(textColor);

    // 设置字体
    painter->setFont(getEffectiveFont());

    // 获取要显示的文本
    QString displayText = getDisplayText();

    if (m_textWrapping == NoWrap && m_textTrimming != None) {
        // 单行文本，可能需要省略号
        QFontMetrics fm(getEffectiveFont());
        if (fm.horizontalAdvance(displayText) > rect.width()) {
            displayText = fm.elidedText(displayText,
                m_textTrimming == WordEllipsis ? Qt::ElideRight : Qt::ElideRight,
                rect.width());
        }
        painter->drawText(rect, m_textAlignment, displayText);
    } else {
        // 多行文本或无省略号
        if (m_textDocument) {
            painter->translate(rect.topLeft());

            // 设置裁剪区域
            painter->setClipRect(QRect(0, 0, rect.width(), rect.height()));

            // 绘制文档（颜色已在updateTextDocument中设置）
            m_textDocument->drawContents(painter);
        }
    }

    painter->restore();
}

void QWinUITextBlock::drawSelection(QPainter* painter, const QRect& rect)
{
    Q_UNUSED(rect)

    if (!m_textSelectionEnabled || m_selectionStart < 0 || m_selectionEnd < 0 || m_selectionStart == m_selectionEnd) {
        return;
    }

    painter->save();

    // 获取选择背景色 - 使用WinUI 3风格的选择颜色
    QWinUITheme* theme = QWinUITheme::getInstance();
    QColor selectionColor;
    if (theme && theme->themeMode() == QWinUIThemeMode::Dark) {
        selectionColor = QColor(0, 120, 215, 80); // 深色模式：蓝色半透明，更明显
    } else {
        selectionColor = QColor(0, 120, 215, 60); // 浅色模式：蓝色半透明，更明显
    }

    // 绘制选择背景
    QRect selectionRect = getSelectionRect();
    if (selectionRect.isValid() && !selectionRect.isEmpty()) {
        // 使用圆角矩形，更符合WinUI 3风格
        painter->setRenderHint(QPainter::Antialiasing);
        painter->fillRect(selectionRect, selectionColor);

        // 可选：添加边框
        if (hasFocus()) {
            QPen borderPen(selectionColor.darker(120), 1);
            painter->setPen(borderPen);
            painter->drawRect(selectionRect);
        }
    }

    painter->restore();
}

QString QWinUITextBlock::getDisplayText() const
{
    return m_text;
}

QRect QWinUITextBlock::getTextRect() const
{
    return rect().adjusted(4, 2, -4, -2); // WinUI 3 标准内边距
}

QRect QWinUITextBlock::getSelectionRect() const
{
    // 只有在有实际选择时才返回选择矩形
    if (!m_textSelectionEnabled || m_selectionStart == m_selectionEnd || m_selectionStart < 0 || m_selectionEnd < 0) {
        return QRect(); // 返回无效矩形
    }

    QRect textRect = getTextRect();
    QFontMetrics fm(getEffectiveFont());

    int start = qMin(m_selectionStart, m_selectionEnd);
    int end = qMax(m_selectionStart, m_selectionEnd);

    // 简化的选择矩形计算 - 基于字符位置
    QString textToStart = m_text.left(start);
    QString textToEnd = m_text.left(end);

    int startX = fm.horizontalAdvance(textToStart);
    int endX = fm.horizontalAdvance(textToEnd);

    // 确保选择矩形在文本区域内
    startX = qMax(0, qMin(startX, textRect.width()));
    endX = qMax(startX, qMin(endX, textRect.width()));

    QRect selectionRect(textRect.left() + startX, textRect.top(),
                       endX - startX, textRect.height());

    return selectionRect;
}

QFont QWinUITextBlock::getEffectiveFont() const
{
    if (!m_font.family().isEmpty()) {
        return m_font;
    }

    // 使用 WinUI 3 默认字体
    QFont defaultFont("Segoe UI", 14);
    return defaultFont;
}

QColor QWinUITextBlock::getEffectiveForeground() const
{
    if (m_foreground.isValid()) {
        return m_foreground;
    }

    // 使用主题文本颜色
    return themeTextColor();
}

void QWinUITextBlock::onThemeChanged()
{
    // 主题变化时重新更新文本文档以应用新的颜色
    if (m_useInlines) {
        buildDocumentFromInlines();
    } else {
        updateTextDocument();
    }
    // 重绘控件
    update();
}

// 内联元素管理方法
void QWinUITextBlock::clearInlines()
{
    m_inlines.clear();
    m_useInlines = false;
    m_sizeHintDirty = true;
    updateTextDocument();
    update();
}

void QWinUITextBlock::addInline(const QWinUIInline& inlineElement)
{
    m_inlines.append(inlineElement);
    m_useInlines = true;
    m_sizeHintDirty = true;
    buildDocumentFromInlines();
    update();
}

QList<QWinUITextBlock::QWinUIInline> QWinUITextBlock::getInlines() const
{
    return m_inlines;
}

void QWinUITextBlock::addRun(const QString& text, const QTextCharFormat& format)
{
    QWinUIInline run;
    run.type = Run;
    run.text = text;
    run.format = format.font().family().isEmpty() ? getDefaultCharFormat() : format;
    addInline(run);
}

void QWinUITextBlock::addBold(const QString& text)
{
    QTextCharFormat format = getDefaultCharFormat();
    format.setFontWeight(QFont::Bold);
    addRun(text, format);
}

void QWinUITextBlock::addItalic(const QString& text)
{
    QTextCharFormat format = getDefaultCharFormat();
    format.setFontItalic(true);
    addRun(text, format);
}

void QWinUITextBlock::addUnderline(const QString& text)
{
    QTextCharFormat format = getDefaultCharFormat();
    format.setFontUnderline(true);
    addRun(text, format);
}

void QWinUITextBlock::addHyperlink(const QString& text, const QUrl& url, const QTextCharFormat& format)
{
    Q_UNUSED(format) // 暂时不使用自定义格式，总是使用主题格式

    QWinUIInline hyperlink;
    hyperlink.type = Hyperlink;
    hyperlink.text = text;
    hyperlink.properties["url"] = url;
    hyperlink.format = getHyperlinkFormat();
    addInline(hyperlink);
}

void QWinUITextBlock::addLineBreak()
{
    QWinUIInline lineBreak;
    lineBreak.type = LineBreak;
    lineBreak.text = "\n";
    lineBreak.format = getDefaultCharFormat();
    addInline(lineBreak);
}

// 样式设置方法
void QWinUITextBlock::setFontWeight(QFont::Weight weight)
{
    QFont newFont = m_font;
    newFont.setWeight(weight);
    setFont(newFont);
}

void QWinUITextBlock::setFontItalic(bool italic)
{
    QFont newFont = m_font;
    newFont.setItalic(italic);
    setFont(newFont);
}

void QWinUITextBlock::setFontUnderline(bool underline)
{
    QFont newFont = m_font;
    newFont.setUnderline(underline);
    setFont(newFont);
}

void QWinUITextBlock::setFontSize(int pointSize)
{
    QFont newFont = m_font;
    newFont.setPointSize(pointSize);
    setFont(newFont);
}

void QWinUITextBlock::setFontFamily(const QString& family)
{
    QFont newFont = m_font;
    newFont.setFamily(family);
    setFont(newFont);
}

// 格式方法
QTextCharFormat QWinUITextBlock::getDefaultCharFormat() const
{
    QTextCharFormat format;
    format.setFont(getEffectiveFont());
    format.setForeground(getEffectiveForeground());
    return format;
}

QTextCharFormat QWinUITextBlock::getHyperlinkFormat() const
{
    QTextCharFormat format = getDefaultCharFormat();

    // 根据主题模式设置超链接颜色
    QWinUITheme* theme = QWinUITheme::getInstance();
    if (theme && theme->themeMode() == QWinUIThemeMode::Dark) {
        format.setForeground(QColor(99, 162, 255)); // 深色模式浅蓝色
    } else {
        format.setForeground(QColor(0, 120, 215)); // 浅色模式深蓝色
    }

    format.setFontUnderline(false); // WinUI 3 超链接默认无下划线
    return format;
}

void QWinUITextBlock::buildDocumentFromInlines()
{
    if (!m_textDocument || m_inlines.isEmpty()) return;

    m_textDocument->clear();

    QTextCursor cursor(m_textDocument);

    // 设置文档默认格式
    QTextCharFormat defaultFormat = getDefaultCharFormat();
    cursor.setCharFormat(defaultFormat);

    // 设置段落格式
    QTextBlockFormat blockFormat;
    blockFormat.setAlignment(m_textAlignment);
    if (m_lineHeight != 1.0) {
        blockFormat.setLineHeight(m_lineHeight * 100, QTextBlockFormat::ProportionalHeight);
    }
    cursor.setBlockFormat(blockFormat);

    // 插入内联元素
    for (const QWinUIInline& inlineElement : m_inlines) {
        QTextCharFormat format = inlineElement.format;

        // 确保基础文字颜色正确
        if (format.foreground().color() == QColor() ||
            (inlineElement.type != Hyperlink && format.foreground().color() == QColor(0, 120, 215))) {
            format.setForeground(getEffectiveForeground());
        }

        switch (inlineElement.type) {
        case Run:
        case Bold:
        case Italic:
        case Underline:
            cursor.setCharFormat(format);
            cursor.insertText(inlineElement.text);
            break;

        case Hyperlink:
            if (m_textSelectionEnabled) {
                // 重新设置超链接颜色
                QWinUITheme* theme = QWinUITheme::getInstance();
                if (theme && theme->themeMode() == QWinUIThemeMode::Dark) {
                    format.setForeground(QColor(99, 162, 255)); // 深色模式浅蓝色
                } else {
                    format.setForeground(QColor(0, 120, 215)); // 浅色模式深蓝色
                }

                QString url = inlineElement.properties.value("url").toString();
                format.setAnchor(true);
                format.setAnchorHref(url);
                cursor.setCharFormat(format);
                cursor.insertText(inlineElement.text);
            } else {
                cursor.setCharFormat(format);
                cursor.insertText(inlineElement.text);
            }
            break;

        case LineBreak:
            cursor.insertBlock();
            cursor.setBlockFormat(blockFormat);
            break;
        }
    }

    // 更新文本内容
    m_text = m_textDocument->toPlainText();
}

void QWinUITextBlock::resizeEvent(QResizeEvent* event)
{
    QWinUIWidget::resizeEvent(event);

    // 更新文档宽度以适应新的控件大小
    if (m_textDocument && m_textWrapping != NoWrap) {
        m_textDocument->setTextWidth(width());
        m_sizeHintDirty = true;
    }
}

void QWinUITextBlock::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        // 检查是否点击了超链接
        if (m_useInlines && m_textDocument) {
            // 使用QTextLayout来检测超链接点击
            QRect textRect = getTextRect();
            QPoint relativePos = event->pos() - textRect.topLeft();

            // 简单的超链接检测 - 遍历内联元素
            for (const QWinUIInline& inlineElement : m_inlines) {
                if (inlineElement.type == Hyperlink) {
                    // 这里可以添加更精确的位置检测
                    // 暂时简化处理：如果点击在文本区域内且有超链接，就触发第一个超链接
                    if (textRect.contains(event->pos())) {
                        QString url = inlineElement.properties.value("url").toString();
                        if (!url.isEmpty()) {
                            emit hyperlinkClicked(QUrl(url));
                            return;
                        }
                    }
                }
            }
        }

        // 处理文本选择
        if (m_textSelectionEnabled) {
            setFocus(); // 获取焦点以支持键盘操作
            m_selectionStartPos = event->pos();
            int charIndex = getCharacterAtPosition(event->pos());

            // 清除之前的选择
            m_selectionStart = charIndex;
            m_selectionEnd = charIndex;
            m_isSelecting = false;

            update(); // 立即更新显示
        }
    }
    QWinUIWidget::mousePressEvent(event);
}

void QWinUITextBlock::mouseMoveEvent(QMouseEvent* event)
{
    if (m_textSelectionEnabled && (event->buttons() & Qt::LeftButton)) {
        // 只有当鼠标移动距离足够大时才开始选择
        QPoint delta = event->pos() - m_selectionStartPos;
        if (!m_isSelecting && (qAbs(delta.x()) > 2 || qAbs(delta.y()) > 2)) {
            m_isSelecting = true;
        }

        if (m_isSelecting) {
            int charIndex = getCharacterAtPosition(event->pos());
            if (m_selectionEnd != charIndex) {
                m_selectionEnd = charIndex;
                update();
                emit selectionChanged();
            }
        }
    } else if (m_textSelectionEnabled) {
        // 更新鼠标光标样式
        QRect textRect = getTextRect();
        if (textRect.contains(event->pos())) {
            setCursor(Qt::IBeamCursor);
        } else {
            setCursor(Qt::ArrowCursor);
        }
    }
    QWinUIWidget::mouseMoveEvent(event);
}

void QWinUITextBlock::mouseReleaseEvent(QMouseEvent* event)
{
    if (m_textSelectionEnabled && event->button() == Qt::LeftButton) {
        // 如果没有真正开始选择，或者选择范围为空，则清除选择
        if (!m_isSelecting || m_selectionStart == m_selectionEnd) {
            clearSelection();
        } else {
            // 确保选择范围有效
            if (m_selectionStart >= 0 && m_selectionEnd >= 0) {
                emit selectionChanged();
            }
        }
        m_isSelecting = false;
    }
    QWinUIWidget::mouseReleaseEvent(event);
}

void QWinUITextBlock::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (m_textSelectionEnabled && event->button() == Qt::LeftButton) {
        // 双击选择单词
        int charIndex = getCharacterAtPosition(event->pos());
        selectWordAt(charIndex);
    }
    QWinUIWidget::mouseDoubleClickEvent(event);
}

void QWinUITextBlock::keyPressEvent(QKeyEvent* event)
{
    if (m_textSelectionEnabled) {
        if (event->matches(QKeySequence::SelectAll)) {
            selectAll();
            return;
        } else if (event->matches(QKeySequence::Copy)) {
            QString selected = selectedText();
            if (!selected.isEmpty()) {
                QApplication::clipboard()->setText(selected);
            }
            return;
        }
    }
    QWinUIWidget::keyPressEvent(event);
}

void QWinUITextBlock::focusInEvent(QFocusEvent* event)
{
    if (m_textSelectionEnabled) {
        update();
    }
    QWinUIWidget::focusInEvent(event);
}

void QWinUITextBlock::focusOutEvent(QFocusEvent* event)
{
    if (m_textSelectionEnabled) {
        update();
    }
    QWinUIWidget::focusOutEvent(event);
}

void QWinUITextBlock::contextMenuEvent(QContextMenuEvent* event)
{
    // 取消右键菜单功能
    Q_UNUSED(event)
    // 不显示任何右键菜单
}

int QWinUITextBlock::getCharacterAtPosition(const QPoint& pos) const
{
    QRect textRect = getTextRect();
    QFontMetrics fm(getEffectiveFont());

    // 检查是否在文本区域内
    if (!textRect.contains(pos)) {
        return pos.x() < textRect.left() ? 0 : m_text.length();
    }

    // 计算相对于文本开始的X位置
    int relativeX = pos.x() - textRect.left();

    // 使用二分查找来找到最接近的字符位置
    int left = 0;
    int right = m_text.length();

    while (left < right) {
        int mid = (left + right) / 2;
        QString textToMid = m_text.left(mid);
        int midX = fm.horizontalAdvance(textToMid);

        if (midX < relativeX) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }

    // 检查是否更接近前一个字符
    if (left > 0) {
        QString textToPrev = m_text.left(left - 1);
        QString textToCurrent = m_text.left(left);
        int prevX = fm.horizontalAdvance(textToPrev);
        int currentX = fm.horizontalAdvance(textToCurrent);

        if (qAbs(relativeX - prevX) < qAbs(relativeX - currentX)) {
            left--;
        }
    }

    return qBound(0, left, m_text.length());
}

QT_END_NAMESPACE
