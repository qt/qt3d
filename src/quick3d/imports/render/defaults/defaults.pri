# When adding new QML files that should be built into the plugin,
# add them to this variable and they will be listed into a generated
# resource file.
#
# To have the plugin register them as types, add an entries to the
# qmldir array in qt3dquick3drenderplugin.cpp
QML_FILES = \
    $$PWD/qml/PhongMaterial.qml
