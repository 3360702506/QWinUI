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
#include <QWinUI/Controls/QWinUIDropDownButton.h>
#include <QWinUI/Controls/QWinUISimpleCard.h>
#include <QWinUI/Controls/QWinUIContentDialog.h>
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
    // setupSplitButtonExamples();

    // 添加DropDownButton示例
    setupDropDownButtonExamples();

    // // 添加SimpleCard示例
    // setupSimpleCardExamples();

    // 添加ContentDialog示例
    setupContentDialogExamples();
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

void Widget::setupSimpleCardExamples()
{
    // 添加分组标题
    QLabel* cardLabel = new QLabel("SimpleCard 示例", this);
    cardLabel->setStyleSheet("font-size: 16px; font-weight: bold; margin-top: 20px;");
    m_layout->addWidget(cardLabel);

    // 创建水平布局容器
    QHBoxLayout* cardLayout = new QHBoxLayout();
    cardLayout->setSpacing(15);

    // 1. 基本信息卡片
    QWinUISimpleCard* infoCard = new QWinUISimpleCard(this);
    infoCard->setFixedSize(200, 150);

    QLabel* cardTitle = new QLabel("用户信息", infoCard);
    cardTitle->setStyleSheet("font-size: 14px; font-weight: bold; margin-bottom: 10px;");

    QLabel* cardContent = new QLabel("姓名：张三\n邮箱：zhangsan@example.com\n部门：技术部", infoCard);
    cardContent->setStyleSheet("font-size: 12px; line-height: 1.5;");

    infoCard->addWidget(cardTitle);
    infoCard->addWidget(cardContent);

    cardLayout->addWidget(infoCard);

    // 2. 统计卡片
    QWinUISimpleCard* statsCard = new QWinUISimpleCard(this);
    statsCard->setFixedSize(200, 150);
    statsCard->setCornerRadius(12); // 自定义圆角

    QLabel* statsTitle = new QLabel("今日统计", statsCard);
    statsTitle->setStyleSheet("font-size: 14px; font-weight: bold; margin-bottom: 10px;");

    QLabel* statsNumber = new QLabel("1,234", statsCard);
    statsNumber->setStyleSheet("font-size: 24px; font-weight: bold; color: #0078d4;");

    QLabel* statsDesc = new QLabel("访问量", statsCard);
    statsDesc->setStyleSheet("font-size: 12px; color: #666;");

    statsCard->addWidget(statsTitle);
    statsCard->addWidget(statsNumber);
    statsCard->addWidget(statsDesc);

    cardLayout->addWidget(statsCard);

    // 3. 操作卡片
    QWinUISimpleCard* actionCard = new QWinUISimpleCard(this);
    actionCard->setFixedSize(200, 150);
    actionCard->setContentMargins(20, 20, 20, 20); // 自定义内边距

    QLabel* actionTitle = new QLabel("快速操作", actionCard);
    actionTitle->setStyleSheet("font-size: 14px; font-weight: bold; margin-bottom: 15px;");

    QWinUIButton* actionBtn1 = new QWinUIButton("新建文档", actionCard);
    QWinUIButton* actionBtn2 = new QWinUIButton("导入数据", actionCard);

    actionCard->addWidget(actionTitle);
    actionCard->addWidget(actionBtn1);
    actionCard->addWidget(actionBtn2);

    connect(actionBtn1, &QWinUIButton::clicked, [this]() {
        m_titleLabel->setText("新建文档按钮被点击！");
    });

    connect(actionBtn2, &QWinUIButton::clicked, [this]() {
        m_titleLabel->setText("导入数据按钮被点击！");
    });

    cardLayout->addWidget(actionCard);

    // 添加弹性空间
    cardLayout->addStretch();

    // 将水平布局添加到主布局
    m_layout->addLayout(cardLayout);

    // 添加说明文本
    QLabel* cardDescLabel = new QLabel("SimpleCard 是一个容器组件，提供圆角背景、阴影效果和悬浮反馈，可以包含任意子控件。", this);
    cardDescLabel->setWordWrap(true);
    cardDescLabel->setStyleSheet("color: #666; font-size: 12px; margin-top: 10px;");
    m_layout->addWidget(cardDescLabel);
}

