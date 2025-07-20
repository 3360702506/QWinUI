#include "widget.h"
#include <QWinUI/QWinUIBlurEffect.h>
#include <QPainter>
#include <QDebug>
#include <QWinUI/Controls/QWinUIToggleSwitch.h>
#include <QWinUI/Controls/QWinUIScrollView.h>
#include <QWinUI/Controls/QWinUINotification.h>
#include <QWinUI/Controls/QWinUIProgressRing.h>
#include <QWinUI/Controls/QWinUIIcon.h>
#include <QWinUI/QWinUIIconConstants.h>
#include <QWinUI/QWinUIFluentIcons.h>
#include <QWinUI/Controls/QWinUIAcrylicBrush.h>
#include <QWinUI/Controls/QWinUIRichEditBox.h>
#include <QWinUI/Controls/QWinUITextBox.h>
#include <QWinUI/Controls/QWinUITextInput.h>
#include <QWinUI/Controls/QWinUIPasswordBox.h>
#include <QWinUI/Controls/QWinUIIconButton.h>
#include <QWinUI/Controls/QWinUIAppBarSeparator.h>
#include <QWinUI/QWinUIIconManager.h>
#include <QMessageBox>
#include <QApplication>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QColorDialog>

Widget::Widget(QWidget *parent)
    : QWinUIWidget(parent)
{
    setWindowTitle("QWinUI 测试");

    // 创建布局
    m_layout = new QVBoxLayout(this);
    m_layout->setSpacing(10);
    m_layout->setContentsMargins(20, 20, 20, 20);

    // 标题
    m_titleLabel = new QLabel("QWinUI 测试", this);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_layout->addWidget(m_titleLabel);

    // 为标题添加 ToolTip
    QWinUIToolTip* titleTooltip = new QWinUIToolTip("QWinUI 演示程序\n基于 Qt 的 WinUI3 风格控件库\n支持现代化的 Fluent Design 效果", m_titleLabel, this);
    titleTooltip->setPlacement(QWinUIToolTip::Bottom);

    // 主题切换
    QWinUIToggleSwitch* themeToggle = new QWinUIToggleSwitch(this);
    themeToggle->setOffContent("浅色主题");
    themeToggle->setOnContent("深色主题");
    connect(themeToggle, &QWinUIToggleSwitch::toggled, [this, themeToggle](bool checked) {
        QWinUITheme* theme = QWinUITheme::getInstance();
        // 简化：直接切换主题，系统会自动处理涟漪动画
        theme->setThemeMode(checked ? QWinUIThemeMode::Dark : QWinUIThemeMode::Light);
    });
    m_layout->addWidget(themeToggle);
    themeToggle->setToolTipText("切换应用主题\n支持浅色和深色两种主题模式\n带有平滑的过渡动画");

    // TextBox 演示 - 使用新的自定义实现
    QLabel* textBoxLabel = new QLabel("TextBox 演示:", this);
    textBoxLabel->setStyleSheet("font-weight: bold; margin-top: 20px;");
    m_layout->addWidget(textBoxLabel);

    // 基本文本输入框
    QWinUITextInput* basicTextInput = new QWinUITextInput(this);
    basicTextInput->setPlaceholderText("这是一个只读的文本输入框");
    m_layout->addWidget(basicTextInput);

    // 只读文本输入框
    QWinUITextInput* readOnlyTextInput = new QWinUITextInput(this);
    readOnlyTextInput->setText("这是一个只读的文本输入框");
    readOnlyTextInput->setReadOnly(true);
    m_layout->addWidget(readOnlyTextInput);

    // 限制长度的文本输入框
    QWinUITextInput* limitedTextInput = new QWinUITextInput(this);
    limitedTextInput->setPlaceholderText("最多输入20个字符");
    limitedTextInput->setMaxLength(20);
    m_layout->addWidget(limitedTextInput);

    // 多行文本输入框
    QWinUITextInput* multiLineTextInput = new QWinUITextInput(this);
    multiLineTextInput->setPlaceholderText("多行文本输入...");
    multiLineTextInput->setMultiLine(true);
    multiLineTextInput->setMinimumHeight(80);
    m_layout->addWidget(multiLineTextInput);

    // 基本密码输入框
    QWinUIPasswordBox* basicPasswordBox = new QWinUIPasswordBox(this);
    basicPasswordBox->setPlaceholderText("请输入密码...");
    m_layout->addWidget(basicPasswordBox);

    // 不显示切换按钮的密码框
    QWinUIPasswordBox* simplePasswordBox = new QWinUIPasswordBox(this);
    simplePasswordBox->setPlaceholderText("简单密码框（无切换按钮）");
    simplePasswordBox->setShowPasswordToggle(false);
    m_layout->addWidget(simplePasswordBox);

    // IconButton 演示
    QLabel* iconButtonLabel = new QLabel("IconButton 演示:", this);
    iconButtonLabel->setStyleSheet("font-weight: bold; margin-top: 20px;");
    m_layout->addWidget(iconButtonLabel);

    // 创建水平布局来放置图标按钮
    QHBoxLayout* iconButtonLayout = new QHBoxLayout();

    // Fluent Icons常量
    QChar homeChar = QWinUIFluentIcons::Navigation::HOME;
    QChar searchChar = QWinUIFluentIcons::Navigation::SEARCH;

    // Fluent Icons 按钮 - 使用setFluentIcon方法
    QWinUIIconButton* homeButton = new QWinUIIconButton(this);
    homeButton->setFluentIcon(homeChar);
    homeButton->setToolTipText("主页");  // 使用QWinUIWidget的tooltip
    iconButtonLayout->addWidget(homeButton);

    // 搜索按钮
    QWinUIIconButton* searchButton = new QWinUIIconButton(this);
    searchButton->setFluentIcon(searchChar);
    searchButton->setToolTipText("搜索");  // 使用QWinUIWidget的tooltip
    iconButtonLayout->addWidget(searchButton);

    // 也尝试用setFluentIcon方法
    QWinUIIconButton* settingsButton = new QWinUIIconButton(this);
    settingsButton->setFluentIcon(QWinUIFluentIcons::System::SETTINGS);
    settingsButton->setToolTipText("设置");  // 使用QWinUIWidget的tooltip
    iconButtonLayout->addWidget(settingsButton);

    // 使用IconManager的按钮
    QWinUIIconButton* iconManagerButton = new QWinUIIconButton(this);
    iconManagerButton->setIconName(QWinUIIcons::Solid::HEART);
    iconManagerButton->setToolTipText("收藏");  // 使用QWinUIWidget的tooltip
    iconButtonLayout->addWidget(iconManagerButton);

    // 添加更多图标按钮来测试
    QWinUIIconButton* addButton = new QWinUIIconButton(this);
    addButton->setFluentIcon(QWinUIFluentIcons::Actions::ADD);
    addButton->setToolTipText("添加");
    iconButtonLayout->addWidget(addButton);

    QWinUIIconButton* deleteButton = new QWinUIIconButton(this);
    deleteButton->setFluentIcon(QWinUIFluentIcons::Actions::DELETE_ICON);
    deleteButton->setToolTipText("删除");
    iconButtonLayout->addWidget(deleteButton);

    // 添加垂直分隔线
    QWinUIAppBarSeparator* verticalSeparator = new QWinUIAppBarSeparator(Qt::Vertical, this);
    iconButtonLayout->addWidget(verticalSeparator);

    // 添加更多按钮来演示分隔线效果
    QWinUIIconButton* copyButton = new QWinUIIconButton(this);
    copyButton->setFluentIcon(QWinUIFluentIcons::Actions::COPY);
    copyButton->setToolTipText("复制");
    iconButtonLayout->addWidget(copyButton);

    QWinUIIconButton* pasteButton = new QWinUIIconButton(this);
    pasteButton->setFluentIcon(QWinUIFluentIcons::Actions::PASTE);
    pasteButton->setToolTipText("粘贴");
    iconButtonLayout->addWidget(pasteButton);

    // 调试：检查IconManager状态
    QWinUIIconManager* iconManager = QWinUIIconManager::getInstance();
    if (iconManager) {
        qDebug() << "IconManager available icons:" << iconManager->getIconNames().size();
        qDebug() << "Available categories:" << iconManager->getCategories();
        qDebug() << "Icons in 'solid' category:" << iconManager->getIconsByCategory("solid");
    } else {
        qDebug() << "IconManager is null!";
    }

    iconButtonLayout->addStretch(); // 添加弹性空间
    m_layout->addLayout(iconButtonLayout);

    // 添加水平分隔线演示
    QLabel* separatorLabel = new QLabel("AppBarSeparator 演示:", this);
    separatorLabel->setStyleSheet("font-weight: bold; margin-top: 20px;");
    m_layout->addWidget(separatorLabel);

    // 水平分隔线
    QWinUIAppBarSeparator* horizontalSeparator = new QWinUIAppBarSeparator(Qt::Horizontal, this);
    m_layout->addWidget(horizontalSeparator);

    // 添加一些说明文字
    QLabel* descLabel = new QLabel("分隔线会自动适配当前主题颜色\n切换主题可以看到分隔线颜色的变化", this);
    descLabel->setAlignment(Qt::AlignCenter);
    m_layout->addWidget(descLabel);

    // 添加另一个水平分隔线，测试多个分隔线的情况
    QWinUIAppBarSeparator* horizontalSeparator2 = new QWinUIAppBarSeparator(Qt::Horizontal, this);
    horizontalSeparator2->setSeparatorWidth(2); // 设置更粗的分隔线
    m_layout->addWidget(horizontalSeparator2);

    QLabel* thickLabel = new QLabel("上面是2像素粗的分隔线", this);
    thickLabel->setAlignment(Qt::AlignCenter);
    thickLabel->setStyleSheet("font-size: 12px; color: gray;");
    m_layout->addWidget(thickLabel);

    // 初始化设置
    // setAttribute(Qt::WA_TranslucentBackground, true);
    // enableWindowsBlur(true);
    // setWindowsBlurType(2);
    enableWindowsBlur(false);
    m_titleLabel->setText("当前效果: Acrylic");
    m_transitionType = QWinUIWidget::RippleTransition;

    // 配置主题切换动画
    QWinUITheme* theme = QWinUITheme::getInstance();
    theme->setThemeTransitionEnabled(true);  // 启用主题切换动画
    theme->setThemeTransitionMode(0);  // 设置为涟漪动画 (0 = RippleTransition)
}

