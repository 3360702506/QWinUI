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
// #include <QWinUI/Controls/QWinUIIconButton.h> // Temporarily disabled
#include <QWinUI/QWinUIIconManager.h>
#include <QMessageBox>
#include <QApplication>

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

    // 模糊效果切换
    QWinUIToggleSwitch* blurToggle = new QWinUIToggleSwitch(this);
    blurToggle->setOffContent("模糊效果：关闭");
    blurToggle->setOnContent("模糊效果：开启");
    blurToggle->setIsOn(true);
    connect(blurToggle, &QWinUIToggleSwitch::toggled, [this](bool checked) {
        if (checked) {
            setAttribute(Qt::WA_TranslucentBackground, true);
            enableWindowsBlur(true);
            setWindowsBlurType(2);
            m_titleLabel->setText("当前效果: Acrylic");
        } else {
            enableWindowsBlur(false);
            setAttribute(Qt::WA_TranslucentBackground, false);
            setStyleSheet("Widget { background-color: #f0f0f0; }");
            m_titleLabel->setText("当前效果: 无模糊");
        }
    });
    m_layout->addWidget(blurToggle);
    blurToggle->setToolTipText("切换窗口模糊效果\n开启后窗口将显示 Acrylic 或 Mica 效果");

    // 主题切换
    QWinUIToggleSwitch* themeToggle = new QWinUIToggleSwitch(this);
    themeToggle->setOffContent("浅色主题");
    themeToggle->setOnContent("深色主题");
    connect(themeToggle, &QWinUIToggleSwitch::toggled, [this](bool checked) {
        QWinUITheme* theme = QWinUITheme::getInstance();
        startThemeTransition(QWinUIWidget::FadeTransition);
        theme->setThemeMode(checked ? QWinUIThemeMode::Dark : QWinUIThemeMode::Light);
    });
    m_layout->addWidget(themeToggle);
    themeToggle->setToolTipText("切换应用主题\n支持浅色和深色两种主题模式\n带有平滑的过渡动画");

    // 模糊类型切换
    QWinUIToggleSwitch* blurTypeToggle = new QWinUIToggleSwitch(this);
    blurTypeToggle->setOffContent("Acrylic效果");
    blurTypeToggle->setOnContent("Mica效果");
    connect(blurTypeToggle, &QWinUIToggleSwitch::toggled, [this](bool checked) {
        setWindowsBlurType(checked ? 3 : 2);
        m_titleLabel->setText(QString("当前效果: %1").arg(checked ? "Mica" : "Acrylic"));
    });
    m_layout->addWidget(blurTypeToggle);
    blurTypeToggle->setToolTipText("选择模糊效果类型\nAcrylic: 半透明模糊效果\nMica: Windows 11 新材质效果");

    // 窗口装饰切换
    QWinUIToggleSwitch* decorationToggle = new QWinUIToggleSwitch(this);
    decorationToggle->setOffContent("标准窗口");
    decorationToggle->setOnContent("无边框窗口");
    connect(decorationToggle, &QWinUIToggleSwitch::toggled, [this](bool checked) {
        setWindowFlags(checked ? (Qt::Window | Qt::FramelessWindowHint) : Qt::Window);
        show();
    });
    m_layout->addWidget(decorationToggle);
    decorationToggle->setToolTipText("切换窗口边框样式\n标准窗口: 带有系统标题栏\n无边框窗口: 现代化无边框设计");

    // 动画效果切换
    QWinUIToggleSwitch* animationToggle = new QWinUIToggleSwitch(this);
    animationToggle->setOffContent("渐变动画");
    animationToggle->setOnContent("波纹动画");
    connect(animationToggle, &QWinUIToggleSwitch::toggled, [this](bool checked) {
        m_transitionType = checked ? QWinUIWidget::RippleTransition : QWinUIWidget::FadeTransition;
    });
    m_layout->addWidget(animationToggle);
    animationToggle->setToolTipText("选择主题切换动画类型\n渐变动画: 平滑的颜色过渡\n波纹动画: 从点击位置扩散的圆圈效果");

    // 窗口圆角切换
    QWinUIToggleSwitch* cornerToggle = new QWinUIToggleSwitch(this);
    cornerToggle->setOffContent("标准圆角");
    cornerToggle->setOnContent("大圆角");
    connect(cornerToggle, &QWinUIToggleSwitch::toggled, [this](bool checked) {
        // setWindowCornerPreference(checked ? 2 : 1);  // 2=大圆角, 1=标准圆角
    });
    m_layout->addWidget(cornerToggle);
    cornerToggle->setToolTipText("调整窗口圆角大小\n标准圆角: 4px 圆角半径\n大圆角: 8px 圆角半径\n符合 Windows 11 设计规范");

    // 通知测试按钮
    QWinUIButton* notificationBtn = new QWinUIButton("测试通知", this);
    connect(notificationBtn, &QWinUIButton::clicked, [this]() {
        // 测试不同类型的通知
        static int notifyType = 0;
        switch (notifyType % 4) {
        case 0:
            QWinUINotification::showInfo("信息通知", "这是一个信息通知示例");
            break;
        case 1:
            QWinUINotification::showWarning("警告通知", "这是一个警告通知示例");
            break;
        case 2:
            QWinUINotification::showError("错误通知", "这是一个错误通知示例");
            break;
        case 3:
            QWinUINotification::showSuccess("成功通知", "这是一个成功通知示例");
            break;
        }
        notifyType++;
    });
    m_layout->addWidget(notificationBtn);
    notificationBtn->setToolTipText("测试系统通知功能\nWindows: 使用原生Toast通知\n其他平台: 使用回退通知窗口");

    // ProgressRing测试
    QHBoxLayout* progressRingLayout = new QHBoxLayout();

    // 确定进度环
    QWinUIProgressRing* determinateRing = new QWinUIProgressRing(this);
    determinateRing->setValue(50);
    determinateRing->setToolTipText("确定进度环\n当前进度: 50%");
    progressRingLayout->addWidget(determinateRing);

    // 不确定进度环
    QWinUIProgressRing* indeterminateRing = new QWinUIProgressRing(this);
    indeterminateRing->setIsIndeterminate(true);
    indeterminateRing->setToolTipText("不确定进度环\n正在加载...");
    progressRingLayout->addWidget(indeterminateRing);

    // 大尺寸进度环
    QWinUIProgressRing* largeRing = new QWinUIProgressRing(this);
    largeRing->setFixedSize(48, 48);
    largeRing->setRingThickness(6);
    largeRing->setValue(75);
    largeRing->setToolTipText("大尺寸进度环\n当前进度: 75%");
    progressRingLayout->addWidget(largeRing);

    // 非活动进度环
    QWinUIProgressRing* inactiveRing = new QWinUIProgressRing(this);
    inactiveRing->setValue(25);
    inactiveRing->setIsActive(false);
    inactiveRing->setToolTipText("非活动进度环\n已暂停");
    progressRingLayout->addWidget(inactiveRing);

    progressRingLayout->addStretch();
    m_layout->addLayout(progressRingLayout);

    // 进度环控制按钮
    QWinUIButton* progressRingBtn = new QWinUIButton("更新进度环", this);
    connect(progressRingBtn, &QWinUIButton::clicked, [=]() {
        static int progress = 0;
        progress = (progress + 25) % 125;
        determinateRing->setValue(progress);
        largeRing->setValue(progress);
        inactiveRing->setValue(progress);

        QString tooltip = QString("确定进度环\n当前进度: %1%").arg(progress);
        determinateRing->setToolTipText(tooltip);

        tooltip = QString("大尺寸进度环\n当前进度: %1%").arg(progress);
        largeRing->setToolTipText(tooltip);
    });
    m_layout->addWidget(progressRingBtn);
    progressRingBtn->setToolTipText("点击更新进度环的进度值\n演示进度变化动画");

    // 图标系统测试
    QHBoxLayout* iconLayout = new QHBoxLayout();

    // 不再使用图标管理器，直接使用资源路径

    // 基础图标测试
    QWinUIIcon* basicIcon = new QWinUIIcon(this);
    basicIcon->setIconName("solid/house");
    basicIcon->setIconSize(24, 24);
    basicIcon->setToolTipText("基础图标\n支持缩放和着色");
    iconLayout->addWidget(basicIcon);

    // 彩色图标测试
    QWinUIIcon* coloredIcon = new QWinUIIcon(this);
    coloredIcon->setIconName("solid/heart");
    coloredIcon->setIconSize(32, 32);
    coloredIcon->setIconColor(QColor(255, 100, 100));
    coloredIcon->setUseThemeColor(false);
    coloredIcon->setToolTipText("彩色图标\n自定义颜色");
    iconLayout->addWidget(coloredIcon);

    // 旋转图标测试
    QWinUIIcon* rotatingIcon = new QWinUIIcon(this);
    rotatingIcon->setIconName("solid/arrows-rotate");
    rotatingIcon->setIconSize(28, 28);
    rotatingIcon->setToolTipText("旋转图标\n点击开始旋转");
    connect(rotatingIcon, &QWinUIIcon::clicked, [=]() {
        rotatingIcon->startSpinAnimation(2000);
    });
    iconLayout->addWidget(rotatingIcon);

    // 动态SVG测试
    QWinUIIcon* dynamicIcon = new QWinUIIcon(this);
    QString simpleSvg = R"(
        <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24">
            <circle cx="12" cy="12" r="10" fill="currentColor"/>
            <text x="12" y="16" text-anchor="middle" fill="white" font-size="8">SVG</text>
        </svg>
    )";
    dynamicIcon->loadSvgFromString(simpleSvg);
    dynamicIcon->setIconSize(32, 32);
    dynamicIcon->setToolTipText("动态加载的SVG\n从字符串创建");
    iconLayout->addWidget(dynamicIcon);

    iconLayout->addStretch();
    m_layout->addLayout(iconLayout);

    // 图标按钮测试 (暂时禁用)
    // QHBoxLayout* iconButtonLayout = new QHBoxLayout();
    // ... 图标按钮代码暂时移除 ...

    // 图标控制按钮
    QWinUIButton* iconControlBtn = new QWinUIButton("切换图标主题", this);
    connect(iconControlBtn, &QWinUIButton::clicked, [=]() {
        static bool useTheme = true;
        useTheme = !useTheme;

        basicIcon->setUseThemeColor(useTheme);
        coloredIcon->setUseThemeColor(useTheme);
        rotatingIcon->setUseThemeColor(useTheme);
        dynamicIcon->setUseThemeColor(useTheme);

        iconControlBtn->setText(useTheme ? "切换图标主题" : "使用自定义颜色");
    });
    m_layout->addWidget(iconControlBtn);
    iconControlBtn->setToolTipText("切换图标颜色主题\n演示主题联动功能\n悬停图标查看强调色效果");

    // 添加弹性空间
    QSpacerItem* spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    m_layout->addItem(spacer);

    // 初始化设置
    setAttribute(Qt::WA_TranslucentBackground, true);
    enableWindowsBlur(true);
    setWindowsBlurType(2);
    m_titleLabel->setText("当前效果: Acrylic");
    m_transitionType = QWinUIWidget::FadeTransition;
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
    startThemeTransition(QWinUIWidget::RippleTransition);
    if (theme->themeMode() == QWinUIThemeMode::Light) {
        theme->setThemeMode(QWinUIThemeMode::Dark);
    } else {
        theme->setThemeMode(QWinUIThemeMode::Light);
    }
}

void Widget::onFadeThemeToggled()
{
    QWinUITheme* theme = QWinUITheme::getInstance();
    startThemeTransition(QWinUIWidget::FadeTransition);
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
