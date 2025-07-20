#include "QWinUI/Controls/QWinUIPasswordBox.h"
#include "QWinUI/Controls/QWinUIScrollBar.h"
#include "QWinUI/QWinUITheme.h"
#include <QKeyEvent>
#include <QTextCursor>
#include <QTextDocument>
#include <QHBoxLayout>
#include <QApplication>
#include <QClipboard>

QWinUIPasswordBox::QWinUIPasswordBox(QWidget* parent)
    : QWinUIRichEditBox(parent)
    , m_passwordCharacter('*')
    , m_passwordVisible(false)
    , m_showPasswordToggle(true)
    , m_ignoreTextChange(false)
    , m_passwordToggleButton(nullptr)
{
    initializePasswordBox();
}

QWinUIPasswordBox::~QWinUIPasswordBox()
{
    // 基类会自动清理
}

void QWinUIPasswordBox::initializePasswordBox()
{
    // 设置单行模式
    setupSingleLineMode();
    
    // 禁用富文本功能
    setAcceptRichText(false);

    // 单行输入框设置默认高度，但允许调整
    setMinimumHeight(24);
    setMaximumHeight(48);
    resize(250, 32); // 设置默认大小
    
    // 设置密码切换按钮
    setupPasswordToggle();
}

void QWinUIPasswordBox::setupSingleLineMode()
{
    if (m_textEdit) {
        // 禁用换行
        m_textEdit->setLineWrapMode(QTextEdit::NoWrap);
        m_textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        
        // 不设置固定高度，让QTextEdit跟随父控件大小调整
        
        // 连接文档变化信号来确保单行
        connect(m_textEdit->document(), &QTextDocument::contentsChanged,
                this, &QWinUIPasswordBox::ensureSingleLine);
    }
    
    // 隐藏自定义滚动条（单行不需要）
    if (m_customScrollBar) {
        m_customScrollBar->hide();
        m_customScrollBar->setVisible(false);
    }
}

void QWinUIPasswordBox::setupPasswordToggle()
{
    m_passwordToggleButton = new QToolButton(this);
    m_passwordToggleButton->setText("Show");
    m_passwordToggleButton->setFixedSize(24, 24);
    m_passwordToggleButton->setStyleSheet(
        "QToolButton {"
        "    border: none;"
        "    background: transparent;"
        "    font-size: 14px;"
        "    border-radius: 4px;"
        "}"
        "QToolButton:hover {"
        "    background: rgba(0, 0, 0, 0.1);"
        "}"
        "QToolButton:pressed {"
        "    background: rgba(0, 0, 0, 0.2);"
        "}"
    );
    
    connect(m_passwordToggleButton, &QToolButton::clicked,
            this, &QWinUIPasswordBox::onPasswordToggleClicked);
    
    // 初始状态
    m_passwordToggleButton->setVisible(m_showPasswordToggle);
}

QString QWinUIPasswordBox::password() const
{
    return m_actualPassword;
}

void QWinUIPasswordBox::setPassword(const QString& password)
{
    if (m_actualPassword != password) {
        m_actualPassword = password;
        updateDisplayText();
        emit passwordChanged(password);
    }
}

QChar QWinUIPasswordBox::passwordCharacter() const
{
    return m_passwordCharacter;
}

void QWinUIPasswordBox::setPasswordCharacter(const QChar& character)
{
    if (m_passwordCharacter != character) {
        m_passwordCharacter = character;
        updateDisplayText();
        emit passwordCharacterChanged(character);
    }
}

bool QWinUIPasswordBox::isPasswordVisible() const
{
    return m_passwordVisible;
}

void QWinUIPasswordBox::setPasswordVisible(bool visible)
{
    if (m_passwordVisible != visible) {
        m_passwordVisible = visible;
        updateDisplayText();
        
        // 更新按钮图标
        if (m_passwordToggleButton) {
            m_passwordToggleButton->setText(visible ? "Hide" : "Show");
        }
        
        emit passwordVisibilityChanged(visible);
    }
}

bool QWinUIPasswordBox::showPasswordToggle() const
{
    return m_showPasswordToggle;
}

void QWinUIPasswordBox::setShowPasswordToggle(bool show)
{
    if (m_showPasswordToggle != show) {
        m_showPasswordToggle = show;
        if (m_passwordToggleButton) {
            m_passwordToggleButton->setVisible(show);
        }
        emit showPasswordToggleChanged(show);
    }
}

