/*
 * QWinUIVariableSizedWrapGrid 使用示例
 * 
 * 这个示例展示了如何使用 QWinUIVariableSizedWrapGrid 控件
 * 创建一个可变大小的网格布局，支持自动换行和动画效果
 */

#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QComboBox>
#include <QGroupBox>
#include <QCheckBox>
#include <QSlider>
#include "QWinUI/QWinUI.h"

class VariableSizedWrapGridDemo : public QMainWindow
{
    Q_OBJECT

public:
    VariableSizedWrapGridDemo(QWidget* parent = nullptr)
        : QMainWindow(parent)
    {
        setupUI();
        createSampleItems();
    }

private slots:
    void onOrientationChanged()
    {
        QComboBox* combo = qobject_cast<QComboBox*>(sender());
        if (combo) {
            m_grid->setOrientation(static_cast<QWinUIOrientation>(combo->currentIndex()));
        }
    }

    void onItemSizeChanged()
    {
        m_grid->setItemWidth(m_widthSpinBox->value());
        m_grid->setItemHeight(m_heightSpinBox->value());
    }

    void onMaxRowsColumnsChanged()
    {
        m_grid->setMaximumRowsOrColumns(m_maxRowsColumnsSpinBox->value());
    }

    void onSpacingChanged()
    {
        m_grid->setItemSpacing(m_spacingSlider->value());
    }

    void onAddItem()
    {
        // 创建一个新的按钮
        QPushButton* button = new QPushButton(QString("Item %1").arg(m_itemCounter++));
        button->setMinimumSize(50, 30);
        
        // 随机设置跨度
        int rowSpan = (qrand() % 2) + 1;
        int colSpan = (qrand() % 2) + 1;
        
        m_grid->addItem(button, rowSpan, colSpan);
        
        // 设置样式
        button->setStyleSheet(QString(
            "QPushButton {"
            "    background-color: %1;"
            "    border: 1px solid #ccc;"
            "    border-radius: 4px;"
            "    color: white;"
            "    font-weight: bold;"
            "}"
            "QPushButton:hover {"
            "    background-color: %2;"
            "}"
        ).arg(getRandomColor().name()).arg(getRandomColor().lighter().name()));
    }

    void onRemoveItem()
    {
        // 移除最后一个项目
        QList<QWidget*> children = m_grid->findChildren<QWidget*>();
        if (!children.isEmpty()) {
            QWidget* lastChild = children.last();
            m_grid->removeItem(lastChild);
            lastChild->deleteLater();
        }
    }

    void onClearItems()
    {
        m_grid->clearItems();
    }

private:
    void setupUI()
    {
        // 初始化 QWinUI
        QWinUI::initialize();
        
        // 创建中央控件
        QWidget* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
        
        // 创建网格控件
        m_grid = new QWinUIVariableSizedWrapGrid();
        m_grid->setItemWidth(100);
        m_grid->setItemHeight(80);
        m_grid->setItemSpacing(8);
        m_grid->setOrientation(QWinUIOrientation::Horizontal);
        
        // 创建控制面板
        QWidget* controlPanel = createControlPanel();
        
        // 布局
        mainLayout->addWidget(m_grid, 1);
        mainLayout->addWidget(controlPanel, 0);
        
        // 设置窗口属性
        setWindowTitle("QWinUIVariableSizedWrapGrid Demo");
        resize(800, 600);
        
        // 应用主题
        QWinUITheme::getInstance()->setThemeMode(QWinUIThemeMode::Light);
    }

