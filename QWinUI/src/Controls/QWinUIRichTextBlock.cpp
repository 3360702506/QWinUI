#include "QWinUI/Controls/QWinUIRichTextBlock.h"
#include "QWinUI/QWinUITheme.h"
#include <QPainter>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QApplication>
#include <QClipboard>
#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QTextBlock>
#include <QTextBlockFormat>
#include <QTextCharFormat>
#include <QScrollBar>
#include <QDesktopServices>
#include <cmath>

QT_BEGIN_NAMESPACE

QWinUIRichTextBlock::QWinUIRichTextBlock(QWidget* parent)
    : QWinUIWidget(parent)
    , m_textAlignment(Qt::AlignLeft | Qt::AlignTop)
    , m_textWrapping(Wrap)
    , m_maxLines(-1)
    , m_lineHeight(1.0)
    , m_textSelectionEnabled(true)
    , m_hyperlinkEnabled(true)
    , m_readOnly(true)
    , m_textDocument(nullptr)
    , m_textBrowser(nullptr)
    , m_sizeHintDirty(true)
{
    initializeRichTextBlock();
}

QWinUIRichTextBlock::QWinUIRichTextBlock(const QString& text, QWidget* parent)
    : QWinUIWidget(parent)
    , m_text(text)
    , m_textAlignment(Qt::AlignLeft | Qt::AlignTop)
    , m_textWrapping(Wrap)
    , m_maxLines(-1)
    , m_lineHeight(1.0)
    , m_textSelectionEnabled(true)
    , m_hyperlinkEnabled(true)
    , m_readOnly(true)
    , m_textDocument(nullptr)
    , m_textBrowser(nullptr)
    , m_sizeHintDirty(true)
{
    initializeRichTextBlock();
}

QWinUIRichTextBlock::~QWinUIRichTextBlock()
{
    if (m_textDocument) {
        delete m_textDocument;
    }
}

void QWinUIRichTextBlock::initializeRichTextBlock()
{
    // 设置基本属性
    setFocusPolicy(m_textSelectionEnabled ? Qt::StrongFocus : Qt::NoFocus);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    
    // 创建布局
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    
    // 创建文本浏览器
    m_textBrowser = new QTextBrowser(this);
    m_textBrowser->setFrameStyle(QFrame::NoFrame);
    m_textBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_textBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_textBrowser->setReadOnly(m_readOnly);
    m_textBrowser->setOpenExternalLinks(false);
    m_textBrowser->setOpenLinks(false);
    m_textBrowser->setContextMenuPolicy(Qt::NoContextMenu); // 禁用右键菜单
    
    // 设置透明背景
    m_textBrowser->setStyleSheet("QTextBrowser { background: transparent; border: none; }");
    
    layout->addWidget(m_textBrowser);
    
    // 设置默认字体
    m_font = QApplication::font();
    
    // 连接信号
    connect(m_textBrowser, &QTextBrowser::anchorClicked, this, &QWinUIRichTextBlock::onAnchorClicked);
    connect(m_textBrowser, &QTextBrowser::selectionChanged, this, &QWinUIRichTextBlock::onTextBrowserSelectionChanged);
    
    // 连接主题变化信号
    connect(this, &QWinUIWidget::themeChanged, this, &QWinUIRichTextBlock::onThemeChanged);
    
    // 初始化文档
    updateTextBrowser();
}

QString QWinUIRichTextBlock::text() const
{
    return m_text;
}

void QWinUIRichTextBlock::setText(const QString& text)
{
    if (m_text != text) {
        m_text = text;
        m_html.clear();
        m_inlines.clear();
        
        // 创建简单的Run内联元素
        if (!text.isEmpty()) {
            Inline run;
            run.type = Run;
            run.text = text;
            run.format = getDefaultCharFormat();
            m_inlines.append(run);
        }
        
        m_sizeHintDirty = true;
        updateTextBrowser();
        emit textChanged(text);
    }
}

QString QWinUIRichTextBlock::html() const
{
    return m_html;
}

void QWinUIRichTextBlock::setHtml(const QString& html)
{
    if (m_html != html) {
        m_html = html;
        m_text = m_textBrowser ? m_textBrowser->toPlainText() : QString();
        m_sizeHintDirty = true;
        
        if (m_textBrowser) {
            m_textBrowser->setHtml(html);
            applyThemeToDocument();
        }
        
        emit htmlChanged(html);
        emit textChanged(m_text);
    }
}

QFont QWinUIRichTextBlock::font() const
{
    return m_font;
}