QString QWinUIPasswordBox::text() const
{
    return m_actualPassword;
}

void QWinUIPasswordBox::setText(const QString& text)
{
    setPassword(text);
}

QSize QWinUIPasswordBox::sizeHint() const
{
    return QSize(250, 32);
}

QSize QWinUIPasswordBox::minimumSizeHint() const
{
    return QSize(100, 32);
}

void QWinUIPasswordBox::copy()
{
    // 禁用复制操作以保护密码
}

void QWinUIPasswordBox::cut()
{
    // 禁用剪切操作以保护密码
}

void QWinUIPasswordBox::keyPressEvent(QKeyEvent* event)
{
    // 阻止换行键
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        event->accept();
        return;
    }

    // 传递给基类处理
    QWinUIRichEditBox::keyPressEvent(event);
}

void QWinUIPasswordBox::resizeEvent(QResizeEvent* event)
{
    QWinUIRichEditBox::resizeEvent(event);
    positionPasswordToggle();
}

bool QWinUIPasswordBox::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == m_textEdit && event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

        // 阻止换行键
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            return true;
        }
    }

    return QWinUIRichEditBox::eventFilter(obj, event);
}

void QWinUIPasswordBox::onTextChanged()
{
    if (!m_ignoreTextChange && m_textEdit) {
        // 获取当前显示的文本
        QString displayText = m_textEdit->toPlainText();

        // 如果是密码可见模式，直接更新实际密码
        if (m_passwordVisible) {
            m_actualPassword = displayText;
        } else {
            // 密码隐藏模式，需要处理掩码字符
            handleMaskedInput(displayText);
        }

        ensureSingleLine();
        emit passwordChanged(m_actualPassword);
    }

    // 不调用基类的onTextChanged，避免重复处理
}

void QWinUIPasswordBox::onPasswordToggleClicked()
{
    setPasswordVisible(!m_passwordVisible);
}

void QWinUIPasswordBox::updateDisplayText()
{
    if (!m_textEdit) return;

    m_ignoreTextChange = true;

    QString displayText;
    if (m_passwordVisible) {
        displayText = m_actualPassword;
    } else {
        displayText = maskPassword(m_actualPassword);
    }

    // 保存光标位置
    QTextCursor cursor = m_textEdit->textCursor();
    int position = cursor.position();

    m_textEdit->setPlainText(displayText);

    // 恢复光标位置
    cursor.setPosition(qMin(position, displayText.length()));
    m_textEdit->setTextCursor(cursor);

    m_ignoreTextChange = false;
}

void QWinUIPasswordBox::ensureSingleLine()
{
    if (!m_textEdit || m_ignoreTextChange) return;

    QString currentText = m_textEdit->toPlainText();

    if (currentText.contains('\n') || currentText.contains('\r')) {
        m_ignoreTextChange = true;

        QTextCursor cursor = m_textEdit->textCursor();
        int position = cursor.position();

        QString singleLineText = currentText;
        singleLineText.replace('\n', "").replace('\r', "");

        m_textEdit->setPlainText(singleLineText);

        cursor.setPosition(qMin(position, singleLineText.length()));
        m_textEdit->setTextCursor(cursor);

        m_ignoreTextChange = false;
    }
}

void QWinUIPasswordBox::positionPasswordToggle()
{
    if (m_passwordToggleButton) {
        int buttonSize = 24;
        int margin = 4;
        int x = width() - buttonSize - margin;
        int y = (height() - buttonSize) / 2;

        m_passwordToggleButton->setGeometry(x, y, buttonSize, buttonSize);
    }
}

QString QWinUIPasswordBox::maskPassword(const QString& password) const
{
    return QString(password.length(), m_passwordCharacter);
}

void QWinUIPasswordBox::handleMaskedInput(const QString& displayText)
{
    // 这是一个简化的实现，实际应用中可能需要更复杂的逻辑
    // 来处理插入、删除等操作

    int displayLength = displayText.length();
    int actualLength = m_actualPassword.length();

    if (displayLength > actualLength) {
        // 有新字符输入
        QString newChars = displayText.right(displayLength - actualLength);
        // 移除掩码字符，只保留新输入的字符
        newChars.remove(m_passwordCharacter);
        m_actualPassword += newChars;
    } else if (displayLength < actualLength) {
        // 有字符被删除
        m_actualPassword = m_actualPassword.left(displayLength);
    }

    // 更新显示
    updateDisplayText();
}
