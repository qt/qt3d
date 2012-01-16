import QtQuick 1.0

Grid {
    property variant color1: "#FFFFFF"
    property variant color2: "#BBBBBB"
    property variant cellSize: 10
    width: 20
    height: parent.height
    clip: true

    rows: height/cellSize
    columns: width/cellSize
    Repeater {
        model: parent.rows * parent.columns
        Rectangle {
            width: cellSize; height: cellSize;
            color: {
                var check;
                if (columns%2) check = index;
                else check = Math.floor(index/columns)%2 + index;

                if (check%2) return color1;
                else return color2;
            }
        }
    }
}
