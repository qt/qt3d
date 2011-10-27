import QtQuick 1.0
import Qt3D 1.0
import Qt3D.Shapes 1.0

Item3D {
    id: planetoid
    property real size: 0.5
    Sphere {
        levelOfDetail: 3
        axis: Qt.YAxis
        radius: parent.size
    }
}
