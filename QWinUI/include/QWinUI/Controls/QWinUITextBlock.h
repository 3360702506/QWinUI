#ifndef QWINUITEXTBLOCK_H
#define QWINUITEXTBLOCK_H

#include "../QWinUIWidget.h"
#include <QString>
#include <QFont>
#include <QTextOption>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QList>
#include <QUrl>

QT_BEGIN_NAMESPACE

class QWINUI_EXPORT QWinUITextBlock : public QWinUIWidget
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)
    Q_PROPERTY(Qt::Alignment textAlignment READ textAlignment WRITE setTextAlignment NOTIFY textAlignmentChanged)
    Q_PROPERTY(QWinUITextWrapping textWrapping READ textWrapping WRITE setTextWrapping NOTIFY textWrappingChanged)
    Q_PROPERTY(QWinUITextTrimming textTrimming READ textTrimming WRITE setTextTrimming NOTIFY textTrimmingChanged)
    Q_PROPERTY(int maxLines READ maxLines WRITE setMaxLines NOTIFY maxLinesChanged)
    Q_PROPERTY(double lineHeight READ lineHeight WRITE setLineHeight NOTIFY lineHeightChanged)
    Q_PROPERTY(QColor foreground READ foreground WRITE setForeground NOTIFY foregroundChanged)
    Q_PROPERTY(bool isTextSelectionEnabled READ isTextSelectionEnabled WRITE setTextSelectionEnabled NOTIFY textSelectionEnabledChanged)

public:
    enum QWinUITextWrapping {
        NoWrap = 0,
        Wrap = 1,
        WrapWholeWords = 2
    };
    Q_ENUM(QWinUITextWrapping)

    enum QWinUITextTrimming {
        None = 0,
        CharacterEllipsis = 1,
        WordEllipsis = 2
    };
    Q_ENUM(QWinUITextTrimming)

    // 内联元素类型
    enum QWinUIInlineType {
        Run = 0,
        Bold = 1,
        Italic = 2,
        Underline = 3,
        Hyperlink = 4,
        LineBreak = 5
    };
    Q_ENUM(QWinUIInlineType)

    // 内联元素结构
    struct QWinUIInline {
        QWinUIInlineType type;
        QString text;
        QTextCharFormat format;
        QVariantMap properties; // 用于存储超链接URL等额外属性
    };

    explicit QWinUITextBlock(QWidget* parent = nullptr);
    explicit QWinUITextBlock(const QString& text, QWidget* parent = nullptr);
    ~QWinUITextBlock();

    // 属性访问器
    QString text() const;
    void setText(const QString& text);

    QFont font() const;
    void setFont(const QFont& font);

    Qt::Alignment textAlignment() const;
    void setTextAlignment(Qt::Alignment alignment);

    QWinUITextWrapping textWrapping() const;
    void setTextWrapping(QWinUITextWrapping wrapping);

    QWinUITextTrimming textTrimming() const;
    void setTextTrimming(QWinUITextTrimming trimming);

    int maxLines() const;
    void setMaxLines(int maxLines);

    double lineHeight() const;
    void setLineHeight(double height);

    QColor foreground() const;
    void setForeground(const QColor& color);

    bool isTextSelectionEnabled() const;
    void setTextSelectionEnabled(bool enabled);

    // 文本操作
    QString selectedText() const;
    void selectAll();
    void clearSelection();
    void selectWordAt(int position);

    // 内联元素管理
    void clearInlines();
    void addInline(const QWinUIInline& inlineElement);
    void addRun(const QString& text, const QTextCharFormat& format = QTextCharFormat());
    void addBold(const QString& text);
    void addItalic(const QString& text);
    void addUnderline(const QString& text);
    void addHyperlink(const QString& text, const QUrl& url, const QTextCharFormat& format = QTextCharFormat());
    void addLineBreak();
    QList<QWinUIInline> getInlines() const;

    // 样式设置
    void setFontWeight(QFont::Weight weight);
    void setFontItalic(bool italic);
    void setFontUnderline(bool underline);
    void setFontSize(int pointSize);
    void setFontFamily(const QString& family);

    // 尺寸计算
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

    // 主题响应
    void onThemeChanged() override;

signals:
    void textChanged(const QString& text);
    void fontChanged(const QFont& font);
    void textAlignmentChanged(Qt::Alignment alignment);
    void textWrappingChanged(QWinUITextWrapping wrapping);
    void textTrimmingChanged(QWinUITextTrimming trimming);
    void maxLinesChanged(int maxLines);
    void lineHeightChanged(double height);
    void foregroundChanged(const QColor& color);
    void textSelectionEnabledChanged(bool enabled);
    void hyperlinkClicked(const QUrl& url);
    void selectionChanged();

private:
    void initializeTextBlock();
    void updateTextDocument();
    void updateTextLayout();
    void buildDocumentFromInlines();
    void drawText(QPainter* painter, const QRect& rect);
    void drawSelection(QPainter* painter, const QRect& rect);
    QString getDisplayText() const;
    QRect getTextRect() const;
    int getCharacterAtPosition(const QPoint& pos) const;
    QRect getSelectionRect() const;
    void updateCursor();
    QFont getEffectiveFont() const;
    QColor getEffectiveForeground() const;
    QSize calculateSizeHint() const;
    QTextCharFormat getDefaultCharFormat() const;
    QTextCharFormat getHyperlinkFormat() const;

private:
    QString m_text;
    QFont m_font;
    Qt::Alignment m_textAlignment;
    QWinUITextWrapping m_textWrapping;
    QWinUITextTrimming m_textTrimming;
    int m_maxLines;
    double m_lineHeight;
    QColor m_foreground;
    bool m_textSelectionEnabled;

    // 文本布局和渲染
    QTextDocument* m_textDocument;
    QTextOption m_textOption;

    // 内联元素
    QList<QWinUIInline> m_inlines;
    bool m_useInlines;

    // 文本选择
    bool m_isSelecting;
    int m_selectionStart;
    int m_selectionEnd;
    QPoint m_selectionStartPos;
    
    // 缓存
    mutable QSize m_cachedSizeHint;
    mutable bool m_sizeHintDirty;
};

QT_END_NAMESPACE

#endif // QWINUITEXTBLOCK_H
