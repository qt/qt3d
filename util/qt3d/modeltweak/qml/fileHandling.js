function save_qml(closePrompt) {

    var saveData =
            "//                 --------|  WARNING  |--------!!\n" +
            "// This is a generated file.  Modifying the text or layout of \n" +
            "// this file will mean that the Model-Tweaker application will no \n" +
            "// longer be able to read it. \n\n" +
            "// Changing numeric values is allowable, though care should be taken\n" +
            "// not to inadvertantly change the file structure.\n\n" +
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
            "        transformScale,\n" +
            "        transformRotateX,\n" +
            "        transformRotateY,\n" +
            "        transformRotateZ,\n" +
            "        transformTranslate,\n" +
            "    ]\n" +
            "}\n";

    quickFile.filename = source_mesh.source
    quickFile.data = saveData
    if (closePrompt)
        var result = quickFile.promptSave()
    else
        result = quickFile.save()

    if (result !== "")
        console.log("If there was an error it will be after here:" + result)
}
