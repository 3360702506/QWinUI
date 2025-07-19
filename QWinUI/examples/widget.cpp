#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QWinUI/QWinUI.h>

class QWinUIDemo : public QWidget
{
    Q_OBJECT

public:
    QWinUIDemo(QWidget* parent = nullptr) : QWidget(parent)
    {
        setupUI();
        connectSignals();
    }

private slots:
    void onToggleSwitchChanged(bool checked)
    {
        QWinUIToggleSwitch* sender = qobject_cast<QWinUIToggleSwitch*>(this->sender());
        if (sender) {
            qDebug() << "ToggleSwitch changed:" << checked;
        }
    }

    void onButtonClicked()
    {
        QWinUIButton* sender = qobject_cast<QWinUIButton*>(this->sender());
        if (sender) {
            qDebug() << "Button clicked:" << sender->text();
        }
    }

private:
    void setupUI()
    {
        setWindowTitle("QWinUI Controls Demo");
        setMinimumSize(600, 400);
        
        // 主布局
        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        mainLayout->setSpacing(20);
        mainLayout->setContentsMargins(20, 20, 20, 20);
        
        // 标题
        QLabel* titleLabel = new QLabel("QWinUI Controls Demo", this);
        titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; margin-bottom: 10px;");
        mainLayout->addWidget(titleLabel);
        
        // 按钮演示
        setupButtonDemo(mainLayout);
        
        // ToggleSwitch 演示
        setupToggleSwitchDemo(mainLayout);
        
        mainLayout->addStretch();
    }
    
    void setupButtonDemo(QVBoxLayout* parentLayout)
    {
        QLabel* buttonLabel = new QLabel("Button Styles:", this);
        buttonLabel->setStyleSheet("font-weight: bold; margin-top: 10px;");
        parentLayout->addWidget(buttonLabel);
        
        QHBoxLayout* buttonLayout = new QHBoxLayout();
        
        // Accent 按钮
        m_accentButton = new QWinUIButton("Accent style button", this);
        m_accentButton->setButtonStyle(QWinUIButton::Accent);
        buttonLayout->addWidget(m_accentButton);
        
        // Standard 按钮
        m_standardButton = new QWinUIButton("Standard XAML button", this);
        m_standardButton->setButtonStyle(QWinUIButton::Standard);
        buttonLayout->addWidget(m_standardButton);
        
        // Subtle 按钮
        m_subtleButton = new QWinUIButton("Subtle style button", this);
        m_subtleButton->setButtonStyle(QWinUIButton::Subtle);
        buttonLayout->addWidget(m_subtleButton);
        
        buttonLayout->addStretch();
        parentLayout->addLayout(buttonLayout);
        
        // 第二行按钮
        QHBoxLayout* buttonLayout2 = new QHBoxLayout();
        
        // Hyperlink 按钮
        m_hyperlinkButton = new QWinUIButton("Hyperlink style button", this);
        m_hyperlinkButton->setButtonStyle(QWinUIButton::Hyperlink);
        buttonLayout2->addWidget(m_hyperlinkButton);
        
        buttonLayout2->addStretch();
        parentLayout->addLayout(buttonLayout2);
    }
    
    void setupToggleSwitchDemo(QVBoxLayout* parentLayout)
    {
        QLabel* toggleLabel = new QLabel("ToggleSwitch Examples:", this);
        toggleLabel->setStyleSheet("font-weight: bold; margin-top: 20px;");
        parentLayout->addWidget(toggleLabel);
        
        // 基本 ToggleSwitch
        m_toggleSwitch1 = new QWinUIToggleSwitch(this);
        m_toggleSwitch1->setOffContent("Off");
        m_toggleSwitch1->setOnContent("On");
        parentLayout->addWidget(m_toggleSwitch1);
        
        // 开启状态的 ToggleSwitch
        m_toggleSwitch2 = new QWinUIToggleSwitch(this);
        m_toggleSwitch2->setOffContent("Source code");
        m_toggleSwitch2->setOnContent("Working");
        m_toggleSwitch2->setIsOn(true);
        parentLayout->addWidget(m_toggleSwitch2);
        
        // 自定义文本的 ToggleSwitch
        m_toggleSwitch3 = new QWinUIToggleSwitch(this);
        m_toggleSwitch3->setOffContent("Toggle work");
        m_toggleSwitch3->setOnContent("Toggle work");
        parentLayout->addWidget(m_toggleSwitch3);
        
        // 说明文本
        QLabel* descLabel = new QLabel(
            "• 第一个开关演示基本的开/关状态\n"
            "• 第二个开关默认为开启状态\n"
            "• 第三个开关在两种状态下显示相同文本\n"
            "• 点击开关或按空格键可以切换状态", this);
        descLabel->setStyleSheet("color: gray; font-size: 12px; margin-top: 10px;");
        parentLayout->addWidget(descLabel);
    }
    
    void connectSignals()
    {
        // 连接按钮信号
        connect(m_accentButton, &QWinUIButton::clicked, this, &QWinUIDemo::onButtonClicked);
        connect(m_standardButton, &QWinUIButton::clicked, this, &QWinUIDemo::onButtonClicked);
        connect(m_subtleButton, &QWinUIButton::clicked, this, &QWinUIDemo::onButtonClicked);
        connect(m_hyperlinkButton, &QWinUIButton::clicked, this, &QWinUIDemo::onButtonClicked);
        
        // 连接 ToggleSwitch 信号
        connect(m_toggleSwitch1, &QWinUIToggleSwitch::toggled, this, &QWinUIDemo::onToggleSwitchChanged);
        connect(m_toggleSwitch2, &QWinUIToggleSwitch::toggled, this, &QWinUIDemo::onToggleSwitchChanged);
        connect(m_toggleSwitch3, &QWinUIToggleSwitch::toggled, this, &QWinUIDemo::onToggleSwitchChanged);
    }

private:
    // 按钮
    QWinUIButton* m_accentButton;
    QWinUIButton* m_standardButton;
    QWinUIButton* m_subtleButton;
    QWinUIButton* m_hyperlinkButton;
    
    // ToggleSwitch
    QWinUIToggleSwitch* m_toggleSwitch1;
    QWinUIToggleSwitch* m_toggleSwitch2;
    QWinUIToggleSwitch* m_toggleSwitch3;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 初始化 QWinUI 主题
    QWinUITheme* theme = QWinUITheme::getInstance();
    theme->setThemeMode(QWinUIThemeMode::Auto);
    
    // 创建并显示演示窗口
    QWinUIDemo demo;
    demo.show();
    
    return app.exec();
}

#include "widget.moc"