Widget::~Widget()
{
}

void Widget::onButtonClicked()
{
    m_titleLabel->setText("按钮被点击了！");
}

void Widget::onThemeToggled()
{
    QWinUITheme* theme = QWinUITheme::getInstance();
    // 简化：直接切换主题，系统会自动处理涟漪动画
    if (theme->themeMode() == QWinUIThemeMode::Light) {
        theme->setThemeMode(QWinUIThemeMode::Dark);
    } else {
        theme->setThemeMode(QWinUIThemeMode::Light);
    }
}

void Widget::onFadeThemeToggled()
{
    QWinUITheme* theme = QWinUITheme::getInstance();
    // 简化：直接切换主题，系统会自动处理涟漪动画
    if (theme->themeMode() == QWinUIThemeMode::Light) {
        theme->setThemeMode(QWinUIThemeMode::Dark);
    } else {
        theme->setThemeMode(QWinUIThemeMode::Light);
    }
}

void Widget::setupRichTextContent()
{
    // 清空现有内容
    m_richTextBlock->clearInlines();

    // 重新添加内容，确保使用当前主题的格式
    m_richTextBlock->addRun("这是一个 ");
    m_richTextBlock->addBold("粗体文本");
    m_richTextBlock->addRun(" 和 ");
    m_richTextBlock->addItalic("斜体文本");
    m_richTextBlock->addRun(" 以及 ");
    m_richTextBlock->addHyperlink("超链接", QUrl("https://github.com/microsoft/WinUI"));
    m_richTextBlock->addLineBreak();
    m_richTextBlock->addRun("支持多行显示和丰富的文本格式。");
}
