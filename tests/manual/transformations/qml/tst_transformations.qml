import QtQuick 1.0
import Qt3D 1.0

// QTBUG-17408
Flickable {

    contentWidth: aTestCase.defaultWidth * 2
    contentHeight: aTestCase.defaultHeight * 1

    width: aTestCase.defaultWidth * 2
    height: aTestCase.defaultHeight * 1

    Flow {
        ComposedItemTransformationsTestCase {
            id: aTestCase
            focus: false
        }

        TransformationListTestCase {
            focus: false
        }
    }
}
