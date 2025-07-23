#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QWinUI/QWinUI.h>

class SplitButtonExample : public QMainWindow
{
    Q_OBJECT

public:
    SplitButtonExample(QWidget* parent = nullptr)
        : QMainWindow(parent)
    {
        setupUI();
        connectSignals();
    }

private slots:
    void onSplitButtonClicked()
    {
        qDebug() << "SplitButton main action clicked!";
    }
    
    void onSplitButtonDropDownClicked()
    {
        qDebug() << "SplitButton dropdown clicked!";
    }
    
    void onMenuItemClicked(QWinUIMenuFlyoutItem* item)
    {
        qDebug() << "Menu item clicked:" << item->text();
    }

private:
    void setupUI()
    {
        setWindowTitle("QWinUI SplitButton Example");
        setMinimumSize(600, 400);
        
        // 应用QWinUI主题
        QWinUI::initialize();
        QWinUITheme::instance()->setThemeMode(QWinUIThemeMode::Auto);
        
        QWidget* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
        mainLayout->setSpacing(20);
        mainLayout->setContentsMargins(40, 40, 40, 40);
        
        // 标题
        QLabel* titleLabel = new QLabel("SplitButton Examples", this);
        titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #323130;");
        mainLayout->addWidget(titleLabel);
        
        // 基本SplitButton
        createBasicSplitButton(mainLayout);
        
        // 带图标的SplitButton
        createIconSplitButton(mainLayout);
        
        // 颜色选择SplitButton
        createColorSplitButton(mainLayout);
        
        mainLayout->addStretch();
    }
    
    void createBasicSplitButton(QVBoxLayout* parentLayout)
    {
        QLabel* label = new QLabel("Basic SplitButton:", this);
        label->setStyleSheet("font-size: 14px; font-weight: 600; color: #323130;");
        parentLayout->addWidget(label);
        
        QHBoxLayout* layout = new QHBoxLayout();
        
        // 创建基本的SplitButton
        QWinUISplitButton* splitButton = new QWinUISplitButton("Save", this);
        
        // 创建菜单
        QWinUIMenuFlyout* menu = new QWinUIMenuFlyout(this);
        menu->addItem("Save");
        menu->addItem("Save As...");
        menu->addSeparator();
        menu->addItem("Save All");
        
        splitButton->setFlyout(menu);
        
        layout->addWidget(splitButton);
        layout->addStretch();
        
        parentLayout->addLayout(layout);
        
        // 连接信号
        connect(splitButton, &QWinUISplitButton::clicked, this, &SplitButtonExample::onSplitButtonClicked);
        connect(splitButton, &QWinUISplitButton::dropDownClicked, this, &SplitButtonExample::onSplitButtonDropDownClicked);
        connect(menu, &QWinUIMenuFlyout::itemClicked, this, &SplitButtonExample::onMenuItemClicked);
    }
    
    void createIconSplitButton(QVBoxLayout* parentLayout)
    {
        QLabel* label = new QLabel("SplitButton with Icon:", this);
        label->setStyleSheet("font-size: 14px; font-weight: 600; color: #323130;");
        parentLayout->addWidget(label);
        
        QHBoxLayout* layout = new QHBoxLayout();
        
        // 创建带图标的SplitButton
        QIcon icon = style()->standardIcon(QStyle::SP_DialogSaveButton);
        QWinUISplitButton* splitButton = new QWinUISplitButton("New", icon, this);
        
        // 创建菜单
        QWinUIMenuFlyout* menu = new QWinUIMenuFlyout(this);
        menu->addItem("New File");
        menu->addItem("New Folder");
        menu->addSeparator();
        menu->addItem("New Project");
        
        splitButton->setFlyout(menu);
        
        layout->addWidget(splitButton);
        layout->addStretch();
        
        parentLayout->addLayout(layout);
        
        // 连接信号
        connect(menu, &QWinUIMenuFlyout::itemClicked, this, &SplitButtonExample::onMenuItemClicked);
    }
    
    void createColorSplitButton(QVBoxLayout* parentLayout)
    {
        QLabel* label = new QLabel("Color Picker SplitButton:", this);
        label->setStyleSheet("font-size: 14px; font-weight: 600; color: #323130;");
        parentLayout->addWidget(label);
        
        QHBoxLayout* layout = new QHBoxLayout();
        
        // 创建颜色选择SplitButton
        QWinUISplitButton* splitButton = new QWinUISplitButton("Choose color", this);
        
        // 创建颜色菜单
        QWinUIMenuFlyout* menu = new QWinUIMenuFlyout(this);
        menu->addItem("Red");
        menu->addItem("Green");
        menu->addItem("Blue");
        menu->addSeparator();
        menu->addItem("Yellow");
        menu->addItem("Orange");
        menu->addItem("Purple");
        menu->addSeparator();
        menu->addItem("More colors...");
        
        splitButton->setFlyout(menu);
        
        layout->addWidget(splitButton);
        layout->addStretch();
        
        parentLayout->addLayout(layout);
        
        // 连接信号
        connect(menu, &QWinUIMenuFlyout::itemClicked, this, &SplitButtonExample::onMenuItemClicked);
    }
    
    void connectSignals()
    {
        // 这里可以添加全局信号连接
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    SplitButtonExample window;
    window.show();
    
    return app.exec();
}

#include "QWinUISplitButton_Example.moc"
