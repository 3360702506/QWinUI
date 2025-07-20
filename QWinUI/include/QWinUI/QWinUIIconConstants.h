#pragma once

// QWinUI 图标路径常量
// 提供预定义的常用图标路径

#include <QString>

namespace QWinUIIcons {

    // Solid Icons - 实心图标（常用）
    namespace Solid {
        inline const QString HOME = "solid/house";
        inline const QString HEART = "solid/heart";
        inline const QString STAR = "solid/star";
        inline const QString USER = "solid/user";
        inline const QString GEAR = "solid/gear";
        inline const QString BELL = "solid/bell";
        inline const QString ENVELOPE = "solid/envelope";
        inline const QString FOLDER = "solid/folder";
        inline const QString FILE_ICON = "solid/file";
        inline const QString EDIT = "solid/pen-to-square";
        inline const QString SAVE = "solid/floppy-disk";
        inline const QString TRASH = "solid/trash";
        inline const QString PLUS = "solid/plus";
        inline const QString MINUS = "solid/minus";
        inline const QString CHECK = "solid/check";
        inline const QString XMARK = "solid/xmark";
        inline const QString REFRESH = "solid/arrows-rotate";
        inline const QString DOWNLOAD = "solid/download";
        inline const QString UPLOAD = "solid/upload";
        inline const QString SEARCH = "solid/magnifying-glass";
        inline const QString LOCK = "solid/lock";
        inline const QString UNLOCK = "solid/unlock";
        inline const QString EYE = "solid/eye";
        inline const QString PLAY = "solid/play";
        inline const QString PAUSE = "solid/pause";
        inline const QString STOP = "solid/stop";
        inline const QString ARROW_LEFT = "solid/arrow-left";
        inline const QString ARROW_RIGHT = "solid/arrow-right";
        inline const QString ARROW_UP = "solid/arrow-up";
        inline const QString ARROW_DOWN = "solid/arrow-down";
        inline const QString BARS = "solid/bars";
        inline const QString INFO = "solid/info";
        inline const QString WARNING = "solid/triangle-exclamation";
        inline const QString POWER_OFF = "solid/power-off";
    }

    // Regular Icons - 线框图标（常用）
    namespace Regular {
        inline const QString HEART = "regular/heart";
        inline const QString STAR = "regular/star";
        inline const QString USER = "regular/user";
        inline const QString BELL = "regular/bell";
        inline const QString ENVELOPE = "regular/envelope";
        inline const QString FOLDER = "regular/folder";
        inline const QString FILE_ICON = "regular/file";
        inline const QString BOOKMARK = "regular/bookmark";
        inline const QString CALENDAR = "regular/calendar";
        inline const QString CLOCK = "regular/clock";
        inline const QString CIRCLE = "regular/circle";
        inline const QString COPY = "regular/copy";
        inline const QString EYE = "regular/eye";
    }

    // Brand Icons - 品牌图标（常用）
    namespace Brands {
        inline const QString WINDOWS = "brands/windows";
        inline const QString MICROSOFT = "brands/microsoft";
        inline const QString APPLE = "brands/apple";
        inline const QString GOOGLE = "brands/google";
        inline const QString GITHUB = "brands/github";
        inline const QString CHROME = "brands/chrome";
        inline const QString FIREFOX = "brands/firefox";
        inline const QString ANDROID = "brands/android";
    }

    // 便捷方法
    namespace Utils {
        inline QString getResourcePath(const QString& iconPath) {
            return ":/icons/" + iconPath;
        }
    }

} // namespace QWinUIIcons
