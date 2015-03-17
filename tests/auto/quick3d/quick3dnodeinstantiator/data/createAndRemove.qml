import QtQml 2.1
// To avoid clash with Component.onCompleted
// TODO: Export Qt3D::QComponent with a different name
import Qt3D 2.0 as Qt3D

Qt3D.Entity {
    Qt3D.NodeInstantiator {
        objectName: "instantiator1"
        model: model1
        delegate: Qt3D.Entity {
            property string datum: model.text
        }
    }
    Component.onCompleted: {
        model1.add("Delta");
        model1.add("Gamma");
        model1.add("Beta");
        model1.add("Alpha");
    }
}
