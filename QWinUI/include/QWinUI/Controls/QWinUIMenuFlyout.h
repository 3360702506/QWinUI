#ifndef QWINUIMENUFLYOUT_H
#define QWINUIMENUFLYOUT_H

#include "../QWinUIWidget.h"
#include "../QWinUIGlobal.h"
#include "QWinUIButton.h"
#include "QWinUIIcon.h"
#include "QWinUIAppBarSeparator.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QTimer>
#include <QList>

QT_BEGIN_NAMESPACE

class QWinUIMenuFlyoutItem;
class QWinUIMenuFlyout;

// 菜单项类型枚举
enum class QWinUIMenuFlyoutItemType {
    Standard,
    Checkable,
    Radio,
    Separator,
    SubMenu
};

// 菜单项基类
class QWINUI_EXPORT QWinUIMenuFlyoutItem : public QWinUIButton
{
    Q_OBJECT
    Q_PROPERTY(QWinUIMenuFlyoutItemType itemType READ itemType WRITE setItemType)
    Q_PROPERTY(bool checked READ isChecked WRITE setChecked NOTIFY checkedChanged)
    Q_PROPERTY(QString radioGroup READ radioGroup WRITE setRadioGroup)
    Q_PROPERTY(double hoverProgress READ hoverProgress WRITE setHoverProgress)
    Q_PROPERTY(double pressProgress READ pressProgress WRITE setPressProgress)

public:
    explicit QWinUIMenuFlyoutItem(const QString& text, QWidget* parent = nullptr);
    explicit QWinUIMenuFlyoutItem(const QString& text, const QIcon& icon, QWidget* parent = nullptr);
    explicit QWinUIMenuFlyoutItem(QWinUIMenuFlyoutItemType type, QWidget* parent = nullptr);
    ~QWinUIMenuFlyoutItem() override;

    // 类型管理
    QWinUIMenuFlyoutItemType itemType() const;
    void setItemType(QWinUIMenuFlyoutItemType type);

    // 选中状态（用于 Checkable 和 Radio 类型）
    bool isChecked() const;
    void setChecked(bool checked);

    // 单选组（用于 Radio 类型）
    QString radioGroup() const;
    void setRadioGroup(const QString& group);

    // 子菜单
    QWinUIMenuFlyout* subMenu() const;
    void setSubMenu(QWinUIMenuFlyout* menu);

    // 快捷键
    QString shortcut() const;
    void setShortcut(const QString& shortcut);

    // 动画属性
    double hoverProgress() const;
    void setHoverProgress(double progress);

    double pressProgress() const;
    void setPressProgress(double progress);

    // 尺寸提示
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void checkedChanged(bool checked);
    void triggered();
    void subMenuRequested();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    void initializeItem();
    void updateItemAppearance();
    void drawCheckIndicator(QPainter* painter, const QRect& rect);
    void drawSubMenuArrow(QPainter* painter, const QRect& rect);

private:
    QWinUIMenuFlyoutItemType m_itemType;
    bool m_checked;
    QString m_radioGroup;
    QWinUIMenuFlyout* m_subMenu;
    QString m_shortcut;

    // 动画相关
    QPropertyAnimation* m_hoverAnimation;
    QPropertyAnimation* m_pressAnimation;
    double m_hoverProgress;
    double m_pressProgress;
    
    static constexpr int ITEM_HEIGHT = 40;
    static constexpr int ITEM_PADDING = 12;
    static constexpr int ICON_SIZE = 16;
    static constexpr int CHECK_SIZE = 16;
    static constexpr int ARROW_SIZE = 8;
    static constexpr int SHORTCUT_SPACING = 24;
};

// 分隔符类型定义
typedef QWinUIAppBarSeparator QWinUIMenuFlyoutSeparator;

// 主菜单飞出控件
class QWINUI_EXPORT QWinUIMenuFlyout : public QWinUIWidget
{
    Q_OBJECT
    Q_PROPERTY(bool autoClose READ autoClose WRITE setAutoClose NOTIFY autoCloseChanged)
    Q_PROPERTY(int animationDuration READ animationDuration WRITE setAnimationDuration NOTIFY animationDurationChanged)
    Q_PROPERTY(int maxWidth READ maxWidth WRITE setMaxWidth NOTIFY maxWidthChanged)
    Q_PROPERTY(int minWidth READ minWidth WRITE setMinWidth NOTIFY minWidthChanged)
    Q_PROPERTY(double heightProgress READ heightProgress WRITE setHeightProgress)

public:
    explicit QWinUIMenuFlyout(QWidget* parent = nullptr);
    ~QWinUIMenuFlyout() override;

