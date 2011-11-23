import QtQuick 1.0
import Qt3D 1.0
import ModelTweak 1.0

Flow {
    width: buttonWidth * 2 + 8
    spacing: 8

    property double buttonWidth: 80

    BlenderToggle {
        width: buttonWidth
        buttonText: "Save!"
        function onClicked() {
            var saveData =
                    "import QtQuick 1.0\n" +
                    "import Qt3D 1.0\n" +
                    "\n" +
                    "Item3D {\n" +
                    "    Translation3D {\n" +
                    "        id: transformTranslate\n" +
                    "        translate: Qt.vector3d(" +
                        transformTranslate.translate.x + ", " +
                        transformTranslate.translate.y + ", " +
                        transformTranslate.translate.z + ")\n" +
                    "    }\n" +
                    "\n" +
                    "    Rotation3D {\n" +
                    "        id: transformRotateX\n" +
                    "        angle: " + transformRotateX.angle + "\n" +
                    "        axis: Qt.vector3d(1, 0, 0)\n" +
                    "    }\n" +
                    "\n" +
                    "    Rotation3D {\n" +
                    "        id: transformRotateY\n" +
                    "        angle: " + transformRotateY.angle + "\n" +
                    "        axis: Qt.vector3d(0, 1, 0)\n" +
                    "    }\n" +
                    "\n" +
                    "    Rotation3D {\n" +
                    "        id: transformRotateZ\n" +
                    "        angle: " + transformRotateZ.angle + "\n" +
                    "        axis: Qt.vector3d(0, 0, 1)\n" +
                    "    }\n" +
                    "\n" +
                    "    Scale3D {\n" +
                    "        id: transformScale\n" +
                    "        scale: Qt.vector3d(" +
                        transformScale.scale.x + ", " +
                        transformScale.scale.y + ", " +
                        transformScale.scale.z + ")\n" +
                    "    }\n" +
                    "\n" +
                    "    Mesh {\n" +
                    "        id: source_mesh\n" +
                    "        source: \"%1\"\n" +
                    "    }\n" +
                    "\n" +
                    "    mesh: source_mesh\n" +
                    "    transform: [\n" +
                    "        transformScale\n" +
                    "        transformRotateX,\n" +
                    "        transformRotateY,\n" +
                    "        transformRotateZ,\n" +
                    "        transformTranslate,\n" +
                    "    ]\n" +
                    "}\n";

            quickSave.filename = source_mesh.source
            quickSave.data = saveData
            var result = quickSave.save()
            console.log("If there was an error it will be after here:" + result)
        }
    }

    BlenderToggle {
        width: buttonWidth
        buttonText: "Load!"
        function onClicked() { console.log(quickLoad.openModelFile()); }
    }

    BlenderToggle {
        width: buttonWidth
        buttonText: "Help!"
        function onClicked() { helpOverlay.visible = true }
    }

    BlenderToggle {
        width: buttonWidth
        buttonText: "To let."
        function onClicked() { console.log("Dunno what this does yet...") }
    }
}