    QWidget* createControlPanel()
    {
        QGroupBox* panel = new QGroupBox("控制面板");
        panel->setFixedWidth(250);
        
        QVBoxLayout* layout = new QVBoxLayout(panel);
        
        // 方向选择
        layout->addWidget(new QLabel("方向:"));
        QComboBox* orientationCombo = new QComboBox();
        orientationCombo->addItems({"水平", "垂直"});
        connect(orientationCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &VariableSizedWrapGridDemo::onOrientationChanged);
        layout->addWidget(orientationCombo);
        
        // 项目尺寸
        layout->addWidget(new QLabel("项目宽度:"));
        m_widthSpinBox = new QSpinBox();
        m_widthSpinBox->setRange(50, 200);
        m_widthSpinBox->setValue(100);
        connect(m_widthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                this, &VariableSizedWrapGridDemo::onItemSizeChanged);
        layout->addWidget(m_widthSpinBox);
        
        layout->addWidget(new QLabel("项目高度:"));
        m_heightSpinBox = new QSpinBox();
        m_heightSpinBox->setRange(50, 200);
        m_heightSpinBox->setValue(80);
        connect(m_heightSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                this, &VariableSizedWrapGridDemo::onItemSizeChanged);
        layout->addWidget(m_heightSpinBox);
        
        // 最大行/列数
        layout->addWidget(new QLabel("最大行/列数:"));
        m_maxRowsColumnsSpinBox = new QSpinBox();
        m_maxRowsColumnsSpinBox->setRange(-1, 10);
        m_maxRowsColumnsSpinBox->setValue(-1);
        m_maxRowsColumnsSpinBox->setSpecialValueText("无限制");
        connect(m_maxRowsColumnsSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                this, &VariableSizedWrapGridDemo::onMaxRowsColumnsChanged);
        layout->addWidget(m_maxRowsColumnsSpinBox);
        
        // 间距
        layout->addWidget(new QLabel("项目间距:"));
        m_spacingSlider = new QSlider(Qt::Horizontal);
        m_spacingSlider->setRange(0, 20);
        m_spacingSlider->setValue(8);
        connect(m_spacingSlider, &QSlider::valueChanged,
                this, &VariableSizedWrapGridDemo::onSpacingChanged);
        layout->addWidget(m_spacingSlider);
        
        // 操作按钮
        layout->addSpacing(20);
        
        QPushButton* addButton = new QPushButton("添加项目");
        connect(addButton, &QPushButton::clicked, this, &VariableSizedWrapGridDemo::onAddItem);
        layout->addWidget(addButton);
        
        QPushButton* removeButton = new QPushButton("移除项目");
        connect(removeButton, &QPushButton::clicked, this, &VariableSizedWrapGridDemo::onRemoveItem);
        layout->addWidget(removeButton);
        
        QPushButton* clearButton = new QPushButton("清空所有");
        connect(clearButton, &QPushButton::clicked, this, &VariableSizedWrapGridDemo::onClearItems);
        layout->addWidget(clearButton);
        
        layout->addStretch();
        
        return panel;
    }

    void createSampleItems()
    {
        // 创建一些示例项目
        QStringList itemNames = {
            "邮件", "日历", "照片", "音乐", "视频", "地图", 
            "天气", "新闻", "设置", "商店", "游戏", "办公"
        };
        
        QList<QColor> colors = {
            QColor("#FF6B6B"), QColor("#4ECDC4"), QColor("#45B7D1"), 
            QColor("#96CEB4"), QColor("#FFEAA7"), QColor("#DDA0DD"),
            QColor("#98D8C8"), QColor("#F7DC6F"), QColor("#BB8FCE"), 
            QColor("#85C1E9"), QColor("#F8C471"), QColor("#82E0AA")
        };
        
        for (int i = 0; i < qMin(itemNames.size(), colors.size()); ++i) {
            QPushButton* button = new QPushButton(itemNames[i]);
            button->setMinimumSize(50, 30);
            
            // 设置不同的跨度
            int rowSpan = (i % 3 == 0) ? 2 : 1;
            int colSpan = (i % 4 == 0) ? 2 : 1;
            
            m_grid->addItem(button, rowSpan, colSpan);
            
            // 设置样式
            button->setStyleSheet(QString(
                "QPushButton {"
                "    background-color: %1;"
                "    border: none;"
                "    border-radius: 4px;"
                "    color: white;"
                "    font-weight: bold;"
                "    font-size: 12px;"
                "}"
                "QPushButton:hover {"
                "    background-color: %2;"
                "}"
                "QPushButton:pressed {"
                "    background-color: %3;"
                "}"
            ).arg(colors[i].name())
             .arg(colors[i].lighter(110).name())
             .arg(colors[i].darker(110).name()));
        }
    }

    QColor getRandomColor()
    {
        QList<QColor> colors = {
            QColor("#FF6B6B"), QColor("#4ECDC4"), QColor("#45B7D1"), 
            QColor("#96CEB4"), QColor("#FFEAA7"), QColor("#DDA0DD"),
            QColor("#98D8C8"), QColor("#F7DC6F"), QColor("#BB8FCE"), 
            QColor("#85C1E9"), QColor("#F8C471"), QColor("#82E0AA")
        };
        return colors[qrand() % colors.size()];
    }

private:
    QWinUIVariableSizedWrapGrid* m_grid;
    QSpinBox* m_widthSpinBox;
    QSpinBox* m_heightSpinBox;
    QSpinBox* m_maxRowsColumnsSpinBox;
    QSlider* m_spacingSlider;
    int m_itemCounter = 1;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    VariableSizedWrapGridDemo demo;
    demo.show();
    
    return app.exec();
}

#include "QWinUIVariableSizedWrapGrid_Example.moc"
