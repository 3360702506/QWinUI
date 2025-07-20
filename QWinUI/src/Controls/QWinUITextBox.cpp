#include "QWinUI/Controls/QWinUITextBox.h"
#include "QWinUI/Controls/QWinUIScrollBar.h"
#include <QKeyEvent>
#include <QTextCursor>
#include <QTextDocument>

QWinUITextBox::QWinUITextBox(QWidget* parent)
    : QWinUIRichEditBox(parent)
    , m_ignoreTextChange(false)
{
    initializeTextBox();
}

QWinUITextBox::~QWinUITextBox()
{
    // 基类会自动清理
}

void QWinUITextBox::initializeTextBox()
{
    // 设置单行模式
    setupSingleLineMode();
    
    // 禁用富文本功能
    setAcceptRichText(false);

    // 单行输入框设置默认高度，但允许调整
    setMinimumHeight(24);
    setMaximumHeight(48);
    resize(250, 32); // 设置默认大小
}

void QWinUITextBox::setupSingleLineMode()
{
    if (m_textEdit) {
        // 禁用换行
        m_textEdit->setLineWrapMode(QTextEdit::NoWrap);
        m_textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        
        // 不设置固定高度，让QTextEdit跟随父控件大小调整
        
        // 连接文档变化信号来确保单行
        connect(m_textEdit->document(), &QTextDocument::contentsChanged,
                this, &QWinUITextBox::ensureSingleLine);
    }
    
    // 隐藏自定义滚动条（单行不需要）
    if (m_customScrollBar) {
        m_customScrollBar->hide();
        m_customScrollBar->setVisible(false);
    }
}

QString QWinUITextBox::text() const
{
    QString plainText = QWinUIRichEditBox::text();
    // 移除所有换行符，确保返回单行文本
    return plainText.replace('\n', ' ').replace('\r', ' ');
}

void QWinUITextBox::setText(const QString& text)
{
    // 移除换行符，确保设置单行文本
    QString singleLineText = text;
    singleLineText.replace('\n', ' ').replace('\r', ' ');
    
    m_ignoreTextChange = true;
    QWinUIRichEditBox::setText(singleLineText);
    m_ignoreTextChange = false;
}

QSize QWinUITextBox::sizeHint() const
{
    return QSize(250, 32);
}

QSize QWinUITextBox::minimumSizeHint() const
{
    return QSize(100, 32);
}

void QWinUITextBox::keyPressEvent(QKeyEvent* event)
{
    // 阻止换行键
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        // 发出回车信号（如果需要的话）
        event->accept();
        return;
    }
    
    // 传递给基类处理
    QWinUIRichEditBox::keyPressEvent(event);
}

bool QWinUITextBox::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == m_textEdit && event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        
        // 阻止换行键
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            return true; // 消费事件，不传递给QTextEdit
        }
    }
    
    return QWinUIRichEditBox::eventFilter(obj, event);
}

void QWinUITextBox::onTextChanged()
{
    if (!m_ignoreTextChange) {
        ensureSingleLine();
    }
    
    // 调用基类方法
    QWinUIRichEditBox::onTextChanged();
}

void QWinUITextBox::ensureSingleLine()
{
    if (!m_textEdit || m_ignoreTextChange) return;
    
    QString currentText = m_textEdit->toPlainText();
    
    // 检查是否包含换行符
    if (currentText.contains('\n') || currentText.contains('\r')) {
        m_ignoreTextChange = true;
        
        // 保存光标位置
        QTextCursor cursor = m_textEdit->textCursor();
        int position = cursor.position();
        
        // 移除换行符
        QString singleLineText = currentText;
        singleLineText.replace('\n', ' ').replace('\r', ' ');
        
        // 设置新文本
        m_textEdit->setPlainText(singleLineText);
        
        // 恢复光标位置
        cursor.setPosition(qMin(position, singleLineText.length()));
        m_textEdit->setTextCursor(cursor);
        
        m_ignoreTextChange = false;
    }
}