void Widget::setupContentDialogExamples()
{
    // 添加分组标题
    QLabel* dialogLabel = new QLabel("ContentDialog 示例", this);
    dialogLabel->setStyleSheet("font-size: 16px; font-weight: bold; margin-top: 20px;");
    m_layout->addWidget(dialogLabel);

    // 创建水平布局容器
    QHBoxLayout* dialogLayout = new QHBoxLayout();
    dialogLayout->setSpacing(15);

    // 1. 基本对话框按钮
    QWinUIButton* basicDialogBtn = new QWinUIButton("显示基本对话框", this);
    connect(basicDialogBtn, &QWinUIButton::clicked, [this]() {
        QWinUIContentDialog* dialog = new QWinUIContentDialog(this);
        dialog->setTitle("基本对话框");
        dialog->setCloseButtonText("确定");

        // 暂时不在内容区域添加任何控件，先测试基本功能
        QWidget* contentWidget = dialog->contentWidget();
        QVBoxLayout* contentLayout = new QVBoxLayout(contentWidget);
        contentLayout->setContentsMargins(24, 24, 24, 24);

        QLabel* content = new QLabel("这是一个基本的内容对话框示例。", contentWidget);
        content->setAlignment(Qt::AlignCenter);
        content->setStyleSheet("font-size: 14px;");

        contentLayout->addWidget(content);

        connect(dialog, &QWinUIContentDialog::dialogClosed, [this, dialog](QWinUIContentDialog::DialogResult result) {
            m_titleLabel->setText(QString("基本对话框关闭，结果: %1").arg(result));
            dialog->deleteLater();
        });

        // 现在尝试显示对话框
        qDebug() << "对话框创建成功，开始显示";
        dialog->showDialog();
    });

    // 2. 确认对话框按钮
    QWinUIButton* confirmDialogBtn = new QWinUIButton("显示确认对话框", this);
    connect(confirmDialogBtn, &QWinUIButton::clicked, [this]() {
        QWinUIContentDialog* dialog = new QWinUIContentDialog(this);
        dialog->setTitle("确认操作");
        dialog->setPrimaryButtonText("确定");
        dialog->setSecondaryButtonText("取消");
        dialog->setCloseButtonText("");

        // 暂时注释掉内容设置
        // QWidget* contentWidget = dialog->contentWidget();
        // QVBoxLayout* contentLayout = new QVBoxLayout(contentWidget);
        // contentLayout->setContentsMargins(24, 24, 24, 24);

        // QLabel* content = new QLabel("您确定要执行此操作吗？\n此操作无法撤销。", contentWidget);
        // content->setAlignment(Qt::AlignCenter);
        // content->setStyleSheet("font-size: 14px;");
        // content->setWordWrap(true);

        // contentLayout->addWidget(content);

        connect(dialog, &QWinUIContentDialog::primaryButtonClicked, [this]() {
            m_titleLabel->setText("用户点击了确定按钮");
        });

        connect(dialog, &QWinUIContentDialog::secondaryButtonClicked, [this]() {
            m_titleLabel->setText("用户点击了取消按钮");
        });

        connect(dialog, &QWinUIContentDialog::dialogClosed, [dialog](QWinUIContentDialog::DialogResult result) {
            Q_UNUSED(result)
            dialog->deleteLater();
        });

        // 暂时不显示对话框，只测试创建
        qDebug() << "确认对话框创建成功，暂不显示";
        // dialog->showDialog();
    });

    // 3. 模态对话框按钮
    QWinUIButton* modalDialogBtn = new QWinUIButton("显示模态对话框", this);
    connect(modalDialogBtn, &QWinUIButton::clicked, [this]() {
        QWinUIContentDialog* dialog = new QWinUIContentDialog(this);
        dialog->setTitle("模态对话框");
        dialog->setPrimaryButtonText("保存");
        dialog->setSecondaryButtonText("不保存");
        dialog->setCloseButtonText("取消");
        dialog->setModal(true);

        // 暂时注释掉表单内容
        // QWidget* contentWidget = dialog->contentWidget();
        // QVBoxLayout* contentLayout = new QVBoxLayout(contentWidget);
        // contentLayout->setContentsMargins(24, 24, 24, 24);

        // QLabel* nameLabel = new QLabel("请输入您的姓名:", contentWidget);
        // QWinUITextInput* nameInput = new QWinUITextInput(contentWidget);
        // nameInput->setPlaceholderText("姓名");

        // QLabel* emailLabel = new QLabel("请输入您的邮箱:", contentWidget);
        // QWinUITextInput* emailInput = new QWinUITextInput(contentWidget);
        // emailInput->setPlaceholderText("邮箱地址");

        // contentLayout->addWidget(nameLabel);
        // contentLayout->addWidget(nameInput);
        // contentLayout->addWidget(emailLabel);
        // contentLayout->addWidget(emailInput);
        // contentLayout->addStretch();

        connect(dialog, &QWinUIContentDialog::primaryButtonClicked, [this]() {
            // QString name = nameInput->text();
            // QString email = emailInput->text();
            m_titleLabel->setText("保存信息");
        });

        connect(dialog, &QWinUIContentDialog::secondaryButtonClicked, [this]() {
            m_titleLabel->setText("用户选择不保存");
        });

        connect(dialog, &QWinUIContentDialog::closeButtonClicked, [this]() {
            m_titleLabel->setText("用户取消了操作");
        });

        connect(dialog, &QWinUIContentDialog::dialogClosed, [dialog](QWinUIContentDialog::DialogResult result) {
            Q_UNUSED(result)
            dialog->deleteLater();
        });

        // 暂时不显示对话框，只测试创建
        qDebug() << "模态对话框创建成功，暂不显示";
        // dialog->showDialog();
    });

    dialogLayout->addWidget(basicDialogBtn);
    dialogLayout->addWidget(confirmDialogBtn);
    dialogLayout->addWidget(modalDialogBtn);
    dialogLayout->addStretch();

    // 将水平布局添加到主布局
    m_layout->addLayout(dialogLayout);

    // 添加说明文本
    QLabel* dialogDescLabel = new QLabel("ContentDialog 提供模态和非模态对话框功能，支持自定义内容区域和按钮配置。", this);
    dialogDescLabel->setWordWrap(true);
    dialogDescLabel->setStyleSheet("color: #666; font-size: 12px; margin-top: 10px;");
    m_layout->addWidget(dialogDescLabel);
}

