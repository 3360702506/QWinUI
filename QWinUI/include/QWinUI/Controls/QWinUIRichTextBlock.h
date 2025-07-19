#ifndef QWINUIRICHTEXTBLOCK_H
#define QWINUIRICHTEXTBLOCK_H

#include "../QWinUIWidget.h"
#include <QString>
#include <QFont>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QTextBlockFormat>
#include <QUrl>
#include <QVariant>

QT_BEGIN_NAMESPACE

class QTextBrowser;

class QWINUI_EXPORT QWinUIRichTextBlock : public QWinUIWidget
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString html READ html WRITE setHtml NOTIFY htmlChanged)
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)
    Q_PROPERTY(Qt::Alignment textAlignment READ textAlignment WRITE setTextAlignment NOTIFY textAlignmentChanged)
    Q_PROPERTY(QWinUITextWrapping textWrapping READ textWrapping WRITE setTextWrapping NOTIFY textWrappingChanged)
    Q_PROPERTY(int maxLines READ maxLines WRITE setMaxLines NOTIFY maxLinesChanged)
    Q_PROPERTY(double lineHeight READ lineHeight WRITE setLineHeight NOTIFY lineHeightChanged)
    Q_PROPERTY(QColor foreground READ foreground WRITE setForeground NOTIFY foregroundChanged)
    Q_PROPERTY(bool isTextSelectionEnabled READ isTextSelectionEnabled WRITE setTextSelectionEnabled NOTIFY textSelectionEnabledChanged)
    Q_PROPERTY(bool isHyperlinkEnabled READ isHyperlinkEnabled WRITE setHyperlinkEnabled NOTIFY hyperlinkEnabledChanged)
    Q_PROPERTY(bool isReadOnly READ isReadOnly WRITE setReadOnly NOTIFY readOnlyChanged)

public:
    enum QWinUITextWrapping {
        NoWrap = 0,
        Wrap = 1,
        WrapWholeWords = 2
    };
    Q_ENUM(QWinUITextWrapping)

    // 内联元素类型
    enum InlineType {
        Run,           // 普通文本
        Hyperlink,     // 超链接
        Bold,          // 粗体
        Italic,        // 斜体
        Underline,     // 下划线
        LineBreak      // 换行
    };
    Q_ENUM(InlineType)

    // 内联元素结构
    struct Inline {
        InlineType type;
        QString text;
        QVariantMap properties; // 存储额外属性，如URL、颜色等
        QTextCharFormat format;
    };

    explicit QWinUIRichTextBlock(QWidget* parent = nullptr);
    explicit QWinUIRichTextBlock(const QString& text, QWidget* parent = nullptr);
    ~QWinUIRichTextBlock();

    // 基本属性
    QString text() const;
    void setText(const QString& text);

    QString html() const;
    void setHtml(const QString& html);

    QFont font() const;
    void setFont(const QFont& font);

    Qt::Alignment textAlignment() const;
    void setTextAlignment(Qt::Alignment alignment);

    QWinUITextWrapping textWrapping() const;
    void setTextWrapping(QWinUITextWrapping wrapping);

    int maxLines() const;
    void setMaxLines(int maxLines);

    double lineHeight() const;
    void setLineHeight(double height);

    QColor foreground() const;
    void setForeground(const QColor& color);

    bool isTextSelectionEnabled() const;
    void setTextSelectionEnabled(bool enabled);

    bool isHyperlinkEnabled() const;
    void setHyperlinkEnabled(bool enabled);

    bool isReadOnly() const;
    void setReadOnly(bool readOnly);

    // 富文本操作
    void addInline(const Inline& inlineElement);
    void clearInlines();
    QList<Inline> getInlines() const;

    // 便利方法
    void addRun(const QString& text, const QTextCharFormat& format = QTextCharFormat());
    void addHyperlink(const QString& text, const QUrl& url, const QTextCharFormat& format = QTextCharFormat());
    void addBold(const QString& text);
    void addItalic(const QString& text);
    void addUnderline(const QString& text);
    void addLineBreak();

    // 文本操作
    QString selectedText() const;
    void selectAll();
    void clearSelection();

    // 尺寸计算
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

    // 主题响应
    void onThemeChanged() override;

signals:
    void textChanged(const QString& text);
    void htmlChanged(const QString& html);
    void fontChanged(const QFont& font);
    void textAlignmentChanged(Qt::Alignment alignment);
    void textWrappingChanged(QWinUITextWrapping wrapping);
    void maxLinesChanged(int maxLines);
    void lineHeightChanged(double height);
    void foregroundChanged(const QColor& color);
    void textSelectionEnabledChanged(bool enabled);
    void hyperlinkEnabledChanged(bool enabled);
    void readOnlyChanged(bool readOnly);
    void selectionChanged();
    void hyperlinkClicked(const QUrl& url);

private:
    void initializeRichTextBlock();
    void updateTextDocument();
    void updateTextBrowser();
    void buildDocumentFromInlines();
    void applyThemeToDocument();
    QTextCharFormat getDefaultCharFormat() const;
    QTextCharFormat getHyperlinkFormat() const;
    QColor getEffectiveForeground() const;
    QFont getEffectiveFont() const;

private slots:
    void onAnchorClicked(const QUrl& url);
    void onTextBrowserSelectionChanged();

private:
    // 基本属性
    QString m_text;
    QString m_html;
    QFont m_font;
    Qt::Alignment m_textAlignment;
    QWinUITextWrapping m_textWrapping;
    int m_maxLines;
    double m_lineHeight;
    QColor m_foreground;
    bool m_textSelectionEnabled;
    bool m_hyperlinkEnabled;
    bool m_readOnly;

    // 富文本内容
    QList<Inline> m_inlines;
    
    // 文本渲染
    QTextDocument* m_textDocument;
    QTextBrowser* m_textBrowser;
    
    // 缓存
    mutable QSize m_cachedSizeHint;
    mutable bool m_sizeHintDirty;
};

QT_END_NAMESPACE

#endif // QWINUIRICHTEXTBLOCK_H
