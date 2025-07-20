#ifndef QWINUIPASSWORDBOX_H
#define QWINUIPASSWORDBOX_H

#include "QWinUIRichEditBox.h"
#include <QToolButton>

QT_BEGIN_NAMESPACE

class QWINUI_EXPORT QWinUIPasswordBox : public QWinUIRichEditBox
{
    Q_OBJECT
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QString placeholderText READ placeholderText WRITE setPlaceholderText)
    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly)
    Q_PROPERTY(int maxLength READ maxLength WRITE setMaxLength)
    Q_PROPERTY(QChar passwordCharacter READ passwordCharacter WRITE setPasswordCharacter NOTIFY passwordCharacterChanged)
    Q_PROPERTY(bool passwordVisible READ isPasswordVisible WRITE setPasswordVisible NOTIFY passwordVisibilityChanged)
    Q_PROPERTY(bool showPasswordToggle READ showPasswordToggle WRITE setShowPasswordToggle NOTIFY showPasswordToggleChanged)

public:
    explicit QWinUIPasswordBox(QWidget* parent = nullptr);
    ~QWinUIPasswordBox() override;

    // 密码相关属性
    QString password() const;
    void setPassword(const QString& password);
    
    QChar passwordCharacter() const;
    void setPasswordCharacter(const QChar& character);
    
    bool isPasswordVisible() const;
    void setPasswordVisible(bool visible);
    
    bool showPasswordToggle() const;
    void setShowPasswordToggle(bool show);

    // 重写基类方法
    QString text() const;
    void setText(const QString& text);

    // 尺寸提示
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    // 禁用的操作
    void copy();
    void cut();

signals:
    void passwordChanged(const QString& password);
    void passwordCharacterChanged(const QChar& character);
    void passwordVisibilityChanged(bool visible);
    void showPasswordToggleChanged(bool show);

protected:
    // 事件处理
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;
    void onTextChanged() override;

private slots:
    void onPasswordToggleClicked();

private:
    void initializePasswordBox();
    void setupSingleLineMode();
    void setupPasswordToggle();
    void updateDisplayText();
    void ensureSingleLine();
    void positionPasswordToggle();
    QString maskPassword(const QString& password) const;
    void handleMaskedInput(const QString& displayText);

    QString m_actualPassword;
    QChar m_passwordCharacter;
    bool m_passwordVisible;
    bool m_showPasswordToggle;
    bool m_ignoreTextChange;
    
    QToolButton* m_passwordToggleButton;
};

QT_END_NAMESPACE

#endif // QWINUIPASSWORDBOX_H
