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

    // 添加 QWinUIMenuFlyout 演示
    QLabel* menuLabel = new QLabel("菜单飞出控件演示:", this);
    m_layout->addWidget(menuLabel);

    // 创建触发菜单的按钮
    QWinUIButton* menuButton = new QWinUIButton("显示上下文菜单", this);
    menuButton->setButtonStyle(QWinUIButton::Standard);
    menuButton->setToolTipText("点击显示WinUI3风格的上下文菜单");
    m_layout->addWidget(menuButton);

    // 创建菜单飞出控件
    QWinUIMenuFlyout* contextMenu = new QWinUIMenuFlyout(this);

    // 添加基本菜单项
    auto newItem = contextMenu->addItem("新建");

    // 添加分隔符
    contextMenu->addSeparator();

    // 添加可选中的菜单项
    auto boldItem = contextMenu->addCheckableItem("粗体", false);

    // 添加分隔符
    contextMenu->addSeparator();

    // 添加单选菜单项组
    auto leftAlign = contextMenu->addRadioItem("左对齐", "alignment", true);

    // 添加分隔符
    contextMenu->addSeparator();

    // 添加子菜单
    auto formatSubMenu = contextMenu->addSubMenu("格式");
    formatSubMenu->addItem("字体...");
    formatSubMenu->addItem("段落...");
    formatSubMenu->addSeparator();
    formatSubMenu->addItem("样式...");

    // 添加更多菜单项
    contextMenu->addSeparator();
    auto exitItem = contextMenu->addItem("退出");

    // 连接菜单按钮点击事件
    connect(menuButton, &QWinUIButton::clicked, [this, contextMenu, menuButton]() {
        // 在按钮下方显示菜单
        QPoint buttonPos = menuButton->mapToGlobal(QPoint(0, menuButton->height()));
        contextMenu->showAt(buttonPos);
    });

    // 连接菜单项点击事件
    connect(contextMenu, &QWinUIMenuFlyout::itemClicked, [this](QWinUIMenuFlyoutItem* item) {
        QString itemText = item->text();

        if (itemText == "新建") {
            m_titleLabel->setText("菜单: 新建文档");
        } else if (itemText == "打开") {
            m_titleLabel->setText("菜单: 打开文件");
        } else if (itemText == "保存") {
            m_titleLabel->setText("菜单: 保存文件");
        } else if (itemText == "粗体") {
            m_titleLabel->setText(QString("菜单: 粗体 %1").arg(item->isChecked() ? "开启" : "关闭"));
        } else if (itemText == "斜体") {
            m_titleLabel->setText(QString("菜单: 斜体 %1").arg(item->isChecked() ? "开启" : "关闭"));
        } else if (itemText == "下划线") {
            m_titleLabel->setText(QString("菜单: 下划线 %1").arg(item->isChecked() ? "开启" : "关闭"));
        } else if (itemText.contains("对齐")) {
            m_titleLabel->setText(QString("菜单: 选择了 %1").arg(itemText));
        } else if (itemText == "字体...") {
            m_titleLabel->setText("菜单: 打开字体对话框");
        } else if (itemText == "段落...") {
            m_titleLabel->setText("菜单: 打开段落对话框");
        } else if (itemText == "样式...") {
            m_titleLabel->setText("菜单: 打开样式对话框");
        } else if (itemText == "退出") {
            m_titleLabel->setText("菜单: 退出应用程序");
        } else {
            m_titleLabel->setText(QString("菜单: 点击了 %1").arg(itemText));
        }
    });

    enableWindowsBlur(false);

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
