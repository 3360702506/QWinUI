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
#include <QWinUI/Controls/QWinUIVariableSizedWrapGrid.h>
#include <QWinUI/Controls/QWinUIToggleButton.h>
#include <QWinUI/Controls/QWinUIMenuFlyout.h>
#include <QWinUI/Controls/QWinUISplitButton.h>
#include <QWinUI/Controls/QWinUIRadioButton.h>
#include <QWinUI/Controls/QWinUISlider.h>
#include <QWinUI/QWinUIIconManager.h>
#include <QMessageBox>
#include <QApplication>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QColorDialog>
#include <QPushButton>

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

    enableWindowsBlur(false);

    m_transitionType = QWinUIWidget::RippleTransition;

    // 配置主题切换动画
    QWinUITheme* theme = QWinUITheme::getInstance();
    theme->setThemeTransitionEnabled(true);  // 启用主题切换动画
    theme->setThemeTransitionMode(0);  // 设置为涟漪动画 (0 = RippleTransition)

    // 添加SplitButton示例
    setupSplitButtonExamples();
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

void Widget::setupSplitButtonExamples()
{
    // 添加分组标题
    QLabel* splitButtonLabel = new QLabel("SplitButton 示例", this);
    splitButtonLabel->setStyleSheet("font-size: 16px; font-weight: bold; margin-top: 20px;");
    m_layout->addWidget(splitButtonLabel);

    // 创建水平布局容器
    QHBoxLayout* splitButtonLayout = new QHBoxLayout();
    splitButtonLayout->setSpacing(15);

    // 1. 基本的保存SplitButton
    QWinUISplitButton* saveSplitButton = new QWinUISplitButton("保存", this);

    // 创建保存菜单
    QWinUIMenuFlyout* saveMenu = new QWinUIMenuFlyout(this);
    saveMenu->addItem("保存");
    saveMenu->addItem("另存为...");
    saveMenu->addSeparator();
    saveMenu->addItem("保存所有");

    saveSplitButton->setFlyout(saveMenu);

    // 连接信号
    connect(saveSplitButton, &QWinUISplitButton::clicked, [this]() {
        m_titleLabel->setText("保存按钮被点击！");
        qDebug() << "Save button clicked!";
    });

    connect(saveMenu, &QWinUIMenuFlyout::itemClicked, [this](QWinUIMenuFlyoutItem* item) {
        m_titleLabel->setText(QString("菜单项被点击：%1").arg(item->text()));
        qDebug() << "Save menu item clicked:" << item->text();
    });

    splitButtonLayout->addWidget(saveSplitButton);

    // 2. 带图标的新建SplitButton
    QIcon newIcon = style()->standardIcon(QStyle::SP_FileDialogNewFolder);
    QWinUISplitButton* newSplitButton = new QWinUISplitButton("新建", newIcon, this);

    // 创建新建菜单
    QWinUIMenuFlyout* newMenu = new QWinUIMenuFlyout(this);
    newMenu->addItem("新建文件");
    newMenu->addItem("新建文件夹");
    newMenu->addSeparator();
    newMenu->addItem("新建项目");

    newSplitButton->setFlyout(newMenu);

    // 连接信号
    connect(newSplitButton, &QWinUISplitButton::clicked, [this]() {
        m_titleLabel->setText("新建按钮被点击！");
        qDebug() << "New button clicked!";
    });

    connect(newMenu, &QWinUIMenuFlyout::itemClicked, [this](QWinUIMenuFlyoutItem* item) {
        m_titleLabel->setText(QString("新建菜单项：%1").arg(item->text()));
        qDebug() << "New menu item clicked:" << item->text();
    });

    splitButtonLayout->addWidget(newSplitButton);

    // 3. 颜色选择SplitButton
    QWinUISplitButton* colorSplitButton = new QWinUISplitButton("选择颜色", this);

    // 创建颜色菜单
    QWinUIMenuFlyout* colorMenu = new QWinUIMenuFlyout(this);
    colorMenu->addItem("红色");
    colorMenu->addItem("绿色");
    colorMenu->addItem("蓝色");
    colorMenu->addSeparator();
    colorMenu->addItem("黄色");
    colorMenu->addItem("橙色");
    colorMenu->addItem("紫色");
    colorMenu->addSeparator();
    colorMenu->addItem("更多颜色...");

    colorSplitButton->setFlyout(colorMenu);

    // 连接信号
    connect(colorSplitButton, &QWinUISplitButton::clicked, [this]() {
        QColor color = QColorDialog::getColor(Qt::white, this, "选择颜色");
        if (color.isValid()) {
            m_titleLabel->setText(QString("选择了颜色：%1").arg(color.name()));
        }
    });

    connect(colorMenu, &QWinUIMenuFlyout::itemClicked, [this](QWinUIMenuFlyoutItem* item) {
        if (item->text() == "更多颜色...") {
            QColor color = QColorDialog::getColor(Qt::white, this, "选择颜色");
            if (color.isValid()) {
                m_titleLabel->setText(QString("选择了颜色：%1").arg(color.name()));
            }
        } else {
            m_titleLabel->setText(QString("选择了颜色：%1").arg(item->text()));
        }
        qDebug() << "Color menu item clicked:" << item->text();
    });

    splitButtonLayout->addWidget(colorSplitButton);

    // 添加弹性空间
    splitButtonLayout->addStretch();

    // 将水平布局添加到主布局
    m_layout->addLayout(splitButtonLayout);

    // 添加说明文本
    QLabel* descLabel = new QLabel("SplitButton 包含主按钮和下拉菜单两部分，点击主按钮执行默认操作，点击下拉箭头显示更多选项。", this);
    descLabel->setWordWrap(true);
    descLabel->setStyleSheet("color: #666; font-size: 12px; margin-top: 10px;");
    m_layout->addWidget(descLabel);
}
