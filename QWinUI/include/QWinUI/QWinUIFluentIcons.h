#pragma once

// QWinUI Fluent Icons 常量
// 基于 Segoe Fluent Icons 字体的图标字符映射

#include <QChar>
#include <QString>

namespace QWinUIFluentIcons {

    // 导航图标
    namespace Navigation {
        inline const QChar HOME = QChar(0xE80F);
        inline const QChar BACK = QChar(0xE72B);
        inline const QChar FORWARD = QChar(0xE72A);
        inline const QChar REFRESH = QChar(0xE72C);
        inline const QChar SEARCH = QChar(0xE721);
        inline const QChar MENU = QChar(0xE700);
        inline const QChar MORE = QChar(0xE712);
        inline const QChar CLOSE = QChar(0xE8BB);
    }

    // 操作图标
    namespace Actions {
        inline const QChar ADD = QChar(0xE710);
        inline const QChar REMOVE = QChar(0xE738);
        inline const QChar DELETE_ICON = QChar(0xE74D);
        inline const QChar EDIT = QChar(0xE70F);
        inline const QChar SAVE = QChar(0xE74E);
        inline const QChar OPEN = QChar(0xE8E5);
        inline const QChar COPY = QChar(0xE8C8);
        inline const QChar PASTE = QChar(0xE77F);
        inline const QChar CUT = QChar(0xE8C6);
        inline const QChar UNDO = QChar(0xE7A7);
        inline const QChar REDO = QChar(0xE7A6);
        inline const QChar SHARE = QChar(0xE72D);
        inline const QChar DOWNLOAD = QChar(0xE896);
        inline const QChar UPLOAD = QChar(0xE898);
    }

    // 媒体图标
    namespace Media {
        inline const QChar PLAY = QChar(0xE768);
        inline const QChar PAUSE = QChar(0xE769);
        inline const QChar STOP = QChar(0xE71A);
        inline const QChar VOLUME = QChar(0xE767);
        inline const QChar MUTE = QChar(0xE74F);
        inline const QChar CAMERA = QChar(0xE722);
        inline const QChar PICTURE = QChar(0xE91B);
        inline const QChar VIDEO = QChar(0xE714);
        inline const QChar MUSIC = QChar(0xE8D6);
        inline const QChar MICROPHONE = QChar(0xE720);
    }

    // 文件和文档
    namespace Files {
        inline const QChar FOLDER = QChar(0xE8B7);
        inline const QChar FILE_ICON = QChar(0xE8A5);
        inline const QChar DOCUMENT = QChar(0xE8A5);
        inline const QChar ATTACH = QChar(0xE723);
        inline const QChar LINK = QChar(0xE71B);
    }

    // 通信图标
    namespace Communication {
        inline const QChar MAIL = QChar(0xE715);
        inline const QChar MESSAGE = QChar(0xE8BD);
        inline const QChar PHONE = QChar(0xE717);
        inline const QChar CONTACT = QChar(0xE77B);
        inline const QChar PEOPLE = QChar(0xE716);
    }

    // 系统图标
    namespace System {
        inline const QChar SETTINGS = QChar(0xE713);
        inline const QChar POWER = QChar(0xE7E8);
        inline const QChar LOCK = QChar(0xE72E);
        inline const QChar UNLOCK = QChar(0xE785);
        inline const QChar WIFI = QChar(0xE701);
        inline const QChar BLUETOOTH = QChar(0xE702);
        inline const QChar BATTERY = QChar(0xE83F);
        inline const QChar KEYBOARD = QChar(0xE765);
        inline const QChar MOUSE = QChar(0xE962);
        inline const QChar PRINT = QChar(0xE749);
    }

    // 状态图标
    namespace Status {
        inline const QChar FAVORITE = QChar(0xE734);
        inline const QChar HEART = QChar(0xE734);
        inline const QChar STAR = QChar(0xE735);
        inline const QChar BOOKMARK = QChar(0xE8A4);
        inline const QChar FLAG = QChar(0xE7C1);
        inline const QChar TAG = QChar(0xE8EC);
        inline const QChar CHECK = QChar(0xE73E);
        inline const QChar CANCEL = QChar(0xE711);
        inline const QChar WARNING = QChar(0xE7BA);
        inline const QChar ERROR_ICON = QChar(0xE783);
        inline const QChar INFO = QChar(0xE946);
        inline const QChar HELP = QChar(0xE897);
    }

    // 箭头图标
    namespace Arrows {
        inline const QChar ARROW_UP = QChar(0xE70E);
        inline const QChar ARROW_DOWN = QChar(0xE70D);
        inline const QChar ARROW_LEFT = QChar(0xE70C);
        inline const QChar ARROW_RIGHT = QChar(0xE70B);
        inline const QChar CHEVRON_UP = QChar(0xE70E);
        inline const QChar CHEVRON_DOWN = QChar(0xE70D);
        inline const QChar CHEVRON_LEFT = QChar(0xE76B);
        inline const QChar CHEVRON_RIGHT = QChar(0xE76C);
    }

    // 视图图标
    namespace View {
        inline const QChar LIST = QChar(0xE8FD);
        inline const QChar GRID = QChar(0xE80A);
        inline const QChar VIEW_ICON = QChar(0xE890);
        inline const QChar FILTER = QChar(0xE71C);
        inline const QChar SORT = QChar(0xE8CB);
        inline const QChar ZOOM_IN = QChar(0xE8A3);
        inline const QChar ZOOM_OUT = QChar(0xE71F);
        inline const QChar FULL_SCREEN = QChar(0xE740);
        inline const QChar MINIMIZE = QChar(0xE921);
        inline const QChar MAXIMIZE = QChar(0xE922);
        inline const QChar RESTORE = QChar(0xE923);
    }

    // 时间和位置
    namespace TimeLocation {
        inline const QChar CALENDAR = QChar(0xE787);
        inline const QChar CLOCK = QChar(0xE823);
        inline const QChar GLOBE = QChar(0xE774);
        inline const QChar LOCATION = QChar(0xE81D);
        inline const QChar MAP = QChar(0xE707);
    }

    // 交通工具
    namespace Transport {
        inline const QChar CAR = QChar(0xE804);
        inline const QChar AIRPLANE = QChar(0xE709);
        inline const QChar TRAIN = QChar(0xE7C0);
    }

    // 购物和金融
    namespace Shopping {
        inline const QChar SHOPPING = QChar(0xE7BF);
        inline const QChar CART = QChar(0xE7BF);
        inline const QChar MONEY = QChar(0xE8D4);
        inline const QChar CALCULATOR = QChar(0xE8EF);
    }

    // 便捷方法
    namespace Utils {
        // 根据图标名称获取字符
        inline QChar getIconChar(const QString& iconName) {
            // 这里可以实现名称到字符的映射
            // 暂时返回空字符，具体实现在QWinUIIcon中
            Q_UNUSED(iconName)
            return QChar();
        }
    }

} // namespace QWinUIFluentIcons
