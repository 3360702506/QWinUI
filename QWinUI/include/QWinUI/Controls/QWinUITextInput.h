#ifndef QWINUITEXTINPUT_H
#define QWINUITEXTINPUT_H

#include "../QWinUIWidget.h"
#include <QTimer>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QUndoStack>
#include <QMimeData>

QT_BEGIN_NAMESPACE

// 前向声明
class QWinUITextInput;

// 文本片段结构，支持富文本格式
struct QWinUITextFragment {
    QString text;
    QTextCharFormat format;
    int start;
    int length;
    
    QWinUITextFragment() : start(0), length(0) {}
    QWinUITextFragment(const QString& t, int s, int l, const QTextCharFormat& f = QTextCharFormat()) 
        : text(t), format(f), start(s), length(l) {}
};

// 撤销/重做命令
class QWinUITextCommand : public QUndoCommand {
public:
    enum Type { Insert, Delete, Format };
    
    QWinUITextCommand(Type type, int position, const QString& text, 
                      const QTextCharFormat& format = QTextCharFormat());
    
    void undo() override;
    void redo() override;

    void setTextInput(QWinUITextInput* textInput);
    
private:
    Type m_type;
    int m_position;
    QString m_text;
    QTextCharFormat m_format;
    QWinUITextInput* m_textInput;
};

class QWINUI_EXPORT QWinUITextInput : public QWinUIWidget
{
    Q_OBJECT
    friend class QWinUITextCommand;
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString placeholderText READ placeholderText WRITE setPlaceholderText)
    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly)
    Q_PROPERTY(int maxLength READ maxLength WRITE setMaxLength)
    Q_PROPERTY(bool multiLine READ isMultiLine WRITE setMultiLine)
    Q_PROPERTY(bool richTextEnabled READ isRichTextEnabled WRITE setRichTextEnabled)
    Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor)
    Q_PROPERTY(QColor selectionColor READ selectionColor WRITE setSelectionColor)
    Q_PROPERTY(QColor cursorColor READ cursorColor WRITE setCursorColor)

public:
    explicit QWinUITextInput(QWidget* parent = nullptr);
    ~QWinUITextInput() override;

    // 文本属性
    QString text() const;
    void setText(const QString& text);
    
    QString placeholderText() const;
    void setPlaceholderText(const QString& text);
    
    bool isReadOnly() const;
    void setReadOnly(bool readOnly);
    
    int maxLength() const;
    void setMaxLength(int length);
    
    bool isMultiLine() const;
    void setMultiLine(bool multiLine);
    
    bool isRichTextEnabled() const;
    void setRichTextEnabled(bool enabled);

    // 颜色属性
    QColor textColor() const;
    void setTextColor(const QColor& color);
    
    QColor selectionColor() const;
    void setSelectionColor(const QColor& color);
    
    QColor cursorColor() const;
    void setCursorColor(const QColor& color);

    // 光标和选择
    int cursorPosition() const;
    void setCursorPosition(int position);
    
    bool hasSelection() const;
    QString selectedText() const;
    void selectAll();
    void clearSelection();
    
    // 编辑操作
    void cut();
    void copy();
    void paste();
    void undo();
    void redo();
    void clear();
    
    // 富文本格式
    void setBold(bool bold);
    void setItalic(bool italic);
    void setUnderline(bool underline);
    void setTextColor(int start, int length, const QColor& color);
    void setFont(int start, int length, const QFont& font);

    // 尺寸提示
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void textChanged(const QString& text);
    void cursorPositionChanged(int position);
    void selectionChanged();
    void editingFinished();

protected:
    // 事件处理
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void inputMethodEvent(QInputMethodEvent* event) override;
    QVariant inputMethodQuery(Qt::InputMethodQuery query) const override;
    
    // 主题变化处理
    void onThemeChanged() override;

private slots:
    void onCursorBlink();
    void updateColors();

private:
    // 核心方法
    void initializeTextInput();
    void updateTextLayout();
    void ensureCursorVisible();
    
    // 绘制方法
    void drawBackground(QPainter* painter, const QRect& rect);
    void drawText(QPainter* painter, const QRect& rect);
    void drawSelection(QPainter* painter, const QRect& rect);
    void drawCursor(QPainter* painter, const QRect& rect);
    void drawPlaceholder(QPainter* painter, const QRect& rect);
    
    // 文本处理
    int positionFromPoint(const QPoint& point) const;
    QRect cursorRect(int position) const;
    QRect selectionRect() const;
    void insertText(const QString& text);
    void deleteText(int start, int length);
    void deleteSelection();
    void formatText(int start, int length, const QTextCharFormat& format);
    
    // 光标和选择
    void moveCursor(int position, bool select = false);
    void updateSelection(int start, int end);
    
    // 撤销/重做
    void addCommand(QWinUITextCommand* command);

private:
    // 文本数据
    QString m_text;
    QList<QWinUITextFragment> m_fragments;
    QString m_placeholderText;

    // 滚动偏移
    int m_horizontalOffset;
    
    // 光标和选择
    int m_cursorPosition;
    int m_selectionStart;
    int m_selectionEnd;
    bool m_cursorVisible;
    QTimer* m_cursorTimer;
    
    // 状态
    bool m_readOnly;
    bool m_multiLine;
    bool m_richTextEnabled;
    int m_maxLength;
    bool m_selecting;
    
    // 颜色
    QColor m_textColor;
    QColor m_selectionColor;
    QColor m_cursorColor;
    QColor m_placeholderColor;
    QColor m_backgroundColor;
    QColor m_borderColor;
    
    // 撤销/重做
    QUndoStack* m_undoStack;
    
    // 布局缓存
    mutable QRect m_textRect;
    mutable bool m_layoutDirty;
    
    // 常量
    static constexpr int DEFAULT_CURSOR_WIDTH = 1;
    static constexpr int DEFAULT_PADDING = 8;
    static constexpr int CURSOR_BLINK_INTERVAL = 500;
};

QT_END_NAMESPACE

#endif // QWINUITEXTINPUT_H
