#ifndef QWINUIRICHEDITBOX_H
#define QWINUIRICHEDITBOX_H

#include "../QWinUIWidget.h"
#include "QWinUITextInput.h"
#include <QPropertyAnimation>
#include <QPainter>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTimer>

QT_BEGIN_NAMESPACE
class QWinUIScrollBar;
QT_END_NAMESPACE

QT_BEGIN_NAMESPACE

class QWINUI_EXPORT QWinUIRichEditBox : public QWinUIWidget
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString placeholderText READ placeholderText WRITE setPlaceholderText NOTIFY placeholderTextChanged)
    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly NOTIFY readOnlyChanged)
    Q_PROPERTY(int maxLength READ maxLength WRITE setMaxLength NOTIFY maxLengthChanged)
    Q_PROPERTY(bool acceptRichText READ acceptRichText WRITE setAcceptRichText NOTIFY acceptRichTextChanged)
    Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor NOTIFY borderColorChanged)
    Q_PROPERTY(QColor focusBorderColor READ focusBorderColor WRITE setFocusBorderColor NOTIFY focusBorderColorChanged)
    Q_PROPERTY(double underlineProgress READ underlineProgress WRITE setUnderlineProgress)

public:
    explicit QWinUIRichEditBox(QWidget* parent = nullptr);
    ~QWinUIRichEditBox() override;

    // 文本属性
    QString text() const;
    void setText(const QString& text);
    
    QString placeholderText() const;
    void setPlaceholderText(const QString& text);
    
    bool isReadOnly() const;
    void setReadOnly(bool readOnly);
    
    int maxLength() const;
    void setMaxLength(int length);
    
    bool acceptRichText() const;
    void setAcceptRichText(bool accept);

    // 外观属性
    QColor borderColor() const;
    void setBorderColor(const QColor& color);
    
    QColor focusBorderColor() const;
    void setFocusBorderColor(const QColor& color);
    
    double underlineProgress() const;
    void setUnderlineProgress(double progress);

    // 富文本格式化方法
    void setFontBold(bool bold);
    void setFontItalic(bool italic);
    void setFontUnderline(bool underline);
    void setFontSize(int size);
    void setFontFamily(const QString& family);
    void setTextColor(const QColor& color);
    void setTextBackgroundColor(const QColor& color);

    // 获取当前格式
    bool isFontBold() const;
    bool isFontItalic() const;
    bool isFontUnderline() const;
    int fontSize() const;
    QString fontFamily() const;
    QColor textColor() const;
    QColor textBackgroundColor() const;

    // 便利方法
    void selectAll();
    void copy();
    void cut();
    void paste();
    void undo();
    void redo();
    void clear();

    // 尺寸提示
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void textChanged();
    void placeholderTextChanged(const QString& text);
    void readOnlyChanged(bool readOnly);
    void maxLengthChanged(int length);
    void acceptRichTextChanged(bool accept);
    void borderColorChanged(const QColor& color);
    void focusBorderColorChanged(const QColor& color);
    void focusIn();
    void focusOut();
    void selectionChanged();
    void currentCharFormatChanged(const QTextCharFormat& format);

protected:
    // 事件处理
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

    // 主题变化处理
    void onThemeChanged() override;

    // 供子类重写的方法
    virtual void onTextChanged();

private slots:
    void onSelectionChanged();
    void onCurrentCharFormatChanged(const QTextCharFormat& format);
    void onFocusAnimationFinished();
    void updatePlaceholderVisibility();

protected:
    // 核心组件 - 供子类访问
    QWinUITextInput* m_textInput;
    QWinUIScrollBar* m_customScrollBar;

private:
    void initializeComponent();
    void updateColors();
    void paintBottomLine(QPainter* painter);
    void setupScrollBarConnections();
    
    // 动画
    QPropertyAnimation* m_underlineAnimation;
    QPropertyAnimation* m_borderColorAnimation;
    
    // 状态
    bool m_hasFocus;
    bool m_isHovered;
    double m_underlineProgress;
    
    // 外观属性
    QColor m_borderColor;
    QColor m_hoverBorderColor;
    QColor m_focusBorderColor;
    QColor m_backgroundColor;
    QColor m_textColor;
    QColor m_placeholderColor;
    
    // 文本属性
    QString m_placeholderText;
    int m_maxLength;
    
    // 常量
    static constexpr int DEFAULT_BORDER_RADIUS = 4;
    static constexpr int DEFAULT_BORDER_WIDTH = 1;
    static constexpr int FOCUS_BORDER_WIDTH = 2;
    static constexpr int ANIMATION_DURATION = 200;
    static constexpr int UNDERLINE_HEIGHT = 2;
};

QT_END_NAMESPACE

#endif // QWINUIRICHEDITBOX_H