void QWinUIRichTextBlock::setFont(const QFont& font)
{
    if (m_font != font) {
        m_font = font;
        m_sizeHintDirty = true;
        updateTextBrowser();
        emit fontChanged(font);
    }
}

Qt::Alignment QWinUIRichTextBlock::textAlignment() const
{
    return m_textAlignment;
}

void QWinUIRichTextBlock::setTextAlignment(Qt::Alignment alignment)
{
    if (m_textAlignment != alignment) {
        m_textAlignment = alignment;
        updateTextBrowser();
        emit textAlignmentChanged(alignment);
    }
}

QWinUIRichTextBlock::QWinUITextWrapping QWinUIRichTextBlock::textWrapping() const
{
    return m_textWrapping;
}

void QWinUIRichTextBlock::setTextWrapping(QWinUITextWrapping wrapping)
{
    if (m_textWrapping != wrapping) {
        m_textWrapping = wrapping;
        m_sizeHintDirty = true;
        updateTextBrowser();
        emit textWrappingChanged(wrapping);
    }
}

int QWinUIRichTextBlock::maxLines() const
{
    return m_maxLines;
}

void QWinUIRichTextBlock::setMaxLines(int maxLines)
{
    if (m_maxLines != maxLines) {
        m_maxLines = maxLines;
        m_sizeHintDirty = true;
        updateTextBrowser();
        emit maxLinesChanged(maxLines);
    }
}

double QWinUIRichTextBlock::lineHeight() const
{
    return m_lineHeight;
}

void QWinUIRichTextBlock::setLineHeight(double height)
{
    if (qAbs(m_lineHeight - height) > 0.001) {
        m_lineHeight = height;
        m_sizeHintDirty = true;
        updateTextBrowser();
        emit lineHeightChanged(height);
    }
}

QColor QWinUIRichTextBlock::foreground() const
{
    return m_foreground;
}

void QWinUIRichTextBlock::setForeground(const QColor& color)
{
    if (m_foreground != color) {
        m_foreground = color;
        updateTextBrowser();
        emit foregroundChanged(color);
    }
}

bool QWinUIRichTextBlock::isTextSelectionEnabled() const
{
    return m_textSelectionEnabled;
}

void QWinUIRichTextBlock::setTextSelectionEnabled(bool enabled)
{
    if (m_textSelectionEnabled != enabled) {
        m_textSelectionEnabled = enabled;
        setFocusPolicy(enabled ? Qt::StrongFocus : Qt::NoFocus);
        
        if (m_textBrowser) {
            m_textBrowser->setTextInteractionFlags(enabled ? 
                Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard : Qt::NoTextInteraction);
        }
        
        emit textSelectionEnabledChanged(enabled);
    }
}

bool QWinUIRichTextBlock::isHyperlinkEnabled() const
{
    return m_hyperlinkEnabled;
}

void QWinUIRichTextBlock::setHyperlinkEnabled(bool enabled)
{
    if (m_hyperlinkEnabled != enabled) {
        m_hyperlinkEnabled = enabled;
        updateTextBrowser();
        emit hyperlinkEnabledChanged(enabled);
    }
}

bool QWinUIRichTextBlock::isReadOnly() const
{
    return m_readOnly;
}

void QWinUIRichTextBlock::setReadOnly(bool readOnly)
{
    if (m_readOnly != readOnly) {
        m_readOnly = readOnly;
        if (m_textBrowser) {
            m_textBrowser->setReadOnly(readOnly);
        }
        emit readOnlyChanged(readOnly);
    }
}

void QWinUIRichTextBlock::addInline(const Inline& inlineElement)
{
    m_inlines.append(inlineElement);
    m_sizeHintDirty = true;
    buildDocumentFromInlines();
}

void QWinUIRichTextBlock::clearInlines()
{
    m_inlines.clear();
    m_text.clear();
    m_html.clear();
    m_sizeHintDirty = true;
    updateTextBrowser();
}

QList<QWinUIRichTextBlock::Inline> QWinUIRichTextBlock::getInlines() const
{
    return m_inlines;
}

void QWinUIRichTextBlock::addRun(const QString& text, const QTextCharFormat& format)
{
    Inline run;
    run.type = Run;
    run.text = text;
    run.format = format.isEmpty() ? getDefaultCharFormat() : format;
    addInline(run);
}

void QWinUIRichTextBlock::addHyperlink(const QString& text, const QUrl& url, const QTextCharFormat& format)
{
    Inline hyperlink;
    hyperlink.type = Hyperlink;
    hyperlink.text = text;
    hyperlink.properties["url"] = url;
    // 总是使用当前主题的超链接格式
    hyperlink.format = getHyperlinkFormat();
    addInline(hyperlink);
}

