function save_qml(closePrompt) {

    var effectData = "";
    if (useCustomEffect) {
        effectData +=
            "    effect: Effect {\n" +
            "        decal: " + customEffect.decal + "\n" +
            "        blending: " + customEffect.blending + "\n";

        if (useCustomMaterial) {
            effectData +=
                "        material: Material {\n" +
                "            ambientColor: \"" + modelMaterial.ambientColor + "\"\n" +
                "            diffuseColor: \"" + modelMaterial.diffuseColor + "\"\n" +
                "            specularColor: \"" + modelMaterial.specularColor + "\"\n" +
                "            shininess: " + modelMaterial.shininess + "\n" +
                "            textureUrl: \"" + modelMaterial.textureUrl + "\"\n" +
                "        }\n"
        } else {
            effectData +=
                "        color: \"" + modelEffect.color + "\"\n" +
                "        texture: \"" + modelEffect.texture + "\"\n"
        }

        effectData += "    }\n"
    }

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
        "    property double translateX: 0; \n"+
        "    property double translateY: 0; \n"+
        "    property double translateZ: 0; \n"+
        "    property double rotationX: 0; \n"+
        "    property double rotationY: 0; \n"+
        "    property double rotationZ: 0; \n"+
        "    property double assetScale: 1.0; \n"+

        "    Translation3D {\n" +
        "        id: transformTranslate\n" +
        "        translate: Qt.vector3d(" +
        transformTranslate.translate.x + "+translateX, " +
        transformTranslate.translate.y + "+translateY, " +
        transformTranslate.translate.z + "+translateZ)\n" +
        "    }\n" +
        "\n" +
        "    Rotation3D {\n" +
        "        id: transformRotateX\n" +
        "        angle: " + transformRotateX.angle + "+rotationX\n" +
        "        axis: Qt.vector3d(1, 0, 0)\n" +
        "    }\n" +
        "\n" +
        "    Rotation3D {\n" +
        "        id: transformRotateY\n" +
        "        angle: " + transformRotateY.angle + "+rotationY\n" +
        "        axis: Qt.vector3d(0, 1, 0)\n" +
        "    }\n" +
        "\n" +
        "    Rotation3D {\n" +
        "        id: transformRotateZ\n" +
        "        angle: " + transformRotateZ.angle + "+rotationZ\n" +
        "        axis: Qt.vector3d(0, 0, 1)\n" +
        "    }\n" +
        "\n" +
        "    Scale3D {\n" +
        "        id: transformScale\n" +
        "        scale: Qt.vector3d(" +
        transformScale.scale.x + "*assetScale, " +
        transformScale.scale.y + "*assetScale, " +
        transformScale.scale.z + "*assetScale)\n" +
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
        effectData +
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
