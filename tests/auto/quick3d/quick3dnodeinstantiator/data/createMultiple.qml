import QtQml 2.1
import Qt3D.Core 2.0

NodeInstantiator {
    model: 10
    delegate: Entity {
        property bool success: true
        property int idx: index
    }
}