    // 菜单项管理
    QWinUIMenuFlyoutItem* addItem(const QString& text, const QIcon& icon = QIcon());
    QWinUIMenuFlyoutItem* addCheckableItem(const QString& text, bool checked = false, const QIcon& icon = QIcon());
    QWinUIMenuFlyoutItem* addRadioItem(const QString& text, const QString& group, bool checked = false, const QIcon& icon = QIcon());
    QWinUIMenuFlyout* addSubMenu(const QString& text, const QIcon& icon = QIcon());
    QWinUIMenuFlyoutSeparator* addSeparator();
    
    void insertItem(int index, QWinUIMenuFlyoutItem* item);
    void removeItem(QWinUIMenuFlyoutItem* item);
    void clear();

    // 显示控制
    void showAt(QWidget* parent, const QPoint& position);
    void showAt(const QPoint& globalPosition);
    void hide();

    // 属性
    bool autoClose() const;
    void setAutoClose(bool autoClose);

    int animationDuration() const;
    void setAnimationDuration(int duration);

    int maxWidth() const;
    void setMaxWidth(int width);

    int minWidth() const;
    void setMinWidth(int width);

    // 高度动画属性
    double heightProgress() const;
    void setHeightProgress(double progress);

    // 查找方法
    QList<QWinUIMenuFlyoutItem*> items() const;
    QWinUIMenuFlyoutItem* itemAt(int index) const;
    int itemCount() const;

    // 尺寸提示
    QSize sizeHint() const override;

public slots:
    void closeAllSubMenus();

signals:
    void itemClicked(QWinUIMenuFlyoutItem* item);
    void aboutToShow();
    void aboutToHide();
    void autoCloseChanged(bool autoClose);
    void animationDurationChanged(int duration);
    void maxWidthChanged(int width);
    void minWidthChanged(int width);

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

    // 主题变化处理
    void onThemeChanged() override;

private slots:
    void onItemTriggered();
    void onItemSubMenuRequested();
    void onSubMenuAboutToShow();
    void onSubMenuAboutToHide();
    void onAnimationFinished();

private:
    // 初始化
    void initializeMenuFlyout();
    void setupShadowEffect();
    void updateColors();

    // 布局和定位
    void updateLayout();
    void positionMenu(const QPoint& globalPos);
    void positionSubMenu(QWinUIMenuFlyout* subMenu, QWinUIMenuFlyoutItem* parentItem);
    QRect getAvailableGeometry(const QPoint& pos) const;

    // 动画
    void startShowAnimation();
    void startHideAnimation();
    int calculateTotalHeight() const;

    // 键盘导航
    void navigateUp();
    void navigateDown();
    void navigateLeft();
    void navigateRight();
    void activateCurrentItem();
    void selectItem(int index);

    // 子菜单管理
    void showSubMenu(QWinUIMenuFlyoutItem* item);
    void hideSubMenu();
    void hideAllSubMenus();

    // 动画计算
    void calculateAnimationParameters(const QPoint& triggerPosition);

private:
    // 布局
    QVBoxLayout* m_layout;
    QList<QWidget*> m_menuItems; // 包含 QWinUIMenuFlyoutItem 和 QWinUIMenuFlyoutSeparator
    
    // 状态
    QWinUIMenuFlyout* m_parentMenu;
    QWinUIMenuFlyout* m_currentSubMenu;
    QWinUIMenuFlyoutItem* m_currentItem;
    int m_selectedIndex;
    bool m_isVisible;
    
    // 属性
    bool m_autoClose;
    int m_animationDuration;
    int m_maxWidth;
    int m_minWidth;
    
    // 动画
    QPropertyAnimation* m_showAnimation;
    QPropertyAnimation* m_hideAnimation;
    QPropertyAnimation* m_heightAnimation;
    QPropertyAnimation* m_hideHeightAnimation;
    QGraphicsDropShadowEffect* m_shadowEffect;

    // 动画属性
    double m_heightProgress;
    int m_targetHeight;
    int m_currentAnimatedHeight;
    bool m_isAnimating;

    // 展开动画相关
    QPoint m_baselinePosition;      // 基准线位置（触发按钮位置）
    QPoint m_finalPosition;         // 最终位置
    bool m_expandUpward;            // 是否向上展开
    int m_animationOffset;          // 当前动画偏移量
    QSize m_triggerButtonSize;      // 触发按钮的尺寸
    
    // 定时器
    QTimer* m_subMenuTimer;
    
    // 常量
    static constexpr int DEFAULT_MIN_WIDTH = 120;
    static constexpr int DEFAULT_MAX_WIDTH = 300;
    static constexpr int DEFAULT_ANIMATION_DURATION = 150;
    static constexpr int MENU_PADDING = 6;
    static constexpr int SUBMENU_DELAY = 300;
    static constexpr int SUBMENU_OFFSET = 4;
    static constexpr int SHADOW_BLUR_RADIUS = 16;
    static constexpr int SHADOW_OFFSET = 4;
};

QT_END_NAMESPACE

#endif // QWINUIMENUFLYOUT_H
