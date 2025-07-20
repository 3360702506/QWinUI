#ifndef QWINUITEXTBOX_H
#define QWINUITEXTBOX_H

#include "QWinUIRichEditBox.h"

QT_BEGIN_NAMESPACE

class QWINUI_EXPORT QWinUITextBox : public QWinUIRichEditBox
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString placeholderText READ placeholderText WRITE setPlaceholderText)
    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly)
    Q_PROPERTY(int maxLength READ maxLength WRITE setMaxLength)
    Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor)
    Q_PROPERTY(QColor focusBorderColor READ focusBorderColor WRITE setFocusBorderColor)

public:
    explicit QWinUITextBox(QWidget* parent = nullptr);
    ~QWinUITextBox() override;

    // 重写基类方法以确保单行行为
    QString text() const;
    void setText(const QString& text);

    // 尺寸提示
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    // 事件处理
    void keyPressEvent(QKeyEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

protected:
    void onTextChanged() override;

private:
    void initializeTextBox();
    void setupSingleLineMode();
    void ensureSingleLine();

    bool m_ignoreTextChange;
};

QT_END_NAMESPACE

#endif // QWINUITEXTBOX_H