void Widget::setupDropDownButtonExamples()
{
    // 添加标题
    QLabel* dropDownLabel = new QLabel("QWinUIDropDownButton 示例", this);
    dropDownLabel->setStyleSheet("font-size: 16px; font-weight: bold; margin-top: 20px;");
    m_layout->addWidget(dropDownLabel);

    // 创建水平布局来放置按钮
    QHBoxLayout* dropDownLayout = new QHBoxLayout();
    dropDownLayout->setSpacing(15);

    // 基本下拉按钮
    QWinUIDropDownButton* basicDropDown = new QWinUIDropDownButton(this);
    basicDropDown->setText("基本下拉按钮");
    basicDropDown->setMinimumWidth(120);

    // 添加菜单项
    QWinUIMenuFlyoutItem* item1 = basicDropDown->addItem("选项 1");
    QWinUIMenuFlyoutItem* item2 = basicDropDown->addItem("选项 2");
    basicDropDown->addSeparator();
    QWinUIMenuFlyoutItem* item3 = basicDropDown->addItem("选项 3");

    connect(item1, &QWinUIMenuFlyoutItem::clicked, [this]() {
        m_titleLabel->setText("选择了选项 1");
    });
    connect(item2, &QWinUIMenuFlyoutItem::clicked, [this]() {
        m_titleLabel->setText("选择了选项 2");
    });
    connect(item3, &QWinUIMenuFlyoutItem::clicked, [this]() {
        m_titleLabel->setText("选择了选项 3");
    });

    // 带图标的下拉按钮
    QWinUIDropDownButton* iconDropDown = new QWinUIDropDownButton(this);
    iconDropDown->setText("带图标");
    iconDropDown->setMinimumWidth(120);

    // 添加菜单项
    QWinUIMenuFlyoutItem* iconItem1 = iconDropDown->addItem("操作 A");
    QWinUIMenuFlyoutItem* iconItem2 = iconDropDown->addItem("操作 B");
    QWinUIMenuFlyoutItem* iconItem3 = iconDropDown->addItem("操作 C");

    connect(iconItem1, &QWinUIMenuFlyoutItem::clicked, [this]() {
        m_titleLabel->setText("执行了操作 A");
    });
    connect(iconItem2, &QWinUIMenuFlyoutItem::clicked, [this]() {
        m_titleLabel->setText("执行了操作 B");
    });
    connect(iconItem3, &QWinUIMenuFlyoutItem::clicked, [this]() {
        m_titleLabel->setText("执行了操作 C");
    });

    // 禁用状态的下拉按钮
    QWinUIDropDownButton* disabledDropDown = new QWinUIDropDownButton(this);
    disabledDropDown->setText("禁用状态");
    disabledDropDown->setEnabled(false);
    disabledDropDown->setMinimumWidth(120);

    // 自定义样式的下拉按钮
    QWinUIDropDownButton* customDropDown = new QWinUIDropDownButton(this);
    customDropDown->setText("自定义样式");
    customDropDown->setButtonStyle(QWinUIButton::Accent);
    customDropDown->setMinimumWidth(120);

    // 添加菜单项
    QWinUIMenuFlyoutItem* customItem1 = customDropDown->addItem("功能 1");
    QWinUIMenuFlyoutItem* customItem2 = customDropDown->addItem("功能 2");
    customDropDown->addSeparator();
    QWinUIMenuFlyoutItem* customItem3 = customDropDown->addItem("设置");

    connect(customItem1, &QWinUIMenuFlyoutItem::clicked, [this]() {
        m_titleLabel->setText("启用了功能 1");
    });
    connect(customItem2, &QWinUIMenuFlyoutItem::clicked, [this]() {
        m_titleLabel->setText("启用了功能 2");
    });
    connect(customItem3, &QWinUIMenuFlyoutItem::clicked, [this]() {
        m_titleLabel->setText("打开了设置");
    });

    dropDownLayout->addWidget(basicDropDown);
    dropDownLayout->addWidget(iconDropDown);
    dropDownLayout->addWidget(disabledDropDown);
    dropDownLayout->addWidget(customDropDown);
    dropDownLayout->addStretch();

    // 将水平布局添加到主布局
    m_layout->addLayout(dropDownLayout);

    // 添加说明文本
    QLabel* dropDownDescLabel = new QLabel("QWinUIDropDownButton 是独立的下拉按钮组件，支持自定义内容和下拉箭头动画效果。", this);
    dropDownDescLabel->setWordWrap(true);
    dropDownDescLabel->setStyleSheet("color: #666; font-size: 12px; margin-top: 10px;");
    m_layout->addWidget(dropDownDescLabel);
}