void QWinUIRichTextBlock::addBold(const QString& text)
{
    QTextCharFormat format = getDefaultCharFormat();
    format.setFontWeight(QFont::Bold);
    addRun(text, format);
}

void QWinUIRichTextBlock::addItalic(const QString& text)
{
    QTextCharFormat format = getDefaultCharFormat();
    format.setFontItalic(true);
    addRun(text, format);
}

void QWinUIRichTextBlock::addUnderline(const QString& text)
{
    QTextCharFormat format = getDefaultCharFormat();
    format.setFontUnderline(true);
    addRun(text, format);
}

void QWinUIRichTextBlock::addLineBreak()
{
    Inline lineBreak;
    lineBreak.type = LineBreak;
    lineBreak.text = "\n";
    addInline(lineBreak);
}

QString QWinUIRichTextBlock::selectedText() const
{
    return m_textBrowser ? m_textBrowser->textCursor().selectedText() : QString();
}

void QWinUIRichTextBlock::selectAll()
{
    if (m_textBrowser && m_textSelectionEnabled) {
        m_textBrowser->selectAll();
    }
}

void QWinUIRichTextBlock::clearSelection()
{
    if (m_textBrowser) {
        QTextCursor cursor = m_textBrowser->textCursor();
        cursor.clearSelection();
        m_textBrowser->setTextCursor(cursor);
    }
}

QSize QWinUIRichTextBlock::sizeHint() const
{
    if (m_sizeHintDirty) {
        if (m_textBrowser) {
            QTextDocument* doc = m_textBrowser->document();
            if (doc) {
                m_cachedSizeHint = doc->size().toSize();

                // 应用最大行数限制
                if (m_maxLines > 0) {
                    QFontMetrics fm(getEffectiveFont());
                    int maxHeight = fm.height() * m_maxLines * m_lineHeight;
                    m_cachedSizeHint.setHeight(qMin(m_cachedSizeHint.height(), maxHeight));
                }
            } else {
                m_cachedSizeHint = QSize(200, 20);
            }
        } else {
            m_cachedSizeHint = QSize(200, 20);
        }
        m_sizeHintDirty = false;
    }
    return m_cachedSizeHint;
}

QSize QWinUIRichTextBlock::minimumSizeHint() const
{
    QFontMetrics fm(getEffectiveFont());
    return QSize(fm.horizontalAdvance("..."), fm.height());
}

void QWinUIRichTextBlock::paintEvent(QPaintEvent* event)
{
    // 绘制基础背景
    QWinUIWidget::paintEvent(event);

    // TextBrowser 会自己处理绘制
}

void QWinUIRichTextBlock::mousePressEvent(QMouseEvent* event)
{
    QWinUIWidget::mousePressEvent(event);
}

void QWinUIRichTextBlock::mouseMoveEvent(QMouseEvent* event)
{
    QWinUIWidget::mouseMoveEvent(event);
}

void QWinUIRichTextBlock::mouseReleaseEvent(QMouseEvent* event)
{
    QWinUIWidget::mouseReleaseEvent(event);
}

void QWinUIRichTextBlock::keyPressEvent(QKeyEvent* event)
{
    QWinUIWidget::keyPressEvent(event);
}

void QWinUIRichTextBlock::focusInEvent(QFocusEvent* event)
{
    if (m_textBrowser) {
        m_textBrowser->setFocus();
    }
    QWinUIWidget::focusInEvent(event);
}

void QWinUIRichTextBlock::focusOutEvent(QFocusEvent* event)
{
    QWinUIWidget::focusOutEvent(event);
}

void QWinUIRichTextBlock::contextMenuEvent(QContextMenuEvent* event)
{
    // 取消右键菜单功能
    Q_UNUSED(event)
    // 不显示任何右键菜单
}

void QWinUIRichTextBlock::resizeEvent(QResizeEvent* event)
{
    QWinUIWidget::resizeEvent(event);
    m_sizeHintDirty = true;

    if (m_textBrowser && m_textWrapping != NoWrap) {
        // 更新文档宽度以支持换行
        QTextDocument* doc = m_textBrowser->document();
        if (doc) {
            doc->setTextWidth(width());
        }
    }
}

void QWinUIRichTextBlock::onThemeChanged()
{
    // 重新构建文档以保持格式
    if (!m_inlines.isEmpty()) {
        buildDocumentFromInlines();
    } else {
        applyThemeToDocument();
    }
    update();
}

