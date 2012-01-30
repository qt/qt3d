import QtQuick 1.0
import "../ColorUtils.js" as ColorUtils

Item {
    property real hue: 0.0
    property real sat: 0.0
    property real val: 1.0
    property real alpha: 1.0
    property color color: ColorUtils.hsvToColor(hue, sat, val, alpha)
    property int red: rgb[0]
    property int green: rgb[1]
    property int blue: rgb[2]

    property variant rgb: ColorUtils.hsvToRgb(hue, sat, val)
}