void QWinUIRichTextBlock::updateTextBrowser()
{
    if (!m_textBrowser) return;

    // 设置字体
    m_textBrowser->setFont(getEffectiveFont());

    // 设置文本交互标志
    Qt::TextInteractionFlags flags = Qt::NoTextInteraction;
    if (m_textSelectionEnabled) {
        flags |= Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard;
    }
    if (m_hyperlinkEnabled) {
        flags |= Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard;
    }
    m_textBrowser->setTextInteractionFlags(flags);

    // 设置换行模式
    switch (m_textWrapping) {
    case NoWrap:
        m_textBrowser->setLineWrapMode(QTextEdit::NoWrap);
        break;
    case Wrap:
    case WrapWholeWords:
        m_textBrowser->setLineWrapMode(QTextEdit::WidgetWidth);
        break;
    }

    // 构建文档内容
    if (!m_html.isEmpty()) {
        m_textBrowser->setHtml(m_html);
    } else if (!m_inlines.isEmpty()) {
        buildDocumentFromInlines();
    } else if (!m_text.isEmpty()) {
        m_textBrowser->setPlainText(m_text);
    } else {
        m_textBrowser->clear();
    }

    // 应用主题
    applyThemeToDocument();
}

void QWinUIRichTextBlock::buildDocumentFromInlines()
{
    if (!m_textBrowser || m_inlines.isEmpty()) return;

    QTextDocument* doc = m_textBrowser->document();
    doc->clear();

    QTextCursor cursor(doc);

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
    for (const Inline& inlineElement : m_inlines) {
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
            if (m_hyperlinkEnabled) {
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
    m_text = doc->toPlainText();
    m_html = doc->toHtml();
}

void QWinUIRichTextBlock::applyThemeToDocument()
{
    if (!m_textBrowser) return;

    QTextDocument* doc = m_textBrowser->document();
    if (!doc) return;

    // 根据主题模式设置超链接颜色
    QWinUITheme* theme = QWinUITheme::getInstance();
    QString linkColor;
    if (theme && theme->themeMode() == QWinUIThemeMode::Dark) {
        linkColor = "#63A2FF"; // 深色模式下使用浅蓝色
    } else {
        linkColor = "#0078D7"; // 浅色模式下使用深蓝色
    }

    // 设置默认样式表
    QString styleSheet = QString(
        "body { color: %1; font-family: %2; font-size: %3pt; }"
        "a { color: %4; text-decoration: none; }"
        "a:hover { text-decoration: underline; }"
    ).arg(getEffectiveForeground().name())
     .arg(getEffectiveFont().family())
     .arg(getEffectiveFont().pointSize())
     .arg(linkColor);

    doc->setDefaultStyleSheet(styleSheet);

    // 只更新默认字体，不覆盖现有格式
    QTextCharFormat defaultFormat;
    defaultFormat.setFont(getEffectiveFont());
    defaultFormat.setForeground(getEffectiveForeground());
    doc->setDefaultFont(getEffectiveFont());
}

QTextCharFormat QWinUIRichTextBlock::getDefaultCharFormat() const
{
    QTextCharFormat format;
    format.setFont(getEffectiveFont());
    format.setForeground(getEffectiveForeground());
    return format;
}

QTextCharFormat QWinUIRichTextBlock::getHyperlinkFormat() const
{
    QTextCharFormat format = getDefaultCharFormat();

    // 根据主题模式设置超链接颜色
    QWinUITheme* theme = QWinUITheme::getInstance();
    if (theme && theme->themeMode() == QWinUIThemeMode::Dark) {
        // 深色模式下使用浅蓝色
        format.setForeground(QColor(99, 162, 255));
    } else {
        // 浅色模式下使用深蓝色
        format.setForeground(QColor(0, 120, 215));
    }

    format.setFontUnderline(false); // WinUI 3 超链接默认无下划线
    return format;
}

QColor QWinUIRichTextBlock::getEffectiveForeground() const
{
    if (m_foreground.isValid()) {
        return m_foreground;
    }
    return themeTextColor();
}

QFont QWinUIRichTextBlock::getEffectiveFont() const
{
    if (!m_font.family().isEmpty()) {
        return m_font;
    }

    // 使用 WinUI 3 默认字体
    QFont defaultFont("Segoe UI", 14);
    return defaultFont;
}

void QWinUIRichTextBlock::onAnchorClicked(const QUrl& url)
{
    if (m_hyperlinkEnabled) {
        emit hyperlinkClicked(url);

        // 默认行为：打开外部链接
        if (url.scheme() == "http" || url.scheme() == "https" || url.scheme() == "mailto") {
            QDesktopServices::openUrl(url);
        }
    }
}

void QWinUIRichTextBlock::onTextBrowserSelectionChanged()
{
    emit selectionChanged();
}

QT_END_NAMESPACE
